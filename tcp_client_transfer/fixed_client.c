#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <string.h>
#include <ws2tcpip.h>
#include <winsock2.h>

#define SERVERIP   "::1"
#define SERVERPORT 9000
#define BUFSIZE    50

#pragma comment(lib, "ws2_32")

void FixedTcp6Client() {
	SOCKET sock = socket(PF_INET6, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) exit(1);

	int retval;

	struct sockaddr_in6 serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin6_family = AF_INET6;
	serveraddr.sin6_port = htons(SERVERPORT);
	inet_pton(AF_INET6, SERVERIP, &serveraddr.sin6_addr);

	retval = connect(sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) exit(1);

	// 변수 & 버퍼
	char buf[BUFSIZE];
	const char* testdata[] = {
		"안녕하세요",
		"반가워요",
		"저도 그렇네요"
	};

	for (int i = 0; i < 3; i++) {
		memset(buf, '#', sizeof(buf));
		strncpy(buf, testdata[i], strlen(testdata[i]));

		retval = send(sock, buf, BUFSIZE, 0);
		if (retval == SOCKET_ERROR) exit(1);
		printf("[TCP CLIENT] %d 바이트 전송\n", retval);
	}

	closesocket(sock);
	return;
}