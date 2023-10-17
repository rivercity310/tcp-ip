#include "Common.h"

static bool isLittleEndian() {
	u_short x1 = 0x1234;
	u_short x2 = htons(x1);

	return x1 != x2;
}

void ByteOrder()
{
	u_short x1 = 0x1234;
	u_long y1 = 0x12345678;
	u_short x2;
	u_long y2;

	// Host Byte -> Network Byte (빅 엔디언 고정)
	printf("[Host Byte -> Network Byte]\n");
	printf("%#x -> %#x\n", x1, x2 = htons(x1));
	printf("%#x -> %#x\n", y1, y2 = htonl(y1));

	printf("\n[잘못된 사용 예시]\n");
	printf("%#x -> %#x\n", x1, htonl(x1));

	u_short x3 = 0xABCD;
	u_short x4;

	printf("\n[ex1] %#X -> %#X\n", x3, x4 = htons(x3));
	printf("\nisLittleEndian = %d\n", isLittleEndian());
}

