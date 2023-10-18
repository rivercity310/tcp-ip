#include "common.h"

#define BUFSIZE 50

void FixedVariableTcpClient4() {
	int retval;
	SOCKET sock = *InitClientSocket4(&retval, NULL);

	// 데이터 통신에 사용할 변수
	int i, len;
	char buf[BUFSIZE];
	const char* testdata[] = {
		"안녕하세요",
		"반갑워요",
		"오늘따라 할 이야기가 참 많아요",
		"저도 그렇네요"
	};

	// 서버와 데이터 통신
	for (i = 0; i < 4; i++) {
		len = (int)strlen(testdata[i]);
		strncpy(buf, testdata[i], len);

		// 고정 길이 데이터 전송
		retval = send(sock, (char*)&len, sizeof(int), 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
			break;
		}

		// 가변 길이 데이터 전송
		retval = send(sock, buf, len, 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
			break;
		}

		printf("[TCP CLIENT] %d+%d바이트 전송\n", (int)sizeof(int), retval);
	}

	closesocket(sock);
}
