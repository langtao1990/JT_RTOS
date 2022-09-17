#include "delay.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//�����Ҫʹ��OS,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "FreeRTOS.h"					//FreeRTOSʹ��		  
#include "task.h"
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//ʹ��SysTick����ͨ����ģʽ���ӳٽ��й���(֧��OS)
//����delay_us,delay_ms
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2022/7/9
//�汾��V1.8
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//********************************************************************************
//�޸�˵��
//////////////////////////////////////////////////////////////////////////////////  

static u8  fac_us=0;							//us��ʱ������			   
static u16 fac_ms=0;							//ms��ʱ������,��ucos��,����ÿ�����ĵ�ms��
	
	
extern void xPortSysTickHandler(void);

//systick�жϷ�����,ʹ��freertosʱ�õ�
void SysTick_Handler(void)
{	
    if(xTaskGetSchedulerState()!=taskSCHEDULER_NOT_STARTED)//ϵͳ�Ѿ�����
    {
        xPortSysTickHandler();	
    }
}
		   
/*
*func:	delay_init
*brief: systick�ĳ�ʼ����Ϊ�˼���FreeRTOS�����Խ�SYSTICK��ʱ��Ƶ�ʸ�ΪAHB��Ƶ��
*para ��#NONE
*rtval: #NONE
*
*/  
void delay_init()
{
	u32 reload;
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);//ѡ��HCLK��ΪFreeRTOSʱ��Դ

	fac_us = SystemCoreClock/1000000; //��1us�ķ�Ƶ����
	reload = SystemCoreClock/1000000; //RTOS ��1us����
	reload *= 1000000/configTICK_RATE_HZ;	//���� configTICK_RATE_HZ �趨���
										//ʱ�� reload Ϊ 24 λ�Ĵ���,���ֵ:
										//16777216,�� 72M ��,Լ�� 0.233s ����

	fac_ms = 1000/configTICK_RATE_HZ; //���� OS ������ʱ�����ٵ�λ
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk; //���� SYSTICK �ж�
	SysTick->LOAD = reload; 			//���ؼ���ֵ��ÿ 1/configTICK_RATE_HZ ���ж�

	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk; //���� SYSTICK
}							    


/*
*func:	delay_us
*brief: systick������ʱ
*para ��#nms ����ʱn us
*rtval: NONE
*
*/    								   
void delay_us(u32 nus)
{		
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload = SysTick->LOAD;					//LOAD��ֵ	    	 
	ticks=nus*fac_us; 							//��Ҫ�Ľ�����	  		 
	tcnt=0;
	told=SysTick->VAL;        					//�ս���ʱ�ļ�����ֵ
	while(1)
	{
		tnow = SysTick->VAL;	
		if(tnow != told)
		{	    
			if(tnow < told){
				tcnt += told-tnow;		//����ע��һ��SYSTICK��һ���ݼ��ļ������Ϳ�����.
			}else {
				tcnt += reload-tnow+told;
			}	    
			told = tnow;
			if(tcnt >= ticks){
				break;				//ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳�.
			}
		}  
	}					    
}

/*
*func:	delay_ms
*brief: ��ʱn ms,�����RTOS����ʱ�������������
*para ��#nms ����ʱn ms
*rtval: NONE
*
*/
void delay_ms(u32 nms)
{	
	if(xTaskGetSchedulerState()!=taskSCHEDULER_NOT_STARTED)//ϵͳ�Ѿ�����
	{		
		if(nms>=fac_ms)						//��ʱ��ʱ�����OS������ʱ������ 
		{ 
   			vTaskDelay(nms/fac_ms);	 		//FreeRTOS��ʱ
		}
		nms%=fac_ms;						//OS�Ѿ��޷��ṩ��ôС����ʱ��,������ͨ��ʽ��ʱ    
	}
	delay_us((u32)(nms*1000));				//��ͨ��ʽ��ʱ
}

/*
*func:	delay_xms
*brief: ��ʱn ms,��������������ȣ��������ʱ
*para ��#nms ����ʱn ms
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


