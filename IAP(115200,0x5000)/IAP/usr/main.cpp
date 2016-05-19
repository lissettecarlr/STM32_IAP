
#include "IAP.h"
#include "USART.h"
#include "TaskManager.h"
#include "Memory.h"


Memory procedure(0x08005000);
Memory updata_switch(0x0800F000);
USART com(1,115200,true);
IAP BootLoader(com,procedure,updata_switch);


int main ()
{
	//如果不需要更新 则直接执行新程序
	if(!BootLoader.GetUpdataUesOrNo())
		BootLoader.StartNewProcedure();
	BootLoader.state_ = 0x01;
	while(1)
	{
		
		if(BootLoader.state_ == 0x01)
		{
			if(BootLoader.Unpacking_msg())//成功获取包的信息
			{
					BootLoader.EraseFlash();
					BootLoader.state_ = 0x03; //将状态改为等待数据接收
			}
		}
		else if(BootLoader.state_ == 0x03)
		{
			if(BootLoader.Unpacking_data())//成功获取包的信息
			{
					BootLoader.state_ = 0x04;
			}
		}
		else if(BootLoader.state_ == 0x04) 		//存入
		{
				BootLoader.UpdataProcedure();//存入一个包到flash
				if(BootLoader.cheakPacketSum())
					BootLoader.state_ = 0x05; 
				else
					BootLoader.state_ = 0x03;//返回继续接收
					
		}
		else if(BootLoader.state_ == 0x05)//接收完毕
		{
				BootLoader.StartNewProcedure();
		}
		else
		{
		}	
		
	}
}


