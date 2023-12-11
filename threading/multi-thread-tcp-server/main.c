#include <stdio.h>
#include <ws2tcpip.h>
#include <winsock2.h>

#define SERVERPORT 9000
#define BUFSIZE    512

#pragma comment(lib, "ws2_32")

static DWORD WINAPI ProcessClient(LPVOID arg);

int main() {
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;

	SOCKET listen_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listen_sock == INVALID_SOCKET) return 1;

	int retval;

	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(SERVERPORT);
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	retval = bind(listen_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) return 1;

	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) return 1;

	SOCKET client_sock;
	struct sockaddr_in clientaddr;
	int addrlen;
	HANDLE hThread;

	while (1) {
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) return 1;

		char addr[INET_ADDRSTRLEN];
		inet_ntop(PF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
		printf("\n[TCP Server] 클라이언트 접속 %s:%d\n", addr, ntohs(clientaddr.sin_port));

		hThread = (HANDLE)_beginthreadex(NULL, 0, ProcessClient, &client_sock, 0, NULL);
		if (hThread == NULL) closesocket(client_sock);
		else CloseHandle(hThread);
	}

	closesocket(listen_sock);
	
	WSACleanup();
	return 0;
}

static DWORD WINAPI ProcessClient(LPVOID arg) {
	SOCKET client_sock = *(SOCKET*)arg;
	int retval;
	struct sockaddr_in clientaddr;
	char addr[INET_ADDRSTRLEN];
	int addrlen;
	char buf[BUFSIZE + 1];
	
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (struct sockaddr*)&clientaddr, &addrlen);
	inet_ntop(PF_INET, &clientaddr.sin_addr, addr, sizeof(addr));

	while (1) {
		retval = recv(client_sock, buf, BUFSIZE, 0);
		if (retval == SOCKET_ERROR) return 1;
		if (retval == 0) break;

		buf[retval] = '\0';
		printf("[TCP/%s:%d] %s\n", addr, ntohs(clientaddr.sin_port), buf);

		retval = send(client_sock, buf, retval, 0);
		if (retval == SOCKET_ERROR) return 1;
	}

	closesocket(client_sock);
	printf("[TCP Server] 클라이언트 종료 %s:%d\n", addr, ntohs(clientaddr.sin_port));
	return 0;
}