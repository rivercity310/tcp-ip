#include "common.h"

#define SERVERPORT 9000

#pragma comment(lib, "ws2_32")

struct calc_args {
	int a, b;
};

void TcpServerStart() {
	// WinSock 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		err_quit("winsock init fail");
	}

	// Socket 초기화
	SOCKET sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) {
		err_quit("socket()");
	}

	int retval;

	// Server Socket 구조체 초기화 & bind()
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = INADDR_ANY;
	serveraddr.sin_port = htons(SERVERPORT);

	retval = bind(sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	
	// listen()
	retval = listen(sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) {
		err_quit("listen()");
	}

	// Client 접속 대기
	SOCKET clientsock;
	struct sockaddr_in clientaddr;
	int len = sizeof(clientaddr);
	int i, sum;

	struct calc_args calcargs;
	memset(&calcargs, 0, sizeof(calcargs));

	while (1) {
		clientsock = accept(sock, (struct sockaddr*)&clientaddr, &len);
		if (clientsock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}

		puts("[TCP SERVER] 클라이언트 접속!");
		printf("IP = %s, PORT = %d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		len = sizeof(calcargs);

		while (1) {
			retval = recv(clientsock, (char*)&calcargs, len, 0);

			if (retval == 0) {
				printf("[TCP SERVER] Client로부터 접속 종료\n");
				return;
			}
			else if (retval == SOCKET_ERROR) {
				err_display("recv()");
				return;
			}
			
			sum = calcargs.a + calcargs.b;
			printf("[TCP SERVER] 데이터 수신: a = %d, b = %d, a + b = %d\n", calcargs.a, calcargs.b, sum);

			calcargs.a = sum;
			calcargs.b = NULL;

			retval = send(clientsock, (const char*)&calcargs, len, 0);
			if (retval == SOCKET_ERROR) {
				err_display("send()");
				break;
			}
		}

		closesocket(clientsock);
	}

	// Socket Close
	closesocket(sock);

	// WinSock Close
	WSACleanup();
}
