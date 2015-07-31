#include "windows.h"
#include "stdio.h"
#include "string.h"

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

int Screen_Width=0;    //ÆÁÄ»¿í¶È
int Screen_Height=0;	//ÆÁÄ»¸ß¶È

LOGBRUSH logbrush;   ///¶¨ÒåÂß¼­»­Ë¢
HBRUSH	 hBrush;	///»­Ë¢¾ä±ú

static HWND 				hwndScroll;

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,PSTR szCmdLine,int iCmdShow)
{
	static TCHAR szAppName[] = TEXT("HelloWin");
	HWND 				hwnd;
	MSG	 				msg;
	WNDCLASS 			wndclass;

	logbrush.lbStyle		=	BS_SOLID;				///±£»¤É«»­Ë¢
	logbrush.lbColor		=	RGB(204,232,207);
	//logbrush.lbHatch		=	NULL;

	hBrush	=	CreateBrushIndirect(&logbrush);

	wndclass.style			=	CS_HREDRAW|CS_VREDRAW;
	wndclass.lpfnWndProc	=	WndProc;
	wndclass.cbClsExtra		=	0;
	wndclass.cbWndExtra		=	0;
	wndclass.hInstance		=	hInstance;
	wndclass.hIcon			= 	LoadIcon(NULL,IDI_APPLICATION);
	wndclass.hCursor		=	LoadCursor(NULL,IDC_ARROW);
	wndclass.hbrBackground	=	hBrush;   //(HBRUSH)GetStockObject(WHITE_BRUSH);
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
						 WS_CAPTION|WS_SYSMENU|WS_VSCROLL,
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

	static int vPos=0;
	char szScrollPos[100]; 

	switch(message)
	{
		case WM_CREATE:
			SetScrollRange(hwnd,SB_VERT,0,1000,TRUE);

			hwndScroll = CreateWindow(TEXT("scrollbar"),
									  "Scroll",
									  WS_CHILD|SBS_VERT|WS_VISIBLE,
									  0,
									  0,
									  0,
									  0,
									  hwnd,
									  (HMENU)10,
									  NULL,
									  NULL
					
			);
		return 0;

		case WM_PAINT:
			hdc = BeginPaint(hwnd,&ps);

			GetClientRect(hwnd,&rect);
			sprintf(szScrollPos,"%d",vPos);

			DrawText(hdc,szScrollPos,strlen(szScrollPos),&rect,DT_SINGLELINE | DT_CENTER | DT_VCENTER);
			
			
			EndPaint(hwnd,&ps);
		return 0;

		case WM_VSCROLL:
			switch(LOWORD(wParam))
			{
				case SB_THUMBTRACK:
					vPos = HIWORD(wParam);
				break;

				default:

				break;
			}
			if(vPos != GetScrollPos(hwnd,SB_VERT))
			{
				SetScrollPos(hwnd,SB_VERT,vPos,TRUE);
				InvalidateRect(hwnd,NULL,TRUE);
			}
			
		return 0;

		case WM_DESTROY:
			PostQuitMessage(0);
		return 0;
	}
	

	return DefWindowProc(hwnd,message,wParam,lParam);
}