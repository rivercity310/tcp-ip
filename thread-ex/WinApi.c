#include <windows.h>
#include <stdio.h>

struct Point3D {
	int x, y, z;
};

DWORD WINAPI MyThread(LPVOID arg) {
	Sleep(1000);
	struct Point3D* pt = (struct Point3D*)arg;
	printf("Running MyThread() %d: %d, %d, %d\n", GetCurrentThreadId(), pt->x, pt->y, pt->z);
	return 0;
}

unsigned __stdcall MyThread2(void* arg) {
	Sleep(1500);
	struct Point3D* pt = (struct Point3D*)arg;
	printf("Running MyThread2() %d: %d, %d, %d\n", GetCurrentThreadId(), pt->x, pt->y, pt->z);
	_endthreadex(0);
	return 0;
}

void WinApiEx() {
	// 첫번째 스레드 생성
	struct Point3D pt1 = { 10, 20, 30 };
	HANDLE hThread1 = CreateThread(NULL, 0, MyThread, &pt1, 0, NULL);
	if (hThread1 == NULL) return 1;
	CloseHandle(hThread1);

	// 두번째 스레드 생성
	struct Point3D pt2 = { 40, 50, 60 };
	HANDLE hThread2 = CreateThread(NULL, 0, MyThread, &pt2, 0, NULL);
	if (hThread2 == NULL) return 1;
	CloseHandle(hThread2);

	printf("Running main() %d\n", GetCurrentThreadId());
	Sleep(10000);
	return 0;
}

void WinApiEx2() {
	struct Point3D pt1 = { 15, 25, 35 };
	HANDLE hThread1 = (HANDLE)_beginthreadex(NULL, 0, MyThread2, &pt1, 0, NULL);
	if (hThread1 == NULL) return 1;
	CloseHandle(hThread1);

	struct Point3D pt2 = { 45, 55, 65 };
	HANDLE hThread2 = (HANDLE)_beginthreadex(NULL, 0, MyThread2, &pt2, 0, NULL);
	if (hThread2 == NULL) return 1;
	CloseHandle(hThread2);

	printf("Running main() %d\n", GetCurrentThreadId());
	Sleep(5000);
	return 0;
}
