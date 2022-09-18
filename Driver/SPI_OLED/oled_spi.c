#include "oled_spi.h"
#include "sys.h"
#include "util.h"

/********************************************************************
*	@func: spi_gpio_init
*	@brief: OLED的SPI接口的初始化,其中只用到：CLK，MOSI，没用到MISO
*	@param: NONE
*	@rtvl:  NONE
*	@date： 20220918
************************************************************************/
static void spi_gpio_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA, ENABLE );//PORTA时钟使能 
 
	GPIO_InitStructure.GPIO_Pin = SPI_CLK | SPI_MOSI_DATA;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //SPI的CLK和MOSI脚输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(OLED_GPIO, &GPIO_InitStructure);//初始化OLED_GPIO

 	GPIO_SetBits(OLED_GPIO, SPI_CLK | SPI_MOSI_DATA);  //默认上拉
	
}

/********************************************************************
*	@func: spi_gpio_init
*	@brief: OLED的GPIO接口的初始化
*	@param: NONE
*	@rtvl:  NONE
*	@date： 20220918
************************************************************************/
static void oled_gpio_init()
{
	 GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA, ENABLE );//PORTA时钟使能 
 
	GPIO_InitStructure.GPIO_Pin = OLED_GPIO_CS | OLED_GPIO_DC | OLED_GPIO_RES;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //复用推挽输出 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(OLED_GPIO, &GPIO_InitStructure);//初始化OLED_GPIO

 	GPIO_SetBits(OLED_GPIO,OLED_GPIO_CS | OLED_GPIO_DC | OLED_GPIO_RES);  //默认上拉
}

/********************************************************************
*	@func: spi_master_send_recv_byte
*	@brief: 发送并接受一个字节
*	@param: uint8_t spi_byte：	待发送的字节
*	@rtvl:  uint8_t byte		返回的字节
*	@date： 20220918
************************************************************************/
uint8_t spi_master_send_recv_byte(uint8_t spi_byte)
{		
	uint8_t time = 0;
	
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET){ //检查指定的SPI标志位设置与否:发送缓存空标志位
		time++;
		if(time > 200){
			return 0;
		}
	}			  
	SPI_I2S_SendData(SPI1, spi_byte); //通过外设SPIx发送一个数据
	time = 0;
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET){//检查指定的SPI标志位设置与否:接受缓存非空标志位
		time++;
		if(time>200){
			return 0;
		}
	}	  						    
		return SPI_I2S_ReceiveData(SPI1); //返回通过SPIx最近接收的数据	
}

/********************************************************************
*	@func: spi_master_init
*	@brief: OLED的SPI接口的初始化
*	@param: NONE
*	@rtvl:  NONE
*	@date： 20220918
************************************************************************/
void spi_master_init(void)
{
	SPI_InitTypeDef  spi_config_init;
	//spi flash cs 初始化
	spi_gpio_init();	
	//spi gpio 初始化
	oled_gpio_init();	

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);	//SPI1时钟使能

	spi_config_init.SPI_Direction 			= SPI_Direction_2Lines_FullDuplex;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
	spi_config_init.SPI_Mode 				= SPI_Mode_Master;		//设置SPI工作模式:设置为主SPI
	spi_config_init.SPI_DataSize 			= SPI_DataSize_8b;		//设置SPI的数据大小:SPI发送接收8位帧结构
	spi_config_init.SPI_CPOL 				= SPI_CPOL_Low;		//选择了串行时钟的稳态:空闲时钟低
	spi_config_init.SPI_CPHA 				= SPI_CPHA_1Edge;	//数据捕获(采样)于第1个时钟沿
	spi_config_init.SPI_NSS					= SPI_NSS_Soft;//SPI_NSS_Soft;		//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	spi_config_init.SPI_BaudRatePrescaler 	= SPI_BaudRatePrescaler_256;		//定义波特率预分频的值:波特率预分频值为256
	spi_config_init.SPI_FirstBit 			= SPI_FirstBit_MSB;	//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	spi_config_init.SPI_CRCPolynomial 		= 7;	//CRC值计算的多项式
	
	SPI_Init(SPI1, &spi_config_init);  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
	SPI_Cmd(SPI1, ENABLE); //使能SPI外设
} 

