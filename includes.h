#include <STC12C.H>
#include <stdio.h>
#include <stdlib.h>
#include <intrins.h>
#include <proce.h>
#include <main.h>
#include <figer.h>
//#include <Time.h>
#include <ADC.h>
//#include <KEY.h>
#include <IAP.h>
#include <OS.h>
#include <string.h>
#include <math.h>

#define INT8U unsigned char        //无符号8位数
#define INT8S signed char          //有符号8位数
#define INT16U unsigned int        //无符号16位数
#define INT16S signed int          //有符号16位数
#define INT32U unsigned long       //无符号32位数
#define INT32S signed long         //有符号32位数
#define FP32 float                 //单精度浮点数
#define FP64 double                //双精度浮点数

//************************** 8051中断源定义 ***********************
#define		INT_EX0		0		//外部中断0源,P3.2
#define		INT_EX1		2		//外部中断1源,P3.3
#define		INT_T0		1		//定时器0中断源,Timer0
#define		INT_T1		3		//定时器1中断源,Timer1
#define		INT_SER		4		//串口中断源,P3.0,P3.1
#define   INT_ADC   5   //ADC中断源
//****************************************************************
//***********************定义LED控制与I/O控制端口*******************

sbit PIN_IA        =P3^4;
sbit PIN_IB        =P3^5;
sbit PIN_PWR       =P3^2;
sbit PIN_FPWR      =P3^3;
sbit PIN_RLED      =P1^3;
sbit PIN_GLED      =P1^2;
sbit PIN_BUZ       =P3^7;

//sbit Pin_NC        =P1^4;
//sbit Pin_SP        =P2^5;
sbit Pin_CLOSE     =P2^4;
//**************************定义按键端口***************************
sbit Pin_K1=P2^0;
sbit Pin_K2=P2^1;
sbit Pin_K3=P1^5;
sbit Pin_K4=P1^6;
sbit Pin_K5=P1^7;
sbit Pin_K6=P2^2;
sbit Pin_K7=P2^3;

//***************************************************************

#define BZ_ON         1              //蜂鸣器开关定义
#define BZ_OFF        0
#define POWER_OFF     0              //
#define POWER_ON      1

//***************************************************************
#define LED_FLASH  1
#define LED_NORMAL 0

#define red   0
#define green 1
#define on    1
#define off   0
#define FALSE 0
#define TURE  1
#define key_login 1
#define key_del   2
#define key_clear_all  3

#define land   1 //登陆
#define NoLand 0 //未登陆

sbit Add_Key=P0^7;
sbit Del_Key=P0^6;

//sbit PIN_LED0=P1^3;