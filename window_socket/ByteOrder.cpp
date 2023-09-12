#include "Common.h"

void ByteOrder()
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		return;
	}
	
	u_short x1 = 0x1234;
	u_long y1 = 0x12345678;
	u_short x2;
	u_long y2;

	// Host Byte -> Network Byte
	printf("[Host Byte -> Network Byte]\n");
	printf("%#x -> %#x\n", x1, x2 = htons(x1));
	printf("%#x -> %#x\n", y1, y2 = htonl(y1));

	printf("\n[잘못된 사용 예시]\n");
	printf("%#x -> %#x\n", x1, htonl(x1));

	WSACleanup();
}