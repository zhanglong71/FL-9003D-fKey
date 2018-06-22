//****************************************************************
//  项目名称:指纹锁
//  作    者:张俊锋
//  芯片名称:STC12LE5616AD
//  V1.0日期:2008-05-20
//  版    本:V2.0
//  备    注:方程式指纹模块
//  修改日期:2011-09-14
//****************************************************************
#define MAIN_GLOBALS
#include <includes.h>
#define CODE_K1 0x01
#define CODE_K2 0x02
#define CODE_K3 0x04
#define CODE_K4 0x08
#define CODE_K5 0x10
//#define CODE_K6 0x20
#define CODE_K7 0x40
#define CMAX_PSERR 6		//最大密码失败次数
unsigned char key_number;
unsigned char key();
unsigned char key_pwr();
unsigned char key_fun();
unsigned char FlagBuf;
unsigned char T0cnt=0;
unsigned char ScanPwrBuf[8];
unsigned char fPwrOk;
unsigned char iErr = 0;		//输入错误密码次数(三次输入错误密码就，让密码开锁功能失效)
unsigned char fDkey;
unsigned char fAdCir;
//unsigned char MK;
INT8U UsePwrBuf[6][8];  //用户密码数据
INT8U SuperPwrBuf[6];   //厂家密码数据

INT8U Version[2] = {0xA5, 0x00};   //版本号
INT8U datetime[4] = {0x20, 0x14, 0x12, 0x25};   //版本号

INT16U code ADR_PWRDATA[]={0x0800,0x0810,0x0820,0x0830,0x0840,0x0850};
//密码存放地址 1个管理密码和6个用户密码

//------------------------------------------------------------------------------------------------
//以下是
//------------------------------------------------------------------------------------------------
static void MainTxdByte(unsigned char i)
{

    TI = 0;
    SBUF = i;
    while(!TI);
   TI = 0;
}

void  mem_ini()                   //内存初始化程序
{
	INT8U i,j,k;
	INT16U i_16u;
	INT8U pwr_ran[6];
	i=0;
	j=0;
	i_16u=0;
	clk0=0;
	clk1=0;
	clk2=0;
	Second=0;
	Minute=0;
	doorflag=0;
	fDkey=0;
	fAdCir=0;
	PointADC=0;
	SearchNumber=0xffff;//搜索到的指纹序列号
	//SaveNumber=0xffff;
	P1M0=0x11;//0001 0001b
	P1M1=0x00;//0000 0000b
	P2M0=0x00;//
	P2M1=0x00;
	P3M0=0x00;//0000 0000b
	P3M1=0xB0;//1011 0000b
	P1=0xff;
	P2=0xff;
	P3=0x4b;  //0100 1011b
	PIN_IA=0;
	PIN_IB=0;
	PIN_PWR=0;
	PIN_FPWR=1;
	flag_com_rx_finish=0;
	Module_power(off);
	AUXR=0x30;
	ADCrackDat=IAP_read(ADR_ADCRACK);
	if(ADCrackDat<=0x80)ADCrackDat=0xa4;    //当检验值小0x80时认为错误,取0xa4为校验值

	i=IAP_read(IAP_ADR_DEFAULT);
	if(i==0x00)            //初始化,清空密码数据
	{
		LED_delay_time(10);
		i=IAP_read(IAP_ADR_DEFAULT);
		if(i==0x00)          //再次确认是否初始化
		{
			
			IAP_erase(ADR_ERR);
			IAP_erase(ADR_FTAB);
			IAP_erase(ADR_USERPWR);
			IAP_erase(ADR_SUPERPWR);
			IAP_erase(IAP_ADR_DEFAULT);
			for(k=0;k<6;k++)
			{
				i=0x5f;
				j=rand();
				pwr_ran[k]=j%5+1;      //产生随机密码
			}
			IAP_pro_ver(IAP_ADR_DEFAULT,0xff);
			for(i_16u=ADR_SUPERPWR;i_16u<(ADR_SUPERPWR+6);i_16u++)
			{
				j=i_16u-ADR_SUPERPWR;
				IAP_pro_ver(i_16u,pwr_ran[j]);
			}
			for(i_16u=0;i_16u<58;i_16u++)
			{
				IAP_pro_ver(ADR_FTAB+i_16u,0x30);
			}
		}
	}
	//------------------------------------------------------------------
	//------------------------------------------------------------------
	for(k=0;k<6;k++)  //读取用户密码数据
	{
		for(i_16u=ADR_PWRDATA[k];i_16u<(ADR_PWRDATA[k]+8);i_16u++)
		{
			j=i_16u-ADR_PWRDATA[k];
			UsePwrBuf[k][j]=IAP_read(i_16u);
		}
	}
	for(i_16u=ADR_SUPERPWR;i_16u<(ADR_SUPERPWR+6);i_16u++)//读取厂家密码数据
	{
		j=i_16u-ADR_SUPERPWR;
		SuperPwrBuf[j]=IAP_read(i_16u);
	}
	for(i_16u=0;i_16u<58;i_16u++)   //读取指纹模板表
	{
		userFigerTab[i_16u]=IAP_read(ADR_FTAB+i_16u);
	}

	iErr = IAP_read(ADR_ERR + 0);
}

