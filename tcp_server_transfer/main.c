#include <winsock2.h>

extern void FixedTcpServer6();
extern void VariableTcpServer4();

int main() {
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		exit(1);
	}

	// FixedTcpServer6();
	VariableTcpServer4();

	WSACleanup();

	return 0;
}