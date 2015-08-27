#include "windows.h"
#include "stdio.h"
#include "string.h"
#include "conio.h"

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

///////////////////////////////////////������涨һЩ�̶��ĳߴ磬���細�ڡ����������ڳ���ʼʱ��ֵ�����Ժ����ʹ��

int Screen_Width=0;    //��Ļ���
int Screen_Height=0;	//��Ļ�߶�

int Window_Width = 0;   ///���ڵĿ��
int Window_Height = 0;  ///���ڵĸ߶�

int Button_Width = 0;  ///��ť�Ŀ��
int Button_Height = 0;  ///��ť�ĸ߶�

int Button_xLocal = 0;  ///��һ����ť�ĺ����꣬�Ժ�İ�ť����Ϊ��׼
int Button_yLocal = 0;  ///��һ����ť��������

int ScrollBar_Width = 0;  ///�������Ŀ��
int ScrollBar_Height = 0;  ///�������ĸ߶�

int ScrollBar_xLocal = 0;  ///��һ���������ĺ����꣬�Ժ�İ�ť����Ϊ��׼
int ScrollBar_yLocal = 0;  ///��һ����������������


int Grid_Width=0;   ///128*32������Ŀ��

unsigned char Grid_Struct[32][128];  /////����Ľṹ�壬���λ��־�������ʾ״̬��

LOGBRUSH logbrush;   ///�����߼���ˢ
HBRUSH	 hBrush;	///��ˢ���

HANDLE hFile;    ///�ļ����

HWND			hwndButton1,hwndButton2;    ///��ť  
	
HWND 				hwndScroll1,hwndScroll2,hwndScroll3;  ///������


//////////////////////�ڵ�ǰ����λ�����һ�������Σ��ߴ�Ϊ����ߴ磬����ѡ����ε���ɫ
void Paint_GridRectangle(HDC hdc,int x,int y,long Color)
{
	LOGBRUSH Glogbrush1;  //��ʱ��ˢ
	LOGPEN  Glogpen1;    ///��ʱ����
	POINT Gpoint;         ///��ʱ��

	HPEN hGPen1,hGPen2;  //���ʾ��
	HBRUSH hGBrush1,hGBrush2;  ///��ʱ��ˢ���

	Glogbrush1.lbStyle=BS_SOLID;
	Glogbrush1.lbColor=RGB(Color>>16,(Color>>8)&0xff,Color&0xff);
	Glogbrush1.lbHatch=HS_VERTICAL;

	Gpoint.x=1;
	Gpoint.y=0;
	
	Color=(~Color)&0xffffff;

	Glogpen1.lopnStyle=PS_SOLID;
	Glogpen1.lopnWidth=Gpoint;
	Glogpen1.lopnColor=RGB(0xf0,(Color>>8)&0xff,Color&0xff);

	hGPen1=CreatePenIndirect(&Glogpen1); ///��������
	hGBrush1=CreateBrushIndirect(&Glogbrush1);  ///������ˢ

	hGPen2=SelectObject(hdc,hGPen1);      ///ѡ�񻭱�
	hGBrush2=SelectObject(hdc,hGBrush1);

	Rectangle(hdc,x,y,x+Grid_Width,y+Grid_Width);  ///������

	SelectObject(hdc,hGPen2);      ///��ԭ����
	SelectObject(hdc,hGBrush2);
	
	DeleteObject(hGPen1);
	DeleteObject(hGBrush1);
}

///////////////////////�ػ�ÿ��С���񣬸�������ṹ��
void Paint_Rectangle(HDC hdc)  ///�ػ�ÿ��С����	
{
	int x,y;
	for(x=0;x<32;x++)
	{
		for(y=0;y<128;y++)
		{
			if(Grid_Struct[x][y]&0x01)
			{
				Paint_GridRectangle(hdc,y*Grid_Width,x*Grid_Width,0x00);
			}
			else
				Paint_GridRectangle(hdc,y*Grid_Width,x*Grid_Width,0xffffff);
		}
	}
}