//*******************************************************************************************************
unsigned char keytest()
{
	unsigned char key_temp;
  key_temp=0x00;
//************************键盘扫描************************************
  Pin_K1=1;
  Pin_K2=1;
  Pin_K3=1;
  Pin_K4=1;
  Pin_K5=1;
  //Pin_K6=1;
  Pin_K7=1;
	if(Pin_K1)key_temp|=0x01;else key_temp&=0xfe;
	if(Pin_K2)key_temp|=0x02;else key_temp&=0xfd;
	if(!Pin_K3)key_temp|=0x04;else key_temp&=0xfb;
	if(!Pin_K4)key_temp|=0x08;else key_temp&=0xf7;
	if(!Pin_K5)key_temp|=0x10;else key_temp&=0xef;
	//if(!Pin_K6)key_temp|=0x20;else key_temp&=0xdf;
	if(Pin_K7)key_temp|=0x40;else key_temp&=0xbf;
	//if(!Pin_K8)key_temp|=0x80;else key_temp&=0x7f;
	return key_temp;
}
unsigned char key()
{
	unsigned char key_buf00=0;
	unsigned char key_buf01=0;
	LED_delay_time(10);
  key_buf00=keytest();
  LED_delay_time(10);
  key_buf01=keytest();
  if(key_buf00!=key_buf01)return 0;
  else
  {
  	return key_buf00;
  }
}
unsigned char key_pwr()
{
	unsigned char key_buf0=0;
	unsigned char key_buf1=0;
	LED_delay_time(10);
  key_buf0=keytest();
  LED_delay_time(10);
  key_buf1=keytest();
  if(key_buf0!=key_buf1)return 0;
  else
  {
  	if(key_buf0==(CODE_K1|CODE_K3|CODE_K5))return key_buf0;
  	else
  	{
  		key_buf1=keytest();
  	  //if(key_buf0!=CODE_K7)	//zhanglong 2014/12/25 10:51 注释掉此代码
  	  {
  	  	BuzzerCTL(1,150);
		    GreenOn();
		  }
  	  while(key_buf1!=0)
  	  {
  	  	key_buf1=keytest();  //等待松开按键
  	  	WDG_CONTR=0x3d;  //喂狗 1.13s
  	  }
  	  return key_buf0;
  	}
  }
}
unsigned char key_fun()
{
	unsigned char key_buf0=0;
	unsigned char key_buf1=0;
	LED_delay_time(10);
  key_buf0=keytest();
  LED_delay_time(10);
  key_buf1=keytest();
  if(key_buf0!=key_buf1)return 0;
  else
  {
  	key_buf1=keytest();
  	if(key_buf0==(CODE_K5|CODE_K7))
  	{
  		return key_buf0;
  	}
  	if((key_buf0==CODE_K5)||(key_buf1==CODE_K7))
  	{
  		while(key_buf1!=0)
  		{
  			key_buf1=keytest();
  			WDG_CONTR=0x3d;  //喂狗 1.13s
  			if(key_buf1==(CODE_K5|CODE_K7))
  	    {
  		    return key_buf1;
  	    }
  		}
  		return key_buf0;
  	}
  	else
  	{
  		//while(key_buf1!=0)
  	  //{
  		//  key_buf1=keytest();  //等待松开按键
  	  //}
  	  return key_buf0;
  	}
  }
}
unsigned char ScanPwr()
{
	unsigned char key_number;
	unsigned char l=0;
	Clear_Timer();  //清定时器
	while(l<6)  //6位密码
	{
		WDG_CONTR=0x3d;  //喂狗 1.13s
		key_number=key();
  	switch(key_number)
  	{
  		case CODE_K1:
  		{
  			BuzzerCTL(1,150);
  			ScanPwrBuf[l]=1;
  			l++;
  			Clear_Timer();
  		}break;
  		case CODE_K2:
  		{
  			ScanPwrBuf[l]=2;
  			BuzzerCTL(1,150);
  			l++;
  			Clear_Timer();
  		}break;
  		case CODE_K3:
  		{
  			ScanPwrBuf[l]=3;
  			BuzzerCTL(1,150);
  			l++;
  			Clear_Timer();
  		}break;
  		case CODE_K4:
  		{
  			ScanPwrBuf[l]=4;
  			BuzzerCTL(1,150);
  			l++;
  			Clear_Timer();
  		}break;
  		case CODE_K5:
  		{
  			ScanPwrBuf[l]=5;
  			BuzzerCTL(1,150);
  			l++;
  			Clear_Timer();
  		}break;
  		case CODE_K7:
  		{
  			BuzzerCTL(1,150);
  			if(l==0)return(0);
  			else
  			{
  				GreenOff();
			    RedOn();
			    BuzzerCTL(2,150);
			    RedOff();
			    power(off);
  			}
  		}break;
  		default:break;
  	}
  	if(Second>=10)
		{
			GreenOff();
			RedOn();
			BuzzerCTL(2,150);
			RedOff();
			power(off);
		}
  }
  return(1);
}

