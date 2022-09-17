#include "DS18B20.h"
#include "timer.h"
#include "util.h"
#define USING_TIM3


//IO��������
#define DS18B20_IO_IN()  {GPIOG->CRH&=0XFFFF0FFF;GPIOG->CRH|=8<<12;}
#define DS18B20_IO_OUT() {GPIOG->CRH&=0XFFFF0FFF;GPIOG->CRH|=3<<12;}
////IO��������											   
#define	DS18B20_DQ_OUT PGout(11) //���ݶ˿�	PG11 
#define	DS18B20_DQ_IN  PGin(11)  //���ݶ˿�	PG11 

//��λDS18B20
static void DS18B20_Rst(void)	   
{                 
	DS18B20_IO_OUT(); 	//SET PG11 OUTPUT
    DS18B20_DQ_OUT=0; 	//����DQ
#ifdef USING_TIM3
	timer_delay_us(750);    	//����750us
#else 
	delay_us(750);
#endif

    DS18B20_DQ_OUT=1; 	//DQ=1 
	
#ifdef USING_TIM3
	timer_delay_us(15);    	
#else 
	delay_us(15);			//15US
#endif 
}
//�ȴ�DS18B20�Ļ�Ӧ
//����-1:δ��⵽DS18B20�Ĵ���
//����0:����
static int DS18B20_Check(void) 	   
{   
	u8 retry=0;
	DS18B20_IO_IN();	//SET PG11 INPUT	 
    while (DS18B20_DQ_IN&&retry<200)
	{
		retry++;
#ifdef USING_TIM3
	timer_delay_us(1);    	
#else 
	delay_us(1);			//1US
#endif  	
	};	 
	if(retry>=200)return PROCESS_FAILURE;
	else retry=0;
    while (!DS18B20_DQ_IN&&retry<240)
	{
		retry++;
#ifdef USING_TIM3
	timer_delay_us(1);    	
#else 
	delay_us(1);			//1US
#endif  
	};
	if(retry>=240)return PROCESS_FAILURE;	    
	return PROCESS_SUCCEED;
}
//��DS18B20��ȡһ��λ
//����ֵ��1/0
static u8 DS18B20_Read_Bit(void) 	 
{
    u8 data;
	DS18B20_IO_OUT();	//SET PG11 OUTPUT
    DS18B20_DQ_OUT=0; 
#ifdef USING_TIM3
	timer_delay_us(2);    	
#else 
	delay_us(2);			//2US
#endif  
    DS18B20_DQ_OUT=1; 
	DS18B20_IO_IN();	//SET PG11 INPUT
#ifdef USING_TIM3
	timer_delay_us(12);    	
#else 
	delay_us(12);			//12US
#endif  
	if(DS18B20_DQ_IN)data=1;
    else data=0;	 
#ifdef USING_TIM3
	timer_delay_us(50);    	
#else 
	delay_us(50);			//50US
#endif       
    return data;
}
//��DS18B20��ȡһ���ֽ�
//����ֵ������������
static u8 DS18B20_Read_Byte(void)     
{        
    u8 i,j,dat;
    dat=0;
	for (i=1;i<=8;i++) 
	{
        j=DS18B20_Read_Bit();
        dat=(j<<7)|(dat>>1);
    }						    
    return dat;
}
//дһ���ֽڵ�DS18B20
//dat��Ҫд����ֽ�
static void DS18B20_Write_Byte(u8 dat)     
 {             
    u8 j;
    u8 testb;
	DS18B20_IO_OUT();	//SET PG11 OUTPUT;
    for (j=1;j<=8;j++) 
	{
        testb=dat&0x01;
        dat=dat>>1;
        if (testb) 
        {
            DS18B20_DQ_OUT=0;	// Write 1
#ifdef USING_TIM3
	timer_delay_us(2);    	
#else 
	delay_us(2);			//1US
#endif                           
            DS18B20_DQ_OUT=1;
#ifdef USING_TIM3
	timer_delay_us(60);    	
#else 
	delay_us(60);			//1US
#endif           
        }
        else 
        {
            DS18B20_DQ_OUT=0;	// Write 0
#ifdef USING_TIM3
	timer_delay_us(60);    	
#else 
	delay_us(60);			//1US
#endif            
            DS18B20_DQ_OUT=1;
#ifdef USING_TIM3
	timer_delay_us(2);    	
#else 
	delay_us(2);			//1US
#endif                         
        }
    }
}
//��ʼ�¶�ת��
static void DS18B20_Start(void) 
{   						               
    DS18B20_Rst();	   
	DS18B20_Check();	 
    DS18B20_Write_Byte(0xcc);	// skip rom
    DS18B20_Write_Byte(0x44);	// convert
} 

//��ʼ��DS18B20��IO�� DQ ͬʱ���DS�Ĵ���
//����1:������
//����0:����    	 
u8 DS18B20_Init(void)
{
 	GPIO_InitTypeDef  GPIO_InitStructure;
 	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);	 //ʹ��PORTG��ʱ�� 
	
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;				//PORTG.11 �������
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		  
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOG, &GPIO_InitStructure);

 	GPIO_SetBits(GPIOG,GPIO_Pin_11);    //���1
	
	DS18B20_Rst();

	return DS18B20_Check();
}  
//��ds18b20�õ��¶�ֵ
//���ȣ�0.1C
//����ֵ���¶�ֵ ��-550~1250�� 
short DS18B20_Get_Temp(void)
{
	static short valid_temp = 0;
    u8 temp;
    u8 TL,TH;
	short tem;
    DS18B20_Start ();  			// ds1820 start convert
    DS18B20_Rst();

    DS18B20_Check();
	
    DS18B20_Write_Byte(0xcc);	// skip rom
    DS18B20_Write_Byte(0xbe);	// convert	    
    TL=DS18B20_Read_Byte(); 	// LSB   
    TH=DS18B20_Read_Byte(); 	// MSB  
	    	  
    if(TH>7)
    {
        TH=~TH;
        TL=~TL; 
        temp=0;					//�¶�Ϊ��  
    }else temp=1;				//�¶�Ϊ��	  	  
    tem=TH; 					//��ø߰�λ
    tem<<=8;    
    tem+=TL;					//��õװ�λ
	//ȥ��85������쳣״̬
	if(tem != 0x550){
		valid_temp = tem;
	}	
    valid_temp=(float)valid_temp*0.625;		//ת��
	if(temp)return valid_temp; 		//�����¶�ֵ
	else return -valid_temp;    
}



 
