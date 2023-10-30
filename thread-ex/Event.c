#include <windows.h>
#include <stdio.h>

#define BUFSIZE 10

static HANDLE hWriteEvent; /* 쓰기 완료 알림 이벤트 */
static HANDLE hReadEvent;  /* 읽기 완료 알림 이벤트 */
static buf[BUFSIZE];

// 1. Thread1은 공유 버퍼에 데이터 쓰기 작업을 한다.
// 2. Thread2, Thread3은 공유 버퍼에 쓰여진 데이터를 읽는다.
// 3. Thread2와 Thread3 중 하나의 Thread만 공유 버퍼에 접근이 가능하다.
// 4. 버퍼를 읽은 Thread의 작업이 끝나야 Thread1이 다시 쓰기 작업을 할 수 있다.

static DWORD WINAPI WriteThread(LPVOID arg) {
	DWORD retval;

	for (int k = 1; k <= 500; k++) {
		// 읽기 완료 대기(= 읽기 Thread가 신호 상태로 변경되면 Wakeup)
		retval = WaitForSingleObject(hReadEvent, INFINITE);
		if (retval != WAIT_OBJECT_0) break;

		// 공유 버퍼에 데이터 쓰기
		for (int i = 0; i < BUFSIZE; i++) {
			buf[i] = k;
		}

		// 쓰기 완료 알림
		SetEvent(hWriteEvent);
	}

	return 0;
}

static DWORD WINAPI ReadThread(LPVOID arg) {
	DWORD retval;

	while (1) {
		// 쓰기 완료 대기(= 쓰기 Thread가 신호 상태로 변경되면 Wakeup)
		retval = WaitForSingleObject(hWriteEvent, INFINITE);
		if (retval != WAIT_OBJECT_0) break;

		// 읽은 데이터를 출력하고 버퍼 초기화
		// 버퍼 초기화: 데이터를 쓰지 않은 상태에서 읽을 시 0이 읽히게 만들어 오류 여부 확인을 위함
		printf("Thread %4d:\t", GetCurrentThreadId());
		for (int i = 0; i < BUFSIZE; i++) printf("%3d ", buf[i]);
		printf("\n");
		memset(buf, 0, BUFSIZE);

		// 읽기 완료 알림
		SetEvent(hReadEvent);
	} 

	return 0;
}

void EventEx() {
	// 이벤트 생성
	hWriteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	hReadEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	if (hWriteEvent == NULL || hReadEvent == NULL) {
		return 1;
	}

	// Thread 3개 생성
	HANDLE hThread[3];
	hThread[0] = CreateThread(NULL, 0, WriteThread, NULL, 0, NULL);
	hThread[1] = CreateThread(NULL, 0, ReadThread, NULL, 0, NULL);
	hThread[2] = CreateThread(NULL, 0, ReadThread, NULL, 0, NULL);

	// 읽기 완료 알림(= 읽기 이벤트를 신호 상태로 변경)
	SetEvent(hReadEvent);

	// Thread 3개 종료 대기
	WaitForMultipleObjects(3, hThread, TRUE, INFINITE);

	// 이벤트 제거
	CloseHandle(hWriteEvent);
	CloseHandle(hReadEvent);

	return 0;
}
