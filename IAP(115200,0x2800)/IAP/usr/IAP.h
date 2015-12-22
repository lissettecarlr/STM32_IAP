#ifndef __IAP_H__
#define __IAP_H__

#include "stm32f10x.h"
#include <stdio.h>

#define USART_BufferSize  1024
#define FLASH_APP_ADDR	  0x08002800         //该程序分配给10K
#define FLASH_APP_BASE	  0x08000000   

#define STM32_FLASH_SIZE  128                
#define STM32_FLASH_BASE  0x08000000
#define STM_SECTOR_SIZE   1024 //字节

typedef  void (*iapfun)(void);	

extern "C"{
	void USART1_IRQHandler(void);
	void MSR_MSP(u32 addr) ;
};

class IAP{


	private:
		iapfun jump2app;//单纯的指针
		bool mUseHalfWord;//储存字长  16/32 默认16位
		void USART_init(u32 baud);
	
	public:
		u16 FlashPages;
		u16 USART_Buffer[512];
		u8 state;
	
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
	
		//Memory****************************************************************
		///////////////////////
		///读取储存器中特定位置的值
		///@param -relativeAddress 相对于开始地址的地址
		///@param -Data 读出的数据存放的地址
		///@retval -1 : 读取成功 -0：读取失败
		///////////////////////
		bool Read(uint16_t pageNumber, uint16_t* data,u16 length);
	
			
};

extern IAP BootLoader;

#endif
