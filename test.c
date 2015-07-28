#include "windows.h"

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

int Screen_Width=0;    //ÆÁÄ»¿í¶È
int Screen_Height=0;	//ÆÁÄ»¸ß¶È

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,PSTR szCmdLine,int iCmdShow)
{
	static TCHAR szAppName[] = TEXT("HelloWin");
	HWND 				hwnd;
	MSG	 				msg;
	WNDCLASS 			wndclass;

	wndclass.style			=	CS_HREDRAW|CS_VREDRAW;
	wndclass.lpfnWndProc	=	WndProc;
	wndclass.cbClsExtra		=	0;
	wndclass.cbWndExtra		=	0;
	wndclass.hInstance		=	hInstance;
	wndclass.hIcon			= 	LoadIcon(NULL,IDI_APPLICATION);
	wndclass.hCursor		=	LoadCursor(NULL,IDC_ARROW);
	wndclass.hbrBackground	=	(HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName	=	NULL;
	wndclass.lpszClassName	=	szAppName;

	if(!RegisterClass(&wndclass))
	{
		MessageBox(NULL,TEXT("This program requires Windows NT!"),
			szAppName,MB_ICONERROR);
		return 0;
	}

	Screen_Width = GetSystemMetrics(SM_CXSCREEN);
	Screen_Height = GetSystemMetrics(SM_CYSCREEN);

	hwnd = CreateWindow( szAppName,
						 TEXT("The Hello Program"),
						 WS_CAPTION|WS_SYSMENU,
						 Screen_Width/4,
						 Screen_Height/4,
						 Screen_Width/2,
						 Screen_Height/2,
						 NULL,
						 NULL,
						 hInstance,
						 NULL);

	ShowWindow(hwnd,iCmdShow);
	UpdateWindow(hwnd);

	while(GetMessage(&msg,NULL,0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	HDC				hdc;
	PAINTSTRUCT		ps;
	RECT			rect;

	switch(message)
	{
		case WM_CREATE:

		return 0;

		case WM_PAINT:
			hdc = BeginPaint(hwnd,&ps);
			//GetClientRect(hwnd,&rect);
			EndPaint(hwnd,&ps);
		return 0;

		case WM_DESTROY:
			PostQuitMessage(0);
		return 0;
	}
	

	return DefWindowProc(hwnd,message,wParam,lParam);
}