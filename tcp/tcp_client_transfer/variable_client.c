#include "common.h"

#define BUFSIZE    50
#define CLIENTPORT 50000

void VariableTcpClient4() {
	int retval;
	SOCKET sock = *InitClientSocket4(&retval, CLIENTPORT);
	
	// 데이터 통신에 사용할 변수
	int len, i;
	char buf[BUFSIZE];
	const char* testdata[] = {
		"안녕하세요",
		"반가워요",
		"오늘따라 할 이야기가 많을 것 같네요",
		"저도 그렇네요"
	};

	for (i = 0; i < 4; i++) {
		len = (int)strlen(testdata[i]);
		strncpy(buf, testdata[i], len);
		buf[len++] = '\n';
	
		// 데이터 전송
		retval = send(sock, buf, len, 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
			break;
		}

		printf("[TCP CLIENT] %d바이트를 전송했습니다.\n", retval);
	}

	closesocket(sock);
	return;
}
