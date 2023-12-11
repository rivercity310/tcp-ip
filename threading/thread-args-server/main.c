#include <stdio.h>
#include <ws2tcpip.h>
#include <winsock.h>

#define SERVERPORT 9000
#define BUFSIZE    512

#pragma comment(lib, "ws2_32")

struct client_sock_info {
	SOCKET client_sock;
	char* client_addr;
	int client_port;
};

static DWORD WINAPI MultiThreadTcpServer(LPVOID arg);

int main(int argc, char* argv[]) {
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		exit(0);

	int retval;

	SOCKET listen_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listen_sock == INVALID_SOCKET) exit(0);

	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(SERVERPORT);
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	retval = bind(listen_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) exit(0);

	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) exit(0);

	SOCKET client_sock;
	struct sockaddr_in clientaddr;
	struct client_sock_info sockinfo;
	int addrlen;
	HANDLE hthread;

	while (1) {
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) exit(0);

		memset(&(sockinfo.client_addr), 0, sizeof(INET_ADDRSTRLEN));
		memset(&sockinfo, 0, sizeof(sockinfo));
		sockinfo.client_addr = ntohl(clientaddr.sin_addr.s_addr);
		sockinfo.client_port = ntohs(clientaddr.sin_port);
		sockinfo.client_sock = client_sock;

		hthread = CreateThread(NULL, 0, MultiThreadTcpServer, &sockinfo, 0, NULL);
		if (hthread == NULL) closesocket(client_sock);
		else CloseHandle(hthread);
	}

	closesocket(listen_sock);
	WSACleanup();
	return 1;
}

static DWORD WINAPI MultiThreadTcpServer(LPVOID arg) {
	struct client_sock_info* sockinfo = (struct client_sock_info*)arg;
	SOCKET sock = sockinfo->client_sock;
}