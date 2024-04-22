#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define ACK_SIZE 3 // Size of ACK frame, adjust as needed

int main() {
    char *ip = "127.0.0.1";
    int port = 5566;

    int sock;
    struct sockaddr_in addr;
    socklen_t addr_size;
    char buffer[1024];
    char ack[ACK_SIZE]; // Acknowledgment frame
    int message_number = 1;
    int n;
    int active = 0;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("[-]Socket error");
        exit(1);
    }
    printf("[+]TCP client socket created.\n");

    memset(&addr, '\0', sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = port;
    addr.sin_addr.s_addr = inet_addr(ip);

    connect(sock, (struct sockaddr*)&addr, sizeof(addr));
    printf("Connected to the server.\n");
    
    fd_set read_fds;
    struct timeval timeout;

    FD_ZERO(&read_fds);
    FD_SET(sock, &read_fds);

    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    while (1) {
    
        // Use select() to check for readability of the socket
        int ready = select(sock + 1, &read_fds, NULL, NULL, &timeout);

        if (ready == -1) {
            perror("[-]Select error");
            break;
        } else if (ready == 0) {
            // Timeout occurred, client is not receiving acknowledgment
            printf("Server acknowledgment not received. Resending message.\n\n");
            continue; // Resend the last message
        } else {

            if (message_number == 4 && active == 0) {
                printf("Client:\n- Send %d \n", message_number);
                sleep(5);
                printf("Timeout... Resending \n\n");
                active = 1;
            }
            else if (message_number < 6){
                bzero(buffer, 1024);
                printf("Client:\n- Send %d\n", message_number);
                sprintf(buffer, "%d", message_number);

                // Send frame to the server
                send(sock, buffer, strlen(buffer), 0);
            }
            else {
                bzero(buffer, 1024);
                printf("Client:\n- Stop\n");
                sprintf(buffer, "Stop");
                send(sock, buffer, strlen(buffer), 0);
                break;
            }
            // Socket is active, handle received acknowledgments as before
            bzero(ack, ACK_SIZE);
            recv(sock, ack, sizeof(ack), 0);
            printf("- Received ACK%s\n\n", ack);

            message_number++;
            
        }
    }
    close(sock);
    printf("Disconnected from the server.\n");
    return 0;
}