unsigned char ScanOpenWrd()
{
	unsigned char key_number;
	unsigned char l=0;
	Clear_Timer();  //清定时器
	while(l<8)  //8位密码
	{
		key_number=key();
		WDG_CONTR=0x3d;  //喂狗 1.13s
  	switch(key_number)
  	{
  		case CODE_K1:
  		{
  			BuzzerCTL(1,150);
  			ScanPwrBuf[l]=1;
  			l++;
  			Clear_Timer();
  		}break;
  		case CODE_K2:
  		{
  			ScanPwrBuf[l]=2;
  			BuzzerCTL(1,150);
  			l++;
  			Clear_Timer();
  		}break;
  		case CODE_K3:
  		{
  			ScanPwrBuf[l]=3;
  			BuzzerCTL(1,150);
  			l++;
  			Clear_Timer();
  		}break;
  		case CODE_K4:
  		{
  			ScanPwrBuf[l]=4;
  			BuzzerCTL(1,150);
  			l++;
  			Clear_Timer();
  		}break;
  		case CODE_K5:
  		{
  			ScanPwrBuf[l]=5;
  			BuzzerCTL(1,150);
  			l++;
  			Clear_Timer();
  		}break;
  		case CODE_K7:
  		{
  			BuzzerCTL(1,150);
  			if(l==0)return(0);
  			else
  			{
  				GreenOff();
			    RedOn();
			    BuzzerCTL(2,150);
			    RedOff();
			    power(off);
  			}
  		}break;
  		default:break;
  	}
  	if(Second>=10)
		{
			if(IAP_read(ADR_ERR + 0) != iErr)	//只有密码开锁流程，并发生过密码错误，才会满足此条件
			{
				/** 密码失败次数更新到flash **/
				IAP_erase(ADR_ERR);
				IAP_pro_ver(ADR_ERR ,iErr);
			}
			
			GreenOff();
			RedOn();
			BuzzerCTL(2,150);
			RedOff();
			power(off);
		}
  }
  return(1);
}

