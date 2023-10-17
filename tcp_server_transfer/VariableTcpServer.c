#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#define SERVERPORT 9000
#define SERVERIP   "127.0.0.1"
#define BUFSIZE    512

#pragma comment(lib, "ws2_32")

int _recv_ahead(SOCKET s, char* p) {
	__declspec(thread) static int nbytes = 0;
}

void VariableTcpServer() {
	SOCKET sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) exit(1);

	int retval;
}