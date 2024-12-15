#include <sys/stat.h> 
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <signal.h>
#include <errno.h>
#include <iostream>


#pragma region Configuration
// descriptors for different processors
int Serv1, Serv2, D, P1, P2, Pr, T, S;
int sem1, sem2, shm1, shm2;

// local methods
void create_signals();
void initialize_semaphores(int sem1, int sem2);
void kill_processors(int s);
void check_program_status(int s);

#pragma endregion



#pragma region Main_Method

int main(int argc, char* argv[]) {
	if(argc < 3) {
        printf("[Zadanie] Error in argument numbers. Please check your arguments!\n");
        exit(EXIT_FAILURE);
    }

    int error; // to check if "execl == -1"

#pragma region Signals
    // create signals for re-read the first and the second configutration files
    create_signals();
#pragma endregion

#pragma region PIPES
    // ============== PIPES ==============
	int pipe1[2];
    int pipe2[2];

	if (pipe(pipe1) < 0) {
        printf("[Zadanie] Error in creating pipe R1\n");
        perror("pipe");
    }     
    if (pipe(pipe2) < 0) {
        printf("[Zadanie] Error in creating pipe R2\n");
        perror("pipe");
    }

    #pragma region INIT_STR_RESOURCES_PIPE
        int pipe1Read = dup(pipe1[0]);
        int pipe1Write = dup(pipe1[1]);
        int pipe2Read = dup(pipe2[0]);
        int pipe2Write = dup(pipe2[1]);

        char pipe1StringR[10];
        char pipe1StringW[10];
        char pipe2StringR[10];
        char pipe2StringW[10];

        // Initialising string representations of resources
        memset(pipe1StringR,'\0', sizeof(pipe1Read));
        memset(pipe1StringW,'\0', sizeof(pipe1Write));
        memset(pipe2StringR,'\0', sizeof(pipe2Read));
        memset(pipe2StringW,'\0', sizeof(pipe2Write));

        sprintf(pipe1StringR, "%d", pipe1Read);
        sprintf(pipe1StringW, "%d", pipe1Write);
        sprintf(pipe2StringR, "%d", pipe2Read);
        sprintf(pipe2StringW, "%d", pipe2Write);
    #pragma endregion
#pragma endregion
	
#pragma region SEMAPHORES
    // ============== SEMAPHORES ==============
    printf("\n");

	sem1 = semget(IPC_PRIVATE, 2, 0666 | IPC_CREAT);
    if(sem1 < 0) {
        printf("[Zadanie] Error in creating SEM1\n");
        perror("semget"); // check why we have that error
        exit(EXIT_FAILURE);
        //get_exit(false, "Error in semget\n");
    }

    sem2 = semget(IPC_PRIVATE, 2, 0666 | IPC_CREAT);    
    if(sem2 < 0) {
        printf("[Zadanie] Error in creating SEM2\n");
        perror("semget"); // check why we have that error
        exit(EXIT_FAILURE);
        //get_exit(false, "Error in semget\n");
    }

	initialize_semaphores(sem1, sem2);

    #pragma region INIT_STR_RESOURCES_SEMAPHORES
        char semaphor1String[10];
	    char semaphor2String[10];

        // Initialising string representations of resources
        memset(semaphor1String,'\0', sizeof(sem1));
        memset(semaphor2String,'\0', sizeof(sem2));

        sprintf(semaphor1String,"%d", sem1);
        sprintf(semaphor2String,"%d", sem2);
    #pragma endregion
#pragma endregion

#pragma region SHARED_MEMORY
    // ============== SHARED MEMORY ==============
	shm1 = shmget(IPC_PRIVATE, 256, 0666 | IPC_CREAT);
    if (shm1 < 0) {
        printf("[Zadanie] Error in creating SHARED MEMORY 1\n");
        perror("shmget"); // writes information about our error
		fprintf(stderr, "shmget failed\n");
		exit(EXIT_FAILURE);
    }

	shm2 = shmget(IPC_PRIVATE, 256, 0666 | IPC_CREAT);
    if (shm2 < 0) {
        printf("[Zadanie] Error in creating SHARED MEMORY 2\n");
        perror("shmget"); // writes information about our error
		fprintf(stderr, "shmget failed\n");
		exit(EXIT_FAILURE);
    }

    #pragma region INIT_STR_RESOURCES_SHARED_MEMORY
        char shm1String[10];
        char shm2String[10];

        // Initialising string representations of resources
        memset(shm1String,'\0', sizeof(shm1));
        memset(shm2String,'\0', sizeof(shm2));

        sprintf(shm1String,"%d", shm1);
	    sprintf(shm2String,"%d", shm2);
    #pragma endregion
#pragma endregion

#pragma region P1
    // ========== P1 ==========
	P1 = fork();
    switch (P1)
    {
        case 0:
        {
            printf("Executing proc_p1...\n");
            error = execl("proc_p1","proc_p1", pipe1StringW, (char*)NULL);
            if (error == -1) {
                printf("[Zadanie] Failed to execute proc_p1. Exiting...\n");
                perror("execl"); // writes the information about this error
                exit(EXIT_FAILURE);
            }
            printf("proc_p1 execution completed.\n");
            exit(EXIT_SUCCESS);
            break;
        }
        case -1:
            printf("[Zadanie] Failed to create process P1. Exiting...\n");
            perror("P1");
            exit(EXIT_FAILURE);
            break;

        default:
            break;
    }
    
	pause();

    #pragma region INIT_STR_RESOURCES_P1
        char P1_d[10];
        memset(P1_d, '\0', sizeof(P1));
        sprintf(P1_d, "%d", P1);
    #pragma endregion
#pragma endregion

#pragma region P2
    // ========== P2 ==========
    P2 = fork();
    switch (P2)
    {
        case 0:
        {
            printf("Executing proc_p2...\n");
            error = execl("proc_p2","proc_p2",pipe1StringW,(char*)NULL);

            if (error == -1) {
                printf("[Zadanie] Failed to execute proc_p2. Exiting...\n");
                perror("execl"); // writes the information about this error
                exit(EXIT_FAILURE);
            }
            exit(EXIT_SUCCESS);
            break;
        }
        case -1:
            printf("[Zadanie] Failed to create process P2. Exiting...\n");
            perror("P2");
            exit(EXIT_FAILURE);
            break;
        default:
            break;
    }
	pause();

    #pragma region INIT_STR_RESOURCES_P2
        char P2_d[10];
        memset(P2_d, '\0', sizeof(P2));
        sprintf(P2_d, "%d", P2);
    #pragma endregion
#pragma endregion

#pragma region Pr
    // ============ Pr ============
    Pr = fork();
    switch (Pr)
    {
        case 0:
        {
            printf("Executing proc_pr...\n");
            error = execl("proc_pr", "proc_pr",P1_d,P2_d,pipe1StringR,pipe2StringW,(char*)NULL);
            if (error == -1) {
                printf("[Zadanie] Failed to execute proc_pr. Exiting...\n");
                perror("execl"); // writes the information about this error
                exit(EXIT_FAILURE);
            }

            printf("proc_pr execution completed.\n");
            exit(EXIT_SUCCESS);
        }
        case -1:
            printf("[Zadanie] Failed to create process Pr. Exiting...\n");
            perror("Pr");
            exit(EXIT_FAILURE);
            break;
        default:
            break;
    }
	pause();
    int statusPr;
    waitpid(Pr, &statusPr, 0);
#pragma endregion

#pragma region SERVERS
    char string_server1[10];
	char string_server2[10];
    int serv1_port = atoi(argv[1]);
    int serv2_port = atoi(argv[2]);

    // Initialising string representations of resources
	memset(string_server1,'\0', sizeof(serv1_port));
	memset(string_server2,'\0', sizeof(serv2_port));

	sprintf(string_server1,"%d", serv1_port);
	sprintf(string_server2,"%d", serv2_port);
#pragma endregion

#pragma region Serv1
    // ============ Serv1 ============
	Serv1 = fork();
    switch (Serv1)
    {
        case 0:
        {
            printf("Executing proc_serv1...\n");
            error = execl("proc_serv1", "proc_serv1", string_server1, string_server2, (char * ) NULL);
            if (error == -1) {
                //fprintf (stderr, "-: Couldn't open file -; %s\n", strerror (errno));
                printf("[Zadanie] Failed to execute proc_serv1. Exiting...\n");
                perror("execl"); // writes the information about this error
                exit(EXIT_FAILURE);
            }

            printf("proc_serv1 execution completed.\n");
            exit(EXIT_SUCCESS);
            break;
        }
        case -1:
            printf("[Zadanie] Failed to create process Serv1. Exiting...\n");
            break;
        default:
            break;
    }
    pause();
#pragma endregion

#pragma region Serv2
    // ============== Serv2 ==============
	Serv2 = fork();
    switch (Serv2)
    {
        case 0:
        {
            printf("Executing proc_serv2...\n");

            error = execl("proc_serv2", "proc_serv2", string_server2, (char * ) NULL);
            if (error == -1) {
                printf("[Zadanie] Failed to execute proc_serv2. Exiting...\n");
                perror("execl"); // writes the information about this error
                exit(EXIT_FAILURE);
            }

            printf("proc_serv2 execution completed.\n");
            exit(EXIT_SUCCESS);
            break;
        }
        case -1:
            printf("[Zadanie] Failed to create process Serv2. Exiting...\n");
            perror("Serv2");
            break;
        default:
            break;
    }
	pause();
#pragma endregion

#pragma region proc_T
    // ============ proc_T ============ 
    T = fork();
    switch (T)
    {
        case 0:
        {
            printf("Executing proc_t...\n");
            error = execl("proc_t", "proc_t",pipe2StringR, shm1String, semaphor1String, (char*)NULL);
            if (error == -1) {
                printf("[Zadanie] Failed to execute proc_t. Exiting...\n");
                perror("execl"); // writes the information about this error
                exit(EXIT_FAILURE);
            }
            printf("proc_t execution completed.\n");
            exit(EXIT_SUCCESS);
        }
        case -1:
            printf("[Zadanie] Failed to create process T. Exiting...\n");
            perror("T");
            exit(EXIT_FAILURE);
            break;
        default:
            break;
    }
	pause();
#pragma endregion

#pragma region D
    // ============ proc_D ============
	D = fork();
    switch (D)
    {
        case 0:
        {
            printf("Executing proc_d...\n");
            error = execl("proc_d", "proc_d", shm2String, semaphor2String, string_server1, (char*)NULL);
            if (error == -1) {
                printf("[Zadanie] Failed to execute proc_d. Exiting...\n");
                perror("execl"); // writes the information about this error
                exit(EXIT_FAILURE);
            }

            printf("proc_d execution completed.\n");
            exit(EXIT_SUCCESS);
            break;
        }
        case -1:
            printf("[Zadanie] Failed to create process D. Exiting...\n");
            perror("D");
            break;
        default:
            break;
    }
	pause();
#pragma endregion

#pragma region proc_S
    // =========== proc_S ===========
    S = fork();
    switch (S)
    {
        case 0:
        {
            printf("Executing proc_s...\n");
            error = execl("proc_s", "proc_s",shm1String, semaphor1String, shm2String, semaphor2String, (char*)NULL);
            if(error == -1){
                perror("execl");
                printf("[Zadanie] Failed to execute proc_s. Exiting...\n");
                exit(EXIT_FAILURE);
            }
            printf("proc_s execution completed.\n");
            exit(EXIT_SUCCESS);
        }
        case -1:
            printf("[Zadanie] Failed to create process T. Exiting...\n");
            perror("S");
            exit(EXIT_FAILURE);
            break;
        default:
            break;
    }
	pause();
#pragma endregion

	pause();
    printf("The end of the program :)\n");
	return EXIT_SUCCESS;
}

