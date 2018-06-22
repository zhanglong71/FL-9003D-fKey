#define  IAP_GLOBALS
#include <includes.h>
/************************** 打开 ISP,IAP 功能 *******************************/
void ISP_IAP_enable(void)
{
	EA	=	0;	/* 关中断 */
	ISP_CONTR	=	ISP_CONTR & 0x18;       /* 0001,1000 */
	ISP_CONTR	=	ISP_CONTR | WAIT_TIME;
	ISP_CONTR	=	ISP_CONTR | 0x80;       /* 1000,0000 */
}

/************************** 关闭 ISP,IAP 功能 ******************************/
void ISP_IAP_disable(void)
{
	ISP_CONTR	=	ISP_CONTR & 0x7f;	/* 0111,1111 */
	ISP_TRIG	=	0x00;
	EA			=   1;                /* 开中断 */
}

/************************** 字节读 *****************************************/
INT8U IAP_read(INT16U byte_addr)
{
	ISP_ADDRH	=	(INT8U)(byte_addr >> 8);
	ISP_ADDRL	=	(INT8U)(byte_addr & 0x00ff);

	ISP_CMD		=	ISP_CMD	&	0xf8;        /* 1111,1000 */
	ISP_CMD		=	ISP_CMD	|	READ_AP_and_Data_Memory_Command;        /* 0000,0001 */

	ISP_IAP_enable();

	ISP_TRIG	=	0x46;
	ISP_TRIG	=	0xb9;
	_nop_();

	ISP_IAP_disable();
	return (ISP_DATA);
}

/**************************** 扇区擦除 ***************************************/
INT8U IAP_erase(INT16U sector_addr)
{
	INT16U get_sector_addr	=	0;
	get_sector_addr			=	(sector_addr & 0xfe00); /* 1111,1110,0000,0000; 取扇区地址 */
	ISP_ADDRH        		=	(INT8U)(get_sector_addr >> 8);
	ISP_ADDRL		        =	0x00;

	ISP_CMD	=	ISP_CMD	&	0xf8;	/* 1111,1000 */
	ISP_CMD	=	ISP_CMD	|	SECTOR_ERASE_AP_and_Data_Memory_Command;	/* 0000,0011 */

	ISP_IAP_enable();
	ISP_TRIG	=	0x46;        /* 触发ISP_IAP命令 */
	ISP_TRIG	=	0xb9;        /* 触发ISP_IAP命令 */
	 _nop_();

	ISP_IAP_disable();
	return OK;
}

/****************************** 字节编程并校验 *********************************/
INT8U IAP_pro_ver(INT16U byte_addr, INT8U original_data)
{
	ISP_ADDRH	=	(INT8U)(byte_addr >> 8);
	ISP_ADDRL	=	(INT8U)(byte_addr & 0x00ff);

	ISP_CMD		=	ISP_CMD	&	0xf8;        /* 1111,1000 */
	ISP_CMD		=	ISP_CMD	|	PROGRAM_AP_and_Data_Memory_Command;	/* 0000,0010 */
	ISP_DATA	=	original_data;

	ISP_IAP_enable();

	ISP_TRIG	=	0x46;
	ISP_TRIG	=	0xb9;
	_nop_();

	ISP_DATA	=	0x00;

	ISP_CMD		=	ISP_CMD	&	0xf8;        /* 1111,1000 */
	ISP_CMD		=	ISP_CMD	|	READ_AP_and_Data_Memory_Command;        /* 0000,0001 */

	ISP_TRIG	=	0x46;        /* 触发ISP_IAP命令 */
	ISP_TRIG	=	0xb9;        /* 触发ISP_IAP命令 */
	_nop_();

	ISP_IAP_disable();

	if(ISP_DATA	==	original_data)
		return	OK;
	else
		return	ERROR;
}