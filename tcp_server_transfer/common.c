#include "common.h"

SOCKET* InitServerSocket6(int* retval) {
	SOCKET sock = socket(PF_INET6, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) {
		err_quit("socket()");
	}

	struct sockaddr_in6 serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin6_family = AF_INET6;
	serveraddr.sin6_port = htons(SERVERPORT);
	serveraddr.sin6_addr = in6addr_any;

	*retval = bind(sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (*retval == SOCKET_ERROR) err_quit("bind()");

	*retval = listen(sock, SOMAXCONN);
	if (*retval == SOCKET_ERROR) err_quit("listen()");

	return &sock;
}

SOCKET* InitServerSocket4(int* retval) {
	SOCKET sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) {
		err_quit("socket()");
	}

	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(SERVERPORT);
	serveraddr.sin_addr.s_addr = INADDR_ANY;

	*retval = bind(sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (*retval == SOCKET_ERROR) err_quit("bind()");

	*retval = listen(sock, SOMAXCONN);
	if (*retval == SOCKET_ERROR) err_quit("listen()");

	return &sock;
}

void err_quit(const char* msg) {
	LPVOID lpMsgBuf; // LPVOID: void*
	
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
	LPVOID lpMsgBuf; // LPVOID: void*

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
