#include "stm32f10x.h"
#include <stdio.h>

void RTC_Config(void);
void RTC_1sCount(void);
void GPIO_Output_Configure(void);

static uint8_t Hours = 0u;
static uint8_t Minutes = 0u;
static uint8_t Seconds = 0u;
static uint8_t Week = 0u;

int main(void)
{
	RTC_Config();
	GPIO_Output_Configure();
	//Default set PC13 on

	while(1)
	{
			if (Seconds%2 == 0U)
			{
				GPIOC->ODR |= 0x2000u;
			}
			else
			{
				GPIOC->ODR &= ~0x2000u;
			}
	}
}

void RTC_Config(void)
{	
	//LSE config
	//enable the power and backup interface clocks by setting the PWREN and BKPEN bits in the RCC_APB1ENR register
	RCC->APB1ENR |= 0x18000000;
	//set the DBP bit the Power Control register (PWR_CR) to enable access to the Backup registers and RTC
	PWR->CR |= 0x100u;
	//
	RCC->BDCR |= 0x01u;
	while(((RCC->BDCR >> 1) & 0x01u) != 1u);
	//Select LSE: RTCSEL[1:0]: RTC clock source selection / 01: LSE oscillator clock used as RTC clock
	RCC->BDCR |= 0x0100u;
	//RTC clock enable
	RCC->BDCR |= 0x8000u;
	
	//config RTC
	//Poll RTOFF, wait until its value goes to ‘1’
	while(((RTC->CRL >> 5u) & 0x01u) != 1u);
	//Set the CNF bit to enter configuration mode
	RTC->CRL |= 0x10u;
	//Write to one or more RTC registers
	//If the input clock frequency (fRTCCLK) is 32.768 kHz, write 7FFFh in this register to get a signal period of 1 second
	RTC->PRLH |= 0u;
	RTC->PRLL |= 0x7FFFu;
	//Bit 0 SECIE: Second interrupt enable
	RTC->CRH = 0x01u;
	//clear interrupt flag
	RTC->CRL &= ~0x01u;
	//Clear the CNF bit to exit configuration mode
	RTC->CRL &= ~0x10u;
	//Poll RTOFF, wait until its value goes to ‘1’ to check the end of the write operation
	while(((RTC->CRL >> 5u) & 0x01u) != 1u);
	//Enable interrupt in Core 
	NVIC->ISER[0] = 0x08;
}

void RTC_1sCount(void)
{
	//Check interrupt flag and enable interrupt bit
	if(((RTC->CRL & 0x01) == 1U) && ((RTC->CRH & 0x01) == 1U))
	{
		RTC->CRL &= ~0x01u;
		Seconds++;
		if(Seconds == 60u)
		{
			Seconds = 0u;
			Minutes++;
			if(Minutes == 60u)
			{
				Minutes = 0u;
				Hours++;
				if(Hours == 24u)
				{
					Hours = 0u;
					Week++;
					if(Week == 9u)
					{
							Week = 2u;
					}
				}
			}
		}			
	}
}

void GPIO_Output_Configure(void)
{
	//Set output for PC13
	RCC->APB2ENR |= 0x10u; //Enable Port C
	GPIOC->CRH &= 0xFF0FFFFF; //Reset Pin 13 GPIOC
	GPIOC->CRH |= 0x00300000; //Output, speed 50MHz, Mode push-pull 
	
}