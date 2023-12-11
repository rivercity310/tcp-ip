#include <winsock2.h>
#include <ws2tcpip.h>

#define SERVERPORT 9000
#define BUFSIZE    512

#pragma comment(lib, "ws2_32")

extern void DisplayText(const char* fmt, ...);
DWORD WINAPI ProcessClient(LPVOID arg);

void DisplayQuit(const char* msg)
{
	LPVOID lpMsgBuf;

	// 1. 오류 발생시 소켓 함수의 리턴값과 WSAGetLastError() 함수를 통해 구체적인 오류 코드를 얻을 수 있다.
	// 2. WSAGetLastError()가 리턴해주는 오류 코드에 대응하는 오류 메세지를 FormatMessage()를 통해 얻을 수 있다.
	// 3. WORD = unsigned short (2 byte), DWORD = unsigned long (4 byte)
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(char*)&lpMsgBuf,
		0,
		NULL
	);

	MessageBoxA(NULL, (const char*)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);    // exit 0 정상종료, 1 에러종료
}

// 소켓 함수 오류 출력
void DisplayError(const char* msg) {
	LPVOID lpMsgBuf;
	
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(char*)&lpMsgBuf, 0, NULL
	);
	
	DisplayText("[%s] %s\r\n", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

// 소켓 통신 스레드 함수
DWORD WINAPI ServerMain(LPVOID arg) {
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// 소켓 생성
	SOCKET listen_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listen_sock == INVALID_SOCKET) DisplayQuit("socket()");

	// bind
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) DisplayQuit("bind()");

	// listen
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) DisplayQuit("listen()");

	// 데이터 통신에 사용할 변수
	SOCKET client_sock;
	struct sockaddr_in clientaddr;
	int addrlen;
	HANDLE hThread;

	while (1) {
		// accept
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			DisplayError("accept()");
			break;
		}

		// 접속한 클라이언트 정보 출력
		char addr[INET_ADDRSTRLEN];
		inet_ntop(PF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
		DisplayText("\r\n[TCP Server] 클라이언트 접속: %s:%d\r\n", addr, htons(clientaddr.sin_port));

		// 스레드 생성
		hThread = (HANDLE)_beginthreadex(NULL, 0, ProcessClient,
			(LPVOID)client_sock, 0, NULL);

		if (hThread == NULL) closesocket(client_sock);
		else CloseHandle(hThread);
	}

	closesocket(listen_sock);

	WSACleanup();
	return 0;
}

DWORD WINAPI ProcessClient(LPVOID arg) {
	SOCKET client_sock = (SOCKET)arg;
	int retval;
	struct sockaddr_in clientaddr;
	char addr[INET_ADDRSTRLEN];
	int addrlen;
	char buf[BUFSIZE + 1];

	// 클라이언트 정보 얻기
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (struct sockaddr*)&clientaddr, &addrlen);
	inet_ntop(PF_INET, &clientaddr.sin_addr, addr, sizeof(addr));

	while (1) {
		retval = recv(client_sock, buf, BUFSIZE, 0);
		if (retval == SOCKET_ERROR) {
			DisplayError("recv()");
			break;
		}

		else if (retval == 0) {
			break;
		}

		// 받은 데이터 출력
		buf[retval] = '\0';
		DisplayText("[TCP/%s:%d] %s\r\n", addr, ntohs(clientaddr.sin_port), buf);

		// 데이터 보내기
		retval = send(client_sock, buf, retval, 0);
		if (retval == SOCKET_ERROR) {
			DisplayError("send()");
			break;
		}
	}

	closesocket(client_sock);
	DisplayText("[TCP Server] 클라이언트 종료: %s:%d\r\n", addr, ntohs(clientaddr.sin_port));
	return 0;
}

