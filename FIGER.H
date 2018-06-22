#ifdef  FIGER_GLOBALS
#define FIGER_EXT
#else
#define FIGER_EXT extern
#endif

#define MAX_NUMBER    20
FIGER_EXT unsigned char userFigerTab[58]; //指纹模板表
FIGER_EXT unsigned char idata comRxDataBuf[MAX_NUMBER];
FIGER_EXT unsigned char comRxLength;
FIGER_EXT unsigned char cntComRxIndex;
FIGER_EXT unsigned char cntComRxTimeout;

FIGER_EXT unsigned char flag_com_rx_finish;

FIGER_EXT unsigned int  SearchNumber;//搜索到的指纹序列号
FIGER_EXT void Clear_Timer();
FIGER_EXT unsigned char AdminFiger_chek();
FIGER_EXT void delete_all(void);
FIGER_EXT unsigned char ReadUserInfo();
FIGER_EXT void Command(unsigned char *p);
FIGER_EXT unsigned char searchfinger(void);
FIGER_EXT unsigned char Main_searchfinger();  //无等待比对指纹模板0-57
FIGER_EXT bit  VefPSW(void);
FIGER_EXT void login(void);
FIGER_EXT void deletef(void);
FIGER_EXT unsigned char ADD_Manager_User(unsigned char Manager_ID);
FIGER_EXT unsigned char SaveFinger(unsigned char ID,unsigned char stime);    //注册指纹
