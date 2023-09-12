#define _WINSOCK_DEPRECATED_NO_WARNINGS  // 구형 소켓 API 사용 시 경고 끄기 
#define _CRT_SECURE_NO_WARNINGS // 구형 C 함수 사용 시 경고 끄기

#include "Common.h"

#define TESTNAME "www.naver.com"

extern void err_display(const char*);

/* 도메인 이름 -> IPv4 주소 */
bool getIPAddr(const char* name, struct in_addr* addr)
{
	struct hostent* ptr = gethostbyname(name);
	if (ptr == NULL) {
		err_display("gethostbyname()");
		return false;
	}

	if (ptr->h_addrtype != AF_INET) {
		return false;
	}

	memcpy(addr, ptr->h_addr, ptr->h_length);
	return true;
}

/* IPv4 주소 -> 도메인 이름 */
bool getDomainName(struct in_addr addr, char* name, int namelen) 
{
	struct hostent* ptr = gethostbyaddr((const char*)&addr, sizeof(addr), AF_INET);
	if (ptr == NULL) {
		err_display("getDomainName()");
		return false;
	}

	if (ptr->h_addrtype != AF_INET) {
		return false;
	}

	strncpy(name, ptr->h_name, namelen);
	return true;
}

void DNSTest()
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		return;
	}

	printf("[변환 전] 도메인 이름 = %s\n", TESTNAME);
	
	// 도메인 이름 -> IP 주소
	struct in_addr addr;
	if (getIPAddr(TESTNAME, &addr)) {
		char str[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &addr, str, sizeof(str));    // numeric(network) to presentation
		printf("[변환 후] IP 주소 = %s\n", str);

		// IP 주소(Network Byte Address) -> 도메인 이름
		char name[256];
		if (getDomainName(addr, name, sizeof(name))) {
			printf("[재변환] 도메인 이름 = %s\n", name);
		}
	}

	WSACleanup();

}