#include <winsock2.h>

extern void FixedTcp6Client();

int main() {
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		exit(1);
	}

	FixedTcp6Client();

	WSACleanup();

	return 0;
}