#include <windows.h>
#include <stdio.h>

#define MAXCNT 10000000

static int count = 0;

static DWORD WINAPI Thread1(LPVOID arg) {
	for (int i = 0; i < MAXCNT; i++) {
		count += 2;
	}

	printf("Thread ID: %d, count: %d\n", GetCurrentThreadId(), count);

	return 0;
}

static DWORD WINAPI Thread2(LPVOID arg) {
	for (int i = 0; i < MAXCNT; i++) {
		count -= 2;
	}
	
	printf("Thread ID: %d, count: %d\n", GetCurrentThreadId(), count);

	return 0;
}

// 공유 자원 접근시 문제 발생
void CriticalSectionEx() {
	HANDLE hThread[2];
	hThread[0] = (HANDLE)_beginthreadex(NULL, 0, Thread1, NULL, 0, NULL);
	hThread[1] = (HANDLE)_beginthreadex(NULL, 0, Thread2, NULL, 0, NULL);
	WaitForMultipleObjects(2, hThread, TRUE, INFINITE);

	printf("count = %d\n", count);
	return 0;
}

// ---

// 일반 동기화 객체(커널 메모리 생성)과 달리, 임계영역은 사용자 메모리에 생성
// 일반적으로 한 프로세스 내 스레드에서만 적용 가능하지만, 공유 메모리 사용사 서로 다른 프로세스간 임계영역 공유 가능
static CRITICAL_SECTION cs;

static DWORD WINAPI Thread3(LPVOID arg) {
	EnterCriticalSection(&cs);
	
	for (int i = 0; i < MAXCNT; i++) {
		count += 2;
	}

	LeaveCriticalSection(&cs);
	
	return 0;
}

static DWORD WINAPI Thread4(LPVOID arg) {
	EnterCriticalSection(&cs);
	
	for (int i = 0; i < MAXCNT; i++) {
		count -= 2;
	}

	LeaveCriticalSection(&cs);

	return 0;
}

void CriticalSectionEx2() {
	// 임계 영역 초기화
	InitializeCriticalSection(&cs);

	HANDLE hThread[2];

	// Create*() 함수는 커널 메모리 영역에 동기화 객체를 생성하고, 이에 접근할 수 있는 핸들을 리턴한다.
	hThread[0] = CreateThread(NULL, 0, Thread3, NULL, 0, NULL);
	hThread[1] = CreateThread(NULL, 0, Thread4, NULL, 0, NULL);

	//스레드 두개 종료 대기
	WaitForMultipleObjects(2, hThread, TRUE, INFINITE);
	
	// 임계 영역 삭제
	DeleteCriticalSection(&cs);

	printf("count = %d\n", count);
	return 0;
}
