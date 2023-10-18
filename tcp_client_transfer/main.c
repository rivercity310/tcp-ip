#include <winsock2.h>

extern void FixedTcpClient6();
extern void VariableTcpClient4();
extern void FixedVariableTcpClient4();
extern void CloseOnTransferClient6();

int main() {
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		exit(1);
	}

	// FixedTcpClient6();
	// VariableTcpClient4();
	// FixedVariableTcpClient4();
	CloseOnTransferClient6();

	WSACleanup();

	return 0;
}