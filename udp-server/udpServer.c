#include <ws2tcpip.h>
#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <string.h>

#define SERVERPORT 9000
#define BUFSIZE    512

#pragma comment(lib, "ws2_32")

void UDPServer() {
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		exit(1);
	}

	int retval;

	// 소켓 생성
	SOCKET sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == INVALID_SOCKET) exit(1);

	// bind
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(SERVERPORT);
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	retval = bind(sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) exit(1);

	// 데이터 통신에 사용할 변수
	struct sockaddr_in clientaddr;
	int addrlen;
	char buf[BUFSIZE + 1];

	// 클라이언트와 데이터 통신
	while (1) {
		// 데이터 받기
		addrlen = sizeof(clientaddr);
		retval = recvfrom(sock, buf, BUFSIZE, 0,
			(struct sockaddr*)&clientaddr, &addrlen);
		if (retval == SOCKET_ERROR) exit(1);

		// 받은 데이터 출력
		buf[retval] = '\0';
		char addr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
		printf("[UDP/%s:%d] %s\n", addr, ntohs(clientaddr.sin_port), buf);

		// 데이터 보내기
		retval = sendto(sock, buf, retval, 0,
			(struct sockaddr*)&clientaddr, sizeof(clientaddr));
		if (retval == SOCKET_ERROR) exit(1);
	}

	closesocket(sock);
	WSACleanup();
	return;
}