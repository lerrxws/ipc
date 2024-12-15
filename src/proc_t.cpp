#include <stdio.h>  
#include <stdlib.h>
#include <unistd.h>    
#include <string.h>  
#include <signal.h>  
#include <sys/sem.h>  
#include <sys/shm.h>   
#include <fcntl.h>     
void pipeToMemory(int read_pipe, int memory_pid, int semaphore_pid);
void semaphoreWait(int semaphore_pid);
void semaphoreSignal(int semaphore_pid);
void handleExit(const char* msg, int exit_status);
void signalHandler(int signum);
// Funkcia pre čítanie z potrubia a zápis do zdieľanej pamäte
void pipeToMemory(int read_pipe, int memory_pid, int semaphore_pid) {
  const int BUFFER_SIZE = 256; 
  char buffer[BUFFER_SIZE]; 
  char *shared_memory = (char*) shmat(memory_pid, NULL, 0); // Pripojenie zdieľanej pamäte
  if (shared_memory == (void*) -1) handleExit("proc_t: shmat failed", 1); 

  while (1) {
    semaphoreWait(semaphore_pid); // Čakanie na povolenie zápisu

    // Čítanie z potrubia
    int i;
    for (i = 0; i < BUFFER_SIZE -1; i++) {
        if (read(read_pipe, &buffer[i], 1) < 1 || buffer[i] == '\n') {
          break;
        }
    }
    buffer[i] = '\0'; // Nastavenie konca reťazca

    // Zápis do zdieľanej pamäte
    strcpy(shared_memory, buffer);
    // Signalizácia konca zápisu
    semaphoreSignal(semaphore_pid); 
  }
}

// Funkcia pre riadenie ukončenia programu
void handleExit(const char* msg, int exit_status) {
  if (exit_status == EXIT_SUCCESS) {
      printf("%s\n", msg); // Výpis správy pri úspešnom ukončení
      exit(EXIT_SUCCESS);
  } else {
      fprintf(stderr,"%s\n",msg); // Výpis chybovej správy
      exit(EXIT_FAILURE);
  }
}

// Spracovanie signálu SIGUSR2
void signalHandler(int signum) {
  if (signum == SIGUSR2) {
      handleExit("proc_t: SUCCESS MESSAGE", EXIT_SUCCESS); // Spracovanie signálu SIGUSR2
  }
}

// Čakanie na semafor
void semaphoreWait(int semaphore_pid) {
  // Inicializácia štruktúry pre semafor
  struct sembuf semaphore_buffer;
  semaphore_buffer.sem_num = 1; 
  semaphore_buffer.sem_op = 1; 
  semaphore_buffer.sem_flg = SEM_UNDO; 

    // Čakanie na semafor
  if (semop(semaphore_pid, &semaphore_buffer, 1) == -1) {
      handleExit("proc_t: semaphoreWait failed", 1); // Spracovanie chyby pri čakaní na semafor
  }
}

// Signalizácia semaforu
void semaphoreSignal(int semaphore_pid) {
  // Inicializácia štruktúry pre semafor
  struct sembuf semaphore_buffer;
  semaphore_buffer.sem_num = 0; 
  semaphore_buffer.sem_op = -1; 
  semaphore_buffer.sem_flg = SEM_UNDO;
    
  // Signalizovanie semaforu
  if (semop(semaphore_pid, &semaphore_buffer, 1) == -1) {
      handleExit("proc_t: semaphoreSignal failed", 1); // Spracovanie chyby pri signalizácii semaforu
  }
}

int main(int argc, char* argv[]) {
  printf("proc_t spusteny\n");

  int pipe_read_fd = atoi(argv[1]); // Fd pre čítanie z potrubia
  int memory_segment_pid = atoi(argv[2]); // ID zdieľanej pamäte
  int semaphore_pid = atoi(argv[3]); // ID semaforu

  kill(getppid(), SIGUSR1); // Poslanie signálu rodičovskému procesu
  signal(SIGUSR2, signalHandler); // Nastavenie správcu signálu SIGUSR2

  pipeToMemory(pipe_read_fd, memory_segment_pid, semaphore_pid); // Spustenie funkcie na čítanie z potrubia a zápis do zdieľanej pamäte

  return 0;
}
