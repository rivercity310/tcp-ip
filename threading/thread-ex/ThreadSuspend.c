#include <windows.h>
#include <stdio.h>

static int sum = 0;

DWORD WINAPI MyThread4(LPVOID arg) {
	int num = (int)(long long)arg;
	for (int i = 1; i <= num; i++)
		sum += i;
	return 0;
}

void ThreadSuspendEx() {
	int num = 100;
	HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, MyThread4, (LPVOID)(long long)num, CREATE_SUSPENDED, NULL);

	printf("스레드 실행 전: %d\n", sum);
	ResumeThread(hThread);

	WaitForSingleObject(hThread, INFINITE);

	printf("스레드 실행 후: %d\n", sum);
	CloseHandle(hThread);
	return 0;
}