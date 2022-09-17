#include "DS18B20.h"
#include "timer.h"
#include "util.h"
#define USING_TIM3


//IO方向设置
#define DS18B20_IO_IN()  {GPIOG->CRH&=0XFFFF0FFF;GPIOG->CRH|=8<<12;}
#define DS18B20_IO_OUT() {GPIOG->CRH&=0XFFFF0FFF;GPIOG->CRH|=3<<12;}
////IO操作函数											   
#define	DS18B20_DQ_OUT PGout(11) //数据端口	PG11 
#define	DS18B20_DQ_IN  PGin(11)  //数据端口	PG11 

//复位DS18B20
static void DS18B20_Rst(void)	   
{                 
	DS18B20_IO_OUT(); 	//SET PG11 OUTPUT
    DS18B20_DQ_OUT=0; 	//拉低DQ
#ifdef USING_TIM3
	timer_delay_us(750);    	//拉低750us
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
//等待DS18B20的回应
//返回-1:未检测到DS18B20的存在
//返回0:存在
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
//从DS18B20读取一个位
//返回值：1/0
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
//从DS18B20读取一个字节
//返回值：读到的数据
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
//写一个字节到DS18B20
//dat：要写入的字节
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
//开始温度转换
static void DS18B20_Start(void) 
{   						               
    DS18B20_Rst();	   
	DS18B20_Check();	 
    DS18B20_Write_Byte(0xcc);	// skip rom
    DS18B20_Write_Byte(0x44);	// convert
} 

//初始化DS18B20的IO口 DQ 同时检测DS的存在
//返回1:不存在
//返回0:存在    	 
u8 DS18B20_Init(void)
{
 	GPIO_InitTypeDef  GPIO_InitStructure;
 	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);	 //使能PORTG口时钟 
	
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;				//PORTG.11 推挽输出
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		  
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOG, &GPIO_InitStructure);

 	GPIO_SetBits(GPIOG,GPIO_Pin_11);    //输出1
	
	DS18B20_Rst();

	return DS18B20_Check();
}  
//从ds18b20得到温度值
//精度：0.1C
//返回值：温度值 （-550~1250） 
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
        temp=0;					//温度为负  
    }else temp=1;				//温度为正	  	  
    tem=TH; 					//获得高八位
    tem<<=8;    
    tem+=TL;					//获得底八位
	//去除85度这个异常状态
	if(tem != 0x550){
		valid_temp = tem;
	}	
    valid_temp=(float)valid_temp*0.625;		//转换
	if(temp)return valid_temp; 		//返回温度值
	else return -valid_temp;    
}



 
