#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "server.h"

int server_sock;
int client_sock;
struct sockaddr_in server_addr;
struct sockaddr_in client_addr;
unsigned short port;

int init_server()
{

	port = 10031;

	// Abrir Socket
	if((server_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        fprintf(stderr, "Error creating server socket\n");
        return -1;
    }

	// Montar a estrutura sockaddr_in
	memset(&server_addr, 0, sizeof(server_addr)); // Zerando a estrutura de dados
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(port);

	// Bind
	if(bind(server_sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
    {
        fprintf(stderr, "Error in bind\n");
        return -2;
    }

	// Listen
	if(listen(server_sock, 2) < 0)
	{
        fprintf(stderr, "Error in listen\n");
        return -3;
    }		

    return 0;
}

void connection_handler(double * H, double * T, int lamp[], int ac[], int sp[], int so[])
{
    unsigned int clen = sizeof(client_addr);
    int opt = 0;

    while((client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &clen)))
    {
        int rcv_size;

        // if((rcv_size = recv(client_sock, opt, sizeof(opt), 0)) < 0)
        // {
        //     send(client_sock, 1, sizeof(int), 0);
        // }

        if(opt == 0)
        {
            if((rcv_size = recv(client_sock, H, sizeof(double), 0)) < 0)
            {
                send(client_sock, 1, sizeof(int), 0);
            }

            if((rcv_size = recv(client_sock, T, sizeof(double), 0)) < 0)
            {
                send(client_sock, 1, sizeof(int), 0);
            }

            if((rcv_size = recv(client_sock, lamp, sizeof(int) * 4, 0)) < 0)
            {
                send(client_sock, 1, sizeof(int), 0);
            }

            if((rcv_size = recv(client_sock, ac, sizeof(int) * 2, 0)) < 0)
            {
                send(client_sock, 1, sizeof(int), 0);
            }

            if((rcv_size = recv(client_sock, sp, sizeof(int) * 2, 0)) < 0)
            {
                send(client_sock, 1, sizeof(int), 0);
            }

            if((rcv_size = recv(client_sock, so, sizeof(int) * 6, 0)) < 0)
            {
                send(client_sock, 1, sizeof(int), 0);
            }
        }

        else
        {
            printf("a\n");
        }

        close(client_sock);
    }
}

void close_sockets()
{
    close(server_sock);
}