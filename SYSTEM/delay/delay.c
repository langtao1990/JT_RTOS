#include "delay.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//如果需要使用OS,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "FreeRTOS.h"					//FreeRTOS使用		  
#include "task.h"
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//使用SysTick的普通计数模式对延迟进行管理(支持OS)
//包括delay_us,delay_ms
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2022/7/9
//版本：V1.8
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
//********************************************************************************
//修改说明
//////////////////////////////////////////////////////////////////////////////////  

static u8  fac_us=0;							//us延时倍乘数			   
static u16 fac_ms=0;							//ms延时倍乘数,在ucos下,代表每个节拍的ms数
	
	
extern void xPortSysTickHandler(void);

//systick中断服务函数,使用freertos时用到
void SysTick_Handler(void)
{	
    if(xTaskGetSchedulerState()!=taskSCHEDULER_NOT_STARTED)//系统已经运行
    {
        xPortSysTickHandler();	
    }
}
		   
/*
*func:	delay_init
*brief: systick的初始化，为了兼容FreeRTOS，所以将SYSTICK的时钟频率改为AHB的频率
*para ：#NONE
*rtval: #NONE
*
*/  
void delay_init()
{
	u32 reload;
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);//选择HCLK作为FreeRTOS时钟源

	fac_us = SystemCoreClock/1000000; //的1us的分频计数
	reload = SystemCoreClock/1000000; //RTOS 的1us计数
	reload *= 1000000/configTICK_RATE_HZ;	//根据 configTICK_RATE_HZ 设定溢出
										//时间 reload 为 24 位寄存器,最大值:
										//16777216,在 72M 下,约合 0.233s 左右

	fac_ms = 1000/configTICK_RATE_HZ; //代表 OS 可以延时的最少单位
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk; //开启 SYSTICK 中断
	SysTick->LOAD = reload; 			//加载计数值，每 1/configTICK_RATE_HZ 秒中断

	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk; //开启 SYSTICK
}							    


/*
*func:	delay_us
*brief: systick计数延时
*para ：#nms ：延时n us
*rtval: NONE
*
*/    								   
void delay_us(u32 nus)
{		
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload = SysTick->LOAD;					//LOAD的值	    	 
	ticks=nus*fac_us; 							//需要的节拍数	  		 
	tcnt=0;
	told=SysTick->VAL;        					//刚进入时的计数器值
	while(1)
	{
		tnow = SysTick->VAL;	
		if(tnow != told)
		{	    
			if(tnow < told){
				tcnt += told-tnow;		//这里注意一下SYSTICK是一个递减的计数器就可以了.
			}else {
				tcnt += reload-tnow+told;
			}	    
			told = tnow;
			if(tcnt >= ticks){
				break;				//时间超过/等于要延迟的时间,则退出.
			}
		}  
	}					    
}

/*
*func:	delay_ms
*brief: 延时n ms,会调用RTOS的延时，引起任务调度
*para ：#nms ：延时n ms
*rtval: NONE
*
*/
void delay_ms(u32 nms)
{	
	if(xTaskGetSchedulerState()!=taskSCHEDULER_NOT_STARTED)//系统已经运行
	{		
		if(nms>=fac_ms)						//延时的时间大于OS的最少时间周期 
		{ 
   			vTaskDelay(nms/fac_ms);	 		//FreeRTOS延时
		}
		nms%=fac_ms;						//OS已经无法提供这么小的延时了,采用普通方式延时    
	}
	delay_us((u32)(nms*1000));				//普通方式延时
}

/*
*func:	delay_xms
*brief: 延时n ms,不会引起任务调度，纯软件延时
*para ：#nms ：延时n ms
*rtval: NONE
*
*/
void delay_xms(u32 nms)
{
	u32 i;
	for(i = 0; i < nms; i++){
		delay_us(1000);
	}
} 


