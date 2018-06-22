#define  FIGER_GLOBALS
#include <includes.h>
//#include <cmd_code.h>
//unsigned int  SearchNumber=0xffff;//搜索到的指纹序列号
//unsigned char SaveNumber=0;
//==========================================================================
//===========================
void Clear_Timer()
{
 clk2=0;
 Second=0;
 Minute=0;
}
unsigned char AdminFiger_chek()    //检测管理指纹
//--返回值 1:指纹ID.其它状态直接关闭电源
{
  unsigned char i;
  unsigned char MK=0;
  unsigned char Compare_Number=0;
  while(1)
  {
  	WDG_CONTR=0x3d;  //喂狗 1.13s
  	if(MK==0){i=ReadUserInfo();MK=1;}
    switch (i)
    {
    	WDG_CONTR=0x3d;  //喂狗 1.13s
      //-------------------------------------------------------------------------
      case 1:                         //指纹库空
      {
      	RedOn();
      	BuzzerCTL(2,150);
    	  RedOff();
        //LowVolAlarm();//电压检测报警
        power(off);             //关闭电源
        return(0);
      }break;
      //-------------------------------------------------------------------------
      case 2:
      {
      		if(searchfinger()==1)
      		{
      			if(SearchNumber==0)return(1);       //返回指纹ID
      			else
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
            }
    		  }
          else
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
}
//==========================================================================
void delete_all(void)    //清空指纹库,删除0-57指纹
{
	unsigned char i=0;
  unsigned char i_buf[3];
  unsigned int i_16u;
  RedOn();              //红灯亮
  BuzzerCTL(1,100);     //蜂鸣器鸣叫一下
  i_buf[0]=4;
  i_buf[1]=0;
  i_buf[2]=57;
  Command(i_buf);
  for(i=0;i<58;i++)userFigerTab[i]=0x30;  //将指纹记入表格
  IAP_erase(ADR_FTAB);
  for(i_16u=0;i_16u<58;i_16u++)
	{
		IAP_pro_ver(ADR_FTAB+i_16u,userFigerTab[i_16u]);
	}
  for(i=0;i<6;i++) //等待3s
  {
  	WDG_CONTR=0x3d;  //喂狗 1.13s
   LED_delay_time(633);
  }
  BuzzerCTL(1,500);    //蜂鸣器长叫1s
  RedOff();
  LED_delay_time(400);
  LED_Red_Flash(3,400); //红绿灯同时闪三下
}
//*******************************************************************************************************
unsigned char ReadUserInfo()
{
  
  if(userFigerTab[0]==0x30){return 1;}   //指纹库空   返回1
  else {return 2;}                       //指纹库非空 返回2
}
//*******************************************************************************************************
 //判断第ID号指纹是否已经登记  1:登记  0:未登记
bit Dis_ID_User_ON_OFF(unsigned int ID)
{
  if(userFigerTab[ID]==0x31){return 1;}
  else{return 0;}
}
//=================================
unsigned char searchfinger()  //比对指纹模板0-57
{
	unsigned char i_buf[3];
	unsigned char err;
  i_buf[0]=3;
  i_buf[1]=0;
  i_buf[2]=57;
  flag_com_rx_finish=0;
  Command(i_buf);
  OSSemPend(&flag_com_rx_finish,500,&err);
  if(err==OS_NO_ERR)
  {
  	flag_com_rx_finish=0;
  	OSSemPend(&flag_com_rx_finish,8000,&err);
  	if(err==OS_NO_ERR)
  	{
  		flag_com_rx_finish=0;
  		if((comRxDataBuf[0]==0x3a)&&(comRxDataBuf[1]==0x03)&&(comRxDataBuf[2]==0xaa)&&(comRxDataBuf[3]==0x01))  //比对成功
  		{
  			SearchNumber=comRxDataBuf[4];
  			return 1;
  		}
  		else return 0;
  	}
  	else return 0;
	}
	else         //超时
  {
  	return 0;
  }
}
unsigned char Main_searchfinger()  //无等待比对指纹模板0-57
{
  if(flag_com_rx_finish==1)
  {
  	flag_com_rx_finish=0;
  	if((comRxDataBuf[0]==0x3a)&&(comRxDataBuf[1]==0x03)&&(comRxDataBuf[3]==0x01))  //比对成功&&(comRxDataBuf[2]==0xaa)
  	{
  		return(comRxDataBuf[2]);
  	}
  	else return 0;
  }
  else return 0;
}
//=================================
unsigned char SaveFinger(unsigned char ID,unsigned char stime)    //注册指纹
{
	unsigned char i_buf[3];
	unsigned char err;
  i_buf[0]=2;
  i_buf[1]=stime;
  i_buf[2]=ID;
  flag_com_rx_finish=0;
  Command(i_buf);
  OSSemPend(&flag_com_rx_finish,500,&err);
  if(err==OS_NO_ERR)
  {
  	//flag_com_rx_finish=0;
  	OSSemPend(&flag_com_rx_finish,8000,&err);
  	if(err==OS_NO_ERR)
  	{
  		flag_com_rx_finish=0;
  		if((comRxDataBuf[0]==0x3a)&&(comRxDataBuf[1]==0x02)&&(comRxDataBuf[2]==0xaa)&&(comRxDataBuf[3]==0x01))return 1;  //注册成功
  		else return 0;
  	}
  	else return 0;
	}
	else         //超时
  {
  	return 0;
  }
}
//============================================================================================
unsigned char  DeleteFinger(unsigned char ID1,unsigned char ID2)
{
	unsigned char i,i_buf[3];
	unsigned int i_16u;
	unsigned char err;
  i_buf[0]=4;
  i_buf[1]=ID1;
  i_buf[2]=ID2;
  flag_com_rx_finish=0;
  Command(i_buf);
  OSSemPend(&flag_com_rx_finish,500,&err);
  if(err==OS_NO_ERR)
  {
  	for(i=ID1;i<=ID2;i++)userFigerTab[i]=0x30;  //将指纹记入表格
    IAP_erase(ADR_FTAB);
    for(i_16u=0;i_16u<58;i_16u++)
	  {
	  	IAP_pro_ver(ADR_FTAB+i_16u,userFigerTab[i_16u]);
	  }
  	flag_com_rx_finish=0;
  	return 1;
	}
	else         //超时
  {
  	return 0;
  }
}
//==========================================================
//==========================================================
unsigned char ADD_Manager_User(unsigned char Manager_ID) //根据管理权限 1，2，3添加指纹
{
  unsigned char i;
  unsigned char err;
  //unsigned char j;
  //unsigned char temp;
  unsigned char buf=0;
  unsigned char add_number=0;
  unsigned char ok_number=0;
  unsigned char blank_id=0;
  unsigned int i_16u;
  bit flag=0;
	buf=0;
  WDG_CONTR=0x3d;  //喂狗 1.13s
  if(Manager_ID==0)
  {
  	for (i=3;i<=20;i++)                  //显示空指纹
    {
    	if(!Dis_ID_User_ON_OFF(i)){break;} //查询首个空ID号码
      if (i==20){i=0xff;break;}  //如果查询到20号都没有空指纹退出
    }
    if(!Dis_ID_User_ON_OFF(1))blank_id=1;
  	else if(!Dis_ID_User_ON_OFF(2))blank_id=2;
  	else blank_id=i;
    if(blank_id==0xff){RedBuzzerCTL(3,1000);return 0;}
  }
  else if(Manager_ID==1)
  {
  	for (i=21;i<=38;i++)                  //显示空指纹
    {
    	if(!Dis_ID_User_ON_OFF(i)){break;} //查询首个空ID号码
	    //  buf++;
      if (i==38){i=0xff;break;}  //如果查询到58号都没有空指纹退出
    }
    if(!Dis_ID_User_ON_OFF(2))blank_id=2;
    else blank_id=i;
    if(i==0xff){RedBuzzerCTL(3,1000);return 0;}
  }
  else
  {
  	for (i=39;i<=57;i++)                  //显示空指纹
    {
    	if(!Dis_ID_User_ON_OFF(i)){break;} //查询首个空ID号码
	    //  buf++;
      if (i==57){i=0xff;break;}  //如果查询到58号都没有空指纹退出
    }
    if(i==0xff){RedBuzzerCTL(3,1000);return 0;}
    blank_id=i;
  }
  add_number++;
  GreenBuzzerCTL(1,150);                       //蜂鸣器叫一声
  //-------------------------------------------//第一次成功获取指纹图像和指纹特征
  i=0;
  OSSemPend(&i,1000,&err);
  GreenBuzzerCTL(1,150);
  i=SaveFinger(blank_id,1);
  if(i==0)return 0;
  i=SaveFinger(blank_id,2);
  if(i==0)return 0;
  userFigerTab[blank_id]=0x31;  //将已存指纹记入表格
  IAP_erase(ADR_FTAB);
  for(i_16u=0;i_16u<58;i_16u++)
	{
		IAP_pro_ver(ADR_FTAB+i_16u,userFigerTab[i_16u]);
	}
  BuzzerCTL(1,1000);
  LED_each_Flash(3,200);
  return 1;
//========================================================================================
}
//========================================================================================

unsigned char DEL_Manager_User(unsigned int Manager_ID)
{
	unsigned char i;
  GreenOn();
  while(1)
  {
  	WDG_CONTR=0x3d;  //喂狗 1.13s
    if(Manager_ID==0)                     //1号指纹则删除其它所有指纹
    {
      i=DeleteFinger(1,57);
      if(i==0)
      {
      	RedBuzzerCTL(3,150);
      	return 0;
      }
      else
      {
      	GreenBuzzerCTL(1,1000);
      	return 1;
      }
   	}
   	else if(Manager_ID==1)
    {
    	
    	i=DeleteFinger(21,38);
    	if(i==0)
      {
      	RedBuzzerCTL(3,150);
      	return 0;
      }
      else
      {
      	GreenBuzzerCTL(1,1000);
      	return 1;
      }
   	}
   	else
    {
    	i=DeleteFinger(39,57);
    	if(i==0)
      {
      	RedBuzzerCTL(3,150);
      	return 0;
      }
      else
      {
      	GreenBuzzerCTL(1,1000);
      	return 1;
      }
   	}
    //delay(100);
    WDG_CONTR=0x3d;  //喂狗 1.13s
    GreenBuzzerCTL(1,1000);
    return 1;
  }
}
//=====================================================================
void login(void)
{
	unsigned char i,err,buf,cmp_number=0;
  unsigned int temp1;
  unsigned int i_16u;
  buf=ReadUserInfo();
  if(buf==1)//指纹库为空  添加第一枚管理员指纹
  {
  	i=SaveFinger(0,1);
    if(i==0)
    {
    	RedBuzzerCTL(3,150);return;
    }
    i=SaveFinger(0,2);
    if(i==0)
    {
    	RedBuzzerCTL(3,150);return;
    }
    userFigerTab[0]=0x31;  //将已存指纹记入表格
    IAP_erase(ADR_FTAB);
    for(i_16u=0;i_16u<58;i_16u++)
	  {
	  	IAP_pro_ver(ADR_FTAB+i_16u,userFigerTab[i_16u]);
	  }
	  GreenBuzzerCTL(1,1000) ;
    LED_each_Flash(3,200) ;
    return;
  }
  if(buf==2)//指纹库不为空 搜索指纹
  {
  	loop_cmp1:
  	WDG_CONTR=0x3d;  //喂狗 1.13s
  	temp1=searchfinger();     //搜索返回ID号
    if (temp1==0)
    {
    	//比对不通过
      RedBuzzerCTL(3,150);
      cmp_number++;
      if(cmp_number<=2) goto loop_cmp1;
      return;
    }
    else
    {
    	if(SearchNumber<3)  //如果ID =0,1,2
      {
      	GreenBuzzerCTL(1,500);
        i=0;
        OSSemPend(&i,2000,&err);
        GreenBuzzerCTL(1,150);    //蜂鸣器叫3声 表示第二次输入指纹
        ADD_Manager_User(SearchNumber);//分组添加
      }
      else     //如果是4~32则没有添加权限
      {
      	RedBuzzerCTL(3,150);
        cmp_number++;
        if(cmp_number<=2) goto loop_cmp1;
        return;
      }
    }
  }
}
//==================================
void deletef(void)
{
	unsigned char buf,cmp_number=0;
  unsigned int temp1;
  buf=ReadUserInfo();
  if(buf==1)//指纹库为空 显示 －－
  {
  	RedBuzzerCTL(3,150);return;
    return;
  }
  if(buf==2)//指纹库不为空 搜索指纹
  {
  	loop_cmp:
  	WDG_CONTR=0x3d;  //喂狗 1.13s
    temp1=searchfinger();
    if (temp1==0)
    {//比对不通过
    	RedBuzzerCTL(3,150);
      cmp_number++;
      if  (cmp_number<=2) goto loop_cmp;
      return;
    }
    else
    {
    	if(SearchNumber<3)//如果ID=0,1,2
      {
      	RedOff();
        GreenBuzzerCTL(1,500);// 如果管理员认证通过,删除指纹
        DEL_Manager_User(SearchNumber);//分组删除
      }
      else  //其他人没有删除权限
      {
        RedBuzzerCTL(3,150);
        cmp_number++;
        if(cmp_number<=2) goto loop_cmp;
        return;
      }
    }
  }
}
//===================================
//==================================
bit VefPSW(void)             //模块验证码判断
{
	unsigned char count,i_buf[3];
	unsigned char err;
	count=0;
  i_buf[0]=0;
  i_buf[1]=0;
  i_buf[2]=0;
  while (1)
  {
    WDG_CONTR=0x3d;  //喂狗 1.13s
    flag_com_rx_finish=0;
    Command(i_buf);
    OSSemPend(&flag_com_rx_finish,500,&err);
    if(err==OS_NO_ERR)
    {
    	flag_com_rx_finish=0;
    	return 1;
	  }
	  count++;
    if(count>=2)
    {
    	RedOn();
      RedGreenBuzzerCTL(2,150); //验证口令不正确：红绿灯同时亮，并且蜂鸣器短叫两声
      return(0);   //error return
 	  }
    delay(10);
	 }
}

/*------------------------------------------------------------*/
//串口发送一个字节数据
/*------------------------------------------------------------*/
static void TxdByte(unsigned char i)
{
	TI = 0;
  SBUF = i;
  while(!TI);
  TI = 0;
}
//==================================================
//  FUNCTION:   Sent Command-string
//=================================================
void Command(unsigned char *p) //发送命令
{
  TxdByte(0x3a);
	TxdByte(0xff);
	TxdByte(0xff);
	TxdByte(0xff);
	TxdByte(p[0]);
	TxdByte(p[1]);
	TxdByte(p[2]);
	TxdByte(0x3a+0xff+0xff+0xff+p[0]+p[1]+p[2]);
}