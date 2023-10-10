#define _WINSOCK_DEPRECATED_NO_WARNINGS  // 구형 소켓 API 사용 시 경고 끄기 
#define _CRT_SECURE_NO_WARNINGS // 구형 C 함수 사용 시 경고 끄기

#include <winsock2.h>     // WinSock 메인 헤더
#include <ws2tcpip.h>     // WinSock 확장 헤더

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>

void err_quit(const char* msg);
void err_display(const char* msg);
void err_display(int errorcode);