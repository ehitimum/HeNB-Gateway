#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/sctp.h>

#define CLIENT_PORT 36412      // Port for clients to connect
#define MME_PORT 36412         // Port to connect to MME
#define MME_ADDRESS "192.168.220.9"
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

int mme_fd;  // Global MME connection (shared by threads, can be thread-safe with mutex)

void *handle_client(void *arg) {
    int client_fd = *(int *)arg;
    char buffer[BUFFER_SIZE];
    
    free(arg); // Free memory allocated for client_fd

    // Receive data from the client
    int bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        printf("Received from client: %s\n", buffer);

        // Forward the data to the MME
        send(mme_fd, buffer, bytes_received, 0);

        // Receive response from the MME
        int bytes_from_mme = recv(mme_fd, buffer, BUFFER_SIZE, 0);
        if (bytes_from_mme > 0) {
            buffer[bytes_from_mme] = '\0';
            printf("Received from MME: %s\n", buffer);

            // Send the response back to the client
            send(client_fd, buffer, bytes_from_mme, 0);
        }
    }

    //close(client_fd); // Close client connection
    //return NULL;
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr, mme_addr;
    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(client_addr);
    pthread_t thread_id;

    // Step 1: Connect to the MME server
    mme_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP);
    if (mme_fd < 0) {
        perror("MME socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&mme_addr, 0, sizeof(mme_addr));
    mme_addr.sin_family = AF_INET;
    mme_addr.sin_port = htons(MME_PORT);
    if (inet_pton(AF_INET, MME_ADDRESS, &mme_addr.sin_addr) <= 0) {
        perror("Invalid MME address");
        close(mme_fd);
        exit(EXIT_FAILURE);
    }

    if (connect(mme_fd, (struct sockaddr *)&mme_addr, sizeof(mme_addr)) < 0) {
        perror("Failed to connect to MME");
        close(mme_fd);
        exit(EXIT_FAILURE);
    }

    printf("Connected to MME at %s:%d\n", MME_ADDRESS, MME_PORT);

    // Step 2: Set up the SCTP server for client connections
    server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP);
    if (server_fd < 0) {
        perror("Client socket creation failed");
        close(mme_fd);
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(CLIENT_PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        close(mme_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        close(server_fd);
        close(mme_fd);
        exit(EXIT_FAILURE);
    }

    printf("Listening for client connections on port %d...\n", CLIENT_PORT);

    // Step 3: Accept and handle client connections
    while (1) {
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
        if (client_fd < 0) {
            perror("Accept failed");
            continue;
        }

        printf("New client connected from %s:%d\n",
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // Create a thread for the new client connection
        int *client_fd_ptr = malloc(sizeof(int));
        *client_fd_ptr = client_fd;

        if (pthread_create(&thread_id, NULL, handle_client, client_fd_ptr) != 0) {
            perror("Thread creation failed");
            close(client_fd);
        } else {
            pthread_detach(thread_id);  // Detach thread to clean up resources when done
        }
    }

    close(server_fd);
    close(mme_fd);
    return 0;
}
