#include "windows.h"
#include "stdio.h"
#include "string.h"
#include "conio.h"

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

///////////////////////////////////////在这里规定一些固定的尺寸，比如窗口、滚动条，在程序开始时赋值，供以后程序使用

int Screen_Width=0;    //屏幕宽度
int Screen_Height=0;	//屏幕高度

int Window_Width = 0;   ///窗口的宽度
int Window_Height = 0;  ///窗口的高度

int Button_Width = 0;  ///按钮的宽度
int Button_Height = 0;  ///按钮的高度

int Button_xLocal = 0;  ///第一个按钮的横坐标，以后的按钮以他为基准
int Button_yLocal = 0;  ///第一个按钮的纵坐标

int ScrollBar_Width = 0;  ///滚动条的宽度
int ScrollBar_Height = 0;  ///滚动条的高度

int ScrollBar_xLocal = 0;  ///第一个滚动条的横坐标，以后的按钮以他为基准
int ScrollBar_yLocal = 0;  ///第一个滚动条的纵坐标


int Grid_Width=0;   ///128*32的网格的宽度

unsigned char Grid_Struct[32][128];  /////网格的结构体，最低位标志网格的显示状态。

LOGBRUSH logbrush;   ///定义逻辑画刷
HBRUSH	 hBrush;	///画刷句柄

HANDLE hFile;    ///文件句柄

HWND			hwndButton1,hwndButton2;    ///按钮  
	
HWND 				hwndScroll1,hwndScroll2,hwndScroll3;  ///滚动条


//////////////////////在当前坐标位置填充一个正方形，尺寸为网格尺寸，可以选择矩形的颜色
void Paint_GridRectangle(HDC hdc,int x,int y,long Color)
{
	LOGBRUSH Glogbrush1;  //临时画刷
	LOGPEN  Glogpen1;    ///临时画笔
	POINT Gpoint;         ///临时点

	HPEN hGPen1,hGPen2;  //画笔句柄
	HBRUSH hGBrush1,hGBrush2;  ///临时画刷句柄

	Glogbrush1.lbStyle=BS_SOLID;
	Glogbrush1.lbColor=RGB(Color>>16,(Color>>8)&0xff,Color&0xff);
	Glogbrush1.lbHatch=HS_VERTICAL;

	Gpoint.x=1;
	Gpoint.y=0;
	
	Color=(~Color)&0xffffff;

	Glogpen1.lopnStyle=PS_SOLID;
	Glogpen1.lopnWidth=Gpoint;
	Glogpen1.lopnColor=RGB(0xf0,(Color>>8)&0xff,Color&0xff);

	hGPen1=CreatePenIndirect(&Glogpen1); ///创建画笔
	hGBrush1=CreateBrushIndirect(&Glogbrush1);  ///创建画刷

	hGPen2=SelectObject(hdc,hGPen1);      ///选择画笔
	hGBrush2=SelectObject(hdc,hGBrush1);

	Rectangle(hdc,x,y,x+Grid_Width,y+Grid_Width);  ///填充矩形

	SelectObject(hdc,hGPen2);      ///还原画笔
	SelectObject(hdc,hGBrush2);
	
	DeleteObject(hGPen1);
	DeleteObject(hGBrush1);
}

///////////////////////重绘每个小方格，根据网格结构体
void Paint_Rectangle(HDC hdc)  ///重绘每个小方格	
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

