// TelNetServerUsingSelect.cpp : Defines the entry point for the console application.
//

// TelnetServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "winsock2.h"
#include "stdlib.h"
#include "stdio.h"

CRITICAL_SECTION CriticalSection;

SOCKET clients[64];
char clientsID[64][100];
int numClients = 0;

void deleteClient(int i) {
	clients[i] = clients[numClients - 1];
	strcpy(clientsID[i], clientsID[numClients - 1]);
	strcpy(clientsID[numClients - 1], "");
	numClients--;
}

int main(int argc, char** argv)
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	if (argc > 1) {
		addr.sin_port = htons(atoi((char*)argv[1]));
	}
	else {
		addr.sin_port = htons(5000);
	}
	bind(listener, (SOCKADDR *)&addr, sizeof(addr));
	listen(listener, 8);

	fd_set fdread;
	int ret;
	SOCKET client;
	char buf[256];

	for (int i = 0; i < 64; i++) memcpy(clientsID[i], "", 1);
	printf("Telnet Server\n");

	while (1)
	{
		FD_ZERO(&fdread);
		FD_SET(listener, &fdread);
		for (int i = 0; i < numClients; i++)
			FD_SET(clients[i], &fdread);

		ret = select(0, &fdread, NULL, NULL, NULL);
		if (ret == SOCKET_ERROR) {
			return 1;
		}
		if (ret > 0) {
			if (FD_ISSET(listener, &fdread)) {
				client = accept(listener, NULL, NULL);
				clients[numClients] = client;
				numClients++;
				printf("Client connected: %d\n", client);
				send(client, "Username: ", 11, 0);
			}
			for (int i = 0; i < numClients; i++) {
				if (FD_ISSET(clients[i], &fdread)) {
					// if client didn't login
					if (!strcmp("", clientsID[i])) {
						ret = recv(clients[i], buf, sizeof(buf), 0);
						if (ret <= 0) {
							deleteClient(i);
							return -1;
						}	
						buf[ret] = 0;
						char username[31];
						memcpy(username, &buf, ret - 1);
						username[ret - 1] = '\0';
						send(client, "Password: ",11, 0);
						ret = recv(clients[i], buf, sizeof(buf), 0);
						if (ret <= 0)
							return -1;
						buf[ret] = 0;
						char password[31];
						memcpy(password, &buf, ret - 1);
						password[ret-1] = '\0';
						char filename[] = "c:\\Users\\USER\\source\\repos\\NetworkProgramming_ChatServerUsingSelect\\TelNetServerUsingSelect\\db.txt";
						//open and get the file handle
						InitializeCriticalSection(&CriticalSection);
						EnterCriticalSection(&CriticalSection);
						FILE* fh;
						fopen_s(&fh, filename, "r");
						//check if file exists
						if (fh == NULL) {
							printf("File does not exists");
							return 0;
						}
						//read line by line
						const size_t line_size = 255;
						char* line = (char*)malloc(line_size);
						while (fgets(line, line_size, fh) != NULL) {
							int len = strlen(line);
							for (int j = 0; j < len; j++) {
								if (line[j] == 32) {
									char c_username[31];
									memcpy(c_username, &line[0], j);
									c_username[j] = '\0';
									char c_password[31];
									memcpy(c_password, &line[j+1], len-j-2);
									c_password[len-j-2] = '\0';
									if (strcmp(username, c_username) == 0 && strcmp(password, c_password) == 0) {
										memcpy(clientsID[i], username, strlen(username) + 1);
										break;
									}
								}
							}
						}
						// free(fh);
						free(line);
						LeaveCriticalSection(&CriticalSection);
						if (!strcmp("", clientsID[i])) {
							send(clients[i], "Login error, Username: ", 24, 0);
						}
						else {
							send(clients[i], "Command: ", 9, 0);
						}
					}
					else {
						// client logged in
						InitializeCriticalSection(&CriticalSection);
						EnterCriticalSection(&CriticalSection);
						ret = recv(client, buf, sizeof(buf), 0);
						if (ret <= 0)
							return -1;
						buf[ret - 1] = 0;
						strcat(buf, " > c:\\Users\\USER\\source\\repos\\NetworkProgramming_ChatServerUsingSelect\\TelNetServerUsingSelect\\output.txt");
						printf("%s", buf);
						system(buf);
						send(clients[i], "Command: ", 9, 0);
						LeaveCriticalSection(&CriticalSection);
					}
				}
			}
		}
	}

	closesocket(client);
	closesocket(listener);
	WSACleanup();
	return 0;
}