/******************************************************************************************************
 * 2014/12/25 14:31 
 * 功能键按下后已经有过声音提示，此处注释掉声音提示
 ******************************************************************************************************/
void K7_Open_Door_Program()                  //功能键开门程序流程
{
  unsigned char i=0;
  Module_power(off);                      //关闭模块电源
  GreenOn();//RedOn();
  //BuzzerCTL(1,200);
  OpenDoor();
  RedOff();
  Clear_Timer();
  for(i=0;i<6;i++)
  {
  	LED_delay_time(633);
  	WDG_CONTR=0x3d;  //喂狗 1.13s
  }
  CloseDoor();
  return;    //如果5秒内没有按门把手  直接关门
}
//*******************************************************************************************************
void Open_Door_Program()                  //开门程序流程
{
  unsigned char i=0;
  Module_power(off);                      //关闭模块电源
  GreenOn();//RedOn();
  BuzzerCTL(1,200);
  OpenDoor();
  RedOff();
  Clear_Timer();
  for(i=0;i<6;i++)
  {
  	LED_delay_time(633);
  	WDG_CONTR=0x3d;  //喂狗 1.13s
  }
  CloseDoor();
  return;    //如果5秒内没有按门把手  直接关门
}
//*******************************************************************************************************

void	T0_Ini() reentrant using 0		//定时器0初始化,5ms中断一次
{
	TR0	= 0;
	TF0	= 0;
	TH0	= 0xdc;
	TL0	= 0x31;
	TR0	= 1;
	ET0	= 1;
}

void	Ser_Ini()  reentrant using 0
{
	PCON &= 0x7f;		//波特率不倍速
	SCON = 0x50;		//8位数据,可变波特率
	AUXR &= 0xbf;		//定时器1时钟为Fosc/12,即12T
	AUXR &= 0xfe;		//串口1选择定时器1为波特率发生器
	//TMOD &= 0x0f;		//清除定时器1模式位
	//TMOD |= 0x20;		//设定定时器1为8位自动重装方式
	TMOD = 0x21;
	TL1 = 0xFA;		//设定定时初值
	TH1 = 0xFA;		//设定定时器重装值
	ET1 = 0;		//禁止定时器1中断
	TR1 = 1;		//启动定时器1
	ES=1;
}