///////////////////在界面上半部分画出128*32的网格，供以后使用
void Paint_Grid(HDC hdc)
{
	unsigned int Step;
	unsigned int Grid_Num1,Grid_Num2,Grid_Num3;
	Step=Window_Width/128;   ///128*32
	Grid_Num2=Step*32;	///纵坐标的位置
	for(Grid_Num1=0;Grid_Num1<=128;Grid_Num1++)
	{
		MoveToEx(hdc,Grid_Num1*Step,0,NULL);   ///起始点
		LineTo(hdc,Grid_Num1*Step,Grid_Num2);					///终止点
	}
	Grid_Num2=Step*128; ///横坐标的位置
	for(Grid_Num1=0;Grid_Num1<=32;Grid_Num1++)
	{
		MoveToEx(hdc,0,Grid_Num1*Step,NULL);   ///起始点
		LineTo(hdc,Grid_Num2,Grid_Num1*Step);					///终止点
		
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
	logbrush.lbStyle		=	BS_SOLID;				///保护色画刷
	logbrush.lbColor		=	RGB(204,232,207);
	//logbrush.lbHatch		=	NULL;

	
	AllocConsole();    /////新建一个控制台窗口
	freopen("CONOUT$","w",stdout);
	
	printf("程序开始运行。。。\n");
	
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
		printf("新建文件失败\n");
	}
	else 
	{
		if(GetLastError()==0)   ////新建的文件
		{
			printf("新建back.back\n");
		
			for(x=0;x<32;x++)       ///结构体初始化
				for(y=0;y<128;y++)
					Grid_Struct[x][y]=0;
		}
		else
		{
			printf("打开back.back\n");
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
	

	Screen_Width = GetSystemMetrics(SM_CXSCREEN);		///当前屏幕尺寸
	Screen_Height = GetSystemMetrics(SM_CYSCREEN);

	Window_Width = Screen_Width * 2 / 3;				///窗口的尺寸
	Window_Height = Screen_Height * 2 / 3;
	
	Button_Width = LOWORD(GetDialogBaseUnits());    ///按钮的尺寸
	Button_Height = HIWORD(GetDialogBaseUnits());
	Button_Width *= 20;
	Button_Height =Button_Height*7/4;

	Button_xLocal = Window_Width-(Button_Width*3/2);  ///第一个按钮的位置坐标
	Button_yLocal = Window_Height/2;

	Grid_Width = Window_Width/128;     ////网格的宽度	

	printf("本窗口的尺寸为:%i*%i\n",Window_Width,Window_Height);	
	
	ScrollBar_xLocal = Button_Width/3;
	ScrollBar_yLocal = Button_yLocal;
	ScrollBar_Width = Window_Width / 5;   ///滚动条的尺寸
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
			
			printf("创建了一个水平滚动条控件\n");
			
			hwndButton1 = CreateWindow(TEXT("button"),
					TEXT("清空显示区"),
					WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
					Button_xLocal,Button_yLocal,
					Button_Width,Button_Height,
					hwnd,(HMENU)11,
					hInstance,NULL
					);
			hwndButton2 = CreateWindow(TEXT("button"),
					TEXT("发送图像"),
					WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
					Button_xLocal,Button_yLocal+Button_Height*3/2,
					Button_Width,Button_Height,
					hwnd,(HMENU)12,
					hInstance,NULL
					);

			
		return 0;
		
		case WM_LBUTTONDOWN:   ////鼠标左键按下
			x=LOWORD(lParam);
			y=HIWORD(lParam);
			x=x/Grid_Width;
			y=y/Grid_Width;
			if((x<128)&&(y<32))
			{
				hdc=GetDC(hwnd);
				if(Grid_Struct[y][x]&0x01)  //判断显示状态
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
		
		case WM_MOUSEMOVE:   ///鼠标移动
			if(wParam & MK_LBUTTON)
			{
				x=LOWORD(lParam);
				y=HIWORD(lParam);
				x=x/Grid_Width;
				y=y/Grid_Width;
				if((x<128)&&(y<32))
				{
					if((Grid_Struct[y][x]&0xf0)==0x00)   ///第一次经过时才会改变状态
					{
						hdc=GetDC(hwnd);
						if(Grid_Struct[y][x]&0x01)  //判断显示状态
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
			if((LOWORD(wParam)==11)&&((HIWORD(wParam)==BN_CLICKED)))  ////清除显示区按钮按下
			{
				for(x=0;x<32;x++)
					for(y=0;y<128;y++)
						Grid_Struct[x][y]&=0xf0;
				hdc=GetDC(hwnd);
				Paint_Rectangle(hdc);  ///重绘每个小方格
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
		//	Paint_Grid(hdc);  ////画网格	
			Paint_Rectangle(hdc);  ///重绘每个小方格
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

		case WM_HSCROLL:    ////水平滚动条
			printf("水平滚动条控件有动作\n");
			if(lParam==(int)hwndScroll1)
			{
				switch(LOWORD(wParam))
				{
					case SB_THUMBTRACK:	///拖动
						vPos = HIWORD(wParam);
						printf("水平滚动条控件位置发生改变，新位置为:%i\n",vPos);
					break;

					case SB_LINERIGHT:  ///右移一个
						vPos = GetScrollPos(hwndScroll1,SB_CTL)+1;
						printf("右移，新位置为:%i\n",vPos);
					break;

					case SB_LINELEFT:  ///左移一个
						vPos = GetScrollPos(hwndScroll1,SB_CTL)-1;
						printf("左移，新位置为:%i\n",vPos);
					break;

					case SB_PAGERIGHT:  ///右移一段
						vPos = GetScrollPos(hwndScroll1,SB_CTL)+10;
						printf("右移一段，新位置为:%i\n",vPos);
					break;

					case SB_PAGELEFT:  ///左移一段
						vPos = GetScrollPos(hwndScroll1,SB_CTL)-10;
						printf("左移一段，新位置为:%i\n",vPos);
					break;
				
					default:

					break;
				}
				if(vPos != GetScrollPos(hwndScroll1,SB_CTL))
				{
					printf("水平滚动条控件更新位置\n");
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
			FreeConsole();    /////关闭一个控制台窗口
			SetFilePointer(hFile,0,0,FILE_BEGIN);
			WriteFile(hFile,
					Grid_Struct,
					128*32,
					&z,
					NULL
					);
			printf("写入并关闭文件\n");
			CloseHandle(hFile);  ///关闭文件
			PostQuitMessage(0);
		return 0;
	}
	

	return DefWindowProc(hwnd,message,wParam,lParam);
}
