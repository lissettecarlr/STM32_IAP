#ifndef __IAP_H__
#define __IAP_H__

#include "stm32f10x.h"
#include <stdio.h>

#define USART_BufferSize  1024
#define FLASH_APP_ADDR	  0x08002800         //该程序分配给10K 

#define STM32_FLASH_SIZE  128                
#define FLASH_ONEPAGE_SIZE   1024 //字节

typedef  void (*iapfun)(void);	

extern "C"{
	void USART1_IRQHandler(void);
	void MSR_MSP(u32 addr) ;
};

class IAP{


	private:
		iapfun jump2app;//单纯的指针
		void USART_init(u32 baud);//串口初始化，传入波特率，改波特率也就是之后下载程序的波特率
	
	public:
		u16 FlashPages;
		u16 USART_Buffer[FLASH_ONEPAGE_SIZE/2];
		u8 state;//0表示等待页码状态 1表示等待下载状态
	
		IAP(u32 baud,bool useHalfWord);//构造函数
	
		//串口*******************************************************************

		u32 USART_COUNT;//判断是否完成读取
		u32 USART_Data_Len;//记录数据
		u8 USART_FLAG;	//转换标识
	
	  IAP& operator<<(const char* pStr);//字符串输出 
		void USART_IRQ(void);//串口中断函数
	
		//延时*******************************************************************
		 void delay_ms(u16 nms);
	
		//IAP*******************************************************************
		bool load_app();			
};

extern IAP BootLoader;

#endif
