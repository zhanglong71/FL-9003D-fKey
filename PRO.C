#define  PROCE_GLOBALS
#include <includes.h>


#define  _Nop()  _nop_()

unsigned char ArrCompare(unsigned char *p1,unsigned char *p2,unsigned char Len)                //比较两数组是否相等
{
	int i;
	unsigned char fEQU=1;
	unsigned char a1,a2;
	for(i=0;i<Len;i++)
	{
		a1=*(p1+i);
		a2=*(p2+i);
		if(a1!=a2)fEQU=0;
	}
	return(fEQU);
}

///////////////////////////////////////////////
//=====================================
//delay t*5ms
//=====================================
void delay (unsigned char t)
{
  clk0=0;
  while (clk0<t)
      {
      	WDG_CONTR=0x3d;  //喂狗 1.13s
      	_nop_();
      	_nop_();
      	_nop_();
      }
}
//=================================
void MOTO(unsigned char type)
{
  switch  (type)
    {
	  case 1:{PIN_IA=1;PIN_IB=0;}break;
	  case 2:{PIN_IA=0;PIN_IB=1;}break;
    case 3:{PIN_IA=1;PIN_IB=1;}break;
	  default:{PIN_IA=0;PIN_IB=0;}break;
	}
}
//============================
void power(bit type)
{
  INT32U i_32u;
  INT8U  i,j;
  if (type){PIN_PWR=0;}
  else
  {
  	for(i=0;i<ADC_BUF_SIZE;i++)           //AD数据滤波处理
    {
    	ADCBuf[i]=ADBuf[i];
    }
    ADC_ArrIbub(ADCBuf,ADC_BUF_SIZE);
	  i_32u=0;
    for(i=ADC_BUF_TIAL;i<(ADC_BUF_SIZE-ADC_BUF_TIAL);i++)
    {
    	i_32u+=ADCBuf[i];
    }
    i_32u=i_32u/(ADC_BUF_SIZE-2*ADC_BUF_TIAL);
    j=i_32u;
    if(j<ADCrackDat)
    {
    	GreenOff();
    	RedOn();
    	//Clear_Timer();
    	for(i=0;i<10;i++)
    	{
    		BuzzerCTL(3,80);
    		LED_delay_time(633);
    		WDG_CONTR=0x3d;  //喂狗 1.13s
    	}
    	BuzzerCTL(7,80);
    	RedOff();
      GreenOff();
      Module_power(off);
    	PIN_PWR=1;
      while(1);
    }
    //Clear_Timer();
    while(Second<=3)      //3秒后不关门则报警
    {
    	WDG_CONTR=0x3d;  //喂狗 1.13s
    	if(Pin_CLOSE==1)    //开门时短路,Pin_CLOSE==0
    	{
    		RedOff();
        GreenOff();
        Module_power(off);
        WDG_CONTR=0x3d;  //喂狗 1.13s
    		PIN_PWR=1;
        while(1);
    	}
    }
    RedOff();
    GreenOff();
    Module_power(off);
    BuzzerCTL(1,1000);
  	PIN_PWR=1;
    while(1);
  }
}

void Err_power(bit type)
{
  //INT32U i_32u;
  //INT8U  i,j;
  if (type){PIN_PWR=0;}
  else
  {
    Clear_Timer();
    RedOff();
    GreenOff();
    Module_power(off);
    PIN_PWR=1;
    while(1);
  }
}


//============================
void Module_power(bit type)
{
  if (type)
      {PIN_FPWR=0;}
  else
      {PIN_FPWR=1;}
}

//
/*******************************************************************************/
//下位机闪灯控制
/*******************************************************************************/
void RedOn()
{
  PIN_RLED=0;
}
/*******************************************************************************/
void RedOff( )
{
  PIN_RLED=1;
}
/*******************************************************************************/
void GreenOn( )
{
  PIN_GLED=0;
}
/*******************************************************************************/
void GreenOff( )
{
  PIN_GLED=1;
}
//*******************************************************************
void LED_delay_time(unsigned int LEDtime)
{
 int i,j;
 for (i=0;i<LEDtime;i++)
 {
   for (j=0;j<1000;j++)
   {
   	WDG_CONTR=0x3d;  //喂狗 1.13s
   }//for (j=0;j<500;j++){;}
 }
}
//*******************************************************************
void LED_Red_Flash(unsigned char Flash_Number,unsigned int LEDtime)
{
 int i;
 for (i=0;i<Flash_Number;i++)
 {
     RedOn();
     LED_delay_time(LEDtime);
     WDG_CONTR=0x3d;  //喂狗 1.13s
     RedOff();
     LED_delay_time(LEDtime);
     WDG_CONTR=0x3d;  //喂狗 1.13s
 }
}
//*******************************************************************
//LED_Green_Flash(unsigned char Flash_Number,unsigned int LEDtime)
//{
// int i;
// for (i=0;i<Flash_Number;i++)
// {
//     GreenOn();
//     LED_delay_time(LEDtime);
//     GreenOff();
//     LED_delay_time(LEDtime);
// }
//}

