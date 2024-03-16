#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "LCD.h"
#include "timer.h"

int main(void)
{
	Timer_Init();
	LCD_Init(WHITE);
	for(int i=0;i<60;i++)
  LCD_DrawcolorPoint(180, i,BLACK);
	while (1)
	{
		
	}
}
