#include <stm32f10x.h>

void GPIO_Conf(void);
void PWM_Conf_MultiChannel(void);

int main(void)
{
	GPIO_Conf();
	PWM_Conf_MultiChannel();
	while(1)
	{
	
	}
}


void GPIO_Conf(void)
{
	//enable clock GPIOA
	RCC->APB2ENR |= 0x04;
	//reset pin0, 1, 2, 3
	GPIOA->CRL &= 0xFFFF0000;
	//config output, Alternate function output Push-pull, max speed 50 MHz for pin0, 1, 2, 3
	GPIOA->CRL |= 0x0000BBBB;
}

//1s cycle, Freq = 1Hz
void PWM_Conf_MultiChannel(void)
{
	//enable clock TIM2
	RCC->APB1ENR |= 0x01;
	//auto-reload register
	TIM2->ARR = 10000 - 1;
	//prescaler = 7200, clock = 72MHz / 7200 = 10000Hz
	TIM2->PSC = 7200 - 1;
	//PWD mode 1
	TIM2->CCMR1 = 0x6060;
	TIM2->CCMR2 = 0x6060;
	/*PWM mode 1 - In upcounting, channel 1 is active as long as TIMx_CNT<TIMx_CCR1
	else inactive. In downcounting, channel 1 is inactive (OC1REF=‘0) as long as
	TIMx_CNT>TIMx_CCR1 else active (OC1REF=1)*/
	TIM2->CCR1 = (20 * TIM2->ARR) / 100;
	TIM2->CCR2 = (30 * TIM2->ARR) / 100;
	TIM2->CCR3 = (40 * TIM2->ARR) / 100;
	TIM2->CCR4 = (50 * TIM2->ARR) / 100;
	//TIMx capture/compare enable register(select active high)
	/*CC1 channel configured as output:
	0: OC1 active high.
	1: OC1 active low*/
	TIM2->CCER = 0x1111;
	//Counter enabled
	TIM2->CR1 = 0x01;
	//Re-initialize the counter and generates an update of the registers
	TIM2->EGR = 0x01;
	//set UG bit
}

