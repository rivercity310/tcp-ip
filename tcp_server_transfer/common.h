#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#define SERVERPORT 9000

#pragma comment(lib, "ws2_32")

/* IPv6 전용 TCP 소켓 초기화 후 반환 (Listening 상태) */
SOCKET* InitServerSocket6(int* retval);

/* IPv4 전용 TCP 소켓 초기화 후 반환 (Listening 상태) */
SOCKET* InitServerSocket4(int* retval);

/* 에러 처리 함수 */
void err_quit(const char* msg);
void err_display(const char* msg);
