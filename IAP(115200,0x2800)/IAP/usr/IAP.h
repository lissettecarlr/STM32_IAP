#ifndef __IAP_H__
#define __IAP_H__

#include "stm32f10x.h"
#include <stdio.h>
#include "Memory.h"
#include "USART.h"
#include "TaskManager.h"


#define FLASH_APP_ADDR	  0x08005000         //该程序分配给15K   1024 *20 =  20480

#define STM32_FLASH_SIZE  128                
#define FLASH_ONEPAGE_SIZE   1024 //字节

typedef  void (*pointer_start)(void);	//用于作为新程序开始地址的指针

extern "C"{
	void MSR_MSP(u32 addr) ;
};

class IAP{
	
	private:
		pointer_start jump_app_;//单纯的指针
	  u16 page_sum_;       //记录总页数
		u8 packet_sum_;    //数据包总数
		u8 packet_number_;   //当前包号
		//u8 packet_count_; //记录当前已经接收的数据包数
		u8 data_[1024];   //程序暂存区域
	
		bool packetIsOk[255]; //用于保存该包是否已经保存
		
	
		USART  &com_;
		Memory &program_memory_;
		Memory &updata_switch_;

	
		bool cheakSum(u8 *data,int lenth);
		bool SendMsgReply(u8 State);					 //发送消息包应答
		bool SendDataReply(u8 State,u8 BagNumber);					 //发送数据包应答

	public:
		u8 state_; //用来标记当前执行状态  0:初始  1:等待接收消息 2:擦除 3:等待接收数据 4:接收并存入 5:更新完毕
	
		IAP(USART &com,Memory &program_memory,Memory &updata_switch);	//构造函数  传入一个串口和一个存储空间
		bool EraseFlash();//将需要保存区域预先擦除
		bool Unpacking_msg();										//监听、拆信息包、暂存
		bool Unpacking_data();									//监听、拆数据包、暂存
		bool UpdataProcedure();  								//将一包数据写入flash
		bool StartNewProcedure();    					 //执行新程序
		bool SetUpdataYesOrNo(bool yes_no);    //设置 是否需要更新程序的标识位
	  bool GetUpdataUesOrNo();               //得到标识位当前状态
	  u8 GetPacketCount();                   //得到当前包数
		bool cheakPacketSum(); 									 //检查是否接收完所有的包
	
		bool CheckPcketIsOk(u8 packet_umber); //检查该包是否已经被读取
		u8 GetANeedPacket();// 得到一个需要的包号，优先地位 返回0X00 表示已经完全接收完毕
		bool SetPacketIsOK(u8 packet_number); //设置该包已经被接收
		
			
};


#endif
