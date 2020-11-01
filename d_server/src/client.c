#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "tcp_client.h"

int client_socket;
struct sockaddr_in server_addr;

int init_client() {
	unsigned short server_port = 10023;
	char * ip_server = "192.168.0.53";

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

int message(float * H, float * T, int sp[], int so[])
{
	if(init_client())
    {
        return 1;
    }

	if(connect(client_socket, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
		return 2;

    if(send(client_socket, (void *) H, sizeof(float), 0) < 0)
		return -1;

    if(send(client_socket, (void *) T, sizeof(float), 0) < 0)
		return -2;

    if(send(client_socket, (void *) sp, sizeof(int) * 2, 0) < 0)
		return -3;
	
	if(send(client_socket, (void *) so, sizeof(int) * 6, 0) < 0)
		return -4;

	close_socket();

    return 0;
}

void close_socket(){
	close(client_socket);
}