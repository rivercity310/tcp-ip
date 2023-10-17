#include <winsock2.h>

extern void FixedTcpClient6();
extern void VariableTcpClient4();

int main() {
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		exit(1);
	}

	FixedTcpClient6();
	// VariableTcpClient4();

	WSACleanup();

	return 0;
}