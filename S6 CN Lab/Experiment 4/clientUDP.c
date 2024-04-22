#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>


int main() {
    int port = 5566;
    char *ip = "127.0.0.1";
    char request_msg[] = "TimeRequest";
    int client_sock;
    struct sockaddr_in server_addr;
    socklen_t addr_size;
    char buffer[1024];

    // Create UDP socket
    client_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_sock < 0) {
        perror("[-] Socket error");
        exit(1);
    }
    printf("[+] UDP client socket created.\n");
    // Fill server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    // Send time request to server
    printf("[+] Requesting for time....\n\n");
    sendto(client_sock, request_msg, strlen(request_msg), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));

    // Receive time from server
    addr_size = sizeof(server_addr);
    bzero(buffer, 1024);
    recvfrom(client_sock, buffer, 1024, 0, (struct sockaddr *)&server_addr, &addr_size);

    // Display received time
    printf("Server time: %s\n", buffer);

    // Close the client socket
    close(client_sock);
    printf("[+] Closing client server...\n");
    return 0;
}

