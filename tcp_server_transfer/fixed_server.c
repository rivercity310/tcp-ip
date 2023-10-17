#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#define SERVERPORT 9000
#define SERVERIP   "::1"
#define BUFSIZE    50

#pragma comment(lib, "ws2_32")

void FixedTcp6Server() {
	SOCKET sock = socket(PF_INET6, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) exit(1);

	int retval;

	struct sockaddr_in6 serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin6_family = AF_INET6;
	serveraddr.sin6_port = htons(SERVERPORT);
	serveraddr.sin6_addr = in6addr_any;

	retval = bind(sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) exit(1);

	retval = listen(sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) exit(1);

	SOCKET clientsock;
	struct sockaddr_in6 clientaddr;
	int addrlen;
	char buf[BUFSIZE + 1];

	while (1) {
		addrlen = sizeof(clientaddr);
		clientsock = accept(sock, (struct sockaddr*)&clientaddr, &addrlen);
		if (clientsock == INVALID_SOCKET) exit(1);

		// 접속 클라이언트 정보 출력
		char addr[INET6_ADDRSTRLEN];
		inet_ntop(AF_INET6, &clientaddr.sin6_addr, addr, sizeof(addr));
		printf("[TCP SERVER] 클라이언트 접속: IP = %s, PORT = %d\n", addr, ntohs(clientaddr.sin6_port));

		// 데이터 통신
		while (1) {
			retval = recv(clientsock, buf, BUFSIZE, MSG_WAITALL);
			if (retval == SOCKET_ERROR) exit(1);
			else if (retval == 0) break;

			// 받은 데이터 출력
			buf[retval] = '\0';
			printf("[TCP/%s:%d] %s\n", addr, ntohs(clientaddr.sin6_port), buf);
		}

		closesocket(clientsock);
	}

	closesocket(sock);
}
