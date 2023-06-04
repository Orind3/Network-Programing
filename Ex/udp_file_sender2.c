#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_SIZE 1024
#define MAX_SENDERS 10

int main(int argc, char *argv[]) {
    int sock_fd, port, sender_id, file_size, num_senders, num_received;
    struct sockaddr_in server_addr, sender_addrs[MAX_SENDERS];
    char buffer[MAX_SIZE], filename[MAX_SIZE], *ptr;
    FILE *file;

    if (argc < 3) {
        printf("Usage: %s <port> <num_senders>\n", argv[0]);
        return 1;
    }

    port = atoi(argv[1]);
    num_senders = atoi(argv[2]);

    // Create socket
    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd < 0) {
        perror("Error creating socket");
        return 1;
    }

    // Bind to port
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    if (bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error binding socket");
        close(sock_fd);
        return 1;
    }

    // Receive data
    num_received = 0;
    while (num_received < num_senders) {
        memset(buffer, 0, sizeof(buffer));
        if (recvfrom(sock_fd, buffer, sizeof(buffer), 0, NULL, NULL) < 0) {
            perror("Error receiving data");
            close(sock_fd);
            return 1;
        }

        ptr = strtok(buffer, ",");
        sender_id = atoi(ptr);

        ptr = strtok(NULL, ",");
        file_size = atoi(ptr);

        ptr = strtok(NULL, ",");
        strcpy(filename, ptr);

        // Check if sender ID is valid
        if (sender_id < 0 || sender_id >= num_senders) {
            printf("Invalid sender ID: %d\n", sender_id);
            continue;
        }

        // Save sender address
        if (num_received == 0) {
            memcpy(&sender_addrs[sender_id], &server_addr, sizeof(server_addr));
        }

        // Open file for writing
        if (num_received == 0) {
            file = fopen(filename, "wb");
        } else {
            file = fopen(filename, "ab");
        }

        // Receive file data
        while (recvfrom(sock_fd, buffer, sizeof(buffer), 0, NULL, NULL) > 0) {
            ptr = strtok(buffer, ",");
            sender_id = atoi(ptr);

            ptr = strtok(NULL, ",");
            int seq_num = atoi(ptr);

            if (sender_id < 0 || sender_id >= num_senders || seq_num < 0) {
                printf("Invalid sender ID or sequence number\n");
                continue;
            }

            // Write data to file
            fwrite(ptr, strlen(ptr), 1, file);

            // Send ACK to sender
            sprintf(buffer, "%d,%d", sender_id, seq_num);
            if (sendto(sock_fd, buffer, strlen(buffer), 0, (struct sockaddr *)&sender_addrs[sender_id], sizeof(sender_addrs[sender_id])) < 0) {
                perror("Error sending ACK");
                close(sock_fd);
                fclose(file);
                return 1;
            }
        }
        fclose(file);
        num_received++;
    }

    printf("All files received successfully\n");

    close(sock_fd);

    return 0;
}