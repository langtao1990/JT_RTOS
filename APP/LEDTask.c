#include "LEDTask.h"
#include "LED.h"
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"


/*
	LED task
*/

void LED_task(void *param)
{
	for(;;){
		LED0 = !LED0;
		vTaskDelay(100);      //��ʱ10ms��Ҳ����10��ʱ�ӽ���
	}

}

