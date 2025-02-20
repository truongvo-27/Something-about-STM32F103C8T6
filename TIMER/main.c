#include <stm32f10x.h>
#include <stdint.h>

void GPIO_Conf(void);
void Delay_1000ms(uint32_t u32time);
void TIM2_Conf(void);

int main(void)
{
	GPIO_Conf();
	TIM2_Conf();
	while(1)
	{
		GPIOC->ODR |= 0x2000;
		Delay_1000ms(1);
		GPIOC->ODR &= ~0x2000;
		Delay_1000ms(1);
	}
}

void GPIO_Conf(void)
{
	//Set output for PC13
	RCC->APB2ENR |= 0x10u; //Enable Port C
	GPIOC->CRH &= 0xFF0FFFFF; //Reset Pin 13 GPIOC
	GPIOC->CRH |= 0x00300000; //Output, speed 50MHz, Mode push-pull 
}

void TIM2_Conf(void)
{
	//enable clock TIM2
	RCC->APB1ENR |= 0x01;
	//set clock 10000Hz, prescaler 72MHz / 7200 = 10000Hz
	TIM2->PSC = 7200 -1;
	//auto-reload register
	TIM2->ARR = 0xFFFF;
	//enable counter, with count-up mode
	TIM2->CR1 |= 0x01;
	//Re-initialize the counter and generates an update of the registers
	TIM2->EGR |= 0x01;
	//phai co bit UG, neu khong cac thanh ghi ARR, PSC, CCRx se khong duoc cap nhat dia chi
}

void Delay_1000ms(uint32_t u32time)
{
	uint32_t u32i = 0;
	for(u32i = 0; u32i < u32time; u32i++)
	{
		TIM2->CNT = 0u;
		while(TIM2->CNT < 10000);
	}
}

