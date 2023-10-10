#include "Common.h"

#define SERVERPORT   9000
#define SERVERPORT6  9090
#define BUFSIZE      512
#define TCPVERSION4  4
#define TCPVERSION6  6

#pragma comment(lib, "ws2_32")

typedef int TCPVERSION;

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

static void ConnectToClient(SOCKET listen_sock, TCPVERSION v) {
	// Client Connection
	SOCKET client_sock;
	int addrlen, retval;
	char buf[BUFSIZE + 1];
	char* ipaddr;

	while (1) {
		if (v != TCPVERSION4 && v != TCPVERSION6) {
			err_display("TCPVERSION");
			break;
		}
		
		// accept()
		if (v == TCPVERSION4) {
			SOCKADDR_IN clientaddr4;
			addrlen = sizeof(clientaddr4);
			client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr4, &addrlen);
			
			ipaddr = (char*)malloc(INET_ADDRSTRLEN);
			if (ipaddr != NULL) {
				memset(ipaddr, 0, INET_ADDRSTRLEN);
				inet_ntop(AF_INET, &clientaddr4.sin_addr, ipaddr, sizeof(ipaddr));
				printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트번호=%d\n", ipaddr, ntohs(clientaddr4.sin_port));
			}
		}

		else if (v == TCPVERSION6) {
			SOCKADDR_IN6 clientaddr6;
			addrlen = sizeof(clientaddr6);
			client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr6, &addrlen);
			
			ipaddr = (char*)malloc(INET6_ADDRSTRLEN);
			if (ipaddr != NULL) {
				memset(ipaddr, 0, INET6_ADDRSTRLEN);
				inet_ntop(AF_INET6, &clientaddr6.sin6_addr, ipaddr, sizeof(ipaddr));
				printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트번호=%d\n", ipaddr, ntohs(clientaddr6.sin6_port));
			}
		}

		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}

		// 클라이언트와 통신
		while (1) {
			// recv()
			retval = recv(client_sock, buf, BUFSIZE, 0);
			if (retval == SOCKET_ERROR) {
				err_display("recv()");
				break;
			}
			else if (retval == 0) break;

			// 클라이언트에서 전달된 데이터 출력
			buf[retval] = '\0';
			printf("%s", buf);

			retval = send(client_sock, buf, BUFSIZE, 0);
			if (retval == SOCKET_ERROR) {
				err_display("send()");
				break;
			}
		}

		closesocket(client_sock);
	}
}

static DWORD WINAPI TcpServer(LPVOID arg) {
	// Server Socket 생성
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) {
		err_quit("socket()");
	}

	int retval;

	// Socket Struct Init & Socket Bind
	SOCKADDR_IN serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
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
	
	ConnectToClient(listen_sock, TCPVERSION4);
	
	closesocket(listen_sock);
	return 0;
}

static DWORD WINAPI TCPServer6(LPVOID arg) {
	int retval;

	// Socket Init
	SOCKET listen_sock = socket(AF_INET6, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit("socket()");
	int no = 1;
	setsockopt(listen_sock, IPPROTO_IPV6, IPV6_V6ONLY, (const char*)&no, sizeof(no));

	// bind()
	SOCKADDR_IN6 serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin6_family = AF_INET6;
	serveraddr.sin6_addr = in6addr_any;
	serveraddr.sin6_port = htons(SERVERPORT6);
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");

	ConnectToClient(listen_sock, TCPVERSION6);

	closesocket(listen_sock);
	return 0;
}

int main() {
	// WINSOCK 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		return 0;
	}
	
	// 멀티스레드를 이용하여 두개의 서버 구동
	HANDLE hThread[2];
	hThread[0] = CreateThread(NULL, 0, TcpServer, NULL, 0, NULL);
	hThread[1] = CreateThread(NULL, 0, TCPServer6, NULL, 0, NULL);
	WaitForMultipleObjects(2, hThread, TRUE, INFINITE);

	// WINSOCK 종료
	WSACleanup();
}