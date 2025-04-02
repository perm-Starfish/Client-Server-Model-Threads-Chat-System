#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024
#define MAX_NAME 50

void *receive_messages(void *arg)
{
    int sock = *(int *)arg;
    char buffer[BUFFER_SIZE];
    while (1)
    {
        int bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0)
        {
            printf("Disconnected from server.\n");
            break;
        }
        buffer[bytes_received] = '\0';
        printf("%s", buffer);
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("Usage: %s <server_ip> <port> <username>\n", argv[0]);
        return 1;
    }
    
    if (strlen(argv[3]) >= MAX_NAME)
    {
        printf("Username too long. Max length: %d\n", MAX_NAME - 1);
        return 1;
    }
    
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("Socket creation failed");
        return 1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) <= 0)
    {
        perror("Invalid address");
        close(sock);
        return 1;
    }
    
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Connection failed");
        close(sock);
        return 1;
    }
    
    // send username to server
    if (send(sock, argv[3], strlen(argv[3]), 0) < 0)
    {
        perror("Failed to send username");
        close(sock);
        return 1;
    }
    
    pthread_t tid;
    pthread_create(&tid, NULL, receive_messages, &sock);
    
    char message[BUFFER_SIZE];
    while (1)
    {
        fgets(message, BUFFER_SIZE, stdin);
        
        if (strncmp(message, "bye", 3) == 0)
        {
            send(sock, "bye", 3, 0);
            break;
        }
        
        if (send(sock, message, strlen(message), 0) < 0)
        {
            perror("Failed to send message");
            break;
        }
    }
    close(sock);
    
    return 0;
}
