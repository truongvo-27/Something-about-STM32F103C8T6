#include "stm32f10x.h"
#include <stdint.h>

void GPIO_Output_Conf(void);
void Delay_Ms(uint32_t u32Count);

int main(void)
{
	GPIO_Output_Conf();
	while(1)
	{
		GPIOC->ODR |= 0x2000u;
		Delay_Ms((uint32_t)1000);
		GPIOC->ODR &= ~0x2000u;
		Delay_Ms((uint32_t)1000);
	}
}

void GPIO_Output_Conf(void)
{
	//Set output for PC13
	RCC->APB2ENR |= 0x10u; //Enable Port C
	GPIOC->CRH &= 0xFF0FFFFF; //Reset Pin 13 GPIOC
	GPIOC->CRH |= 0x00300000; //Output, speed 50MHz, Mode push-pull 
}

void Delay_Ms(uint32_t u32Count)
{
	uint32_t u32i = 0;
	for(u32i = 0; u32i < u32Count; u32i++)
	{
		SysTick->LOAD = 72u * 1000u - 1u;
		//enable counter and select AHB 72MHz
		SysTick->CTRL |= 5u;
		SysTick->VAL = 0u;
		while(!(SysTick->CTRL & 0x10000));
	}
}


