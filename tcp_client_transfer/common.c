#include "common.h"

SOCKET* InitClientSocket4(int* retval, int clientport) {
	SOCKET sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) {
		err_quit("socket()");
	}
	
	if (clientport != NULL && (clientport > 1023 && clientport < 65536)) {
		struct sockaddr_in clientaddr;
		memset(&clientaddr, 0, sizeof(clientaddr));
		clientaddr.sin_family = AF_INET;
		clientaddr.sin_port = htons(clientport);
		inet_pton(AF_INET, "127.0.0.1", &clientaddr.sin_addr);

		*retval = bind(sock, (struct sockaddr*)&clientaddr, sizeof(clientaddr));
		if (*retval == SOCKET_ERROR) err_quit("bind()");
	}
	
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(SERVERPORT);
	inet_pton(AF_INET, SERVERIP4, &serveraddr.sin_addr);

	*retval = connect(sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (*retval == SOCKET_ERROR) err_quit("connect()");

	return &sock;
}

SOCKET* InitClientSocket6(int* retval, int clientport) {
	SOCKET sock = socket(PF_INET6, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) {
		err_quit("socket()");
	}

	if (clientport != NULL && (clientport > 1023 && clientport < 65536)) {
		struct sockaddr_in6 clientaddr;
		memset(&clientaddr, 0, sizeof(clientaddr));
		clientaddr.sin6_family = AF_INET6;
		clientaddr.sin6_port = htons(clientport);
		inet_pton(AF_INET6, SERVERIP6, &clientaddr.sin6_addr);

		*retval = bind(sock, (struct sockaddr*)&clientaddr, sizeof(clientaddr));
		if (*retval == SOCKET_ERROR) err_quit("bind()");
	}

	struct sockaddr_in6 serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin6_family = AF_INET6;
	serveraddr.sin6_port = htons(SERVERPORT);
	inet_pton(AF_INET6, SERVERIP6, &serveraddr.sin6_addr);

	*retval = connect(sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (*retval == SOCKET_ERROR) err_quit("connect()");

	return &sock;
}

void err_quit(const char* msg) {
	LPVOID lpMsgBuf;

	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(char*)&lpMsgBuf,
		0,
		NULL
	);

	MessageBoxA(NULL, (const char*)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}


void err_display(const char* msg) {
	LPVOID lpMsgBuf;

	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(char*)&lpMsgBuf,
		0,
		NULL
	);

	printf("[%s] %s\n", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}
