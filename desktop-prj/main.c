#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>
#include <tchar.h>

#define SERVERPORT 9000
#define BUFSIZE    512

// 윈도우 프로시저
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// 에디트 컨트롤 출력 함수
void DisplayText(const char* fmt, ...);

// 소켓 함수 오류 출력
void DisplayError(const char* msg);

// 소켓 통신 스레드 함수
DWORD WINAPI ServerMain(LPVOID arg);
DWORD WINAPI ProcessClient(LPVOID arg);


HINSTANCE hInst;      // 인스턴스 핸들
HWND hEdit;           // 에디트 컨트롤
CRITICAL_SECTION cs;  // 임계 영역


int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow
) {
	hInst = hInstance;
	InitializeCriticalSection(&cs);

	// 윈도우 클래스 등록
	WNDCLASS wndclass;
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = _T("MyWndClass");
	if (!RegisterClass(&wndclass)) return 1;

	// 윈도우 생성
	HWND hWnd = CreateWindow(
		_T("MyWndClass"),
		_T("WinApp"),
		WS_OVERLAPPEDWINDOW,
		0, 0, 500, 220, 
		NULL, NULL, 
		hInstance, NULL
	);

	if (hWnd == NULL) return 1;
	
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// 소켓 통신 스레드 생성
	CreateThread(NULL, 0, ServerMain, NULL, 0, NULL);

	// 메세지 루프
	MSG msg;
	while (GetMessage(&msg, 0, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	DeleteCriticalSection(&cs);
	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(
	HWND hWnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam
) {
	switch (uMsg) {
	case WM_CREATE:
		hEdit = CreateWindow(_T("edit"), NULL,
			WS_CHILD | WS_VISIBLE | WS_HSCROLL |
			WS_VSCROLL | ES_AUTOHSCROLL |
			ES_AUTOVSCROLL | ES_MULTILINE | ES_READONLY,
			0, 0, 0, 0, hWnd, (HMENU)100, hInst, NULL
		);

		DisplayText("간단한 GUI 응용 프로그램입니다.\r\n");
		DisplayText("인스턴스 핸들값은 %#x입니다.\r\n", hInst);

		return 0;

	case WM_SIZE:
		MoveWindow(hEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
		Sleep(3000);
		return 0;

	case WM_SETFOCUS:
		SetFocus(hEdit);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// 에디트 컨트롤 출력 함수
void DisplayText(const char* fmt, ...) {
	va_list arg;
	va_start(arg, fmt);
	char cbuf[BUFSIZE * 2];
	vsprintf(cbuf, fmt, arg);
	va_end(arg);

	EnterCriticalSection(&cs);

	int nLength = GetWindowTextLength(hEdit);
	SendMessage(hEdit, EM_SETSEL, nLength, nLength);
	SendMessageA(hEdit, EM_REPLACESEL, FALSE, (LPARAM)cbuf);

	LeaveCriticalSection(&cs);
}

// 소켓 함수 오류 출력

