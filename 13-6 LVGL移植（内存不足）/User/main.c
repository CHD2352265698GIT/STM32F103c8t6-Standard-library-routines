#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "LCD.h"
#include "timer.h"
#include "lvgl.h"	 
#include "lv_port_disp_template.h"
#include "lv_port_indev_template.h"
int main(void)
{
	Timer_Init();
	lv_init();
	lv_port_disp_init();
	lv_port_indev_init();

	lv_obj_t* switch_obj = lv_switch_create(lv_scr_act());
	lv_obj_set_size(switch_obj,60,20);
	lv_obj_align(switch_obj,LV_ALIGN_CENTER,0,0);
	while (1)
	{
		 Delay_ms(5);
		lv_timer_handler();
	}
}
