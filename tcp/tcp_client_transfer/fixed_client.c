#include "common.h"

#define BUFSIZE 50

void FixedTcpClient6() {
	int retval = NULL;
	SOCKET sock = *InitClientSocket6(&retval, 50000);

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
		if (retval == SOCKET_ERROR) {
			err_display("send()");
			break;
		}
		
		printf("[TCP CLIENT] %d 바이트 전송\n", retval);
	}

	closesocket(sock);
	return;
}