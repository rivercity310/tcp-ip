#include "Common.h"

#define SERVERPORT 9000
#define BUFSIZE    512

#pragma comment(lib, "ws2_32")

static bool getDomainName(struct in_addr addr, char* name, int namelen) {
	HOSTENT* ptr = gethostbyaddr((const char*)&addr, sizeof(addr), AF_INET);
	if (ptr == NULL) {
		err_display("getDomainName()");
		return false;
	}

	if (ptr->h_addrtype != AF_INET) {
		return false;
	}

	strncpy(name, ptr->h_name, namelen);
	return true;
}

static SOCKET* TcpServer() {
	// Server Socket 생성
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) {
		err_quit("socket()");
	}

	int retval;

	// Socket Struct Init & Socket Bind
	SOCKADDR_IN serveraddr;
	//memset(&serveraddr, 0, sizeof(serveraddr));
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);  // INADDR_ANY(0.0.0.0) : 모든 연결 요청 IP 허용
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) {
		err_quit("bind()");
	}

	// Socket Listen : SOMAXCONN -> OS Socket Max Connection
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) {
		err_quit("listen()");
	}

	// Socket Accept (요청온 클라이언트 소켓 정보 담기)
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	char buf[BUFSIZE + 1];

	while (1) {
		// accept() : Blocking Socket -> Client 접속까지 대기
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}

		char name[256];
		getDomainName(clientaddr.sin_addr, name, sizeof(name));
		printf(
			"\n[TCP 서버] 클라이언트 접속: IP 주소 = %s, 포트번호 = %d, DNS = %s\n",
			inet_ntoa(clientaddr.sin_addr),
			ntohs(clientaddr.sin_port),
			name
		);

		// 클라이언트가 보낸 데이터 받기
		while (1) {
			retval = recv(client_sock, buf, BUFSIZE, 0);
			if (retval == SOCKET_ERROR) {
				err_display("recv()");
				break;
			}
			// 클라이언트에서 연결 종료한 경우 retval = 0
			else if (retval == 0) {
				break;
			}

			buf[retval] = '\0';
			printf(
				"[TCP/%s %d %s] %s\n",
				inet_ntoa(clientaddr.sin_addr),
				ntohs(clientaddr.sin_port),
				name,
				buf
			);

			// 그대로 클라이언트에 전달 (에코)
			retval = send(client_sock, buf, retval, 0);
			if (retval == SOCKET_ERROR) {
				err_display("send()");
				break;
			}
		}

		closesocket(client_sock);
		printf(
			"\n[TCP 서버] 클라이언트 종료: IP 주소 = %s, 포트번호 = %d, DNS = %s\n",
			inet_ntoa(clientaddr.sin_addr),
			ntohs(clientaddr.sin_port),
			name
		);
	}
	
	return &listen_sock;
}

int main() {
	// WINSOCK 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		return 0;
	}

	SOCKET* server_sock = TcpServer();
	
	// Socket Close
	closesocket(*server_sock);

	// WINSOCK 종료
	WSACleanup();
}