#include <stm32f10x.h>

void GPIO_Conf(void);
void Delay_1ms(uint32_t u32Count);
void IWDG_Conf(void);

int main(void)
{
	GPIO_Conf();
	GPIOC->ODR |= 0x2000; //OFF LED
	IWDG_Conf();
	Delay_1ms(450);
	GPIOC->ODR &= ~0x2000; //OPEN LED
	
	while(1)
	{
		IWDG_Conf();
	}
}

void GPIO_Conf(void)
{
	//Set output for PC13
	RCC->APB2ENR |= 0x10u; //Enable Port C
	GPIOC->CRH &= 0xFF0FFFFF; //Reset Pin 13 GPIOC
	GPIOC->CRH |= 0x00300000; //Output, speed 50MHz, Mode push-pull 
}

void Delay_1ms(uint32_t u32Count)
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

void IWDG_Conf(void)
{
	//Access the register, to modify them, first write the code 0x5555 in the IWDG_KR register
	IWDG->KR = 0x5555;
	//Prescaler divider /4, Max timeout RL[11:0]= 0xFFF, time is 409.6
	IWDG->PR = 0x00;
	IWDG->RLR = 0xFFF;
	//reload
	/*Whenever the key value 0xAAAA is written in the IWDG_KR register, the IWDG_RLR value is reloaded in the counter and the watchdog reset is prevented.*/
	IWDG->KR = 0xAAAA;
	//When the independent watchdog is started by writing the value 0xCCCC in the Key register (IWDG_KR)
	//Ngan khong cho tu dong reset lai gia tri 0xFFFF khi gia tri dem ve 0
	IWDG->KR = 0xCCCC;
}
