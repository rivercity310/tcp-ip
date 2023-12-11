#include <stdio.h>
#include <winsock2.h>

#define REMOTEIP   "255.255.255.255"   // IPv4 Local Broadcast Address
#define REMOTEPORT 9000
#define BUFSIZE    512

#pragma comment(lib, "ws2_32")

int main() {
	int retval;

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == INVALID_SOCKET) return 1;

	// 브로드캐스팅 활성화
	DWORD bEnable = 1;
	retval = setsockopt(sock, SOL_SOCKET, SO_BROADCAST,
		(const char*)&bEnable, sizeof(bEnable));
	if (retval == SOCKET_ERROR) return 1;

	// 소켓 주소 구조체 초기화
	struct sockaddr_in remoteaddr;
	memset(&remoteaddr, 0, sizeof(remoteaddr));
	inet_pton(AF_INET, REMOTEIP, &remoteaddr.sin_addr);
	remoteaddr.sin_family = AF_INET;
	remoteaddr.sin_port = htons(REMOTEPORT);

	// 데이터 통신에 사용할 변수
	char buf[BUFSIZE + 1];
	int len;

	// 브로드캐스트 데이터 보내기
	while (1) {
		printf("\n[보낼 데이터] ");
		if (fgets(buf, BUFSIZE + 1, stdin) == NULL)
			break;

		// '\n' 제거
		len = (int)strlen(buf);
		if (buf[len - 1] == '\n')
			buf[len - 1] = '\0';

		if (strlen(buf) == 0)
			break;

		// 데이터 보내기
		retval = sendto(sock, buf, len, 0,
			(struct sockaddr*)&remoteaddr, sizeof(remoteaddr));
		if (retval == SOCKET_ERROR) break;

		printf("[UDP] %d바이트를 보냈습니다.\n", retval);
	}

	closesocket(sock);

	WSACleanup();
	return 0;
}