#include "stm32f10x.h"                  // Device header
#include "myspi.h"                  // Device header

void MySPI_W_SS(uint8_t BitValue)
{
	GPIO_WriteBit(SPI_CSPORT, SPI_CS, (BitAction)BitValue);
}

void MySPI_Init(void)
{
	RCC_APB2PeriphClockCmd(SPI_CSGPIORCCPORT, ENABLE);
	RCC_APB2PeriphClockCmd(SPI_SPIGPIORCCPORT, ENABLE);
	RCC_APB2PeriphClockCmd(SPI_SPIRCC, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = SPI_CS;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI_CSPORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = SPI_SCK | SPI_MOSI;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI_SPIGPIOPORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = SPI_MISO;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI_SPIGPIOPORT, &GPIO_InitStructure);
	
	SPI_InitTypeDef SPI_InitStructure;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;   //主机模式
	SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;   //使用两条线进行全双工通信
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;    //字节大小
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;  //高位先行 
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;      //传输速率 72Mhz/4
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;      //时钟空闲状态
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;    //上升沿信号采样
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;       //时钟信号的第一个边缘
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPIX, &SPI_InitStructure);
	
	SPI_Cmd(SPIX, ENABLE);
	
	MySPI_W_SS(1);
}

void MySPI_Start(void)
{
	MySPI_W_SS(0);
}

void MySPI_Stop(void)
{
	MySPI_W_SS(1);
}

uint8_t MySPI_SwapByte(uint8_t ByteSend)
{
	int WaitTimeout=0; 

	while (SPI_I2S_GetFlagStatus(SPIX, SPI_I2S_FLAG_TXE) != SET)
	{
		if(WaitTimeout++  ==	SPIWaitTimeOut)
		{
			WaitTimeout=0;
			break;
		}
	}	
	SPI_I2S_SendData(SPIX, ByteSend);
	
	while (SPI_I2S_GetFlagStatus(SPIX, SPI_I2S_FLAG_RXNE) != SET)
	{
		if(WaitTimeout++  ==	SPIWaitTimeOut)
		{
			WaitTimeout=0;
			break;
		}
	}	
	return SPI_I2S_ReceiveData(SPIX);
}

void MySPI_SendStr(uint8_t* Send,unsigned int data_size)
{
	for(unsigned int i=0;i<data_size;i++)
	MySPI_SwapByte(Send[i]);
}

