// ServerInfo.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable : 4996)
#include "winsock2.h"
#include "ws2tcpip.h"

typedef struct {
	char disk_name[10];
	double capacity;
} DISK;

typedef struct {
	char computer_name[30];
	DISK disks[10];
	int ndisks;
} COMPUTER;


int main(int argc, char** argv)
{
	// define socket
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// define server's address
	SOCKADDR_IN addr;
	// get argument from command line
	if (argc > 1) {
		addr.sin_family = AF_INET;
		printf("Connecting server on IP: %s port: %d \n", (char*)argv[1], atoi((char*)argv[2]));
		addr.sin_addr.s_addr = inet_addr((char*)argv[1]);
		addr.sin_port = htons(atoi((char*)argv[2]));
	}
	else {
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = inet_addr("127.0.0.1");
		addr.sin_port = htons(8000);
	}

	bind(listener, (SOCKADDR *)&addr, sizeof(addr));
	listen(listener, 5);

	printf("Waiting for connection...\n");
	SOCKET client = accept(listener, NULL, NULL);

	printf("Found connecttion...\n");
	// read file and send hello
	char msg[20] = "Hello client\n";
	send(client, msg, strlen(msg), 0);

	char buf[256];
	int ret;
	COMPUTER comp;

	while (1)
	{
		ret = recv(client, buf, sizeof(comp), 0);
		if (ret <= 0)
			break;

		memcpy(&comp, buf, sizeof(comp));

		printf("Data fetched:\n");
		printf("Computer's name: %s\n", comp.computer_name);
		printf("Disks number:  %d\n", comp.ndisks);
		for (int i = 1; i <= comp.ndisks; i++) {
			printf("%d-th disk's information: \n", i);
			printf("Name: %s\n", comp.disks[i].disk_name);
			printf("Capacity: %lf\n", comp.disks[i].capacity);
		}
		printf("\n************************");
	}

	closesocket(client);

	closesocket(listener);
	WSACleanup();

	return 0;
}

