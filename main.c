//****************************************************************
//  ��Ŀ����:ָ����
//  ��    ��:�ſ���
//  оƬ����:STC12LE5616AD
//  V1.0����:2008-05-20
//  ��    ��:V2.0
//  ��    ע:����ʽָ��ģ��
//  �޸�����:2011-09-14
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
#define CMAX_PSERR 6		//�������ʧ�ܴ���
unsigned char key_number;
unsigned char key();
unsigned char key_pwr();
unsigned char key_fun();
unsigned char FlagBuf;
unsigned char T0cnt=0;
unsigned char ScanPwrBuf[8];
unsigned char fPwrOk;
unsigned char iErr = 0;		//��������������(���������������ͣ������뿪������ʧЧ)
unsigned char fDkey;
unsigned char fAdCir;
//unsigned char MK;
INT8U UsePwrBuf[6][8];  //�û���������
INT8U SuperPwrBuf[6];   //������������

INT8U Version[2] = {0xA5, 0x00};   //�汾��
INT8U datetime[4] = {0x20, 0x14, 0x12, 0x25};   //�汾��

INT16U code ADR_PWRDATA[]={0x0800,0x0810,0x0820,0x0830,0x0840,0x0850};
//�����ŵ�ַ 1�����������6���û�����

//------------------------------------------------------------------------------------------------
//������
//------------------------------------------------------------------------------------------------
static void MainTxdByte(unsigned char i)
{

    TI = 0;
    SBUF = i;
    while(!TI);
   TI = 0;
}

