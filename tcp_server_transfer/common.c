#include "common.h"

SOCKET* InitServerSocket6(int* retval) {
	SOCKET sock = socket(PF_INET6, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) exit(1);

	struct sockaddr_in6 serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin6_family = AF_INET6;
	serveraddr.sin6_port = htons(SERVERPORT);
	serveraddr.sin6_addr = in6addr_any;

	*retval = bind(sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (*retval == SOCKET_ERROR) exit(1);

	*retval = listen(sock, SOMAXCONN);
	if (*retval == SOCKET_ERROR) exit(1);

	return &sock;
}

SOCKET* InitServerSocket4(int* retval) {
	SOCKET sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) exit(1);

	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(SERVERPORT);
	serveraddr.sin_addr.s_addr = INADDR_ANY;

	*retval = bind(sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (*retval == SOCKET_ERROR) exit(1);

	*retval = listen(sock, SOMAXCONN);
	if (*retval == SOCKET_ERROR) exit(1);

	return &sock;
}