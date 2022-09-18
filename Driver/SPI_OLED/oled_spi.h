#ifndef __OLED_SPI_H
#define __OLED_SPI_H

#include "sys.h"



//OLED��gpio���Ŷ���
#define OLED_GPIO		GPIOA

//PSI��gpio���Ŷ���
#define SPI_CLK				GPIO_Pin_5
#define SPI_MOSI_DATA		GPIO_Pin_7

//OLED��gpio���Ŷ���
#define OLED_GPIO_CS	GPIO_Pin_1		//Ƭѡ
#define OLED_GPIO_DC	GPIO_Pin_3		//дָ������ݣ�дָ�0��д���ݣ�1
#define OLED_GPIO_RES	GPIO_Pin_4		//��λ���ţ��ߵ�ƽ��Ч

/*CS pin*/
#define OLED_CS			PAout(1)
/*DC pin*/
#define OLED_DC			PAout(3)
/*Res pin*/
#define OLED_RES		PAout(4)

void spi_master_init(void);
uint8_t spi_master_send_recv_byte(uint8_t spi_byte);
#endif

