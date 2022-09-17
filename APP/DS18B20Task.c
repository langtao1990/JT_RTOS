#include "DS18B20Task.h"
#include "DS18B20.h"
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"

void vDs18b20Task(void * param)
{
	short cur_temper = 0;
	DS18B20_Init();
	while(1){
		taskENTER_CRITICAL();
		cur_temper = DS18B20_Get_Temp();
		taskEXIT_CRITICAL();
		printf("current temper is %f℃\r\n", (float)cur_temper/10.0f);
		vTaskDelay(100);      //延时10ms，也就是10个时钟节拍
	}
}

