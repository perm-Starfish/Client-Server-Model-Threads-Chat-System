#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_CLIENTS 10
#define MAX_NAME 100
#define MAX_MSG_LEN 1024
#define BUFFER_SIZE 1024

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// define client information
typedef struct {
    int socket;
    struct sockaddr_in address;
    char name[MAX_NAME];
    int active;
} client_t;

client_t clients[MAX_CLIENTS];  // list of clients

void broadcast_message(const char *message)
{
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < MAX_CLIENTS; i++)
        if (clients[i].active)
			send(clients[i].socket, message, strlen(message), 0);
    pthread_mutex_unlock(&mutex);
}

void print_time()
{
    time_t now;
    struct tm *timeinfo;
    char timebuffer[20]; // yyyy-mm-dd hh:mm:ss
    
    time(&now);
    timeinfo = localtime(&now);
    
    strftime(timebuffer, sizeof(timebuffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    printf("[%s]\n", timebuffer);
}

void *handle_client(void *arg)
{
    client_t *client = (client_t *)arg;
    char buffer[BUFFER_SIZE];
    int bytes_received;

    // client name
    bytes_received = recv(client->socket, client->name, sizeof(client->name)-1, 0);
    if (bytes_received <= 0)
	{
        close(client->socket);
        client->active = 0;
        return NULL;
    }
    client->name[bytes_received] = '\0';

    // broadcast new active user
    sprintf(buffer, "<User %s is on-line, socket address: %s/%i>\n",
            client->name, inet_ntoa(client->address.sin_addr), ntohs(client->address.sin_port));
    broadcast_message(buffer);

    while (1)
	{
        bytes_received = recv(client->socket, buffer, sizeof(buffer)-1, 0);
        if (bytes_received <= 0) break;
        
        buffer[bytes_received] = '\0';

        if (strncmp(buffer, "chat ", 5) == 0)
		{
            char *message_start = strchr(buffer + 5, ' ');
            if (message_start)
			{
                *message_start = '\0';
                char *recipient = buffer + 5;
                char *message = message_start + 1;

                pthread_mutex_lock(&mutex);
                int found = 0;
                for (int i = 0; i < MAX_CLIENTS; i++)
				{
                    if (strcmp(clients[i].name, recipient) == 0)
					{
						found = 1;
						if (clients[i].active)
						{
	                        char sender_msg[MAX_MSG_LEN];
	                        snprintf(sender_msg, sizeof(sender_msg), "<To %s> %s\n", recipient, message);
	                        send(client->socket, sender_msg, strlen(sender_msg), 0);
	                        
	                        char rcvr_msg[MAX_MSG_LEN];
	                        snprintf(rcvr_msg, sizeof(rcvr_msg), "<From %s> %s\n", client->name, message);
	                        send(clients[i].socket, rcvr_msg, strlen(rcvr_msg), 0);
	                        
	                        break;
	                	}
	                	else
	                    {
	                    	char offline_msg[MAX_MSG_LEN];
	                    	sprintf(offline_msg, "<User %s is off-line.>\n", recipient);
	                    	send(client->socket, offline_msg, strlen(offline_msg), 0);
						}
	                }
                }
                if (!found)
                {
                	char dne_msg[MAX_MSG_LEN];
	                sprintf(dne_msg, "<User %s does not exist.>\n", recipient);
	                send(client->socket, dne_msg, strlen(dne_msg), 0);
				}
                pthread_mutex_unlock(&mutex);
            }
        }
		else if (strncmp(buffer, "bye", 3) == 0) break;
		else send(client->socket, "Invalid input.\n", 16, 0);
    }

    // user off-line
    close(client->socket);
    pthread_mutex_lock(&mutex);
    client->active = 0;
    sprintf(buffer, "<User %s is off-line.>\n", client->name);
    pthread_mutex_unlock(&mutex);

    broadcast_message(buffer);
    return NULL;
}

int main()
{
    struct sockaddr_in server, client;
    socklen_t sockaddr_size = sizeof(struct sockaddr_in);

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(1234);
    server.sin_addr.s_addr = INADDR_ANY;

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)
	{
        perror("Socket creation failed");
        return 1;
    }

    if (bind(server_socket, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
        perror("Bind failed");
        close(server_socket);
        return 1;
    }

    if (listen(server_socket, MAX_CLIENTS) < 0)
	{
        perror("Listen failed");
        close(server_socket);
        return 1;
    }

    printf("Server listening on port 1234...\n");

    while (1)
	{
        int new_socket = accept(server_socket, (struct sockaddr *)&client, &sockaddr_size);
        if (new_socket < 0)
		{
            perror("Accept failed");
            continue;
        }

        // find available client slot
        pthread_mutex_lock(&mutex);
        int i;
        for (i = 0; i < MAX_CLIENTS; i++)
		{
            if (!clients[i].active)
			{
                clients[i].socket = new_socket;
                clients[i].address = client;
                clients[i].active = 1;
                break;
            }
        }
        pthread_mutex_unlock(&mutex);

        if (i == MAX_CLIENTS)
		{
            send(new_socket, "Server is full.\n", 16, 0);
            close(new_socket);
        }
		else
		{
            pthread_t tid;
            pthread_create(&tid, NULL, handle_client, &clients[i]);
            pthread_detach(tid);
        }
    }
    close(server_socket);
    
    return 0;
}
