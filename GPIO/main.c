#include "stm32f10x.h"
#include <stdint.h>

void GPIO_Output_Conf(void);
void GPIO_Input_Conf(void);

int main(void)
{
	GPIO_Input_Conf();
	GPIO_Output_Conf();
	while(1)
	{
		if((GPIOB->IDR & 0x1000u) == 0u)
		{
			GPIOC->ODR |= 0x2000u;
		}
		if((GPIOB->IDR & 0x1000u) != 0u)
		{
			GPIOC->ODR &= ~0x2000u;
		}
	}
}

void GPIO_Output_Conf(void)
{
	//Set output for PC13
	RCC->APB2ENR |= 0x10u; //Enable Port C
	GPIOC->CRH &= 0xFF0FFFFF; //Reset Pin 13 GPIOC
	GPIOC->CRH |= 0x00300000; //Output, speed 50MHz, Mode push-pull 
}

void GPIO_Input_Conf(void)
{
	//Set input for PB12
	RCC->APB2ENR |= 0x08u;
	GPIOB->CRH &= 0xFFF0FFFF; //Reset Pin 12 GPIOB
	GPIOB->CRH |= 0x00080000; //Input, mode pull-up / pull-down
	GPIOB->ODR |= 0x1000u; //pull-up for button default
}


