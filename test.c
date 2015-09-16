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

int Combo_Width = 0;    ////下拉列表框的宽度
int Combo_Height = 0; 

int Combo_xLocal = 0;  ///下拉列表框的坐标
int Combo_yLocal = 0;

int Grid_Width=0;   ///128*32的网格的宽度

int Com_Number = 0;  ///串口号,只是个标号，0表示串口1,1表示串口2，以此类推
int Com_Baud = 2;  ///串口波特率,只是个标号0表示4800  1为9600 2为19200 3为115200
int Com_Status = 0;  ///串口状态，最低位表示串口打开还是关闭，1为打开，0为关闭
unsigned char Grid_Struct[32][128];  /////网格的结构体，最低位标志网格的显示状态。

unsigned int Motor1_Param1=0;  ///电机的第一个参数，即水平转速
unsigned int Motor1_Param2=0;  ///电机的第二个参数，即触发延时，指检测到到位信号后多长时间开始显示
unsigned int Motor1_Param3=0;  ///电机的第三个参数，即脉宽时间，指激光开启的时间
unsigned int Motor1_Param4=0;  ///电机的第四个参数，即间隔时间，指激光关闭的时间

LOGBRUSH logbrush;   ///定义逻辑画刷
HBRUSH	 hBrush;	///画刷句柄

HANDLE hFile;    ///文件句柄

HANDLE hCom=INVALID_HANDLE_VALUE;    ///串口句柄

HWND			hwndButton1,hwndButton2;    ///按钮  
	
HWND 				hwndScroll1,hwndScroll2,hwndScroll3,hwndScroll4;  ///滚动条

HWND			hwndStaticText1,hwndStaticText2,hwndStaticText3,hwndStaticText4;  ////静态文本框

