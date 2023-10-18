#include "common.h"

#define BUFSIZE     50

void CloseOnTransferClient6() {
	int retval;
	
	// 변수 & 버퍼
	int i, len;
	char buf[BUFSIZE];
	const char* testdata[] = {
		"안녕하세요",
		"반가워요",
		"저도 그렇네요"
	};

	// 서버와 데이터 통신
	for (i = 0; i < 3; i++) {
		SOCKET sock = *InitClientSocket6(&retval, NULL);

		len = (int)strlen(testdata[i]);
		strncpy(buf, testdata[i], len);
		
		// 데이터 전송
		retval = send(sock, buf, len, 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
			break;
		}

		printf("[TCP CLIENT] %d바이트 전송\n", retval);
		closesocket(sock);
	}
}