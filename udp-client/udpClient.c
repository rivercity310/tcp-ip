#include <stdio.h>
#include <stdlib.h>
#include <ws2tcpip.h>
#include <winsock2.h>

#define SERVERADDR "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE    512

#pragma comment(lib, "ws2_32")

void UDPClient() {
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		exit(1);
	}

	// 소켓 생성
	SOCKET sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == INVALID_SOCKET) exit(1);

	int retval;

	// 소켓 주소 구조체 초기화
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	inet_pton(AF_INET, SERVERADDR, &serveraddr.sin_addr);
	serveraddr.sin_port = htons(SERVERPORT);
	serveraddr.sin_family = PF_INET;

	// 데이터 통신에 사용할 변수
	struct sockaddr_in peeraddr;
	int addrlen;
	char buf[BUFSIZE + 1];
	int len;

	// 서버와 데이터 통신
	while (1) {
		// 데이터 입력
		printf("\n[보낼 데이터] ");
		if (fgets(buf, BUFSIZE + 1, stdin) == NULL) {
			break;
		}

		// 스페이스바 제거
		len = (int)strlen(buf);
		if (buf[len - 1] == '\n') {
			buf[len - 1] = '\0';
		}
		if (strlen(buf) == 0) {
			break;
		}

		// 데이터 전송
		retval = sendto(sock, buf, (int)strlen(buf), 0,
			(struct sockaddr*)&serveraddr, sizeof(serveraddr));
		if (retval == SOCKET_ERROR) exit(1);

		printf("[UDP Client] %d바이트 전송 완료\n", retval);

		// 데이터 받기
		addrlen = sizeof(peeraddr);
		retval = recvfrom(sock, buf, BUFSIZE, 0,
			(struct sockaddr*)&peeraddr, &addrlen);
		if (retval == SOCKET_ERROR) exit(1);

		// 송신지 IP 주소 체크
		if (memcmp(&peeraddr, &serveraddr, sizeof(peeraddr))) {
			printf("[오류] 잘못된 데이터입니다!\n");
			break;
		}

		// 받은 데이터 출력
		buf[retval] = '\0';
		printf("[UDP Client] %d바이트를 받았습니다.\n", retval);
		printf("[받은 데이터] %s\n", buf);
	}

	closesocket(sock);
	WSACleanup();
}