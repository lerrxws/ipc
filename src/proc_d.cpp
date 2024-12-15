#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>


void handleExit(const char* msg, int exit_status);
void semaphoreWait(int semaphore_pid);
void semaphoreSignal(int semaphore_pid);
void sendData(int memory_pid, int semaphore_pid, int port);
int setupAndConnect(int* socket_descriptor, struct sockaddr_in* server_addr, int port);

// Funkcia na spracovanie ukončenia programu
void handleExit(const char* msg, int exit_status) {
    if (exit_status == EXIT_SUCCESS) {
        printf("%s\n", msg); // Vypíše správu o úspešnom ukončení
        exit(EXIT_SUCCESS);
    } else {
        fprintf(stderr,"%s\n",msg); // Vypíše chybovú správu
        exit(EXIT_FAILURE);
    }
}

// Operácia čakania pre semafor
void semaphoreWait(int semaphore_pid) {
  // Inicializácia štruktúry pre semafor
    struct sembuf semaphore_buffer;
    semaphore_buffer.sem_num = 0;
    semaphore_buffer.sem_op = 1;
    semaphore_buffer.sem_flg = SEM_UNDO;
    // Čakanie na semafor
    if (semop(semaphore_pid, &semaphore_buffer, 1) == -1) {
        handleExit("proc_d: semaphoreWait failed", EXIT_FAILURE); 
    }
}

// Signalizácia semaforu
void semaphoreSignal(int semaphore_pid) {
  // Inicializácia štruktúry pre semafor
    struct sembuf semaphore_buffer;
    semaphore_buffer.sem_num = 1;
    semaphore_buffer.sem_op = -1;
    semaphore_buffer.sem_flg = SEM_UNDO;

    // Signalizovanie semaforu
    if (semop(semaphore_pid, &semaphore_buffer, 1) == -1) {
        handleExit("proc_d: semaphoreSignal failed", EXIT_FAILURE); 
    }
}

// Nastavenie soketu a pripojenie sa k serveru
int setupAndConnect(int* socket_descriptor, struct sockaddr_in* server_addr, int port) {
  // Vytvorenie socketu
    *socket_descriptor = socket(AF_INET, SOCK_STREAM, 0); 
    if (*socket_descriptor < 0) {
        handleExit("proc_d: socket error", EXIT_FAILURE);
    }

    server_addr->sin_family = AF_INET; // Nastavenie typu adresy (IPv4)
    server_addr->sin_port = htons(port); // Nastavenie portu servera
    server_addr->sin_addr.s_addr = inet_addr("127.0.0.1"); // Nastavenie IP adresy servera
    bzero(&(server_addr->sin_zero), 8); // Vynulovanie dodatočných núl v štruktúre adresy

    // Pripojenie k serveru
    if (connect(*socket_descriptor, (struct sockaddr *) server_addr, sizeof(*server_addr)) < 0) {
        handleExit("proc_d: connect error", EXIT_FAILURE); 
    }

    return *socket_descriptor;
}

// Odoslanie údajov na server 
void sendData(int memory_pid, int semaphore_pid, int port) {
    char buffer[256];
    // Priradenie zdieľanej pamäte
    char *shared_memory = (char*) shmat(memory_pid, NULL, 0); 
    if (shared_memory == (void*) -1) {
        handleExit("proc_d: shmat failed", EXIT_FAILURE); 
    }

    int socket_descriptor;
    struct sockaddr_in server_addr;

    setupAndConnect(&socket_descriptor, &server_addr, port); // Nastavenie soketu a pripojenie sa k serveru

    while (1) {
        semaphoreWait(semaphore_pid); // Čakanie na povolenie zapisovania do zdieľanej pamäte

        strcpy(buffer, shared_memory); // Kopírovanie dát zo zdieľanej pamäte do buffera
        write(socket_descriptor, buffer, strlen(buffer) + 1); // Zápis dát na server
        memset(buffer, '\0', sizeof(buffer)); // Vyčistenie buffera

        semaphoreSignal(semaphore_pid); // Signalizovanie konca zápisu
        sleep(1); 
    }
}

int main(int argc, char* argv[]) {
    printf("proc_d spustený\n"); // Výpis o spustení programu
    int memory_segment_pid = atoi(argv[1]); // Získanie ID zdieľanej pamäte
    int semaphore_pid = atoi(argv[2]); // Získanie ID semaforu
    int port = atoi(argv[3]); // Získanie čísla portu

    kill(getppid(), SIGUSR1); // Poslanie signálu rodičovskému procesu

    sendData(memory_segment_pid, semaphore_pid, port); // Spustenie funkcie na odosielanie dát

    return 0;
}

