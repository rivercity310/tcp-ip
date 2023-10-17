#include "common.h"

#define BUFSIZE    512


int _recv_ahead(SOCKET s, char* p) {
	__declspec(thread) static int nbytes = 0;
}

void VariableTcpServer() {
	int retval = NULL;
	SOCKET sock = InitServerSocket4(&retval);
	
	// TODO..

	closesocket(sock);
	return;
}