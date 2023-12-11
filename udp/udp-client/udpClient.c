#include <stdio.h>
#include <stdlib.h>
#include <ws2tcpip.h>
#include <winsock2.h>

#define SERVERADDR4  "127.0.0.1"
#define SERVERPORT4  9000
#define SERVERADDR6  "::1"
#define SERVERPORT6  9090
#define BUFSIZE      512

#pragma comment(lib, "ws2_32")

static void UDPClient4();
static void UDPClient6();

void UDPClient() {
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		exit(1);
	}

	int version;
	printf("연결할 IP Version을 선택하세요: ");
	scanf_s("%d", &version);
	getchar();

	if (version == 4) UDPClient4();
	else UDPClient6();

	WSACleanup();
}

static void UDPClient4() {
	// 소켓 생성
	SOCKET sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == INVALID_SOCKET) exit(1);

	int retval;

	// bind
	struct sockaddr_in clientaddr;
	clientaddr.sin_port = htons(52223);
	clientaddr.sin_family = AF_INET;
	clientaddr.sin_addr.s_addr = INADDR_ANY;
	retval = bind(sock, (struct sockaddr*)&clientaddr, sizeof(clientaddr));
	if (retval == SOCKET_ERROR) exit(1);

	// 브로드캐스팅 활성화
	DWORD bEnable = 1;
	retval = setsockopt(sock, SOL_SOCKET, SO_BROADCAST,
		(const char*)&bEnable, sizeof(bEnable));
	if (retval == SOCKET_ERROR) exit(1);

	// 소켓 주소 구조체 초기화
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	inet_pton(AF_INET, "255.255.255.255", &serveraddr.sin_addr);
	serveraddr.sin_port = htons(SERVERPORT4);
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
		char addr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &serveraddr.sin_addr, addr, sizeof(addr));
		printf("[%s:%d]\n", addr, ntohs(serveraddr.sin_port));

		// 데이터 받기
		addrlen = sizeof(peeraddr);
		retval = recvfrom(sock, buf, BUFSIZE, 0,
			(struct sockaddr*)&peeraddr, &addrlen);
		if (retval == SOCKET_ERROR) exit(1);

		inet_ntop(AF_INET, &peeraddr.sin_addr, addr, sizeof(addr));
		printf("[%s:%d]\n", addr, ntohs(peeraddr.sin_port));
		
		// 송신지 IP 주소 체크
		if (memcmp(&peeraddr, &serveraddr, sizeof(peeraddr))) {
			/*
			if (strcmp(addr, "255.255.255.255") != 0) {
				printf("[오류] 잘못된 데이터입니다!\n");
				break;
			}
			*/
		}

		// 받은 데이터 출력
		buf[retval] = '\0';
		printf("[UDP Client] %d바이트를 받았습니다.\n", retval);
		inet_ntop(AF_INET, &serveraddr.sin_addr, addr, sizeof(addr));
		printf("[%s:%d] 받은 데이터: %s\n", addr, ntohs(serveraddr.sin_port), buf);
	}

	closesocket(sock);
}

static void UDPClient6() {
	int retval;
	
	SOCKET sock = socket(PF_INET6, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == INVALID_SOCKET) exit(1);

	// 서버 구조체 초기화
	struct sockaddr_in6 serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	inet_pton(AF_INET6, SERVERADDR6, &serveraddr.sin6_addr);
	serveraddr.sin6_family = AF_INET6;
	serveraddr.sin6_port = htons(SERVERPORT6);

	// 데이터 통신에 사용할 변수
	struct sockaddr_in6 peeraddr;
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
}