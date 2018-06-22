#ifdef  PROCE_GLOBALS
#define PROCE_EXT
#else
#define PROCE_EXT extern
#endif

#define INT8U unsigned char        //无符号8位数
#define INT8S signed char          //有符号8位数
#define INT16U unsigned int        //无符号16位数
#define INT16S signed int          //有符号16位数
#define INT32U unsigned long       //无符号32位数
#define INT32S signed long         //有符号32位数
#define FP32 float                 //单精度浮点数
#define FP64 double                //双精度浮点数


#define on    1
#define off   0
#define red   0
#define green 1



PROCE_EXT unsigned char ArrCompare(unsigned char *p1,unsigned char *p2,unsigned char Len);
PROCE_EXT void MOTO(unsigned char type);
PROCE_EXT void power(bit type);
PROCE_EXT void Module_power(bit type);
PROCE_EXT void delay (unsigned char t);
/*******************************************************************************/
//闪灯控制
/*******************************************************************************/
PROCE_EXT void RedOn();
/*******************************************************************************/
PROCE_EXT void RedOff();
/*******************************************************************************/
PROCE_EXT void GreenOn();
/*******************************************************************************/
PROCE_EXT void GreenOff();
/*******************************************************************************/
PROCE_EXT void LED_delay_time(unsigned int LEDtime);
//*******************************************************************
PROCE_EXT void LED_Red_Flash(unsigned char Flash_Number,unsigned int LEDtime);
//*******************************************************************
PROCE_EXT void LED_Green_Flash(unsigned char Flash_Number,unsigned int LEDtime);
//*******************************************************************
//extern LED_All_Flash(unsigned char Flash_Number,unsigned int LEDtime);
//*******************************************************************
PROCE_EXT void LED_each_Flash(unsigned char Flash_Number,unsigned int LEDtime);
/*******************************************************************************/
//蜂鸣器控制程序
/**********************************************************************************************/
PROCE_EXT void BuzzerOn();
/*******************************************************************/
PROCE_EXT void BuzzerOff();
/*******************************************************************/
PROCE_EXT void BuzzerCTL(unsigned char Bell_Number,unsigned int DelayTime);
/*******************************************************************/
//灯和蜂鸣器同时工作
/*******************************************************************/
PROCE_EXT void RedBuzzerCTL(unsigned char Number,unsigned int DelayTime);
/*******************************************************************/
PROCE_EXT void GreenBuzzerCTL(unsigned char Number,unsigned int DelayTime);
/*******************************************************************/
PROCE_EXT void RedGreenBuzzerCTL(unsigned char Number,unsigned int DelayTime);

PROCE_EXT void MOTO(unsigned char type);
PROCE_EXT void power(bit type);
PROCE_EXT void Err_power(bit type);
PROCE_EXT void Module_power(bit type);
PROCE_EXT void delay (unsigned char t);

PROCE_EXT void OpenDoor(void);
PROCE_EXT void CloseDoor(void);
PROCE_EXT void systeminit(void);
