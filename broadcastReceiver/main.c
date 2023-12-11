#include <stdio.h>
#include <ws2tcpip.h>
#include <winsock2.h>

#define LOCALPORT 9000
#define BUFSIZE   512

#pragma comment(lib, "ws2_32")

int main() {
	int retval;

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) 
		return 1;

	SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == INVALID_SOCKET) return 1;

	// bind
	struct sockaddr_in localaddr;
	memset(&localaddr, 0, sizeof(localaddr));
	localaddr.sin_family = AF_INET;
	localaddr.sin_port = htons(LOCALPORT);
	localaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	retval = bind(sock, (struct sockaddr*)&localaddr, sizeof(localaddr));
	if (retval == SOCKET_ERROR) return 1;

	struct sockaddr_in peeraddr;
	int addrlen;
	char buf[BUFSIZE + 1];

	while (1) {
		addrlen = sizeof(peeraddr);
		retval = recvfrom(sock, buf, BUFSIZE, 0,
			(struct sockaddr*)&peeraddr, &addrlen);
		if (retval == SOCKET_ERROR) break;

		// 받은 데이터 출력
		buf[retval] = '\0';
		char addr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &peeraddr.sin_addr, addr, sizeof(addr));

		printf("[UDP/%s:%d] %s\n", addr, ntohs(peeraddr.sin_port), buf);
	}

	closesocket(sock);

	WSACleanup();
	return 0;
}