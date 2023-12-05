#include <ws2tcpip.h>
#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <string.h>

#define SERVERPORT   9000
#define SERVERPORT6  9090
#define BUFSIZE      512

#pragma comment(lib, "ws2_32")

static DWORD WINAPI UDPServer4(LPVOID args);
static DWORD WINAPI UDPServer6(LPVOID args);

void UDPServer() {
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		exit(1);
	}

	HANDLE handle[2];
	handle[0] = (HANDLE)_beginthreadex(NULL, 0, UDPServer4, NULL, 0, NULL);
	handle[1] = (HANDLE)_beginthreadex(NULL, 0, UDPServer6, NULL, 0, NULL);

	WaitForMultipleObjects(2, handle, TRUE, INFINITE);
}

static DWORD WINAPI UDPServer4(LPVOID args) {
	int retval;

	// 소켓 생성
	SOCKET sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == INVALID_SOCKET) exit(1);

	// 소켓 옵션 생성
	
	// 1. Receive 타임아웃 10초
	DWORD optval = 10000;
	int optlen = sizeof(optval);
	retval = setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&optval, optlen);
	if (retval == SOCKET_ERROR) exit(1);

	// 2. 수신 버퍼 크기 늘리기
	retval = getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char*)&optval, &optlen);
	if (retval == SOCKET_ERROR) exit(1);
	printf("수신 버퍼 크기(old) = %dByte\n", optval);
	
	optval *= 2;
	retval = setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (const char*)&optval, sizeof(optval));
	if (retval == SOCKET_ERROR) exit(1);
	
	retval = getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char*)&optval, &optlen);
	if (retval == SOCKET_ERROR) exit(1);
	printf("수신 버퍼 크기(new) = %dByte\n", optval);

	// 3. IP 주소 / 포트번호 재사용
	optval = 1;
	retval = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&optval, sizeof(optval));
	if (retval == SOCKET_ERROR) exit(1);

	// bind
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(SERVERPORT);
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	retval = bind(sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) exit(1);

	// 데이터 통신에 사용할 변수
	struct sockaddr_in clientaddr;
	int addrlen;
	char buf[BUFSIZE + 1];

	// 클라이언트와 데이터 통신
	while (1) {
		// 데이터 받기
		addrlen = sizeof(clientaddr);
		retval = recvfrom(sock, buf, BUFSIZE, 0,
			(struct sockaddr*)&clientaddr, &addrlen);
		if (retval == SOCKET_ERROR) exit(1);

		// 받은 데이터 출력
		buf[retval] = '\0';
		char addr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
		printf("[UDP/%s:%d] %s\n", addr, ntohs(clientaddr.sin_port), buf);

		// 데이터 보내기
		retval = sendto(sock, buf, retval, 0,
			(struct sockaddr*)&clientaddr, sizeof(clientaddr));
		if (retval == SOCKET_ERROR) exit(1);
	}

	closesocket(sock);
	return;
}

static DWORD WINAPI UDPServer6(LPVOID args) {
	int retval;
	
	SOCKET sock = socket(PF_INET6, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == INVALID_SOCKET) exit(1);

	// bind()
	struct sockaddr_in6 serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin6_family = AF_INET6;
	serveraddr.sin6_addr = in6addr_any;
	serveraddr.sin6_port = htons(SERVERPORT6);
	retval = bind(sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) exit(1);

	// 데이터 통신에 사용할 변수
	struct sockaddr_in6 clientaddr;
	int addrlen;
	char buf[BUFSIZE + 1];

	while (1) {
		addrlen = sizeof(clientaddr);
		retval = recvfrom(sock, buf, BUFSIZE, 0,
			(struct sockaddr*)&clientaddr, &addrlen);
		if (retval == SOCKET_ERROR) exit(1);

		// 받은 데이터 출력
		buf[retval] = '\0';
		char addr[INET6_ADDRSTRLEN];
		inet_ntop(AF_INET6, &clientaddr.sin6_addr, addr, sizeof(addr));
		printf("[UDP/%s:%d] %s\n", addr, ntohs(clientaddr.sin6_port), buf);

		// 데이터 보내기
		retval = sendto(sock, buf, retval, 0,
			(struct sockaddr*)&clientaddr, sizeof(clientaddr));
		if (retval == SOCKET_ERROR) exit(1);
	}

	closesocket(sock);
	return;
}