void  mem_ini()                   //�ڴ��ʼ������
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
	SearchNumber=0xffff;//��������ָ�����к�
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
	if(ADCrackDat<=0x80)ADCrackDat=0xa4;    //������ֵС0x80ʱ��Ϊ����,ȡ0xa4ΪУ��ֵ

	i=IAP_read(IAP_ADR_DEFAULT);
	if(i==0x00)            //��ʼ��,�����������
	{
		LED_delay_time(10);
		i=IAP_read(IAP_ADR_DEFAULT);
		if(i==0x00)          //�ٴ�ȷ���Ƿ��ʼ��
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
				pwr_ran[k]=j%5+1;      //�����������
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
	for(k=0;k<6;k++)  //��ȡ�û���������
	{
		for(i_16u=ADR_PWRDATA[k];i_16u<(ADR_PWRDATA[k]+8);i_16u++)
		{
			j=i_16u-ADR_PWRDATA[k];
			UsePwrBuf[k][j]=IAP_read(i_16u);
		}
	}
	for(i_16u=ADR_SUPERPWR;i_16u<(ADR_SUPERPWR+6);i_16u++)//��ȡ������������
	{
		j=i_16u-ADR_SUPERPWR;
		SuperPwrBuf[j]=IAP_read(i_16u);
	}
	for(i_16u=0;i_16u<58;i_16u++)   //��ȡָ��ģ���
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
//************************����ɨ��************************************
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
  	  //if(key_buf0!=CODE_K7)	//zhanglong 2014/12/25 10:51 ע�͵��˴���
  	  {
  	  	BuzzerCTL(1,150);
		    GreenOn();
		  }
  	  while(key_buf1!=0)
  	  {
  	  	key_buf1=keytest();  //�ȴ��ɿ�����
  	  	WDG_CONTR=0x3d;  //ι�� 1.13s
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
  			WDG_CONTR=0x3d;  //ι�� 1.13s
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
  		//  key_buf1=keytest();  //�ȴ��ɿ�����
  	  //}
  	  return key_buf0;
  	}
  }
}
unsigned char ScanPwr()
{
	unsigned char key_number;
	unsigned char l=0;
	Clear_Timer();  //�嶨ʱ��
	while(l<6)  //6λ����
	{
		WDG_CONTR=0x3d;  //ι�� 1.13s
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
	Clear_Timer();  //�嶨ʱ��
	while(l<8)  //8λ����
	{
		key_number=key();
		WDG_CONTR=0x3d;  //ι�� 1.13s
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
			if(IAP_read(ADR_ERR + 0) != iErr)	//ֻ�����뿪�����̣���������������󣬲Ż����������
			{
				/** ����ʧ�ܴ������µ�flash **/
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
 * ���ܼ����º��Ѿ��й�������ʾ���˴�ע�͵�������ʾ
 ******************************************************************************************************/
void K7_Open_Door_Program()                  //���ܼ����ų�������
{
  unsigned char i=0;
  Module_power(off);                      //�ر�ģ���Դ
  GreenOn();//RedOn();
  //BuzzerCTL(1,200);
  OpenDoor();
  RedOff();
  Clear_Timer();
  for(i=0;i<6;i++)
  {
  	LED_delay_time(633);
  	WDG_CONTR=0x3d;  //ι�� 1.13s
  }
  CloseDoor();
  return;    //���5����û�а��Ű���  ֱ�ӹ���
}
//*******************************************************************************************************
void Open_Door_Program()                  //���ų�������
{
  unsigned char i=0;
  Module_power(off);                      //�ر�ģ���Դ
  GreenOn();//RedOn();
  BuzzerCTL(1,200);
  OpenDoor();
  RedOff();
  Clear_Timer();
  for(i=0;i<6;i++)
  {
  	LED_delay_time(633);
  	WDG_CONTR=0x3d;  //ι�� 1.13s
  }
  CloseDoor();
  return;    //���5����û�а��Ű���  ֱ�ӹ���
}
//*******************************************************************************************************

void	T0_Ini() reentrant using 0		//��ʱ��0��ʼ��,5ms�ж�һ��
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
	PCON &= 0x7f;		//�����ʲ�����
	SCON = 0x50;		//8λ����,�ɱ䲨����
	AUXR &= 0xbf;		//��ʱ��1ʱ��ΪFosc/12,��12T
	AUXR &= 0xfe;		//����1ѡ��ʱ��1Ϊ�����ʷ�����
	//TMOD &= 0x0f;		//�����ʱ��1ģʽλ
	//TMOD |= 0x20;		//�趨��ʱ��1Ϊ8λ�Զ���װ��ʽ
	TMOD = 0x21;
	TL1 = 0xFA;		//�趨��ʱ��ֵ
	TH1 = 0xFA;		//�趨��ʱ����װֵ
	ET1 = 0;		//��ֹ��ʱ��1�ж�
	TR1 = 1;		//������ʱ��1
	ES=1;
}

//------------------------------------------------------------------------------------------------
//������������
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
	WDG_CONTR=0x3d;  //ι�� 1.13s
	//Pin_SP=0;
	//-------------------------test
	//PIN_GLED=0;
	//LED_delay_time(633);
	//PIN_GLED=1;
	//WDG_CONTR=0x3d;  //ι�� 1.13s
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
	key_number=key_pwr();   //��ȡ�ϵ�ʱ����ֵ
	if((key_number!=CODE_K1)&&(key_number!=CODE_K2)&&(key_number!=CODE_K3)&&(key_number!=CODE_K4)&&(key_number!=CODE_K5)&&(key_number!=CODE_K7))
	{
		if(key_number==(CODE_K1|CODE_K3|CODE_K5))    //ADת��У����
		{
			BuzzerCTL(1,1000);
			GreenOn();
			fAdCir=0;
			PointADC=0;
			ADC_CONTR=ADC_CONTR | 0x08;           //����ADת��
			while(!fAdCir)WDG_CONTR=0x3d;        //�ȴ�һ������ת�����
			//------------------------------------------------------------------------
			for(i=0;i<ADC_BUF_SIZE;i++)           //AD�����˲�����
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
	ADC_CONTR=ADC_CONTR | 0x08;               //����ADת��
	if(key_number==CODE_K7)     //����ֱ�ӿ��Ű�ť,����ң�ؿ���
	{
		if(IAP_read(ADR_ERR + 0) != 0)	//�����������(����������)
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
		//power(off);               //�رյ�Դ
		//ADD_Manager_User(0);
		//power(off);               //�رյ�Դ
		//i=searchfinger();
		//if(i==0)
		//{
		//	RedBuzzerCTL(3,150);
		//	power(off);               //�رյ�Դ
		//}
		//else
		//{
		//	RedBuzzerCTL(1,150);
		//	power(off);               //�رյ�Դ
		//}
		//while(1);
		for(i=0;i<6;i++)
		{
			MainTxdByte(SuperPwrBuf[i]);  //��ʾ��������
		}
		MainTxdByte(ADCrackDat);        //��ʾУ������
		
		for(i=0;i<2;i++)
		{
			MainTxdByte(Version[i]);        //��ʾ�汾��
		}
		for(i=0;i<4;i++)
		{
			MainTxdByte(datetime[i]);        //��ʾʱ��
		}
		
		//Open_Door_Program();	    //���ų�������
		K7_Open_Door_Program();
		power(off);               //�رյ�Դ
	}
	//for(i=0;i<10;i++)
	//{
	//	Par[i]=DEFAULT_CONFIG[i];
	//}
	if(key_number==CODE_K2)     //���뿪��
	{
		//BuzzerCTL(1,150);
		//GreenOn();
		while(1)
		{
			WDG_CONTR=0x3d;  //ι�� 1.13s
			LED_delay_time(10);
			key_number=key();
			Clear_Timer();  //�嶨ʱ��
			if(key_number==CODE_K7||MK2==1)  //����/�޸�����
			{
				//BuzzerCTL(1,150);
				WDG_CONTR=0x3d;  //ι�� 1.13s
				Module_power(on);
				BuzzerCTL(1,150);
				if(VefPSW())//(1)
				{
					while(key_number==CODE_K7)
					{
						key_number=key();
						WDG_CONTR=0x3d;   //ι�� 1.13s
						if(Second>=5)     //����5��,�����������
						{
							RedOff();
							GreenOn();
							BuzzerCTL(1,1000);
							i=AdminFiger_chek();  //У�����ָ��
							GreenOff();
							RedOn();
							BuzzerCTL(1,1000);
							IAP_erase(ADR_USERPWR);
							for(k=0;k<6;k++)  //��ȡ�û���������
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
					Clear_Timer();  //�嶨ʱ��
					i=AdminFiger_chek();
					GreenBuzzerCTL(1,150);
					GreenOn();
					ScanOpenWrd();
					for(i=0;i<6;i++)
					{
						if(UsePwrBuf[i][0]==0||UsePwrBuf[i][0]>=6)  //����Ϊ��
						{
							for(j=0;j<8;j++)
							{
								UsePwrBuf[i][j]=ScanPwrBuf[j];
							}
							IAP_erase(ADR_USERPWR);
							for(k=0;k<6;k++)  //д���û���������
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
					//��������
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
					/** ����ʧ�ܳ���CMAX_PSERR��, ������д��flash **/
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
						if(IAP_read(ADR_ERR + 0) != 0)	//�����������(����������)
						{
							/** ����ʧ�ܳ���CMAX_PSERR��, ������д��flash **/
							IAP_erase(ADR_ERR);
							IAP_pro_ver(ADR_ERR ,0);
						}
						
						Open_Door_Program();	    //���ų�������
						power(off);               //�رյ�Դ
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
			//�رյ�Դ
		}
		power(off);               //�رյ�Դ
	}
	//-----------------------------------ָ�ƿ��Ż���оƬ����--------------------------

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
			WDG_CONTR=0x3d;  //ι�� 1.13s
			if(fDkey==0)key_number=key();
			else key_number=CODE_K5|CODE_K7;
			switch (key_number)
			{
				case CODE_K3:                        //�Ǽ�ָ��
					{
						//Pin_SP=0;
						GreenOn();
						//key_number=0x00;
						BuzzerCTL(1,200);
						login();                          //�Ǽ�ָ�Ƴ���
						if (doorflag==1){CloseDoor();}
						Module_power(off);
						//LowVolAlarm();//��ѹ��ⱨ��
						power(off);                       //�رյ�Դ
					}break;
				case CODE_K4:                       //ɾ��ָ��
					{
						//Pin_SP=0;
						GreenOn();
						BuzzerCTL(1,200);
						deletef();                         //ɾ��ָ�Ƴ���
						if (doorflag==1){CloseDoor();}
						Module_power(off);
						//LowVolAlarm();//��ѹ��ⱨ��
						power(off);                       //�رյ�Դ
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
				case (CODE_K5|CODE_K7):             //K5��K7ͬʱ����,ɾ��ȫ��ָ��
					{
						//GreenOn();
						WDG_CONTR=0x3d;  //ι�� 1.13s
						//Pin_SP=0;
						Clear_Timer();  //�嶨ʱ��
						i_buf[0]=0;
						i_buf[1]=0;
						i_buf[2]=0;
						Command(i_buf);
						i=CODE_K5|CODE_K7;
						while(i==(CODE_K5|CODE_K7))
						{
							i=key();
							WDG_CONTR=0x3d;  //ι�� 1.13s
							if(Second>=3)  //����3��
							{
								i=0xff;
							}
						}
						if(i!=0xff)power(off);   //����3���ɿ�����������
						RedOn();
						BuzzerCTL(1,500);
						j=key();
						while(j!=0)
						{
							j=key();  //�ȴ��ɿ�����.
							WDG_CONTR=0x3d;  //ι�� 1.13s
						}
						RedOff();
						GreenOn();
						if(ScanPwr())                     //ȷ�ϳ����������
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
						delete_all();                     //ɾ��ȫ��ָ�Ƴ���
						if (doorflag==1){CloseDoor();}
						Module_power(off);
						//LowVolAlarm();//��ѹ��ⱨ��
						power(off);                       //�رյ�Դ
					}break;
			    default:                            //ָ�ƿ���
			    	{
			    		WDG_CONTR=0x3d;  //ι�� 1.13s
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
			    			case 1:                         //ָ�ƿ�� ���� ���� ��������
			    				{
			    					if (doorflag==0){OpenDoor();doorflag=1; clk2=0;Second=0;}
			    					if (Second>=5)                //�������5��  �رյ�Դ
			    					{
			    						RedOn();
			    						BuzzerCTL(2,150);
			    						RedOff();
			    						if (doorflag==1){CloseDoor();}
			    						//LowVolAlarm();//��ѹ��ⱨ��
			    						power(off);                 //�رյ�Դ
			    					}
			    					// delay(5);
			    				}break;
			    				//-------------------------------------------------------------------------
			    			case 2:
			    				{
			    					//if(Pin_NC)   //�����⵽оƬ�ź�
			    					//{
			    					//	Module_power(off);
			    					//	Open_Door_Program();	    //���ų�������
			    					//  power(off);               //�رյ�Դ
			    					//}
			    					i=Main_searchfinger();
			    					if(i==0xaa)
			    					{
			    						if(IAP_read(ADR_ERR + 0) != 0)	//�����������(����������)
			    						{
									    	IAP_erase(ADR_ERR);
									    	IAP_pro_ver(ADR_ERR ,0);
			    						}
			    						Module_power(off);
			    						Open_Door_Program();	    //���ų�������
			    						power(off);               //�رյ�Դ
			    					}
			    					else if(i==0x55)            //�ȶ�ʧ�ܣ�������
			    					{
			    						Compare_Number++;
			    						RedOn();
			    						BuzzerCTL(3,150);         //delay(20);
			    						RedOff();
			    						if ((Compare_Number)>=3)  //���3����֤��ͨ��,�رյ�Դ
			    						{
			    							Module_power(off);
			    							//LowVolAlarm();//��ѹ��ⱨ��
			    							power(off);             //�رյ�Դ
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
			    						if (Second>=8)             //�������8��  �رյ�Դ
			    						{
			    							RedOn();
			    							BuzzerCTL(3,150);
			    							RedOff();
			    							if (doorflag==1){CloseDoor();}
			    							//LowVolAlarm();//��ѹ��ⱨ��
			    							power(off);               //�رյ�Դ
			    						}
			    						delay(3);
			    					}
			    					else
			    					{
			    						RedOn();
			    						BuzzerCTL(3,150);         //delay(20);
			    						RedOff();
			    						power(off);               //�رյ�Դ
			    					}
			    				}break;
			    					//-------------------------------------------------------------------------
			    			default:
			    				{
			    					RedGreenBuzzerCTL(3,200);     //��ƣ��̵ƣ�������ͬʱ������3��
			    					//LowVolAlarm();//��ѹ��ⱨ��
			    					power(off);                   //�رյ�Դ
			    				}
			    						//-------------------------------------------------------------------------
			    		}
			    	}
													//----end of default
			}
		}
	}
	//LowVolAlarm();//��ѹ��ⱨ��
	power(off);                               //�رյ�Դ
}


//��ʱ��0�ж�,����8�����ڴ˷���
void T0_int(void) interrupt INT_T0		//5ms�����ж�һ��.
{
	TH0	= 0xdc;
	TL0	= 0x31;
	//Pin_test=!Pin_test;
	clk2++;
	clk0++;
	clk1++;
 //ADC_CONTR=ADC_CONTR | 0x08;           //����ADת��
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
//���ڽ����жϺ��� 
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

void ADC_int(void) interrupt INT_ADC		//ADC�ж�
{
	//INT16U i_16u,j_16u;
  
  INT8U ADC_buf;                      //ADת���Ĵ���
  ADC_CONTR = ADC_CONTR & 0xef;         //��ת��������־
  ADC_buf = ADC_DATA;
	ADBuf[PointADC]=ADC_DATA;
	PointADC++;
	if(PointADC>=ADC_BUF_SIZE)fAdCir=1;
	PointADC%=ADC_BUF_SIZE;
  ADC_CONTR=ADC_CONTR | 0x08;           //����ADת��
}