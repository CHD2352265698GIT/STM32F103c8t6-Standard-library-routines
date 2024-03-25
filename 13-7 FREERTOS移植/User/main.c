#include "stm32f10x.h"                  // Device header
#include "Freertos.h"
#include "task.h"
#include "Delay.h"
#include "LCD.h"
#include "timer.h"
#include "led.h"
#include "serial.h"

TaskHandle_t myTaskHandle1;
TaskHandle_t myTaskHandle2;
TaskHandle_t myTaskHandle3;
void Fun1(void *avrg)
{
	while(1)
	{
		LED1_ON();
		vTaskDelay(100);
		LED1_OFF();
		vTaskDelay(100);
	}
}
void Fun2(void *avrg)
{
	while(1)
	{
		taskENTER_CRITICAL();
		Serial_SendString("hello world\n");
		taskEXIT_CRITICAL();
		
		vTaskDelay(2000);
	}
}
void Fun3(void *avrg)
{
	while(1)
	{
		static char i=0;
		taskENTER_CRITICAL();
		Serial_SendString("test1\n");
		taskEXIT_CRITICAL();
		if(i++==10)
		{
		taskENTER_CRITICAL();
			Serial_SendString("删除任务1\n");
		taskEXIT_CRITICAL();

		vTaskDelete(myTaskHandle1); 
			
		taskENTER_CRITICAL();
		Serial_SendString("删除任务3\n");
		taskEXIT_CRITICAL();
			
		vTaskDelete(NULL); 
		}
		if(i==12)i=11;
		vTaskDelay(500);
	}
}

int main(void)
{
	Timer_Init();
	LED_Init();
	Serial_Init();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	
	xTaskCreate(Fun1,"Fun1",1,NULL,1,&myTaskHandle1);
	xTaskCreate(Fun2,"Fun2",128,NULL,1,&myTaskHandle2);
	xTaskCreate(Fun3,"Fun3",512,NULL,1,&myTaskHandle3);

	vTaskStartScheduler();
}