//------------------------------------------------------------------------------------------------
//以下是主函数
//------------------------------------------------------------------------------------------------
void main()
{
	INT8U i,i_buf[3],j,k,l;
	INT8S i_s,j_s;
	unsigned char err;
	INT32U i_32u,j_32u;
	INT16U i_16u,j_16u;
	//bit low_power_flag=0;
	//unsigned char i;
	unsigned char MK1,MK2,MK3;
	unsigned char Compare_Number=0;
	//MK=0;
	MK1=0;
	MK2=0;
	MK3=0;
	Module_power(off);
	i=0;
	j=0;
	k=0;
	l=0;
	i_s=0;
	j_s=0;
	i_32u=0;
	j_32u=0;
	i_16u=0;
	j_16u=0;
	//T0_Ini();
	//Ser_Ini();
	//EA=1;
	mem_ini();
	WDG_CONTR=0x3d;  //喂狗 1.13s
	//Pin_SP=0;
	//-------------------------test
	//PIN_GLED=0;
	//LED_delay_time(633);
	//PIN_GLED=1;
	//WDG_CONTR=0x3d;  //喂狗 1.13s
	//while(1){;}
	//while(1)
	//{
	//	LED_delay_time(633);  //1000=790ms  633=0.5s
	//	PIN_GLED=!PIN_GLED;
	//}
	//--------------------------
	T0_Ini();
	Ser_Ini();
	ADC_Init();
	fAdCir=0;
	key_number=key_pwr();   //获取上电时按键值
	if((key_number!=CODE_K1)&&(key_number!=CODE_K2)&&(key_number!=CODE_K3)&&(key_number!=CODE_K4)&&(key_number!=CODE_K5)&&(key_number!=CODE_K7))
	{
		if(key_number==(CODE_K1|CODE_K3|CODE_K5))    //AD转换校验用
		{
			BuzzerCTL(1,1000);
			GreenOn();
			fAdCir=0;
			PointADC=0;
			ADC_CONTR=ADC_CONTR | 0x08;           //启动AD转换
			while(!fAdCir)WDG_CONTR=0x3d;        //等待一个周期转换完成
			//------------------------------------------------------------------------
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
			IAP_erase(ADR_ADCRACK);
			IAP_pro_ver(ADR_ADCRACK,j);
			BuzzerCTL(2,1000);
			GreenOff();
			Err_power(off);
		}
		else Err_power(off);
	}

	EA=1;
	ADC_CONTR=ADC_CONTR | 0x08;               //启动AD转换
	if(key_number==CODE_K7)     //门内直接开门按钮,或者遥控开门
	{
		if(IAP_read(ADR_ERR + 0) != 0)	//解除密码锁定(密码错误计数)
		{
	    	IAP_erase(ADR_ERR);
	    	IAP_pro_ver(ADR_ERR ,0);
		}		
		
		//test
		i_buf[0]=2;
		i_buf[1]=1;
		i_buf[2]=0;
		err=0;
		//Command(i_buf);
		//flag_com_rx_finish=0;
		//OSSemPend(&flag_com_rx_finish,5000,&err);
		//Command(i_buf);
		//power(off);               //关闭电源
		//ADD_Manager_User(0);
		//power(off);               //关闭电源
		//i=searchfinger();
		//if(i==0)
		//{
		//	RedBuzzerCTL(3,150);
		//	power(off);               //关闭电源
		//}
		//else
		//{
		//	RedBuzzerCTL(1,150);
		//	power(off);               //关闭电源
		//}
		//while(1);
		for(i=0;i<6;i++)
		{
			MainTxdByte(SuperPwrBuf[i]);  //显示厂家密码
		}
		MainTxdByte(ADCrackDat);        //显示校验数据
		
		for(i=0;i<2;i++)
		{
			MainTxdByte(Version[i]);        //显示版本号
		}
		for(i=0;i<4;i++)
		{
			MainTxdByte(datetime[i]);        //显示时间
		}
		
		//Open_Door_Program();	    //开门程序流程
		K7_Open_Door_Program();
		power(off);               //关闭电源
	}
	//for(i=0;i<10;i++)
	//{
	//	Par[i]=DEFAULT_CONFIG[i];
	//}
	if(key_number==CODE_K2)     //密码开门
	{
		//BuzzerCTL(1,150);
		//GreenOn();
		while(1)
		{
			WDG_CONTR=0x3d;  //喂狗 1.13s
			LED_delay_time(10);
			key_number=key();
			Clear_Timer();  //清定时器
			if(key_number==CODE_K7||MK2==1)  //增加/修改密码
			{
				//BuzzerCTL(1,150);
				WDG_CONTR=0x3d;  //喂狗 1.13s
				Module_power(on);
				BuzzerCTL(1,150);
				if(VefPSW())//(1)
				{
					while(key_number==CODE_K7)
					{
						key_number=key();
						WDG_CONTR=0x3d;   //喂狗 1.13s
						if(Second>=5)     //长按5秒,清除所有密码
						{
							RedOff();
							GreenOn();
							BuzzerCTL(1,1000);
							i=AdminFiger_chek();  //校验管理指纹
							GreenOff();
							RedOn();
							BuzzerCTL(1,1000);
							IAP_erase(ADR_USERPWR);
							for(k=0;k<6;k++)  //读取用户密码数据
							{
								for(i_16u=ADR_PWRDATA[k];i_16u<(ADR_PWRDATA[k]+8);i_16u++)
								{
									j=i_16u-ADR_PWRDATA[k];
									UsePwrBuf[k][j]=IAP_read(i_16u);
								}
							}
							RedOff();
							power(off);
						}
					}
					//key_number=key();
					LED_delay_time(10);
					Clear_Timer();  //清定时器
					i=AdminFiger_chek();
					GreenBuzzerCTL(1,150);
					GreenOn();
					ScanOpenWrd();
					for(i=0;i<6;i++)
					{
						if(UsePwrBuf[i][0]==0||UsePwrBuf[i][0]>=6)  //密码为空
						{
							for(j=0;j<8;j++)
							{
								UsePwrBuf[i][j]=ScanPwrBuf[j];
							}
							IAP_erase(ADR_USERPWR);
							for(k=0;k<6;k++)  //写入用户密码数据
							{
								for(i_16u=ADR_PWRDATA[k];i_16u<(ADR_PWRDATA[k]+8);i_16u++)
								{
									j=i_16u-ADR_PWRDATA[k];
									IAP_pro_ver(i_16u,UsePwrBuf[k][j]);
								}
							}
							RedOff();
							GreenOn();
							BuzzerCTL(1,1000);
							GreenOff();
							power(off);
						}
					}
					//密码已满
					GreenOff();
					RedOn();
					BuzzerCTL(3,150);
					RedOff();
					power(off);
					BuzzerCTL(1,1000);
				}
				power(off);
			}

			if(iErr >= CMAX_PSERR)
			{
			 	if(IAP_read(ADR_ERR + 0) != CMAX_PSERR)
				{
					/** 密码失败超过CMAX_PSERR次, 将数据写入flash **/
					IAP_erase(ADR_ERR);
					IAP_pro_ver(ADR_ERR ,iErr);
				}
				GreenOff();
				RedOn();
				BuzzerCTL(3,150);
				RedOff();
				
				power(off);
			}

			if(ScanOpenWrd())
			{
				MK2=0;
				for(i=0;i<6;i++)
				{
					if(ArrCompare(UsePwrBuf[i],ScanPwrBuf,8))
					{
						if(IAP_read(ADR_ERR + 0) != 0)	//解除密码锁定(密码错误计数)
						{
							/** 密码失败超过CMAX_PSERR次, 将数据写入flash **/
							IAP_erase(ADR_ERR);
							IAP_pro_ver(ADR_ERR ,0);
						}
						
						Open_Door_Program();	    //开门程序流程
						power(off);               //关闭电源
					}
				}
				GreenOff();
				RedOn();
				BuzzerCTL(2,150);
				RedOff();

				iErr++;
				GreenOn();
			}
			else MK2=1;
			//关闭电源
		}
		power(off);               //关闭电源
	}
	//-----------------------------------指纹开门或者芯片开门--------------------------

	systeminit();
	LED_delay_time(10);
	//Pin_SP=0;
	Clear_Timer();
	if (VefPSW())
	{
		while(1)
		{
			//i=key();
			//while(i==CODE_K1)
			//{
			//	i=key();
			//}
			//key_number=key_fun();
			WDG_CONTR=0x3d;  //喂狗 1.13s
			if(fDkey==0)key_number=key();
			else key_number=CODE_K5|CODE_K7;
			switch (key_number)
			{
				case CODE_K3:                        //登记指纹
					{
						//Pin_SP=0;
						GreenOn();
						//key_number=0x00;
						BuzzerCTL(1,200);
						login();                          //登记指纹程序
						if (doorflag==1){CloseDoor();}
						Module_power(off);
						//LowVolAlarm();//电压检测报警
						power(off);                       //关闭电源
					}break;
				case CODE_K4:                       //删除指纹
					{
						//Pin_SP=0;
						GreenOn();
						BuzzerCTL(1,200);
						deletef();                         //删除指纹程序
						if (doorflag==1){CloseDoor();}
						Module_power(off);
						//LowVolAlarm();//电压检测报警
						power(off);                       //关闭电源
					}break;
				case CODE_K7:
					{
						//Pin_SP=0;
						i=key_fun();
						if(i==CODE_K5|CODE_K7)fDkey=1;
					}break;
				case CODE_K5:
					{
						//Pin_SP=0;
						i=key_fun();
						if(i==CODE_K5|CODE_K7)fDkey=1;
					}break;
				case (CODE_K5|CODE_K7):             //K5与K7同时按下,删除全部指纹
					{
						//GreenOn();
						WDG_CONTR=0x3d;  //喂狗 1.13s
						//Pin_SP=0;
						Clear_Timer();  //清定时器
						i_buf[0]=0;
						i_buf[1]=0;
						i_buf[2]=0;
						Command(i_buf);
						i=CODE_K5|CODE_K7;
						while(i==(CODE_K5|CODE_K7))
						{
							i=key();
							WDG_CONTR=0x3d;  //喂狗 1.13s
							if(Second>=3)  //长按3秒
							{
								i=0xff;
							}
						}
						if(i!=0xff)power(off);   //不到3秒松开按键则跳出
						RedOn();
						BuzzerCTL(1,500);
						j=key();
						while(j!=0)
						{
							j=key();  //等待松开按键.
							WDG_CONTR=0x3d;  //喂狗 1.13s
						}
						RedOff();
						GreenOn();
						if(ScanPwr())                     //确认厂家清除密码
						{
							if(!ArrCompare(SuperPwrBuf,ScanPwrBuf,6))
							{
								GreenOff();
								RedOn();
								BuzzerCTL(2,150);
								RedOff();
								power(off);
							}
						}
						else
						{
							GreenOff();
							RedOn();
							BuzzerCTL(2,150);
							RedOff();
							power(off);
						}
						key_number=0x00;
						delete_all();                     //删除全部指纹程序
						if (doorflag==1){CloseDoor();}
						Module_power(off);
						//LowVolAlarm();//电压检测报警
						power(off);                       //关闭电源
					}break;
			    default:                            //指纹开门
			    	{
			    		WDG_CONTR=0x3d;  //喂狗 1.13s
			    		//Pin_SP=1;
			    		if(MK1==0)
			    		{
			    			FlagBuf=ReadUserInfo();
			    			MK1=1;
			    			i_buf[0]=3;
			    			i_buf[1]=0;
			    			i_buf[2]=57;
			    			flag_com_rx_finish=0;
			    			Command(i_buf);
			    			OSSemPend(&flag_com_rx_finish,500,&err);
			    		}
			    		switch (FlagBuf)
			    		{
			    			//-------------------------------------------------------------------------
			    			case 1:                         //指纹库空 开门 进行 出厂设置
			    				{
			    					if (doorflag==0){OpenDoor();doorflag=1; clk2=0;Second=0;}
			    					if (Second>=5)                //如果超过5秒  关闭电源
			    					{
			    						RedOn();
			    						BuzzerCTL(2,150);
			    						RedOff();
			    						if (doorflag==1){CloseDoor();}
			    						//LowVolAlarm();//电压检测报警
			    						power(off);                 //关闭电源
			    					}
			    					// delay(5);
			    				}break;
			    				//-------------------------------------------------------------------------
			    			case 2:
			    				{
			    					//if(Pin_NC)   //如果检测到芯片信号
			    					//{
			    					//	Module_power(off);
			    					//	Open_Door_Program();	    //开门程序流程
			    					//  power(off);               //关闭电源
			    					//}
			    					i=Main_searchfinger();
			    					if(i==0xaa)
			    					{
			    						if(IAP_read(ADR_ERR + 0) != 0)	//解除密码锁定(密码错误计数)
			    						{
									    	IAP_erase(ADR_ERR);
									    	IAP_pro_ver(ADR_ERR ,0);
			    						}
			    						Module_power(off);
			    						Open_Door_Program();	    //开门程序流程
			    						power(off);               //关闭电源
			    					}
			    					else if(i==0x55)            //比对失败，则重试
			    					{
			    						Compare_Number++;
			    						RedOn();
			    						BuzzerCTL(3,150);         //delay(20);
			    						RedOff();
			    						if ((Compare_Number)>=3)  //如果3次验证不通过,关闭电源
			    						{
			    							Module_power(off);
			    							//LowVolAlarm();//电压检测报警
			    							power(off);             //关闭电源
			    						}
			    						clk2=0;
			    						Second=0;
			    						i_buf[0]=3;
			    						i_buf[1]=0;
			    						i_buf[2]=57;
			    						flag_com_rx_finish=0;
			    						Command(i_buf);
			    						OSSemPend(&flag_com_rx_finish,500,&err);
			    					}
			    					else if(i==0)
			    					{
			    						if (Second>=8)             //如果超过8秒  关闭电源
			    						{
			    							RedOn();
			    							BuzzerCTL(3,150);
			    							RedOff();
			    							if (doorflag==1){CloseDoor();}
			    							//LowVolAlarm();//电压检测报警
			    							power(off);               //关闭电源
			    						}
			    						delay(3);
			    					}
			    					else
			    					{
			    						RedOn();
			    						BuzzerCTL(3,150);         //delay(20);
			    						RedOff();
			    						power(off);               //关闭电源
			    					}
			    				}break;
			    					//-------------------------------------------------------------------------
			    			default:
			    				{
			    					RedGreenBuzzerCTL(3,200);     //红灯，绿灯，蜂鸣器同时短闪叫3次
			    					//LowVolAlarm();//电压检测报警
			    					power(off);                   //关闭电源
			    				}
			    						//-------------------------------------------------------------------------
			    		}
			    	}
													//----end of default
			}
		}
	}
	//LowVolAlarm();//电压检测报警
	power(off);                               //关闭电源
}


