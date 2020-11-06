#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "client.h"
#include "gpio.h"

int client_socket;
struct sockaddr_in server_addr;

int init_client() {
	unsigned short server_port = 10031;
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

int message(double * H, double * T, int lamp[], int ac[], int sp[], int so[])
{

	int opt = 0;
	int error;
	
	init_client();

	if(connect(client_socket, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
	{
		printf("Cant connect to server \n");
		return 2;
	}

	// if((error = send(client_socket, (void *) opt, sizeof(int), 0)) < 0)
	// {
	// 	printf("%d\n", error);
	// 	return -8;
	// }
		

    if(send(client_socket, (void *) H, sizeof(double), 0) < 0)
		return -1;

    if(send(client_socket, (void *) T, sizeof(double), 0) < 0)
		return -2;

	if(send(client_socket, (void *) lamp, sizeof(int) * 4, 0) < 0)
		return -3;

	if(send(client_socket, (void *) ac, sizeof(int) * 2, 0) < 0)
		return -4;		

    if(send(client_socket, (void *) sp, sizeof(int) * 2, 0) < 0)
		return -5;
	
	if(send(client_socket, (void *) so, sizeof(int) * 6, 0) < 0)
		return -6;

	// printf("info sended\n");

	close_socket();

    return 0;
}

int sensor_message(char sensor_data[])
{
	if(init_client())
    {
        return 1;
    }

	if(connect(client_socket, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
	{
		printf("Cant connect to server \n");
		return -1;
	}

	if(send(client_socket, (void *) sensor_data, sizeof(sensor_data), 0) < 0)
		return -2;

	close_socket();

	return 0;
}

void close_socket()
{
	close(client_socket);
}