HWND			hwndCombo1,hwndCombo2;   ///下拉列表框

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
	unsigned char ucTemp[100];

	
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
			SetFilePointer(hFile,4096,0,FILE_BEGIN);
			ReadFile(hFile,
					&Com_Number,
					1,
					&z,
					NULL
					);
			SetFilePointer(hFile,4097,0,FILE_BEGIN);
			ReadFile(hFile,
					&Com_Baud,
					1,
					&z,
					NULL
					);
			SetFilePointer(hFile,4098,0,FILE_BEGIN);
			ReadFile(hFile,
					&ucTemp,
					8,
					&z,
					NULL
					);
			Motor1_Param1=ucTemp[0];
			Motor1_Param1=(Motor1_Param1<<8)+ucTemp[1];
			Motor1_Param2=ucTemp[2];
			Motor1_Param2=(Motor1_Param2<<8)+ucTemp[3];
			Motor1_Param3=ucTemp[4];
			Motor1_Param3=(Motor1_Param3<<8)+ucTemp[5];
			Motor1_Param4=ucTemp[6];
			Motor1_Param4=(Motor1_Param4<<8)+ucTemp[7];
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
	
	Combo_Width = Button_Width;     ///下拉列表框的尺寸
	Combo_Height = Button_Height*10;

	Combo_xLocal = Combo_Width/2;		///第一个下拉列表框的位置坐标
	Combo_yLocal = Window_Height-(Button_Height*2);

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
						 WS_CAPTION|WS_SYSMENU,
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
	char szTemp[100];
	unsigned char ucTemp[100];

	switch(message)
	{
		case WM_CREATE:

			hInstance = (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE);

			hwndScroll1 = CreateWindow(TEXT("scrollbar"),
									  "Scroll",
									  WS_CHILD|SBS_HORZ|WS_VISIBLE,
									  ScrollBar_xLocal,
									  ScrollBar_yLocal,
									  ScrollBar_Width,
									  ScrollBar_Height,
									  hwnd,
									  (HMENU)30,
									  hInstance,
									  NULL);
			SetScrollRange(hwndScroll1,SB_CTL,0,1000,TRUE);
			SetScrollPos(hwndScroll1,SB_CTL,Motor1_Param1,TRUE);

			sprintf(szTemp,"水平转速:%d",Motor1_Param1);
			hwndStaticText1 = CreateWindow(TEXT("static"),
									  szTemp,
									  WS_CHILD|SS_LEFTNOWORDWRAP|WS_VISIBLE,
									  ScrollBar_xLocal+(ScrollBar_Width*10/9),
									  ScrollBar_yLocal,
									  ScrollBar_Width*3/5,
									  ScrollBar_Height,
									  hwnd,
									  (HMENU)40,
									  hInstance,
									  NULL);
										
			hwndScroll2 = CreateWindow(TEXT("scrollbar"),
									  "Scroll",
									  WS_CHILD|SBS_HORZ|WS_VISIBLE,
									  ScrollBar_xLocal,
									  ScrollBar_yLocal+(ScrollBar_Height*3/2),
									  ScrollBar_Width,
									  ScrollBar_Height,
									  hwnd,
									  (HMENU)31,
									  hInstance,
									  NULL);
			SetScrollRange(hwndScroll2,SB_CTL,0,1000,TRUE);
			SetScrollPos(hwndScroll2,SB_CTL,Motor1_Param2,TRUE);

			sprintf(szTemp,"触发延时:%d",Motor1_Param2);
			hwndStaticText2 = CreateWindow(TEXT("static"),
									  szTemp,
									  WS_CHILD|SS_LEFTNOWORDWRAP|WS_VISIBLE,
									  ScrollBar_xLocal+(ScrollBar_Width*10/9),
									  ScrollBar_yLocal+(ScrollBar_Height*3/2),
									  ScrollBar_Width*3/5,
									  ScrollBar_Height,
									  hwnd,
									  (HMENU)41,
									  hInstance,
									  NULL);
			
			printf("创建了一个水平滚动条控件\n");

			hwndScroll3 = CreateWindow(TEXT("scrollbar"),
									  "Scroll",
									  WS_CHILD|SBS_HORZ|WS_VISIBLE,
									  ScrollBar_xLocal,
									  ScrollBar_yLocal+(ScrollBar_Height*3),
									  ScrollBar_Width,
									  ScrollBar_Height,
									  hwnd,
									  (HMENU)32,
									  hInstance,
									  NULL);
			SetScrollRange(hwndScroll3,SB_CTL,0,1000,TRUE);
			SetScrollPos(hwndScroll3,SB_CTL,Motor1_Param3,TRUE);
			
			sprintf(szTemp,"脉宽:%d",Motor1_Param3);
			hwndStaticText3 = CreateWindow(TEXT("static"),
									  szTemp,
									  WS_CHILD|SS_LEFTNOWORDWRAP|WS_VISIBLE,
									  ScrollBar_xLocal+(ScrollBar_Width*10/9),
									  ScrollBar_yLocal+(ScrollBar_Height*3),
									  ScrollBar_Width*3/5,
									  ScrollBar_Height,
									  hwnd,
									  (HMENU)42,
									  hInstance,
									  NULL);

			hwndScroll4 = CreateWindow(TEXT("scrollbar"),
									  "Scroll",
									  WS_CHILD|SBS_HORZ|WS_VISIBLE,
									  ScrollBar_xLocal,
									  ScrollBar_yLocal+(ScrollBar_Height*9/2),
									  ScrollBar_Width,
									  ScrollBar_Height,
									  hwnd,
									  (HMENU)33,
									  hInstance,
									  NULL);
			SetScrollRange(hwndScroll4,SB_CTL,0,1000,TRUE);
			SetScrollPos(hwndScroll4,SB_CTL,Motor1_Param4,TRUE);
			
			sprintf(szTemp,"间隔:%d",Motor1_Param4);
			hwndStaticText4 = CreateWindow(TEXT("static"),
									  szTemp,
									  WS_CHILD|SS_LEFTNOWORDWRAP|WS_VISIBLE,
									  ScrollBar_xLocal+(ScrollBar_Width*10/9),
									  ScrollBar_yLocal+(ScrollBar_Height*9/2),
									  ScrollBar_Width*3/5,
									  ScrollBar_Height,
									  hwnd,
									  (HMENU)43,
									  hInstance,
									  NULL);

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
			hwndCombo1 = CreateWindow(TEXT("combobox"),
					NULL,
					WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
					Combo_xLocal,Combo_yLocal,
					Combo_Width,Combo_Height,
					hwnd,(HMENU)20,
					hInstance,NULL
					);
			hwndCombo2 = CreateWindow(TEXT("combobox"),
					NULL,
					WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
					Combo_xLocal+(Combo_Width*3/2),Combo_yLocal,
					Combo_Width,Combo_Height,
					hwnd,(HMENU)21,
					hInstance,NULL
					);
			hwndButton2 = CreateWindow(TEXT("button"),
					TEXT("打开串口"),
					WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
					Combo_xLocal+(Combo_Width*3),Combo_yLocal-5,
					Button_Width,Button_Height,
					hwnd,(HMENU)13,
					hInstance,NULL
					);
			for(x=0;x<10;x++)    ////串口选项
			{
				sprintf(szTemp,"COM%d",x+1);
				SendMessage(hwndCombo1,
						CB_ADDSTRING,
						0,
						(LPARAM)szTemp
						);
			};
			SendMessage(hwndCombo1,
					CB_SETCURSEL,
					Com_Number,
					0
					);
			SendMessage(hwndCombo2,
					CB_ADDSTRING,
					0,
					(LPARAM)TEXT("4800")
					);
			SendMessage(hwndCombo2,
					CB_ADDSTRING,
					0,
					(LPARAM)TEXT("9600")
					);
			SendMessage(hwndCombo2,
					CB_ADDSTRING,
					0,
					(LPARAM)TEXT("19200")
					);
			SendMessage(hwndCombo2,
					CB_ADDSTRING,
					0,
					(LPARAM)TEXT("115200")
					);
			SendMessage(hwndCombo2,
					CB_SETCURSEL,
					Com_Baud,
					0
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
			
			if(((HWND)lParam==hwndCombo1)&&(HIWORD(wParam)==CBN_SELCHANGE))   ///改变串口号选择
			{
				printf("串口号改变\n");
				Com_Number = SendMessage(hwndCombo1,
						CB_GETCURSEL,
						0,
						0
						);
			}
			if(((HWND)lParam==hwndCombo2)&&(HIWORD(wParam)==CBN_SELCHANGE))   ///改变串口号选择
			{
				printf("波特率改变\n");
				Com_Baud = SendMessage(hwndCombo2,
						CB_GETCURSEL,
						0,
						0
						);
			}
			if((LOWORD(wParam)==13)&&((HIWORD(wParam)==BN_CLICKED)))  ////打开串口按钮按下
			{
				if(Com_Status&0x01)   ////关闭串口
				{
					CloseHandle(hCom);
					hCom=INVALID_HANDLE_VALUE;
					EnableWindow(hwndCombo1,TRUE);
					EnableWindow(hwndCombo2,TRUE);
					SendMessage(hwndButton2,
							WM_SETTEXT,
							0,
							(LPARAM)TEXT("打开串口")
							);
					Com_Status&=~0x01;
					printf("关闭串口\n");
				}
				else				///打开串口
				{
					sprintf(szTemp,"COM%d",Com_Number+1);
					if(Com_Baud==0)
						z=4800;
					if(Com_Baud==1)
						z=9600;
					if(Com_Baud==2)
						z=19200;
					if(Com_Baud==3)
						z=115200;
				
					hCom=(HANDLE)Com_Open(szTemp,z);		
					if(hCom==INVALID_HANDLE_VALUE)
					{
						MessageBox(hwnd,"串口已打开或不存在!!!","警告",0);	
					}
					else
					{
						EnableWindow(hwndCombo1,FALSE);
						EnableWindow(hwndCombo2,FALSE);
						SendMessage(hwndButton2,
								WM_SETTEXT,
								0,
								(LPARAM)TEXT("关闭串口")
								);
						Com_Status|=0x01;
					
						SetFilePointer(hFile,4096,0,FILE_BEGIN);  ///存储串口号和波特率
						WriteFile(hFile,
							&Com_Number,
							1,
							&z,
							NULL
							);
						SetFilePointer(hFile,4097,0,FILE_BEGIN);  ///存储串口号和波特率
						WriteFile(hFile,
							&Com_Baud,
							1,
							&z,
							NULL
							);
						printf("打开串口\n");
					}
				}
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

		//	Paint_Grid(hdc);  ////画网格	
			Paint_Rectangle(hdc);  ///重绘每个小方格
			EndPaint(hwnd,&ps);
		return 0;

		case WM_HSCROLL:    ////水平滚动条
			printf("水平滚动条控件有动作\n");
			ucTemp[0]=0xF0;
			ucTemp[1]=0x01;
			ucTemp[2]=0x00;
			ucTemp[3]=0x00;
			x=0;               ////标志是否需要发送
			if(lParam==(int)hwndScroll1)
			{
				switch(LOWORD(wParam))
				{
					case SB_THUMBTRACK:	///拖动
						Motor1_Param1 = HIWORD(wParam);
					break;

					case SB_LINERIGHT:  ///右移一个
						Motor1_Param1 = GetScrollPos(hwndScroll1,SB_CTL)+1;
					break;

					case SB_LINELEFT:  ///左移一个
						Motor1_Param1 = GetScrollPos(hwndScroll1,SB_CTL)-1;
					break;

					case SB_PAGERIGHT:  ///右移一段
						Motor1_Param1 = GetScrollPos(hwndScroll1,SB_CTL)+10;
					break;

					case SB_PAGELEFT:  ///左移一段
						Motor1_Param1 = GetScrollPos(hwndScroll1,SB_CTL)-10;
					break;
				
					default:

					break;
				}
				if(Motor1_Param1 != GetScrollPos(hwndScroll1,SB_CTL))
				{
					//hdc=GetDC(hwnd);
					//GetClientRect(hwnd,&rect);
					x=1;
					ucTemp[1]=0x01;
					ucTemp[2]=Motor1_Param1>>8;
					ucTemp[3]=Motor1_Param1%256;
					SetScrollPos(hwndScroll1,SB_CTL,Motor1_Param1,TRUE);
					sprintf(szTemp,"水平转速:%d",Motor1_Param1);
					SendMessage(hwndStaticText1,
							WM_SETTEXT,
							0,
							(LPARAM)szTemp
							);
					//hdc=GetDC(hwnd);
					
				}
			}
			if(lParam==(int)hwndScroll2)
			{
				switch(LOWORD(wParam))
				{
					case SB_THUMBTRACK:	///拖动
						Motor1_Param2 = HIWORD(wParam);
					break;

					case SB_LINERIGHT:  ///右移一个
						Motor1_Param2 = GetScrollPos(hwndScroll2,SB_CTL)+1;
					break;

					case SB_LINELEFT:  ///左移一个
						Motor1_Param2 = GetScrollPos(hwndScroll2,SB_CTL)-1;
					break;

					case SB_PAGERIGHT:  ///右移一段
						Motor1_Param2 = GetScrollPos(hwndScroll2,SB_CTL)+10;
					break;

					case SB_PAGELEFT:  ///左移一段
						Motor1_Param2 = GetScrollPos(hwndScroll2,SB_CTL)-10;
					break;
				
					default:

					break;
				}
				if(Motor1_Param2 != GetScrollPos(hwndScroll2,SB_CTL))
				{
					x=1;
					ucTemp[1]=0x02;
					ucTemp[2]=Motor1_Param2>>8;
					ucTemp[3]=Motor1_Param2%256;
					SetScrollPos(hwndScroll2,SB_CTL,Motor1_Param2,TRUE);
					sprintf(szTemp,"触发延时:%d",Motor1_Param2);
					SendMessage(hwndStaticText2,
							WM_SETTEXT,
							0,
							(LPARAM)szTemp
							);
				}
			}
			if(lParam==(int)hwndScroll3)
			{
				switch(LOWORD(wParam))
				{
					case SB_THUMBTRACK:	///拖动
						Motor1_Param3 = HIWORD(wParam);
					break;

					case SB_LINERIGHT:  ///右移一个
						Motor1_Param3 = GetScrollPos(hwndScroll3,SB_CTL)+1;
					break;

					case SB_LINELEFT:  ///左移一个
						Motor1_Param3 = GetScrollPos(hwndScroll3,SB_CTL)-1;
					break;

					case SB_PAGERIGHT:  ///右移一段
						Motor1_Param3 = GetScrollPos(hwndScroll3,SB_CTL)+10;
					break;

					case SB_PAGELEFT:  ///左移一段
						Motor1_Param3 = GetScrollPos(hwndScroll3,SB_CTL)-10;
					break;
				
					default:

					break;
				}
				if(Motor1_Param3 != GetScrollPos(hwndScroll3,SB_CTL))
				{
					x=1;
					ucTemp[1]=0x03;
					ucTemp[2]=Motor1_Param3>>8;
					ucTemp[3]=Motor1_Param3%256;
					SetScrollPos(hwndScroll3,SB_CTL,Motor1_Param3,TRUE);
					sprintf(szTemp,"脉宽:%d",Motor1_Param3);
					SendMessage(hwndStaticText3,
							WM_SETTEXT,
							0,
							(LPARAM)szTemp
							);
				}
			}
			if(lParam==(int)hwndScroll4)
			{
				switch(LOWORD(wParam))
				{
					case SB_THUMBTRACK:	///拖动
						Motor1_Param4 = HIWORD(wParam);
					break;

					case SB_LINERIGHT:  ///右移一个
						Motor1_Param4 = GetScrollPos(hwndScroll4,SB_CTL)+1;
					break;

					case SB_LINELEFT:  ///左移一个
						Motor1_Param4 = GetScrollPos(hwndScroll4,SB_CTL)-1;
					break;

					case SB_PAGERIGHT:  ///右移一段
						Motor1_Param4 = GetScrollPos(hwndScroll4,SB_CTL)+10;
					break;

					case SB_PAGELEFT:  ///左移一段
						Motor1_Param4 = GetScrollPos(hwndScroll4,SB_CTL)-10;
					break;
				
					default:

					break;
				}
				if(Motor1_Param4 != GetScrollPos(hwndScroll4,SB_CTL))
				{
					x=1;
					ucTemp[1]=0x04;
					ucTemp[2]=Motor1_Param4>>8;
					ucTemp[3]=Motor1_Param4%256;
					SetScrollPos(hwndScroll4,SB_CTL,Motor1_Param4,TRUE);
					sprintf(szTemp,"间隔:%d",Motor1_Param4);
					SendMessage(hwndStaticText4,
							WM_SETTEXT,
							0,
							(LPARAM)szTemp
							);
				}
			}
			if((x)&&(hCom!=INVALID_HANDLE_VALUE))
			{
				z=Crc_Check(ucTemp,4);
				ucTemp[4]=z&0xff;
				ucTemp[5]=z>>8;
				printf("发送水平转速\n");
				WriteFile(hCom,
						ucTemp,
						6,
						&z,
						NULL
						);
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
			ucTemp[0]=Motor1_Param1/256;
			ucTemp[1]=Motor1_Param1%256;
			ucTemp[2]=Motor1_Param2/256;
			ucTemp[3]=Motor1_Param2%256;
			ucTemp[4]=Motor1_Param3/256;
			ucTemp[5]=Motor1_Param3%256;
			ucTemp[6]=Motor1_Param4/256;
			ucTemp[7]=Motor1_Param4%256;
			SetFilePointer(hFile,4098,0,FILE_BEGIN);
			WriteFile(hFile,
					ucTemp,
					8,
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
