#pragma once

#include <winsock2.h>     // WinSock 메인 헤더
#include <ws2tcpip.h>     // WinSock 확장 헤더

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>

void err_quit(const char* msg);
void err_display(const char* msg);