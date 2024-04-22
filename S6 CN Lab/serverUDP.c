#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    char *ip = "127.0.0.1";
    char Eexit[10] = "Exit";
    int port = 5566;

    int server_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    char buffer[1024];
    int n;

    // Setup socket
    // AF_INET -> for IPV4
    // SOCK_DGRAM -> UDP
    // 0 means 2 way commn. protocol
    server_sock = socket(AF_INET, SOCK_DGRAM, 0);
    // if err, throw err
    if (server_sock < 0) {
        // to print error message
        perror("[-]Socket error");
        exit(1);
    }

    printf("[+]UDP server socket created.\n");

    // to reset server_addr variable
    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    // used for converting ip address to binary format. provided by arpa/inet.h
    server_addr.sin_addr.s_addr = inet_addr(ip);

    // Bind the socket descriptor to the server address:
    n = bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (n < 0) {
        perror("[-]Bind error");
        exit(1);
    }

    printf("[+]Bind to the port number: %d\n", port);

    while (1) {
        addr_size = sizeof(client_addr);

        // Receive message and store in buffer variable
        bzero(buffer, 1024);
        recvfrom(server_sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, &addr_size);

        printf("Client: %s", buffer);
        if (!strcmp(Eexit, buffer)) {
            printf("Exiting");
            break;
        }

        bzero(buffer, 1024);
        printf("Server: ");
        fgets(buffer, 1000, stdin);

        // Send message to the client
        sendto(server_sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, sizeof(client_addr));
    }

    // Close the server socket
    close(server_sock);
    printf("[+]Server closed.\n\n");
    return 0;
}

