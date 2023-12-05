#include <stdio.h>
#include <ws2tcpip.h>
#include <winsock2.h>

#define MULTICASTIP  "235.7.8.9"
#define LOCALPORT    9000
#define BUFSIZE      512

#pragma comment(lib, "ws2_32")

void MulticastServer4() {
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return;

	int retval;

	// 소켓 생성
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == INVALID_SOCKET) exit(1);

	// 옵션 설정
	DWORD optval = 1;
	retval = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,
		(const char*)&optval, sizeof(optval));
	if (retval == SOCKET_ERROR) exit(1);

	// bind()
	struct sockaddr_in localaddr;
	memset(&localaddr, 0, sizeof(localaddr));
	localaddr.sin_family = AF_INET;
	localaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	localaddr.sin_port = htons(LOCALPORT);
	retval = bind(sock, (struct sockaddr*)&localaddr, sizeof(localaddr));
	if (retval == SOCKET_ERROR) exit(1);

	// 멀티캐스트 그룹 가입
	struct ip_mreq mreq;
	inet_pton(AF_INET, MULTICASTIP, &mreq.imr_multiaddr);
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	retval = setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
		(const char*)&mreq, sizeof(mreq));
	if (retval == SOCKET_ERROR) exit(1);

	// 데이터 통신에 사용할 변수
	struct sockaddr_in peeraddr;
	int addrlen;
	char buf[BUFSIZE + 1];

	while (1) {
		addrlen = sizeof(peeraddr);
		retval = recvfrom(sock, buf, BUFSIZE, 0,
			(struct sockaddr*)&peeraddr, &addrlen);
		if (retval == SOCKET_ERROR) exit(1);

		buf[retval] = '\0';
		char addr[INET_ADDRSTRLEN];

		inet_ntop(AF_INET, &peeraddr.sin_addr, addr, sizeof(addr));
		printf("[UDP/%s:%d] %s\n", addr, ntohs(peeraddr.sin_port), buf);
	}

	// 멀티캐스트 그룹 탈퇴
	retval = setsockopt(sock, IPPROTO_IP, IP_DROP_MEMBERSHIP,
		(const char*)&mreq, sizeof(mreq));
	if (retval == SOCKET_ERROR) exit(1);

	closesocket(sock);

	WSACleanup();
	return;
}