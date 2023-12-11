#include <stdio.h>
#include <ws2tcpip.h>
#include <winsock2.h>
#include <windows.h>

#define MULTICASTIPv4  "235.7.8.9"
#define MULTICASTIPv6  "FF12::1:2:3:4"
#define LOCALPORT      9000
#define BUFSIZE        512

#pragma comment(lib, "ws2_32")

static DWORD WINAPI MulticastReceiverv4(LPVOID arg);
static DWORD WINAPI MulticastReceiverv6(LPVOID arg);

void MulticastReceiver() {
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		exit(1);

	HANDLE hThread[2];
	hThread[0] = (HANDLE)_beginthreadex(NULL, 0, MulticastReceiverv4, NULL, CREATE_SUSPENDED, NULL);
	hThread[1] = (HANDLE)_beginthreadex(NULL, 0, MulticastReceiverv6, NULL, CREATE_SUSPENDED, NULL);

	if (hThread[0] != NULL) ResumeThread(hThread[0]);
	if (hThread[1] != NULL) ResumeThread(hThread[1]);

	WaitForMultipleObjects(2, hThread, TRUE, INFINITE);

	if (hThread[0] != NULL) CloseHandle(hThread[0]);
	if (hThread[1] != NULL) CloseHandle(hThread[1]);

	printf("main thread id : %d\n", GetCurrentThreadId());

	WSACleanup();
	return;
}

static DWORD WINAPI MulticastReceiverv4(LPVOID arg) {
	int retval;

	printf("IPv4 Thread Id : %d\n", GetCurrentThreadId());
	
	// 소켓 생성
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == INVALID_SOCKET) exit(1);

	// 옵션 설정
	DWORD optval = 1;
	retval = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,
		(const char*)&optval, sizeof(optval));
	if (retval == SOCKET_ERROR) {
		puts("setsockopt()");
		exit(1);
	}

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
	inet_pton(AF_INET, MULTICASTIPv4, &mreq.imr_multiaddr);
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	retval = setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
		(const char*)&mreq, sizeof(mreq));
	if (retval == SOCKET_ERROR) {
		puts("setsockopt() : 그룹 가입");
		exit(1);
	}

	// 데이터 통신에 사용할 변수
	struct sockaddr_in peeraddr;
	int addrlen;
	char buf[BUFSIZE + 1];

	while (1) {
		addrlen = sizeof(peeraddr);
		retval = recvfrom(sock, buf, BUFSIZE, 0,
			(struct sockaddr*)&peeraddr, &addrlen);
		if (retval == SOCKET_ERROR) {
			puts("recvfrom()");
			exit(1);
		}

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
	return 0;
}

static DWORD WINAPI MulticastReceiverv6(LPVOID arg) {
	int retval;

	printf("IPv6 Thread Id : %d\n", GetCurrentThreadId());
	
	SOCKET sock = socket(PF_INET6, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == INVALID_SOCKET) exit(1);

	// SO_REUSEADDR 옵션 설정
	/*
	DWORD optval = 1;
	retval = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,
		(const char*)&optval, sizeof(optval));
	if (retval == SOCKET_ERROR) exit(1);
	*/

	// bind
	struct sockaddr_in6 localaddr;
	memset(&localaddr, 0, sizeof(localaddr));
	localaddr.sin6_addr = in6addr_any;
	localaddr.sin6_family = AF_INET6;
	localaddr.sin6_port = htons(LOCALPORT);
	retval = bind(sock, (struct sockaddr*)&localaddr, sizeof(localaddr));
	if (retval == SOCKET_ERROR) exit(1);

	// 멀티캐스트 그룹 가입
	struct ipv6_mreq mreq;
	inet_pton(PF_INET6, MULTICASTIPv6, &mreq.ipv6mr_multiaddr);
	mreq.ipv6mr_interface = 0;
	retval = setsockopt(sock, IPPROTO_IPV6, IPV6_JOIN_GROUP,
		(const char*)&mreq, sizeof(mreq));
	if (retval == SOCKET_ERROR) exit(1);

	// 데이터 통신
	struct sockaddr_in6 peeraddr;
	int addrlen;
	char buf[BUFSIZE + 1];

	while (1) {
		addrlen = sizeof(peeraddr);
		retval = recvfrom(sock, buf, BUFSIZE, 0,
			(struct sockaddr*)&peeraddr, &addrlen);

		if (retval == SOCKET_ERROR) break;

		buf[retval] = '\0';
		char addr[INET6_ADDRSTRLEN];
		inet_ntop(PF_INET6, &peeraddr.sin6_addr, addr, sizeof(addr));
	
		printf("[UDP/%s:%d] %s\n", addr, ntohs(peeraddr.sin6_port), buf);
	}

	// 멀티캐스트 그룹 탈퇴
	retval = setsockopt(sock, IPPROTO_IPV6, IPV6_LEAVE_GROUP,
		(const char*)&mreq, sizeof(mreq));
	if (retval == SOCKET_ERROR) exit(1);

	closesocket(sock);
}