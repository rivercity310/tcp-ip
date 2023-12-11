#include "common.h"

#define BUFSIZE 1024

void CloseOnTransferServer6() {
	int retval;
	SOCKET sock = *InitServerSocket6(&retval);
	
	// 소켓의 '송신' 버퍼 크기 확인
	int nBufSize = 0, nLen = sizeof(nBufSize);
	if (getsockopt(sock, SOL_SOCKET, 
		SO_SNDBUF, (char*)&nBufSize, &nLen) != SOCKET_ERROR) {
		printf("Send Buffer size: %d\n", nBufSize);
	}

	// 소켓의 '수신' 버퍼 크기 확인
	nBufSize = 0; nLen = sizeof(nBufSize);
	if (getsockopt(sock, SOL_SOCKET,
		SO_RCVBUF, (char*)&nBufSize, &nLen) != SOCKET_ERROR) {
		printf("Receive Buffer size: %d\n", nBufSize);
	}

	SOCKET client_sock;
	struct sockaddr_in6 clientaddr;
	int addrlen;
	char buf[BUFSIZE + 1];

	while (1) {
		addrlen = sizeof(clientaddr);
		client_sock = accept(sock, (struct sockaddr*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}

		// 접속한 클라이언트 정보 출력
		char addr[INET6_ADDRSTRLEN];
		inet_ntop(AF_INET6, &clientaddr.sin6_addr, addr, sizeof(addr));
		printf("\n[TCP SERVER] 클라이언트 접속: IP=%s, PORT=%d\n", addr, ntohs(clientaddr.sin6_port));

		// 클라이언트와 데이터 통신
		while (1) {
			retval = recv(client_sock, buf, BUFSIZE, MSG_WAITALL);
			if (retval == SOCKET_ERROR) {
				err_display("recv()");
				break;
			}
			else if (retval == 0) {
				break;
			}

			// 받은 데이터 출력
			buf[retval] = '\0';
			printf("[TCP/%s:%d] %s\n", addr, ntohs(clientaddr.sin6_port), buf);
		}

		closesocket(client_sock);
		printf("[TCP SERVER] 클라이언트 종료: IP=%s, PORT=%d\n", addr, ntohs(clientaddr.sin6_port));
	}

	closesocket(sock);
}