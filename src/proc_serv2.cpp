#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#pragma region Configuration
const int BUF_S = 200; // buffer's size for reading and writing file contents
#pragma endregion

// local methods
int get_exit(bool exit_result,const char *error_message); // method that returns the correct output
void send_signal_to_process(int port_number, int socket); // method for sending a process signal

#pragma region Main_Method

int main(int argc, char* argv[]) {
    printf("Server 2 is running...\n");

    if(argc < 2) {
        get_exit(false, "Arguments number error\n");
        //printf("Arguments number error\n");
        //exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);
    if (port == 0) { 
        get_exit(false, "Port number error\n");
    }

    int socket_d = socket(AF_INET, SOCK_DGRAM, 0);
    if(socket_d == -1) {
        get_exit(false, "Server 2 socket() error\n");
    }

    send_signal_to_process(port, socket_d);

    printf("Openning file...\n");
    int file = open("serv2.txt", O_WRONLY | O_TRUNC | O_CREAT, 0777);
    switch (file)
    {
        case -1:
            printf("[Proc_Serv2] Error in openning file serv2.txt");
            perror("open");
            break;
        default:
        {
            printf("Copying data...\n");

            char buf[BUF_S]; // buffer for reading and writing file contents
            for (int i = 0; i < 10; i++) {
                // try to read and write data
                if(recv(socket_d, buf, BUF_S, 0) < 0 || write(file, buf, strlen(buf)) < 0){
                    get_exit(false, "Copying data error\n");
                }
                write(file, "\n", 1);
            }

            // end preparation
            printf("Sending signal to a process...\n");
            break;
        }
    }

    // end preparation
    kill(getppid(), SIGUSR2);
    get_exit(true, "");

    return 0;
}

#pragma endregion



#pragma region Local_Methods

int get_exit(bool exit_result, const char *error_message) {
    if (exit_result == true){
        exit(EXIT_SUCCESS);
    } else {
        // error message
        fprintf(stderr,"[./proc_serv2] %s", error_message);
        exit(EXIT_FAILURE);
    }
}

void send_signal_to_process(int port_number, int socket)
{
    // fill in the server with relevant information
    struct sockaddr_in client;
    //Smemset(&client, 0, sizeof(client));

    client.sin_family = AF_INET;          // set IPv4 protocol
    client.sin_addr.s_addr = INADDR_ANY;  // binding to any available network interface
    client.sin_port = htons(port_number); // set the port number and convert it to network byte order

    if (bind(socket, (struct sockaddr *)&client, sizeof(struct sockaddr)) < 0) {
        perror("bind");
        get_exit(false, "Bind work error\n");
    }

    // Do not bind for UDP sockets

    printf("Sending signal to a process...\n");
    kill(getppid(), SIGUSR1);
}
#pragma endregion