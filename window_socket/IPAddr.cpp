#include "Common.h"

void IpAddr()
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		return;
	}

	/* IPv4 변환 연습 */
	const char* ipv4Test = "255.255.255.255";
	printf("[변환 전] IPv4 주소 = %s\n", ipv4Test);

	struct in_addr ipv4Num;
	inet_pton(AF_INET, ipv4Test, &ipv4Num);
	printf("[변환 후] IPv4 주소 = %#x\n", ipv4Num.s_addr);

	char ipv4Str[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &ipv4Num, ipv4Str, sizeof(ipv4Str));
	printf("[재변환] IPv4 주소 = %s\n", ipv4Str);
	printf("\n");

	/* IPv6 변환 연습 */
	const char* ipv6Test = "2001:0230:abcd:ffab:0023:eb00:ffff:1111";
	printf("[변환 전] IPv6 주소 = %s\n", ipv6Test);

	struct in6_addr ipv6Num;
	inet_pton(AF_INET6, ipv6Test, &ipv6Num);
	printf("[변환 후] IPv6 주소 = 0x");
	for (int i = 0; i < 16; i++) {
		printf("%02x", ipv6Num.s6_addr[i]);
	}
	printf("\n");

	char ipv6Str[INET6_ADDRSTRLEN];
	inet_ntop(AF_INET6, &ipv6Num, ipv6Str, sizeof(ipv6Str));
	printf("[재변환] IPv6 주소 = %s\n", ipv6Str);

	// WinSock 종료
	WSACleanup();
	exit(0);
}