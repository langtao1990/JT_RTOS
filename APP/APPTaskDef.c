#include "APPTaskDef.h"
#include "led.h"
#include "delay.h"
#include "key.h"
#include "DS18B20Task.h"
#include "LEDTask.h"

#define 	TASK_BASIC_PRIORITY			5

#define 		START_TASK_PRIO			1
#define 		START_STACK_SIZE		128
TaskHandle_t 	startTask_handler;  //开始任务句柄
static void 	start_task(void *param);


TaskHandle_t 	LEDTask_handler;  //开始任务句柄
TaskHandle_t 	TimerTask_handler;  //开始任务句柄

#define 		TASK_TOTAL_CNT		2

task_def g_task_group[TASK_TOTAL_CNT] = {
		{LED_task, "LED_task", 128, NULL, TASK_BASIC_PRIORITY - 2, &LEDTask_handler},
		{vDs18b20Task, "DS18B20_task", 128, NULL, TASK_BASIC_PRIORITY - 1, &TimerTask_handler}

};

void APP_task(void)
{
	xTaskCreate((TaskFunction_t)start_task,
				(const char *)"start_task",
				(uint16_t)START_STACK_SIZE,
				 NULL,
				(UBaseType_t)START_TASK_PRIO,
				(TaskHandle_t *)&startTask_handler);
	vTaskStartScheduler();          //开启任务调度
}

static void start_task(void *param)
{
	int i = 0;
	taskENTER_CRITICAL();           //进入临界区
	
    //创建USER的任务
	for( i = 0; i < TASK_TOTAL_CNT; i++){
		xTaskCreate((TaskFunction_t )g_task_group[i].func,    	
			(const char*    )g_task_group[i].name,   	
			(uint16_t       )g_task_group[i].stack_size, 
			(void*          )NULL,				
			(UBaseType_t    )g_task_group[i].priority,	
			(TaskHandle_t*  )g_task_group[i].handle);   
	}

    vTaskDelete(startTask_handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}


