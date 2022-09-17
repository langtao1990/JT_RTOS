#ifndef __APP_TASK_DEF_H
#define __APP_TASK_DEF_H
#include "FreeRTOS.h"
#include "task.h"


/*������ز���*/
typedef struct task_def_s{
	TaskFunction_t	func;					//������
	char*			name;					//��������
	uint16_t 		stack_size;				//��ջ��С
	void *			input_param;			//������ڲ���
	uint8_t 		priority;				//�������ȼ�
	TaskHandle_t	handle;					//������						
}task_def;


void APP_task(void);

#endif

