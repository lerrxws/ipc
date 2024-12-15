#include <iostream>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <csignal>
#include <cstring>

int fd, P2;

void handler(int signum);

int main(int argc, char* argv[]) {
    signal(SIGUSR1, handler);
    fd = open("p2.txt", O_RDONLY);

    if (argc > 1) {
        P2 = std::atoi(argv[1]);
    } else {
        std::cerr << "Error: Missing argument" << std::endl;
        return 1;
    }

    std::cout << "P2.cpp SA HLASI KU SLUZBE!" << std::endl;

    kill(getppid(), SIGUSR1);

    while (true) {
        sleep(5);
    }

    return 0;
}

void handler(int signum) {
    char buffer;
    std::string input;
    input.reserve(151); // Reserve memory to avoid frequent allocations

    while (true) {
        read(fd, &buffer, 1); // Read from the file
        if (buffer != '\n') {
            input += buffer;
        } else {
            input += buffer;
            break;
        }
    }

    write(P2, input.c_str(), input.length()); // Write to the pipe
}