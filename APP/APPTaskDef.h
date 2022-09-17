#ifndef __APP_TASK_DEF_H
#define __APP_TASK_DEF_H
#include "FreeRTOS.h"
#include "task.h"


/*任务相关参数*/
typedef struct task_def_s{
	TaskFunction_t	func;					//任务函数
	char*			name;					//任务名字
	uint16_t 		stack_size;				//堆栈大小
	void *			input_param;			//任务入口参数
	uint8_t 		priority;				//任务优先级
	TaskHandle_t	handle;					//任务句柄						
}task_def;


void APP_task(void);

#endif

