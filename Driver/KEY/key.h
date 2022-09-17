#ifndef __KEY_H
#define __KEY_H
#include "sys.h"


#define		 KEY0		PEin(4)//GPIO_ReadInputDataBit(GPIOE, 4)
#define		 KEY1		PEin(3)//GPIO_ReadInputDataBit(GPIOE, 3)
#define		 WK_UP		PAin(0)//GPIO_ReadInputDataBit(GPIOA, 0)		

typedef enum{
	KEY0_PRESS = 1,
	KEY1_PRESS = 2,
	KEY_UP_PRESS = 3,
	KEYYY = 0xFFFFFF,
}KEYPRESS; 


void init_key(void);
u8 KEY_Scan(u8 mode);





#endif
