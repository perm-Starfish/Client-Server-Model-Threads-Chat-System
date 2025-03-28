#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

void *receive_messages(void *arg) {
    int sock = *(int *)arg;
    char buffer[BUFFER_SIZE];
    while (1) {
        int bytes_received = recv(sock, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
            break;
        }
        buffer[bytes_received] = '\0';
        printf("%s", buffer);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s <server_ip> <port> <username>\n", argv[0]);
        return 1;
    }
    
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr = {AF_INET, htons(atoi(argv[2]))};
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);
    
    connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    send(sock, argv[3], strlen(argv[3]), 0);
    
    pthread_t tid;
    pthread_create(&tid, NULL, receive_messages, &sock);
    
    char message[BUFFER_SIZE];
    while (1) {
        fgets(message, BUFFER_SIZE, stdin);
        send(sock, message, strlen(message), 0);
    }
    return 0;
}