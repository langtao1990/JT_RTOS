#include "key.h"
#include "led.h"
#include "delay.h"
#include "sys.h"
#include "APPTaskDef.h"
#include "timer.h"


#define TIMER_CNT_NTIMES		(1)	//timer计数到N
#define TIMER_CNT_PERIOD		(72 - 1)  	//72M/72 = 1M

int main(void)
{

	//1. init_hardware
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4	 
	delay_init();	    				//初始化delay
	uart_init(115200);					//初始化uart
	LED_Init();		  					//初始化led
	init_key();							//初始化key
	TIM3_Int_Init(TIMER_CNT_NTIMES,TIMER_CNT_PERIOD);//1Mhz的计数频率
	
	
	//2.初始化thread
	APP_task();
	
	return 0;
}


