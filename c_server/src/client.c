#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "client.h"

int client_socket;
struct sockaddr_in server_addr;

void close_socket();

int init_client() {
	unsigned short server_port = 10040;
	char * ip_server = "192.168.0.52";

	// Criar Socket
	if((client_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		return 1;

	// Construir struct sockaddr_in
	memset(&server_addr, 0, sizeof(server_addr)); // Zerando a estrutura de dados
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ip_server);
	server_addr.sin_port = htons(server_port);

	return 0;
}

int message(int type, int opt, int on_off, double temp)
{
    char msg[3];

	if(init_client())
    {
        return 1;
    }

	if(connect(client_socket, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
	{
		printf("Cant connect to server \n");
		return -1;
	}

    if(type == 0)
    {
        msg[0] = 0;
        msg[1] = on_off;
        msg[2] = opt;

        if(send(client_socket, (void *) msg, sizeof(char) * 3, 0) < 0)
        {
            printf("Cant send message\n");
            return -2;
        }
    }

    if(type == 1)
    {
        msg[0] = 1;
        msg[1] = on_off;
        msg[2] = opt;

        if(send(client_socket, (void *) msg, sizeof(char) * 3, 0) < 0)
		    return -3;
    }

    if(type == 2)
    {
        msg[0] = 2;
        msg[1] = 0;
        msg[2] = 0;

        if(send(client_socket, (void *) msg, sizeof(char) * 3, 0) < 0)
		    return -3;

        if(send(client_socket, (void *) &temp, sizeof(double), 0) < 0)
		    return -4;
    }

    
	close_socket();

    return 0;
}


void close_socket()
{
	close(client_socket);
}
