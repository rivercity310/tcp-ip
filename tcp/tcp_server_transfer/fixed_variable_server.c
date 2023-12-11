#include "common.h"

#define BUFSIZE 512

void FixedVariableTcpServer4() {
	int retval;
	SOCKET sock = *InitServerSocket4(&retval);

	SOCKET client_sock;
	struct sockaddr_in clientaddr;
	int addrlen;
	int len;				 // 고정 길이 데이터
	char buf[BUFSIZE + 1];	 // 가변 길이 데이터

	while (1) {
		addrlen = sizeof(clientaddr);
		client_sock = accept(sock, (struct sockaddr*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}

		// 접속한 클라이언트 정보 출력
		char addr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
		printf("\n[TCP SERVER] 클라이언트 접속: IP=%s, PORT=%d\n", addr, ntohs(clientaddr.sin_port));

		// 클라이언트와 데이터 통신
		while (1) {
			// 1. 고정 길이 데이터 받기
			retval = recv(client_sock, (char*)&len, sizeof(int), MSG_WAITALL);
			if (retval == SOCKET_ERROR) {
				err_display("recv()");
				break;
			}
			else if (retval == 0) {
				break;
			}

			// 2. 가변 길이 데이터 받기
			retval = recv(client_sock, buf, len, MSG_WAITALL);
			if (retval == SOCKET_ERROR) {
				err_display("recv()");
				break;
			}
			else if (retval == 0) {
				break;
			}

			buf[retval] = '\0';
			printf("[TCP/%s:%d] %s\n", addr, ntohs(clientaddr.sin_port), buf);
		}

		closesocket(client_sock);
		printf("[TCP SERVER] 클라이언트 종료: IP=%s, PORT=%d\n", addr, ntohs(clientaddr.sin_port));
	}

	closesocket(sock);
}