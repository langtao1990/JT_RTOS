#include "key.h"
#include "led.h"
#include "delay.h"
#include "sys.h"
#include "APPTaskDef.h"
#include "timer.h"


#define TIMER_CNT_NTIMES		(1)	//timer������N
#define TIMER_CNT_PERIOD		(72 - 1)  	//72M/72 = 1M

int main(void)
{

	//1. init_hardware
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4	 
	delay_init();	    				//��ʼ��delay
	uart_init(115200);					//��ʼ��uart
	LED_Init();		  					//��ʼ��led
	init_key();							//��ʼ��key
	TIM3_Int_Init(TIMER_CNT_NTIMES,TIMER_CNT_PERIOD);//1Mhz�ļ���Ƶ��
	
	
	//2.��ʼ��thread
	APP_task();
	
	return 0;
}


