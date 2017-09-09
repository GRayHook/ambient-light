// ambient_agent.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "LogitechLEDLib.h"
#define MY_PORT    51117


int main()
{
	printf("Started!\n");

	LogiLedInit();

	LogiLedSetLighting(100, 0, 0);

	char buff[1024];
	if (WSAStartup(0x0202, (WSADATA *)&buff[0]))
	{
		printf("Error WSAStartup %d\n",
			WSAGetLastError());
		return -1;
	}

	SOCKET mysocket;
	if ((mysocket = socket(AF_INET, SOCK_STREAM, 0))<0)
	{
		// Ошибка!
		printf("Error socket %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}

	sockaddr_in local_addr;
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(MY_PORT);

	local_addr.sin_addr.s_addr = 0;

	if (bind(mysocket, (sockaddr *)&local_addr,
		sizeof(local_addr)))
	{
		printf("Error bind %d\n", WSAGetLastError());
		closesocket(mysocket);
		WSACleanup();
		return -1;
	}

	if (listen(mysocket, 0x100))
	{
		printf("Error listen %d\n", WSAGetLastError());
		closesocket(mysocket);
		WSACleanup();
		return -1;
	}

	printf("Waiting...\n");

	SOCKET client_socket;  
	sockaddr_in client_addr; 
	
	int client_addr_size = sizeof(client_addr);

	while (1)
	{
		client_socket = accept(mysocket, (sockaddr *)&client_addr, &client_addr_size);
		while (1)
		{
			int colors[3];
			int bytes_recv = recv(client_socket, (char *)&colors, sizeof(int) * 3, 0);
			if (bytes_recv <= 0)
			{
				break;
			}
			printf("recvd: %d - %d - %d\n", colors[0], colors[1], colors[2]);

			int brigthness = 0.299 * colors[0] + 0.587 * colors[1] + 0.114 * colors[2];

			/*colors[0] = colors[0] + 10;
			colors[1] = colors[1] + 0.413 * (63 - brigthness) / 2;
			colors[2] = colors[2] + 0.886 * (63 - brigthness) / 2;
			printf("correction: %d - %d - %d\n", colors[0], colors[1], colors[2]);*/
			
			colors[0] = colors[0] * 100 / 63;
			colors[1] = colors[1] * 100 / 63;
			colors[2] = colors[2] * 100 / 63;
			printf("to100%: %d - %d - %d\n", colors[0], colors[1], colors[2]);

			LogiLedSetLighting(colors[0], colors[1], colors[2]);
		}
		closesocket(client_socket);
	}

	LogiLedShutdown();

	return 0;
}