//*******************************************************************
void LED_each_Flash(unsigned char Flash_Number,unsigned int LEDtime)
{
 int i;
 for (i=0;i<Flash_Number;i++)
 {
     RedOn();
     GreenOff();
     LED_delay_time(LEDtime);
     RedOff();
     GreenOn();
     LED_delay_time(LEDtime);
 }
     GreenOff();
}
/*******************************************************************************/
//蜂鸣器控制程序
/**********************************************************************************************/
void BuzzerOn()
{
  PIN_BUZ=1;
}
/*******************************************************************/
void BuzzerOff()
{
  PIN_BUZ=0;
}
/*******************************************************************/
void Buzzer_delay_time(unsigned int  Delaytime)
{
 int i,j;
 for (i=0;i<Delaytime;i++)
 {
 	for (j=0;j<1500;j++)
 	{
 		WDG_CONTR=0x3d;  //喂狗 1.13s;
 	}
 }
}
/*******************************************************************/
void BuzzerCTL(unsigned char Bell_Number,unsigned int DelayTime)
{
 int i;
 for (i=0;i<Bell_Number;i++)
 {
     BuzzerOn();
     Buzzer_delay_time(DelayTime);
     BuzzerOff();
     Buzzer_delay_time(DelayTime);
 }
}
/*******************************************************************/
/*******************************************************************/
//灯和蜂鸣器同时工作
/*******************************************************************/
void RedBuzzerCTL(unsigned char Number,unsigned int DelayTime)
{
 int i;
 for (i=0;i<Number;i++)
 {
     BuzzerOn();
     RedOn();
     Buzzer_delay_time(DelayTime);
     BuzzerOff();
     RedOff();
     Buzzer_delay_time(DelayTime);
 }
}
/*******************************************************************/
void GreenBuzzerCTL(unsigned char Number,unsigned int DelayTime)
{
 int i;
 for (i=0;i<Number;i++)
 {
     BuzzerOn();
     GreenOn();
     Buzzer_delay_time(DelayTime);
     BuzzerOff();
     GreenOff();
     Buzzer_delay_time(DelayTime);
 }
}
/*******************************************************************/
void RedGreenBuzzerCTL(unsigned char Number,unsigned int DelayTime)
{
 int i;
 for (i=0;i<Number;i++)
 {
	BuzzerOn();
	RedOn();
  GreenOn();
  Buzzer_delay_time(DelayTime);
  BuzzerOff();
	RedOff();
	GreenOff();
  Buzzer_delay_time(DelayTime);
 }
}


//********系统初始化**************************************************
void systeminit(void)
{
 MOTO(0);
 power(on);
 //LED_delay_time(200);
 Module_power(on);
 clk0=0;
 //BuzzerOn();
 RedOff();
 GreenOn();
 while (clk0<10) { _nop_(); };
 GreenOff();
 BuzzerOff();
}

//============================
//Open the door
//============================
void OpenDoor(void)
{
  //RedOn();
  GreenOn();
  MOTO(1);
  LED_delay_time(200);
  LED_delay_time(200);
  LED_delay_time(200);
  #if	0
  LED_delay_time(200);
  LED_delay_time(200);
  LED_delay_time(200);
  #endif
  //LED_delay_time(200);
  //MOTO(3);
  //LED_delay_time(10);
  MOTO(0);
  LED_delay_time(10);
  GreenOff();
  //RedOff();
}
//============================
//Close the door
//============================
//---------------------------------------------------
void CloseDoor(void)
{
  //RedOn();
  GreenOn();
  MOTO(2);
  LED_delay_time(200);
  LED_delay_time(200);
  LED_delay_time(200);
  #if	0
  LED_delay_time(200);
  LED_delay_time(200);
  LED_delay_time(200);
  #endif
  //LED_delay_time(200);
  //MOTO(3);
  //LED_delay_time(10);
  MOTO(0);
  LED_delay_time(10);
  //RedOff();
  GreenOff();
}
