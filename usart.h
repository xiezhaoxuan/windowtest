#ifndef _usart_
#define _usart_


/////////////////modbus校验程序
unsigned int Crc_Check(unsigned char fu[],unsigned char g);

/////////////////////////////打开某个串口，返回打开的结果
HANDLE Com_Open(char *lpName,unsigned long Baud);
#endif
