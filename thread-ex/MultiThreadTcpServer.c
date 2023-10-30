#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <stdio.h>

#define SERVERPORT 9000
#define BUFSIZE    512

#pragma comment(lib, "ws2_32")

static DWORD WINAPI ProcessClient(LPVOID arg) {
	SOCKET client_sock = (SOCKET)arg;
	
	int retval;
	struct sockaddr_in clientaddr;
	char addr[INET_ADDRSTRLEN];
	int addrlen;
	char buf[BUFSIZE + 1];

	printf("[TCP SERVER] Current Thread Id: %d\n", GetCurrentThreadId());
	
	// 접속한 클라이언트 정보 얻기
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (struct sockaddr*)&clientaddr, &addrlen);
	inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));

	while (1) {
		// 데이터 받기
		retval = recv(client_sock, buf, BUFSIZE, 0);
		if (retval == SOCKET_ERROR || retval == 0) {
			break;
		}

		// 받은 데이터 출력
		buf[retval] = '\0';
		printf("[TCP/%s:%d] %s\n", addr, ntohs(clientaddr.sin_port), buf);

		// 데이터 보내기
		retval = send(client_sock, buf, retval, 0);
		if (retval == SOCKET_ERROR) {
			break;
		}
	}

	closesocket(client_sock);
	printf("[TCP SERVER] 클라이언트 종료(%s:%d)\n", addr, ntohs(clientaddr.sin_port));
	return 0;
}

void MultiThreadServerEx() {
	int retval;

	// Winsock 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		return 1;
	}

	// 서버 소켓 생성
	SOCKET listen_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listen_sock == INVALID_SOCKET) exit(1);

	// bind()
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) exit(1);

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) exit(1);

	// 데이터 통신
	SOCKET client_sock;
	struct sockaddr_in clientaddr;
	int addrlen;
	HANDLE hThread;

	while (1) {
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) break;

		// 스레드 생성
		hThread = (HANDLE)_beginthreadex(NULL, 0, ProcessClient, (LPVOID)client_sock, CREATE_SUSPENDED, NULL);
		if (hThread == NULL) closesocket(client_sock);
		else {
			ResumeThread(hThread);
			CloseHandle(hThread);
		}
	}

	closesocket(listen_sock);
	WSACleanup();
	return 0;
}