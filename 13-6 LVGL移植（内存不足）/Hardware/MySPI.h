#ifndef __MYSPI_H
#define __MYSPI_H
#include "stm32f10x.h"                  // Device header

#define SPI 2
#if SPI==1
#define  SPI_MISO   				GPIO_Pin_6
#define  SPI_MOSI  					GPIO_Pin_7
#define  SPI_SCK  					GPIO_Pin_5
#define  SPI_CS  						GPIO_Pin_4
#define  SPI_CSPORT 					GPIOA
#define  SPI_CSGPIORCCPORT    RCC_APB2Periph_GPIOA
#define  SPI_SPIGPIOPORT      GPIOA
#define  SPI_GPIORCCPORT  		RCC_APB2Periph_GPIOA
#define  SPI_SPIRCC 		   		RCC_APB2Periph_SPI1
#define  SPIX               SPI1
#elif SPI==2
#define  SPI_MISO   				GPIO_Pin_14
#define  SPI_MOSI  					GPIO_Pin_15
#define  SPI_SCK  					GPIO_Pin_13
#define  SPI_CS  						GPIO_Pin_11
#define  SPI_CSPORT 					GPIOB
#define  SPI_CSGPIORCCPORT    RCC_APB2Periph_GPIOB
#define  SPI_SPIGPIOPORT      GPIOB
#define  SPI_SPIGPIORCCPORT  	RCC_APB2Periph_GPIOB
#define  SPI_SPIRCC 		   		RCC_APB1Periph_SPI2
#define  SPIX               SPI2
#endif

#define  SPIWaitTimeOut 0xfffe

void MySPI_Init(void);
void MySPI_Start(void);
void MySPI_Stop(void);
uint8_t MySPI_SwapByte(uint8_t ByteSend);
void MySPI_SendStr(uint8_t* Send,unsigned int data_size);

#endif
