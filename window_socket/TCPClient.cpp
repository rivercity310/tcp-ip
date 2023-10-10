#include "Common.h"

#define SERVERIP     "127.0.0.1"
#define SERVERIP6    "::1"
#define SERVERPORT   9000
#define SERVERPORT6  9090
#define BUFSIZE      512

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

static void ConnectToServer(SOCKET* sock) {
	// DATA 통신
	char buf[BUFSIZE + 1];
	int len, retval;

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
		retval = send(*sock, buf, strlen(buf), 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
			break;
		}
		printf("[TCP CLIENT] %d 바이트를 보냈습니다 \n", retval);

		// 데이터 받기
		retval = recv(*sock, buf, retval, 0);
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
}


static void TcpClient4() {
	int retval;

	// Client Socket Init & bind
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	SOCKADDR_IN clientaddr;
	memset(&clientaddr, 0, sizeof(clientaddr));
	clientaddr.sin_family = AF_INET;
	clientaddr.sin_addr.s_addr = INADDR_ANY;
	clientaddr.sin_port = htons(50004);
	retval = bind(sock, (SOCKADDR*)&clientaddr, sizeof(clientaddr));

	// TCP/IP Server Socket 주소 구조체 초기화
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);

	// CONNECT : OS가 자동으로 Client 지역 IP, PORT 할당 (수동 설정시 bind() 함수 사용)
	retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");

	ConnectToServer(&sock);

	// SOCKET 닫기
	closesocket(sock);
}

static void TcpClient6() {
	int retval;

	// Client Socket Init & bind()
	SOCKET client_sock = socket(AF_INET6, SOCK_STREAM, 0);
	if (client_sock == INVALID_SOCKET) {
		err_quit("socket()");
	}

	SOCKADDR_IN6 clientaddr;
	memset(&clientaddr, 0, sizeof(clientaddr));
	clientaddr.sin6_family = AF_INET6;
	clientaddr.sin6_port = htons(50006);
	clientaddr.sin6_addr = in6addr_any;
	retval = bind(client_sock, (SOCKADDR*)&clientaddr, sizeof(clientaddr));

	// 연결할 Server Socket 구조체 초기화	
	SOCKADDR_IN6 serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	inet_pton(AF_INET6, SERVERIP6, &serveraddr.sin6_addr);
	serveraddr.sin6_family = AF_INET6;
	serveraddr.sin6_port = htons(SERVERPORT6);
	
	// connect()
	retval = connect(client_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");

	ConnectToServer(&client_sock);

	// SOCKET ClOSE
	closesocket(client_sock);
}

void TcpClientEx() {
	// WINSOCK 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) err_quit("winsock init fail");

	// User Prompt
	int sel;
	puts("[사용할 연결을 선택하세요]");
	puts("(1)IPv4 (2)IPv6");
	scanf_s("%d", &sel);

	if (sel == 1) TcpClient4();
	else TcpClient6();

	// WINSOCK 종료
	WSACleanup();
}
