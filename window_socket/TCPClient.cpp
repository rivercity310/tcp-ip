#include "Common.h"

#define SERVERIP   "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE    512

#pragma comment(lib, "ws2_32")

// 사용자 정의 데이터 수신 함수
static int recvn(SOCKET s, char* buf, int len, int flags) {
	int received;
	char* ptr = buf;
	int left = len;

	while (left > 0) {
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR) {
			return SOCKET_ERROR;
		}
		else if (received == 0) {
			break;
		}

		left -= received;
		ptr += received;
	}

	return (len - left);
}


void TCPClient() {
	int retval;

	// WINSOCK INIT
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		return;
	}

	// Client Socket 생성
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	// TCP/IP Server Socket 주소 구조체 초기화
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);

	// CONNECT : OS가 자동으로 Client 지역 IP, PORT 할당 (수동 설정시 bind() 함수 사용)
	retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");

	// DATA 통신
	char buf[BUFSIZE + 1];
	int len;

	while (1) {
		// 입력받기
		printf("\n[보낼 데이터] ");
		if (fgets(buf, BUFSIZE + 1, stdin) == NULL) {
			break;
		}

		// 마지막 엔터키값을 NULL 값으로 치환
		len = sizeof(buf);
		if (buf[len - 1] == '\n') {
			buf[len - 1] = '\0';
		}

		// 데이터 보내기 (send(): 보낸 byte 수 반환)
		retval = send(sock, buf, strlen(buf), 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
			break;
		}
		printf("[TCP CLIENT] %d 바이트를 보냈습니다 \n", retval);

		// 데이터 받기
		retval = recvn(sock, buf, retval, 0);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			break;
		}
		else if (retval == 0) {
			break;
		}

		// 받은 데이터 출력
		buf[retval] = '\0';
		printf("[TCP 클라이언트] %d 바이트를 받았습니다 \n", retval);
		printf("[받은 데이터] %s\n", buf);
	}

	// SOCKET 닫기
	closesocket(sock);

	// WINSOCK 종료
	WSACleanup();
}