#pragma endregion



#pragma region Local_Methods

void create_signals() {
    // create a signal for re-read the first configuration file
	static_cast<void>(signal(SIGUSR1, check_program_status));
    // create a signal for re-read the second configuration file
    static_cast<void>(signal(SIGUSR2, kill_processors));
}

void initialize_semaphores(int sem1, int sem2) {
    // Set initial values of semaphores
    semctl(sem1, 0, SETVAL, 0); // 0, 0
    semctl(sem2, 0, SETVAL, 0); // 0, 0
    semctl(sem1, 1, SETVAL, 0); // 1, 0
    semctl(sem2, 1, SETVAL, 0); // 1, 0
}

void check_program_status(int s) {
	printf("Program still running...\n");
}

void kill_processors(int s) {
    printf("Terminating all processes...\n");
    sleep(10);
    int buffer[8] = { P1, P2, Pr, T, S, D, Serv1, Serv2 }; 

    for (int i = 0; i < 8; i++)
    {
        if (buffer[i] != -1)
        {
            kill(buffer[i], SIGKILL);
        }
    }

    semctl(sem1, 0, IPC_RMID, 0);
    semctl(sem2, 0, IPC_RMID, 0);
    shmctl(shm1, 0, IPC_RMID);
    shmctl(shm2, 0, IPC_RMID);
}

#pragma endregion