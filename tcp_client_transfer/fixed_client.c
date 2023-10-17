#include "common.h"

#define BUFSIZE    50

void FixedTcp6Client() {
	int retval = NULL;
	SOCKET sock = *InitClientSocket6(&retval, NULL);

	// 변수 & 버퍼
	char buf[BUFSIZE];
	const char* testdata[] = {
		"안녕하세요",
		"반가워요",
		"저도 그렇네요"
	};

	for (int i = 0; i < 3; i++) {
		memset(buf, '#', sizeof(buf));
		strncpy(buf, testdata[i], strlen(testdata[i]));

		retval = send(sock, buf, BUFSIZE, 0);
		if (retval == SOCKET_ERROR) exit(1);
		printf("[TCP CLIENT] %d 바이트 전송\n", retval);
	}

	closesocket(sock);
	return;
}