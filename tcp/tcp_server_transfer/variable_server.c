#include "common.h"

#define BUFSIZE 512
#define EOR     '\n'  /* \n을 EOR로 간주 */

/* 소켓 수신 함수 recv()에서 데이터를 한번에 읽고 요청마다 1바이트씩 리턴 */
int _recv_ahead(SOCKET s, char* p) {
	__declspec(thread) static int nbytes = 0;
	__declspec(thread) static char buf[BUFSIZE * 2];
	__declspec(thread) static char* ptr;

	if (nbytes == 0 || nbytes == SOCKET_ERROR) {
		nbytes = recv(s, buf, sizeof(buf), 0);
		if (nbytes == SOCKET_ERROR) {
			return SOCKET_ERROR;
		}
		else if (nbytes == 0) {
			return 0;
		}
		ptr = buf;
	}

	--nbytes;
	*p = *ptr++;
	return 1;
}

/* 사용자 정의 데이터 수신 함수 */
int recvline(SOCKET s, char* buf, int maxlen) {
	int n, nbytes;
	char c, * ptr = buf;

	for (n = 1; n < maxlen; n++) {
		nbytes = _recv_ahead(s, &c);

		if (nbytes == 1) {
			*ptr++ = c;
			if (c == EOR) {
				break;
			}
		}
		else if (nbytes == 0) {
			*ptr = 0;
			return n - 1;
		}
		else {
			return SOCKET_ERROR;
		}
	}

	*ptr = 0;
	return n;
}

void VariableTcpServer4() {
	int retval = NULL;
	SOCKET sock = *InitServerSocket4(&retval);
	
	SOCKET client_sock;
	struct sockaddr_in clientaddr;
	int addrlen;
	char buf[BUFSIZE + 1];

	while (1) {
		addrlen = sizeof(clientaddr);
		client_sock = accept(sock, (struct sockaddr*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}

		char addr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
		printf("\n[TCP SERVER] 클라이언트 접속: IP=%s, PORT=%d\n", addr, ntohs(clientaddr.sin_port));

		// 클라이언트와 가변길이 데이터 통신
		while (1) {
			retval = recvline(client_sock, buf, BUFSIZE + 1);
			if (retval == SOCKET_ERROR) {
				err_display("recv()");
				break;
			}
			else if (retval == 0) {
				break;
			}

			// 받은 데이터 출력
			printf("[TCP/%s:%d] %s", addr, ntohs(clientaddr.sin_port), buf);
		}
	}

	closesocket(sock);
	return;
}