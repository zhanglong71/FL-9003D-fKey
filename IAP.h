#ifdef  IAP_GLOBALS
#define IAP_EXT
#else
#define IAP_EXT  extern
#endif


/*        新增特殊功能寄存器定义        */
sfr		ISP_DATA	=	0xe2;
sfr   ISP_ADDRH	=	0xe3;
sfr		ISP_ADDRL	= 0xe4;
sfr		ISP_CMD		= 0xe5;
sfr		ISP_TRIG	= 0xe6;
sfr		ISP_CONTR = 0xe7;


#define READ_AP_and_Data_Memory_Command				      0x01        /*  字节读应用程序区和数据存储区   */
#define PROGRAM_AP_and_Data_Memory_Command          0x02        /*  字节编程应用程序区和数据存储区 */
#define SECTOR_ERASE_AP_and_Data_Memory_Command     0x03        /*  扇区擦除应用程序区和数据存储区 */


#define		DELAY_CONST         60000

/* 定义常量 */
#define ERROR   0
#define OK      1

/* 定义Flash 操作等待时间20MHz */
#define WAIT_TIME        0x03


#define USED_BYTE_QTY_IN_ONE_SECTOR                128

IAP_EXT INT8U xdata protect_buffer[USED_BYTE_QTY_IN_ONE_SECTOR];


#define ADR_ERR			0x0400   //密码出错次(3rd sector)
#define ADR_FTAB        0x0600   //指纹占用映射数据存放首地址(4th sector)
#define ADR_USERPWR     0x0800   //设置用户密码数据存放首地址(5th sector)
#define ADR_SUPERPWR    0x0A00   //设置管理密码数据存放首地址(6th sector)
#define IAP_ADR_DEFAULT 0x0C00   //初始化标识地址(7th sector)
#define ADR_ADCRACK     0x0e00   //AD转换校准数据(8th sector)


IAP_EXT void ISP_IAP_enable(void);
IAP_EXT void ISP_IAP_disable(void);
IAP_EXT INT8U IAP_read(INT16U byte_addr);
IAP_EXT INT8U IAP_erase(INT16U sector_addr);
IAP_EXT INT8U IAP_pro_ver(INT16U byte_addr, INT8U original_data);
