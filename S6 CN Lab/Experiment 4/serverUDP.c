#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h> // for time
#include <sys/wait.h>


int main() {
    int port = 5566, n;
    char *ip = "127.0.0.1";
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    char buffer[1024];
    pid_t child_pid;

    // Create UDP socket
    server_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_sock < 0) {
        perror("Socket error");
        exit(1);
    }

    // Fill server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    // Bind socket to the address and port
    n = bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (n < 0) {
        perror("[-] Bind error");
        exit(1);
    }

    printf("[+] UDP server is listening on port %d...\n", port);
    addr_size = sizeof(client_addr);
    // Receive time request from client
    bzero(buffer, 1024);
    recvfrom(server_sock, buffer, 1024, 0, (struct sockaddr *)&client_addr, &addr_size);
    // Create a child process
    if ((child_pid = fork()) == 0) { // Child process
        printf("[+] Received time request from client server.");
        // Get current time
        time_t current_time;
        // Get current time
        current_time = time(NULL);
        char *time_str = ctime(&current_time);
        // Send time to client
        sendto(server_sock, time_str, strlen(time_str), 0, (struct sockaddr *)&client_addr, addr_size);
        exit(1);
    } else if (child_pid < 0) { // Error
        perror("Fork error");
        exit(1);
    } else {
        wait(NULL);
    }

    // Close the server socket
    close(server_sock);
    printf("\n\n[+] Time request successfully serviced.");
    printf("\nServer closed.\n");
    return 0;
}

