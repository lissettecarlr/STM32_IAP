#ifndef __IAP_H__
#define __IAP_H__

#include "stm32f10x.h"
#include <stdio.h>

#define USART_BufferSize  15*1024
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
		uint32_t mStartAddress;//开始储存的地址

		u8 USART_Buffer[USART_BufferSize];
	
		void USART_init(u32 baud);
	
	public:
		IAP(u32 baud,uint32_t startAddress,bool useHalfWord);//构造函数
	
		//串口*******************************************************************

		u16 USART_COUNT;//判断是否完成读取
		u32 USART_Data_Len;//记录数据
		u8 USART_FLAG;	//转换标识
	
	    IAP& operator<<(const char* pStr);//字符串输出 
		void USART_IRQ(void);//串口中断函数
	
		//延时*******************************************************************
		 void delay_ms(u16 nms);
	
		//IAP*******************************************************************
		void write_appbin();
		bool load_app();
	
		//Memory****************************************************************
		///////////////////////
		///读取储存器中特定位置的值
		///@param -relativeAddress 相对于开始地址的地址
		///@param -Data 读出的数据存放的地址
		///@retval -1 : 读取成功 -0：读取失败
		///////////////////////
		bool Read(uint16_t pageNumber, uint16_t* data,u16 length);
		///////////////////////
		///向储存器中特定位置写值
		///@param -pageNumber 相对于开始地址的页地址
		///@param -Data 将要写入的数据
		///@attention 如果构造构造函数的参数useHalfWord为true时，会现将其转换为u16再储存，否则会转换成u32再储存
		///@retval -1 : 写入成功 -0：写入失败
		///////////////////////
		bool Write(uint16_t pageNumber, uint16_t* Data,u16 length);
		
		
		
};

extern IAP BootLoader;

#endif
