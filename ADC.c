
#define  ADC_GLOBALS
#include <includes.h>


/*****************************************************************************
函数名称: ADC_Init
函数功能: ADC初始化
输入参数: ADC通道号
输出参数: 无
****************************************************************************/
void ADC_Init()
{
  INT8U i;
	P1M0 = 0x01;                //P1.0为开漏，其余I/O为传统口
  P1M1 = 0x01;
  ADC_CONTR =CN_INIT_ADC;     //0x80,选第一路ADC P1.0，并开启ADC电源,速率为840个时钟周期
  
  LED_delay_time(5);          //开A/D转换电源后要加延时，1ms以内就足够了
  AUXR=CN_INIT_AUXR;          //0x10,t0,t1为传统定时器,UART为传统速率,EADCI=1,ESPI=0,ELVDI=0
  EADC_SPI=1;
  ADC_DATA = 0;
	//ADC_LOW2 = 0;
  
  for(i=0;i<ADC_BUF_SIZE;i++)              //清空AD转换数据缓存
  {
   	ADBuf[i]=0xff;          //防止计算后出来负值
  }
  PointADC=0;
  ADC_ArrIbub(ADBuf,ADC_BUF_SIZE);
}


/*****************************************************************************
函数名称: ArrIbub
函数功能: 数组排序函数
输入参数: 数组指针,排序数量
输出参数: 无
****************************************************************************/
void ADC_ArrIbub(INT8U p[],INT8U n)                         //数组排序函数
{
	INT8U m,k,j,i;
	INT8U d;
	k=0;
	m=n-1;
	while(k<m)
	{
		j=m-1;
		m=0;
		for(i=k;i<=j;i++)
		{
			if(p[i]>p[i+1])
			{
				d=p[i];
				p[i]=p[i+1];
				p[i+1]=d;
				m=i;
			}
		}
		j=k+1;
		k=0;
		for(i=m;i>=j;i--)
		{
			if(p[i-1]>p[i])
			{
				d=p[i];
				p[i]=p[i-1];
				p[i-1]=d;
				k=i;
			}
		}
	}
}