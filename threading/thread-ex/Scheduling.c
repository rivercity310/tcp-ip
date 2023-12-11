#include <windows.h>
#include <stdio.h>

DWORD WINAPI MyThread3(LPVOID arg) {
	while (1);
	return 0;
}

void SchedulingEx() {
	// 우선순위 값(스레드 우선순위, 우선순위 레벨) 범위 출력
	printf("Priority: %d ~ %d\n", THREAD_PRIORITY_IDLE, THREAD_PRIORITY_TIME_CRITICAL);

	// CPU 개수 알아내기
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	printf("CPU 개수: %d\n", (int)si.dwNumberOfProcessors);

	// CPU 개수만큼 스레드 생성
	for (int i = 0; i < (int)si.dwNumberOfProcessors; i++) {
		HANDLE hThread = CreateThread(NULL, 0, MyThread3, NULL, 0, NULL);
		if (hThread == NULL) return 1;

		// Main Thread보다 우선 순위를 높인다.
		SetThreadPriority(hThread, THREAD_PRIORITY_ABOVE_NORMAL);
		CloseHandle(hThread);
	}

	// Main Thread의 우선 순위를 낮게 설정한다
	// 기아(Starvation) 현상이 발생하는지 체크 
	// -> Windows CPU Scheduling에서는 실행기회를 얻지 못한 스레드의 우선순위를 단계적으로 끌어올림
	// -> 결과적으로 Main Thread가 CPU를 얻을 때까지 잠시 콘솔이 먹통이 되었다가, Main Thread가 실행 기회를 얻으면 정상적으로 함수가 실행된 후 종료된다.
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
	Sleep(1000);
	printf("Main Thread 실행!\n");
	return 0;
}