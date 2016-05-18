/*
   该程序被用来测试IAP功能
   复位命令  FF 00 11 22 AA BB CC DD EE FF
	 该程序将保存于 0x8002800 也就是 留了10K给bootleader程序
	 该程序还将 0x0800F000 处的两个字节用于 作为判断是否需要更新程序
*/

#include "USART.h"
#include "TaskManager.h"
#include "stm32f10x.h"


USART com(1,115200,true);

int main(void)
{
	u8 reseat[10];
  u8 reseat_[10] = {0xff,0x00,0x11,0x22,0xaa,0xbb,0xcc,0xdd,0xee,0xff};
	u8 i = 0;
	double show=0;
	SystemInit();
	
	SCB->VTOR = 0x08000000 | 0x2800;  //偏移向量表

	FLASH_Unlock();
	FLASH_ProgramHalfWord(0x0800F000,0x1111); //标示无需更新程序
	FLASH_Lock();
	
 while (1)
 {
	 if(com.ReceiveBufferSize()>9)
	 {
		  com.GetReceivedData(reseat,10);
		  while(*(reseat +i) == reseat_[i] )
			{
				if(i >= 9)
				{
					com<<"reset......\n";
					
					//标示需要更新程序
					FLASH_Unlock();
					FLASH_ClearFlag(FLASH_FLAG_BSY|FLASH_FLAG_EOP|
					FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);
					FLASH_ErasePage(0x0800F000);	
					FLASH_Lock();
					
					*((u32 *)0xE000ED0C) = 0x05fa0004; //复位
				}
				i++;
			}
	 }
	 	if(tskmgr.ClockTool(show,3)) //每三秒执行一次更新
		{
				com<<"This is new procedure....\n";
		}

  }
}

