#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int client_sockets[MAX_CLIENTS];
char *client_names[MAX_CLIENTS];

void broadcast_message(char *message) {
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_sockets[i] != 0) {
            send(client_sockets[i], message, strlen(message), 0);
        }
    }
    pthread_mutex_unlock(&mutex);
}

void *handle_client(void *arg) {
    int client_socket = *(int *)arg;
    char buffer[BUFFER_SIZE];
    char username[50];
    recv(client_socket, username, sizeof(username), 0);
    
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_sockets[i] == 0) {
            client_sockets[i] = client_socket;
            client_names[i] = strdup(username);
            break;
        }
    }
    pthread_mutex_unlock(&mutex);
    
    sprintf(buffer, "<User %s is on-line.>\n", username);
    broadcast_message(buffer);
    
    while (1) {
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
            break;
        }
        buffer[bytes_received] = '\0';
        broadcast_message(buffer);
    }
    
    close(client_socket);
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_sockets[i] == client_socket) {
            sprintf(buffer, "<User %s is off-line.>\n", client_names[i]);
            free(client_names[i]);
            client_sockets[i] = 0;
            client_names[i] = NULL;
            break;
        }
    }
    pthread_mutex_unlock(&mutex);
    
    broadcast_message(buffer);
    return NULL;
}

int main() {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr = {AF_INET, htons(1234), INADDR_ANY};
    bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_socket, MAX_CLIENTS);
    
    printf("Server listening on port 1234...\n");
    while (1) {
        int client_socket = accept(server_socket, NULL, NULL);
        pthread_t tid;
        pthread_create(&tid, NULL, handle_client, &client_socket);
    }
    return 0;
}