#include "IAP.h"

IAP::IAP(USART &com,Memory &program_memory,Memory &updata_switch)
:com_(com),program_memory_(program_memory),updata_switch_(updata_switch)
{
	packet_sum_=0;
	packet_number_=0;
	state_=0;
	
		for(u8 i = 0;i<255;i++)
		{
			packetIsOk[i] = false;
		}
	
}

//和校验
bool IAP::cheakSum(u8 *data,int lenth)
{
	u8 sum =0 ;
	for(int i=0;i<lenth-1;i++)
	{
		sum+=data[i];
	}
	if(sum == data[lenth-1])
		return true;
	else
		return false;
}

bool IAP::cheakPacketSum()//检查是否接收完所有的包
{
		if(!GetANeedPacket()) 
			return true;
		else
			return false;
}

//逐页擦除
bool IAP::EraseFlash()
{
	state_=2;
	for(int i=0;i<packet_sum_;i++)
		program_memory_.Clear(i);
	return true;
}


//监听信息包
bool IAP::Unpacking_msg()
{
		if(com_.ReceiveBufferSize()>=2) //读取包头
		{
			u8 head[2];
			com_.GetReceivedData(head,2);
			if(head[0] == 0xff && head[1] == 0xAA)
			{
				u8 temp =com_.ReceiveBufferSize();
				while(temp<4)
				{
					//进行超时处理
					temp =com_.ReceiveBufferSize();
				}
					u8 body[4];
					com_.GetReceivedData(body,4);
					if(cheakSum(body,4))
					{
							packet_sum_ = body[1]; //记录总包数
							SendMsgReply(1);
							com_.ClearReceiveBuffer();
							return true;
					}
					else
					{
						SendMsgReply(2);
						com_.ClearReceiveBuffer();
						return false;
					}
			}
			else
			{
				com_.ClearReceiveBuffer();
				return false;
			}
		}
			else
				return false;

}

bool IAP::Unpacking_data()
{
		if(com_.ReceiveBufferSize()>=2) //读取包头
		{
			u8 head[2];
			com_.GetReceivedData(head,2);
			if(head[0] == 0xff && head[1] == 0xBB)
			{
				while(com_.ReceiveBufferSize()<1027); // 1024 + 包型  +包号+校验和
				u8 type;
				com_.GetReceivedData(&type,1);
				if(type ==0x01) //类型为1k
				{
					u8 body[1027];
					body[0] = 0x01;
					com_.GetReceivedData(body+1,1026);
					if(cheakSum(body,1027))
					{
							u8 BagNumber = body[1];
						for(int i=2;i<1026;i++) //将数据转存到缓冲区
									data_[i-2] = body[i]; 
							if(!CheckPcketIsOk(BagNumber)) //检查该包是否已经保存
							{
									SetPacketIsOK(BagNumber);//将该包置为True
//									packet_count_++; //接收包数累加
									packet_number_ = BagNumber;
									state_ =0x04; //修改状态为存入
									SendDataReply(0x01,BagNumber);
									return true;
							}
							else
							{
								//包数错误
								u8 NeedPacket = GetANeedPacket();		
								if(!NeedPacket) 
									SendDataReply(0x02,NeedPacket); //向上位机告诉一个你需要的包
								return false;
							}
					}
					else
					{
						//校验错误
						u8 NeedPacket = GetANeedPacket();		
								if(!NeedPacket) 
										SendDataReply(0x03,NeedPacket);
						return false;
					}					
				}
				else
				{
					return false ;//不是1K类型
				}
			}
			else 
			{
				return false;
			}
		}
			else
				return false;
}

//更新一包程序
bool IAP::UpdataProcedure()
{
	if( (packet_number_-1)<packet_sum_ && packet_number_>0)
	{
		
		u16 data[512];
		for(int i =0;i<512;i++)
		{
			data[i] = (u16)data_[2*i+1]<<8;
			data[i] += (u16)data_[2*i];
		}
		
		program_memory_.Write(packet_number_-1,0,data,512);
		
		return true;
		
		
	}
	else
		return false ;
	
}

//开始新程序
bool IAP::StartNewProcedure()
{
	if(((*(vu32*)FLASH_APP_ADDR)&0x2FFE0000)==0x20000000)	//检查栈顶地址是否合法.
	{ 
		jump_app_=(pointer_start)*(vu32*)(FLASH_APP_ADDR+4);		//用户代码区第二个字为程序开始地址(复位地址)		
		MSR_MSP(*(vu32*)FLASH_APP_ADDR);					//初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)
		jump_app_();											        //跳转到APP.
		return true;
	}
	else
		return false;
}

//这里定义两个字节为标识位，其中等于0x1111表示执行新程序，0xffff表示需要更新程序
bool IAP::SetUpdataYesOrNo(bool yes_no)
{
	u16 yes = 0xffff;
	u16 no = 0x1111;
	
	if(yes_no == true)
	  updata_switch_.Write(0,0,&yes,1);
	else
		updata_switch_.Write(0,0,&no,1);
	
		return true;
}

//需要更新返回true
bool IAP::GetUpdataUesOrNo()
{
	u16 temp_yes_no;
	updata_switch_.Read(0,0,&temp_yes_no,1);
	if(temp_yes_no == 0xffff)
		return true;
	else
		return false;
}

bool IAP::SendMsgReply(u8 State)					 //发送消息包应答
{
		u8 SendData[5]  = {0};
		SendData[0] = 0xBB;
		SendData[1] = 0xaa;
		SendData[2] = State;
		SendData[3] = 0;
		SendData[4] = 0;
		for(u8 i=2;i<4;i++)
		{
			SendData[4]+= SendData[i];
		}
		com_.SendData(SendData,5);
		return true;
			
}
bool IAP::SendDataReply(u8 State,u8 BagNumber)					 //发送数据包应答
{
		u8 SendData[6]  = {0};
		SendData[0] = 0xBB;
		SendData[1] = 0xbb;
		SendData[2] = State;
		SendData[3] = BagNumber;
		SendData[4] = 0;
		for(u8 i=2;i<4;i++)
		{
			SendData[4]+= SendData[i];
		}
		com_.SendData(SendData,5);
		return true;
}


bool IAP::CheckPcketIsOk(u8 packet_umber) //检查该包是否已经被读取
{
	if(packetIsOk[packet_umber-1] == true)
		return true ;
	else
		return false;
}

//得到当前接收到的包数
u8 IAP::GetPacketCount()
{
	 u8 tempSum = 0;
	 for(u8 i =0;i<packet_sum_;i++)
	{
		  if( packetIsOk[i] == true )
				tempSum++;
	}
		return tempSum;
}

// 得到一个需要的包号，优先地位 返回0X00 表示已经完全接收完毕
u8 IAP::GetANeedPacket()
{
	for(u8 i = 0;i<packet_sum_;i++)
		{
			 if(packetIsOk[i] == false)
			 {
					return i+1;			
			 }
		}
		return 0;
}


bool IAP::SetPacketIsOK(u8 packet_number)
{
	if(packet_number > packet_sum_) //不能大于总包大小
		 return false;
	 else
	 {
		 packetIsOk[packet_number-1] = true ; 
		 return true ;
	 }
}

//设置堆栈指针

extern "C"{
	__asm void MSR_MSP(u32 addr) 
	{
			MSR MSP,r0			//set Main Stack value
			BX r14
	}
};



/*
出现问题：新程序没有运行
解决方案：首先检查封包和接收是否存入的仅仅是程序段落
					检查存入数据的大小
					检查最后一位


*/
