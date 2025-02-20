#include <stm32f10x.h>
#include <stdint.h>

void GPIO_Conf(void);
void EXTI0_Conf(void);
void Delay_1Ms(uint32_t count);
void EXTI0_IRQHandler(void);

int main(void)
{
	GPIO_Conf();
	EXTI0_Conf();
	while(1)
	{
//		if((GPIOB->IDR & 0x01) == 0u)
//		{
//			GPIOC->ODR &= ~0x2000;
//		}
//		else
//		{
//			GPIOC->ODR |= 0x2000;
//		}
	}
}

void GPIO_Conf(void)
{
	RCC->APB2ENR |= 0x18; //enable GPIOB and GPIOC
	
	GPIOC->CRH &= 0xFF0FFFFF; //reset GPIOC, pin13
	GPIOB->CRL &= 0xFFFFFFF0; //reset GPIOB, pin0
	
	GPIOC->CRH |= 0x00300000; //GPIOC, output 50MHz, mode push-pull
	GPIOB->CRL |= 0x08; //GPIOB, input, mode pull-up/pull-down
	
	GPIOB->ODR |= 0x01; //GPIOB, pull-up
}

void EXTI0_Conf(void)
{
	//enable gpio
	RCC->APB2ENR |= 0x01;
	AFIO->EXTICR[0] = 0x01;
	//enable interrupt
	EXTI->PR = 0x01; //set 1 to clear, flag
	EXTI->FTSR |= 0x01; //Falling trigger enabled
	EXTI->RTSR = 0x00; //Rising trigger disabled
	EXTI->SWIER |= 0x00; 
	EXTI->EMR = 0x00;
	EXTI->IMR |= 0x01;
	NVIC->ISER[0] = 0x40; //read doc in core-M3
}

void EXTI0_IRQHandler(void)
{
	if((EXTI->PR == 1u) && (EXTI->IMR == 1u)) //kiem tra co yeu cau ngat va cho phep ngat
	{
		EXTI->PR = 0x01; //clear bit flag
		uint8_t u32i = 0;
		for(u32i = 0; u32i < 5; u32i++)
		{
			GPIOC->ODR |= 0x2000;
			Delay_1Ms(500);
			GPIOC->ODR &= ~0x2000;
			Delay_1Ms(500);
		}
	}
   
}

void Delay_1Ms(uint32_t t)
{
	uint32_t count = 0;
	for(count = 0; count < t; count++)
	{
		SysTick->CTRL |= 0x05; //enable and choose system clock (AHB), 72MHz
		SysTick->VAL = 0x00;
		SysTick->LOAD = 72u * 1000u - 1u;
		while(((SysTick->CTRL >> 16) & 1u) == 0u);
	}
}


