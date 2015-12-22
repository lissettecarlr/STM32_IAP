#include "IAP.h"

u8 SWITCH=1;

void start_procedure(void);
void receive_storage_Procedure(void);

int main ()
{
//	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8)==1)
//		SWITCH=3;
	
	SWITCH=1;
	
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_BSY|FLASH_FLAG_EOP|
	FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);
	FLASH_ErasePage(FLASH_APP_ADDR);
	FLASH_ErasePage(FLASH_APP_ADDR+1024);
	//FLASH_Lock();	
	
	
while(1)
	{
	
		switch (SWITCH)
		{
			case 1:{
				 BootLoader<<"please choose file.....\n";
				 SWITCH=2;
			}break;
			
			case 2:{
					receive_storage_Procedure();				
			}break;
			
			case 3:{
				start_procedure();
				
			}break;
			
			case 4:
				BootLoader.load_app();//跳转到APP程序
			break;
		
		}
	}
}

void receive_storage_Procedure(void)
{
	if(BootLoader.USART_FLAG==1){//正在传输文件
		if(BootLoader.USART_COUNT>1000000){//传输完成
			BootLoader.USART_Data_Len=0;
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


//int main()
//{
//	u16 temp[5]={0};
//	BootLoader.Read(0,temp,5);
//	
//	u8 temp2[1024]={1,2,3,4,5,6};
//	BootLoader.Write(0,temp2,1024);

//	u16 temp3[5]={0};
//	BootLoader.Read(0,temp3,5);

//	while(1)
//	{
//		
//		
//	}
//	
//}