///////////////////�ڽ����ϰ벿�ֻ���128*32�����񣬹��Ժ�ʹ��
void Paint_Grid(HDC hdc)
{
	unsigned int Step;
	unsigned int Grid_Num1,Grid_Num2,Grid_Num3;
	Step=Window_Width/128;   ///128*32
	Grid_Num2=Step*32;	///�������λ��
	for(Grid_Num1=0;Grid_Num1<=128;Grid_Num1++)
	{
		MoveToEx(hdc,Grid_Num1*Step,0,NULL);   ///��ʼ��
		LineTo(hdc,Grid_Num1*Step,Grid_Num2);					///��ֹ��
	}
	Grid_Num2=Step*128; ///�������λ��
	for(Grid_Num1=0;Grid_Num1<=32;Grid_Num1++)
	{
		MoveToEx(hdc,0,Grid_Num1*Step,NULL);   ///��ʼ��
		LineTo(hdc,Grid_Num2,Grid_Num1*Step);					///��ֹ��
		
	}
}

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,PSTR szCmdLine,int iCmdShow)
{
	static TCHAR szAppName[] = TEXT("HelloWin");
	HWND 				hwnd;
	MSG	 				msg;
	WNDCLASS 			wndclass;
	
	int x,y;
	unsigned long z;
	logbrush.lbStyle		=	BS_SOLID;				///����ɫ��ˢ
	logbrush.lbColor		=	RGB(204,232,207);
	//logbrush.lbHatch		=	NULL;

	
	AllocConsole();    /////�½�һ������̨����
	freopen("CONOUT$","w",stdout);
	
	printf("����ʼ���С�����\n");
	
	hFile=CreateFile(".\\back.back",
			GENERIC_READ|GENERIC_WRITE,
			FILE_SHARE_READ|FILE_SHARE_WRITE,
			NULL,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL
			);
	if(hFile==INVALID_HANDLE_VALUE)  
	{
		printf("�½��ļ�ʧ��\n");
	}
	else 
	{
		if(GetLastError()==0)   ////�½����ļ�
		{
			printf("�½�back.back\n");
		
			for(x=0;x<32;x++)       ///�ṹ���ʼ��
				for(y=0;y<128;y++)
					Grid_Struct[x][y]=0;
		}
		else
		{
			printf("��back.back\n");
			SetFilePointer(hFile,0,0,FILE_BEGIN);
			ReadFile(hFile,
					Grid_Struct,
					128*32,
					&z,
					NULL
					);
		}
	}


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
	

	Screen_Width = GetSystemMetrics(SM_CXSCREEN);		///��ǰ��Ļ�ߴ�
	Screen_Height = GetSystemMetrics(SM_CYSCREEN);

	Window_Width = Screen_Width * 2 / 3;				///���ڵĳߴ�
	Window_Height = Screen_Height * 2 / 3;
	
	Button_Width = LOWORD(GetDialogBaseUnits());    ///��ť�ĳߴ�
	Button_Height = HIWORD(GetDialogBaseUnits());
	Button_Width *= 20;
	Button_Height =Button_Height*7/4;

	Button_xLocal = Window_Width-(Button_Width*3/2);  ///��һ����ť��λ������
	Button_yLocal = Window_Height/2;

	Grid_Width = Window_Width/128;     ////����Ŀ��	

	printf("�����ڵĳߴ�Ϊ:%i*%i\n",Window_Width,Window_Height);	
	
	ScrollBar_xLocal = Button_Width/3;
	ScrollBar_yLocal = Button_yLocal;
	ScrollBar_Width = Window_Width / 5;   ///�������ĳߴ�
	ScrollBar_Height = Window_Height / 20;

	hwnd = CreateWindow( szAppName,
						 TEXT("The Hello Program"),
						 WS_CAPTION|WS_SYSMENU|WS_VSCROLL,
						 (Screen_Width - Window_Width) / 2,
						 (Screen_Height - Window_Height) / 2,
						 Window_Width,
						 Window_Height,
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
	HINSTANCE		hInstance;

	
	unsigned int x,y;
	unsigned long z;
	static int vPos=0;
	char szScrollPos[100]; 

	switch(message)
	{
		case WM_CREATE:

			SetScrollRange(hwnd,SB_VERT,0,1000,TRUE);

			hInstance = (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE);

			hwndScroll1 = CreateWindow(TEXT("scrollbar"),
									  "Scroll",
									  WS_CHILD|SBS_HORZ|WS_VISIBLE,
									  ScrollBar_xLocal,
									  ScrollBar_yLocal,
									  ScrollBar_Width,
									  ScrollBar_Height,
									  hwnd,
									  (HMENU)10,
									  hInstance,
									  NULL);
			SetScrollRange(hwndScroll1,SB_CTL,0,1000,TRUE);
			
			printf("������һ��ˮƽ�������ؼ�\n");
			
			hwndButton1 = CreateWindow(TEXT("button"),
					TEXT("�����ʾ��"),
					WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
					Button_xLocal,Button_yLocal,
					Button_Width,Button_Height,
					hwnd,(HMENU)11,
					hInstance,NULL
					);
			hwndButton2 = CreateWindow(TEXT("button"),
					TEXT("����ͼ��"),
					WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
					Button_xLocal,Button_yLocal+Button_Height*3/2,
					Button_Width,Button_Height,
					hwnd,(HMENU)12,
					hInstance,NULL
					);

			
		return 0;
		
		case WM_LBUTTONDOWN:   ////����������
			x=LOWORD(lParam);
			y=HIWORD(lParam);
			x=x/Grid_Width;
			y=y/Grid_Width;
			if((x<128)&&(y<32))
			{
				hdc=GetDC(hwnd);
				if(Grid_Struct[y][x]&0x01)  //�ж���ʾ״̬
				{
					Paint_GridRectangle(hdc,x*Grid_Width,y*Grid_Width,0xffffff);
					Grid_Struct[y][x]&=~0x01;
				}
				else
				{
					Paint_GridRectangle(hdc,x*Grid_Width,y*Grid_Width,0x0);
					Grid_Struct[y][x]|=0x01;
				}	
				ReleaseDC(hwnd,hdc);
				Grid_Struct[y][x]|=0x10;

			}
		return 0;
		
		case WM_MOUSEMOVE:   ///����ƶ�
			if(wParam & MK_LBUTTON)
			{
				x=LOWORD(lParam);
				y=HIWORD(lParam);
				x=x/Grid_Width;
				y=y/Grid_Width;
				if((x<128)&&(y<32))
				{
					if((Grid_Struct[y][x]&0xf0)==0x00)   ///��һ�ξ���ʱ�Ż�ı�״̬
					{
						hdc=GetDC(hwnd);
						if(Grid_Struct[y][x]&0x01)  //�ж���ʾ״̬
						{
							Paint_GridRectangle(hdc,x*Grid_Width,y*Grid_Width,0xffffff);
							Grid_Struct[y][x]&=~0x01;
						}
						else
						{
							Paint_GridRectangle(hdc,x*Grid_Width,y*Grid_Width,0x0);
							Grid_Struct[y][x]|=0x01;
						}	
						ReleaseDC(hwnd,hdc);
						Grid_Struct[y][x]|=0x10;
					}
				}

			}
		return 0;

		case WM_COMMAND:
			if((LOWORD(wParam)==11)&&((HIWORD(wParam)==BN_CLICKED)))  ////�����ʾ����ť����
			{
				for(x=0;x<32;x++)
					for(y=0;y<128;y++)
						Grid_Struct[x][y]&=0xf0;
				hdc=GetDC(hwnd);
				Paint_Rectangle(hdc);  ///�ػ�ÿ��С����
				ReleaseDC(hwnd,hdc);

			}
		return 0;
		
		case WM_LBUTTONUP:
			for(x=0;x<32;x++)
				for(y=0;y<128;y++)
					Grid_Struct[x][y]&=0x0f;
		return 0;

		case WM_PAINT:
			hdc = BeginPaint(hwnd,&ps);

			GetClientRect(hwnd,&rect);
			sprintf(szScrollPos,"%d",vPos);

			DrawText(hdc,szScrollPos,strlen(szScrollPos),&rect,DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		//	Paint_Grid(hdc);  ////������	
			Paint_Rectangle(hdc);  ///�ػ�ÿ��С����
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

		case WM_HSCROLL:    ////ˮƽ������
			printf("ˮƽ�������ؼ��ж���\n");
			if(lParam==(int)hwndScroll1)
			{
				switch(LOWORD(wParam))
				{
					case SB_THUMBTRACK:	///�϶�
						vPos = HIWORD(wParam);
						printf("ˮƽ�������ؼ�λ�÷����ı䣬��λ��Ϊ:%i\n",vPos);
					break;

					case SB_LINERIGHT:  ///����һ��
						vPos = GetScrollPos(hwndScroll1,SB_CTL)+1;
						printf("���ƣ���λ��Ϊ:%i\n",vPos);
					break;

					case SB_LINELEFT:  ///����һ��
						vPos = GetScrollPos(hwndScroll1,SB_CTL)-1;
						printf("���ƣ���λ��Ϊ:%i\n",vPos);
					break;

					case SB_PAGERIGHT:  ///����һ��
						vPos = GetScrollPos(hwndScroll1,SB_CTL)+10;
						printf("����һ�Σ���λ��Ϊ:%i\n",vPos);
					break;

					case SB_PAGELEFT:  ///����һ��
						vPos = GetScrollPos(hwndScroll1,SB_CTL)-10;
						printf("����һ�Σ���λ��Ϊ:%i\n",vPos);
					break;
				
					default:

					break;
				}
				if(vPos != GetScrollPos(hwndScroll1,SB_CTL))
				{
					printf("ˮƽ�������ؼ�����λ��\n");
					hdc=GetDC(hwnd);
					GetClientRect(hwnd,&rect);
					sprintf(szScrollPos,"%d",vPos);
					
					SetScrollPos(hwndScroll1,SB_CTL,vPos,TRUE);
					DrawText(hdc,szScrollPos,strlen(szScrollPos),&rect,DT_SINGLELINE | DT_CENTER | DT_VCENTER);
			
					hdc=GetDC(hwnd);
					
				}
			}
		return 0;

		case WM_DESTROY:
			FreeConsole();    /////�ر�һ������̨����
			SetFilePointer(hFile,0,0,FILE_BEGIN);
			WriteFile(hFile,
					Grid_Struct,
					128*32,
					&z,
					NULL
					);
			printf("д�벢�ر��ļ�\n");
			CloseHandle(hFile);  ///�ر��ļ�
			PostQuitMessage(0);
		return 0;
	}
	

	return DefWindowProc(hwnd,message,wParam,lParam);
}
