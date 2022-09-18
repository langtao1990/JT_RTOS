#include "oled_spi.h"
#include "sys.h"
#include "util.h"

/********************************************************************
*	@func: spi_gpio_init
*	@brief: OLED��SPI�ӿڵĳ�ʼ��,����ֻ�õ���CLK��MOSI��û�õ�MISO
*	@param: NONE
*	@rtvl:  NONE
*	@date�� 20220918
************************************************************************/
static void spi_gpio_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA, ENABLE );//PORTAʱ��ʹ�� 
 
	GPIO_InitStructure.GPIO_Pin = SPI_CLK | SPI_MOSI_DATA;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //SPI��CLK��MOSI�����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(OLED_GPIO, &GPIO_InitStructure);//��ʼ��OLED_GPIO

 	GPIO_SetBits(OLED_GPIO, SPI_CLK | SPI_MOSI_DATA);  //Ĭ������
	
}

/********************************************************************
*	@func: spi_gpio_init
*	@brief: OLED��GPIO�ӿڵĳ�ʼ��
*	@param: NONE
*	@rtvl:  NONE
*	@date�� 20220918
************************************************************************/
static void oled_gpio_init()
{
	 GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA, ENABLE );//PORTAʱ��ʹ�� 
 
	GPIO_InitStructure.GPIO_Pin = OLED_GPIO_CS | OLED_GPIO_DC | OLED_GPIO_RES;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //����������� 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(OLED_GPIO, &GPIO_InitStructure);//��ʼ��OLED_GPIO

 	GPIO_SetBits(OLED_GPIO,OLED_GPIO_CS | OLED_GPIO_DC | OLED_GPIO_RES);  //Ĭ������
}

/********************************************************************
*	@func: spi_master_send_recv_byte
*	@brief: ���Ͳ�����һ���ֽ�
*	@param: uint8_t spi_byte��	�����͵��ֽ�
*	@rtvl:  uint8_t byte		���ص��ֽ�
*	@date�� 20220918
************************************************************************/
uint8_t spi_master_send_recv_byte(uint8_t spi_byte)
{		
	uint8_t time = 0;
	
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET){ //���ָ����SPI��־λ�������:���ͻ���ձ�־λ
		time++;
		if(time > 200){
			return 0;
		}
	}			  
	SPI_I2S_SendData(SPI1, spi_byte); //ͨ������SPIx����һ������
	time = 0;
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET){//���ָ����SPI��־λ�������:���ܻ���ǿձ�־λ
		time++;
		if(time>200){
			return 0;
		}
	}	  						    
		return SPI_I2S_ReceiveData(SPI1); //����ͨ��SPIx������յ�����	
}

/********************************************************************
*	@func: spi_master_init
*	@brief: OLED��SPI�ӿڵĳ�ʼ��
*	@param: NONE
*	@rtvl:  NONE
*	@date�� 20220918
************************************************************************/
void spi_master_init(void)
{
	SPI_InitTypeDef  spi_config_init;
	//spi flash cs ��ʼ��
	spi_gpio_init();	
	//spi gpio ��ʼ��
	oled_gpio_init();	

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);	//SPI1ʱ��ʹ��

	spi_config_init.SPI_Direction 			= SPI_Direction_2Lines_FullDuplex;  //����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
	spi_config_init.SPI_Mode 				= SPI_Mode_Master;		//����SPI����ģʽ:����Ϊ��SPI
	spi_config_init.SPI_DataSize 			= SPI_DataSize_8b;		//����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
	spi_config_init.SPI_CPOL 				= SPI_CPOL_Low;		//ѡ���˴���ʱ�ӵ���̬:����ʱ�ӵ�
	spi_config_init.SPI_CPHA 				= SPI_CPHA_1Edge;	//���ݲ���(����)�ڵ�1��ʱ����
	spi_config_init.SPI_NSS					= SPI_NSS_Soft;//SPI_NSS_Soft;		//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
	spi_config_init.SPI_BaudRatePrescaler 	= SPI_BaudRatePrescaler_256;		//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
	spi_config_init.SPI_FirstBit 			= SPI_FirstBit_MSB;	//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
	spi_config_init.SPI_CRCPolynomial 		= 7;	//CRCֵ����Ķ���ʽ
	
	SPI_Init(SPI1, &spi_config_init);  //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���
	SPI_Cmd(SPI1, ENABLE); //ʹ��SPI����
} 

