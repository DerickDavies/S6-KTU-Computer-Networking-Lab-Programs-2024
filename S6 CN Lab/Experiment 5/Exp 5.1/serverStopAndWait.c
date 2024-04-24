#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <fcntl.h>
#include <sys/time.h>


int main() {
    char *ip = "127.0.0.1";
    int port = 5566;
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    char buffer[1024];
    char ack[3]; // Acknowledgment frame
    int expecting_sequence_number = 0;
    char str_expecting_sequence_number[3];
    int n;

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("[-]Socket error");
        exit(1);
    }
    printf("[+]TCP server socket created.\n");

    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = port;
    server_addr.sin_addr.s_addr = inet_addr(ip);

    n = bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (n < 0) {
        perror("[-]Bind error");
        exit(1);
    }
    printf("[+]Bind to the port number: %d\n", port);

    listen(server_sock, 5);
    printf("Listening...\n");

    addr_size = sizeof(client_addr);
    client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);
    printf("[+]Client connected.\n");

    // Main loop to check client activity
    fd_set read_fds;
    struct timeval timeout;
    
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    while (1) {
    
        FD_ZERO(&read_fds);
        FD_SET(client_sock, &read_fds);
    
        bzero(buffer, 1024);
        bzero(ack, 3);
        recv(client_sock, buffer, sizeof(buffer), 0);
        recv(client_sock, ack, sizeof(ack), 0);
        sprintf(str_expecting_sequence_number, "%s", expecting_sequence_number);

        // Use select() to check for readability of the client socket
        int ready = select(client_sock + 1, &read_fds, NULL, NULL, &timeout);

        if (ready == -1) {
            perror("[-]Select error");
            break;
        } else if (ready == 0) {
            // Timeout occurred, client is not active
            printf("Client inactive. TimeOut.....\n\n");
        } else {
         
            
            
            
            if (buffer[0] == '0') {
                printf("Exiting\n");
                break;
            } else if (ack == str_expecting_sequence_number){
	        printf("- Received %s\n", buffer);
                // Simulate acknowledgment delay
                sleep(1);

                // Prepare and send acknowledgment to the client
                send(client_sock, ack, sizeof(ack), 0);
                printf("- Sending ACK%s\n", ack);

                expecting_sequence_number++;
                printf("\n");
            }

        }
    }

    // Disconnect present client from server
    close(client_sock);
    printf("[+]Client disconnected.\n\n");
    return 0;
}
