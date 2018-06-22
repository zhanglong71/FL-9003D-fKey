#ifdef  ADC_GLOBALS
#define ADC_EXT
#else
#define ADC_EXT extern
#endif
#define INT8U unsigned char        //无符号8位数
#define INT8S signed char          //有符号8位数
#define INT16U unsigned int        //无符号16位数
#define INT16S signed int          //有符号16位数
#define INT32U unsigned long       //无符号32位数
#define INT32S signed long         //有符号32位数
#define FP32 float                 //单精度浮点数
#define FP64 double                //双精度浮点数

#define  CN_INIT_ADC     0x80   //选第一路ADC，并开启ADC电源,速率为840个时钟周期
#define  CN_INIT_AUXR    0x30   //t0,t1为传统定时器,UART为传统速率,EADCI=1,ESPI=0,ELVDI=0



#define ADC_BUF_SIZE 32
#define ADC_BUF_TIAL 8                        //排序后去掉两端AD值的个数
                       

ADC_EXT INT8U  ADBuf[ADC_BUF_SIZE];        //AD转换缓存
ADC_EXT INT8U  ADCBuf[ADC_BUF_SIZE];       //AD转换缓存
ADC_EXT INT8U  PointADC;                   //AD转换缓冲指针
ADC_EXT INT8U  ADCrackDat;                 //AD转换校准数据


ADC_EXT void ADC_Init();
ADC_EXT void ADC_ArrIbub(INT8U p[],INT8U n);     //数组排序函数