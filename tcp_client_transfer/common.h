#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <string.h>
#include <ws2tcpip.h>
#include <winsock2.h>

#define SERVERIP4  "127.0.0.1"
#define SERVERIP6  "::1"
#define SERVERPORT 9000

#pragma comment(lib, "ws2_32")

/* IPv4 TCP Client Socket */
SOCKET* InitClientSocket4(int* retval, int clientport);

/* IPv6 TCP Client Socket */
SOCKET* InitClientSocket6(int* retval, int clientport);