//定时器0中断,不够8个就在此发送
void T0_int(void) interrupt INT_T0		//5ms进入中断一次.
{
	TH0	= 0xdc;
	TL0	= 0x31;
	//Pin_test=!Pin_test;
	clk2++;
	clk0++;
	clk1++;
 //ADC_CONTR=ADC_CONTR | 0x08;           //启动AD转换
 if (clk2==200)
 {
   clk2=0;
   Second++;
   if(Second>=60){Minute++;Second=0;}
 }
	cntSemPend++;
	if(cntComRxTimeout>=5)
	{
		cntComRxTimeout=5;
		if(cntComRxIndex!=0)
		{
			comRxLength=cntComRxIndex;
			flag_com_rx_finish=1;
		}
		cntComRxIndex=0;
	}
	cntComRxTimeout++;
	//CntCanTxTimeout++;
}
//串口接收中断函数 
void serial () interrupt 4 using 1 
{
	unsigned char ch;
  if(RI)
  {
  	RI = 0;
    ch=SBUF;
  	comRxDataBuf[cntComRxIndex++] = ch;
  	cntComRxTimeout=0;
  }
}

void ADC_int(void) interrupt INT_ADC		//ADC中断
{
	//INT16U i_16u,j_16u;
  
  INT8U ADC_buf;                      //AD转换寄存器
  ADC_CONTR = ADC_CONTR & 0xef;         //清转换结束标志
  ADC_buf = ADC_DATA;
	ADBuf[PointADC]=ADC_DATA;
	PointADC++;
	if(PointADC>=ADC_BUF_SIZE)fAdCir=1;
	PointADC%=ADC_BUF_SIZE;
  ADC_CONTR=ADC_CONTR | 0x08;           //启动AD转换
}