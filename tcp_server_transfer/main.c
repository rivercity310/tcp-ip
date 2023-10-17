#include <winsock2.h>

extern void FixedTcp6Server();

int main() {
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		exit(1);
	}

	//FixedTcp6Server();

	WSACleanup();

	return 0;
}