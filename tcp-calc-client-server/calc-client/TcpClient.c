#include "common.h"

#define SERVERIP    "127.0.0.1"
#define SERVERPORT  9000
#define CLIENTPORT  50000
#define BUFSIZE     64

#pragma comment(lib, "ws2_32")

void TcpClientStart() {
	// WinSock 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		err_quit("winsock init fail");
	}

	// Socket 초기화
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) {
		err_quit("socket()");
	}

	int retval;
	
	// Client 소켓 구조체 초기화 & bind()
	struct sockaddr_in clientaddr;
	memset(&clientaddr, 0, sizeof(clientaddr));
	clientaddr.sin_family = AF_INET;
	clientaddr.sin_port = htons(CLIENTPORT);
	clientaddr.sin_addr.s_addr = INADDR_ANY;
	retval = bind(sock, (struct sockaddr*)&clientaddr, sizeof(clientaddr));


	// Server 소켓 구조체 초기화 & connect() 
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(SERVERPORT);
	inet_pton(AF_INET, SERVERIP, &serveraddr.sin_addr);
	retval = connect(sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) {
		err_quit("connect()");
	}

	// Server와 데이터 통신
	char buf[BUFSIZE + 1];
	int len, i;

	while (1) {
		// 사용자 입력받기
		puts("[두 수를 입력하면 더한 결과를 보여줍니다]");

		for (i = 0; i < 2; i++) {
			printf("데이터 %d: ", i + 1);
			if (fgets(buf, BUFSIZE, stdin) == NULL) {
				break;
			}

			// 엔터키 값 NULL로 치환
			len = strlen(buf);
			if (buf[len - 1] == '\n') {
				buf[len - 1] = '\0';
			}

			// send()
			retval = send(sock, buf, len, 0);
			if (retval == SOCKET_ERROR) {
				err_display("send()");
				break;
			}

			// recv()
			retval = recv(sock, buf, retval, 0);
			if (retval == SOCKET_ERROR) {
				err_display("recv()");
				break;
			}
			printf("[TCP CLIENT] 서버에서 에코 %s\n", buf);
		}

		// recv()
		retval = recv(sock, buf, retval, 0);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			break;
		}

		// 받은 데이터 출력
		printf("[덧셈 결과] %s\n", buf);
		putchar('\n');
	}

	// Socket 종료
	closesocket(sock);

	// WinSock 종료
	WSACleanup();
}

