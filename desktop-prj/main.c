#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>
#include <tchar.h>

#define SERVERPORT 9000
#define BUFSIZE    512

// extern
extern DWORD WINAPI ServerMain(LPVOID arg);
extern DWORD WINAPI ProcessClient(LPVOID arg);

// Critical Section
CRITICAL_SECTION cs;

// 윈도우 프로시저
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// 에디트 컨트롤 출력 함수
void DisplayText(const char* fmt, ...);

HINSTANCE hInst;      // 인스턴스 핸들: 여러 함수에서 쓰기 위해 WinMain의 hInstance 값을 전역 변수에 담는다.
HWND hEdit;           // 에디트 컨트롤

// 콘솔 응용 프로그램의 main() 함수와 같은 기능을 하는 GUI 응용 프로그램의 실행 시작점
int WINAPI WinMain(
	HINSTANCE hInstance,		// 인스턴스 핸들: 실행 파일에 포함된 각종 리소스에 접근할 때 사용
	HINSTANCE hPrevInstance,	// 16비트 윈도우 시절 잔재, 현재는 사용 X
	LPSTR lpCmdLine,			// 프로그램 실행 시 전달된 명령행 인수를 담고 있는 문자열
	int nCmdShow				// 프로그램이 시작할 때 메인 윈도우를 보일 방법을 제어(최대화, 최소화 등)
) {

	hInst = hInstance;
	InitializeCriticalSection(&cs);

	// 윈도우 클래스 등록
	// - Window Class : 윈도우의 다양한 특성을 정의하는 구조체, 원하는 값으로 초기화한 후 RegisterClass() 함수로 운영체제에 등록
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
	// - 위에서 등록한 Window Class를 기반으로 실제 윈도우 생성
	// - CreateWindow() 함수는 윈도우를 생성한 후 윈도우 핸들(HWND 타입)을 리턴
	// - 윈도우 핸들을 다양한 API 함수에 전달함으로써 윈도우를 자유롭게 조작할 수 있음
	// - 생성한 윈도우는 처음에 보이지 않으므로, ShowWindow(), UpdateWindow()를 차례로 호출하여 화면에 보이게 함
	HWND hWnd = CreateWindow(
		_T("MyWndClass"),		// _T 메크로: tchar.h 헤더에서 제공, 컴파일러 설정에 따라 ANSI 또는 유니코드 문자열로 자동 정의, 윈도우 API 함수에 문자열 전달시 사용
		_T("WinApp"),
		WS_OVERLAPPEDWINDOW,
		0, 0, 500, 220, 
		NULL, NULL, 
		hInstance, NULL
	);
	if (hWnd == NULL) return 1;
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	_beginthreadex(NULL, 0, ServerMain, NULL, 0, NULL);

	// 메세지 루프
	// - GetMessage() : 응용 프로그램에 할당된 메세지 큐에서 메세지를 꺼냄
	// - TranslateMessage() : 키보드 메시지이면 특별한 처리를 한다.
	// - DispatchMessage() : 해당 메시지를 윈도우 프로시저에 전달 
	MSG msg;
	while (GetMessage(&msg, 0, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	DeleteCriticalSection(&cs);
	return (int)msg.wParam;
}


// 윈도우 프로시저: 윈도우 메시지를 처리하는 핵심 함수 
// - WM_CREATE: 윈도우가 생성되었을 때 발생하는 메시지 처리
// - WM_SIZE: 크기가 변경되었을 때 발생하는 메시지 처리
// - WM_DESTROY: 종료 버튼을 눌렀을 때 발생하는 메시지 처리
// 응용 프로그램이 처리하지 않은 메시지는 DefWindowProc() 함수에 전달하여 윈도우 운영체제가 알아서 처리하게 한다.
LRESULT CALLBACK WndProc(
	HWND hWnd,		// 메시지가 발생한 윈도우를 나타내는 핸들값
	UINT uMsg,		// 발생한 메시지를 나타내는 값으로, WM_* 형태의 상수로 정의되어 있다.
	WPARAM wParam,
	LPARAM lParam
) {
	switch (uMsg) {
	case WM_CREATE:		// WM_CREATE: 메인 윈도우 생성될 때 발생. 각종 초기화 작업 수행에 적합(동적 메모리 할당, 자식 윈도우 생성, 파일 열기 등) 
		hEdit = CreateWindow(_T("edit"), NULL,
			WS_CHILD | WS_VISIBLE | WS_HSCROLL |			// WS_* 상수: 일반 윈도우와 컨트롤에 모두 적용할 수 있는 공통 스타일
			WS_VSCROLL | ES_AUTOHSCROLL |					// ES_* 상수: 에디트 컨트롤에만 적용할 수 있는 스타일
			ES_AUTOVSCROLL | ES_MULTILINE | ES_READONLY,
			0, 0, 0, 0, hWnd, (HMENU)100, hInst, NULL
		);

		return 0;

	case WM_SIZE:		// WM_SIZE: 메인 윈도우의 크기가 변경될 때마다 발생. lParam의 하위 16비트와 상위 16비트에는 각각 클라이언트 영역의 폭과 높이가 들어 있다.
		MoveWindow(hEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
		return 0;

	case WM_SETFOCUS:	// WM_SETFOCUS: 메인 윈도우가 키보드 포커스를 얻을 때 발생. SetFocus() 함수를 통해 메인 윈도우의 키보드 포커스를 다른 윈도우로 전환
		SetFocus(hEdit);
		return 0;

	case WM_DESTROY:	// WM_DESTROY: 메인 윈도우를 닫을 때 발생. 각종 청소 작업 수행(동적 메모리 해제, 파일 닫기 등)
		PostQuitMessage(0);	 // PostQuitMessage(): 응용 프로그램 메세지 큐에 WM_QUIT 메시지를 넣음으로써 GetMessage() 함수가 0을 리턴하게 한다.
		return 0;
	}

	// 응용 프로그램이 처리하지 않은 메시지는 DefWindowProc() 함수에 전달하여 윈도우 운영체제가 알아서 처리하게 한다.
	return DefWindowProc(hWnd, uMsg, wParam, lParam);	
}


// 에디트 컨트롤에 출력을 수행하는 함수
void DisplayText(const char* fmt, ...) {
	// va_list 타입과 va_start(), va_end()는 가변 길이 인수의 지원을 위해 표준 C 라이브러리 함수에서 제공
	// 에디트 컨트롤에 최종 출력할 문자열을 vsprintf() 함수를 이용하여 cbuf[] 배열에 미리 출력해둔다.
	va_list arg;
	va_start(arg, fmt);
	char cbuf[BUFSIZE * 2];
	vsprintf(cbuf, fmt, arg);
	va_end(arg);

	EnterCriticalSection(&cs);

	// GetWindowTextLength() 함수로 에디트 컨트롤에 들어 있는 문자열의 길이를 얻음.
	// 132행은 cbuf[]에 들어있는 문자열을 윈도우 API에 전달하므로 ANSI 함수 SendMessageA() 함수를 사용해야 한다.
	int nLength = GetWindowTextLength(hEdit);
	SendMessage(hEdit, EM_SETSEL, nLength, nLength);
	SendMessageA(hEdit, EM_REPLACESEL, FALSE, (LPARAM)cbuf);

	LeaveCriticalSection(&cs);
}
