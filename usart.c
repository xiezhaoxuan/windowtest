/*
   串口的相关操作
*/
#include "windows.h"



/////////////////modbus校验程序
unsigned int Crc_Check(unsigned char fu[],unsigned char g)
{
	unsigned int Crc=0xffff;
	unsigned char h=0;
	unsigned char i;
	unsigned char g1;

	for(g1=g;g1>0;g1--)
	{
		Crc=Crc^fu[h];
		for(i=8;i>0;i--)
		{
			if(Crc&1)
			{
				Crc>>=1;
				Crc^=0xa001;
			}
			else
				Crc>>=1;
		}
		h++;
	}
	return Crc;
}

/////////////////////////////打开某个串口，返回打开的结果，返回0表示打开失败
HANDLE Com_Open(char *lpName,unsigned long Baud)
{
	HANDLE hCom1;
	DWORD dwError;
	DCB dcb;
	COMMTIMEOUTS TimeOuts;

	hCom1=CreateFile(lpName,
			GENERIC_READ|GENERIC_WRITE,
			0,
			NULL,
			OPEN_EXISTING,
			0,                 //			FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED,
			NULL
			);
	if(hCom1==INVALID_HANDLE_VALUE)
	{
		return hCom1;
	}

	SetupComm(hCom1,1024,1024);  ///设置缓冲区大小为1024
	TimeOuts.ReadIntervalTimeout=1000;
	TimeOuts.ReadTotalTimeoutMultiplier=500;
	TimeOuts.ReadTotalTimeoutConstant=5000;
	TimeOuts.WriteTotalTimeoutMultiplier=500;
	TimeOuts.WriteTotalTimeoutConstant=5000;
	SetCommTimeouts(hCom1,&TimeOuts);  ///设置超时

	GetCommState(hCom1,&dcb);
	dcb.BaudRate=Baud;
	dcb.ByteSize=8;
	dcb.Parity=NOPARITY;
	dcb.StopBits=ONESTOPBIT;
	SetCommState(hCom1,&dcb);
	return hCom1;
}

