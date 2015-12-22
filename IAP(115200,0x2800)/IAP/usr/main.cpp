#include "IAP.h"

#define UPDATA_FLAG_ADDR 0x0800F000

u8 SWITCH;

void start_procedure(void);//执行新程序
void receive_storage_Procedure(void); //接收程序
void EraseFlash(u32 addr,u8 pagenumber); //由于在该函数中读取了串口发来的页数，改还是第二个参数无意义
void judge_updata();  //判断是否更新

int main ()
{
	 judge_updata();
	
	//用于调试 start*********************************************
	 if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8)==0)//将PA8接地将直接运行APP程序
	 {
		SWITCH=4;
	 }
	//end******************************************************** 
	 
while(1)
	{
		switch (SWITCH)
		{
			case 1:{
				 EraseFlash(FLASH_APP_ADDR,2);//擦除
			}break;
		
			case 2:{
					receive_storage_Procedure();//下载				
			}break;
			case 3:{
				start_procedure();//执行
			}break;	
			case 4:
				BootLoader.load_app();//直接跳转到APP程序
			break;
		
		}
	}
}


void EraseFlash(u32 addr,u8 pagenumber)
{
    BootLoader.delay_ms(100);
   if(BootLoader.USART_Buffer[0]==0xff&&BootLoader.USART_Buffer[2]==0xaa)
   {
		FLASH_Unlock();
		FLASH_ClearFlag(FLASH_FLAG_BSY|FLASH_FLAG_EOP|
		FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);
		for(u8 i=0;i<BootLoader.USART_Buffer[1];i++)
			FLASH_ErasePage(addr+i*1024);
	   BootLoader.USART_Data_Len=0;
	   SWITCH=2;
	   BootLoader.state=1;
	   BootLoader<<"please choose file.....\n"; 
   }
   BootLoader.USART_Data_Len=0;
}


void receive_storage_Procedure(void)
{
	if(BootLoader.USART_FLAG==1){//正在传输文件
		if(BootLoader.USART_COUNT>1000000){//传输完成
			BootLoader.USART_Data_Len=0;
			FLASH_Lock();
			BootLoader<<"Data is written to complete...\r\n";
			SWITCH=3;	
		}	
		else BootLoader.USART_COUNT++;
	}
}

void start_procedure(void)
{
	BootLoader.delay_ms(500);
	if(((*(u32*)(FLASH_APP_ADDR+4))&0xFF000000)==0x08000000)//判断是否为0X20XXXXXX.
	{	 
		BootLoader<<"loading APP....\r\n";
		BootLoader.load_app();//跳转到APP程序
	}
	else 
	{
		SWITCH=1;
	}		
}

void judge_updata()
{
	uint16_t flag;
	flag=(*(__IO uint16_t*)(UPDATA_FLAG_ADDR)); //读出标识位，该标识为由APP程序修改
	if(flag==0xffff)
	{
		SWITCH=1;
		BootLoader<<"\n Send FF page AA,choose page number";
	}
	else
		SWITCH=4; //直接执行APP程序
}
