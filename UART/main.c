/*This code is written for microcontroller transfer data to PC using PL2303*/

#include <stm32f10x.h>

void GPIO_Conf(void);
void UART_Conf(void);
void Write_Data(char data);
void Delay_Ms(uint32_t u32Count);
void Write_String(uint8_t *str);

int main(void)
{
	GPIO_Conf();
	UART_Conf();
	while(1)
	{
//		Write_Data('T');
//		Write_Data('H');
//		Write_Data('U');
		Write_String("votruong");
		Delay_Ms(100);
	}
}

void GPIO_Conf(void)
{
	//PA2
	//enable clock GPIOA
	RCC->APB2ENR |= 0x04;
	//reset pin2
	GPIOA->CRL &= 0xFFFFF0FF;
	//output,  Alternate function output Push-pul, max speed 50 MHz
	GPIOA->CRL |= 0x0B00;
}

void UART_Conf(void)
{
	//enable clock UART2
	RCC->APB1ENR |= 0x00020000;
	//Enable the USART by writing the UE bit in USART_CR1 register to 1.
	USART2->CR1 |= 0x2000;
	//Program the M bit in USART_CR1 to define the word length.
	USART2->CR1 |= 0x00;
	//Program the number of stop bits in USART_CR2
	USART2->CR2 |= 0x00; 
	//Select the desired baud rate using the USART_BRR register.
	//select baudrate 36MHz / 9600 = 3750 ~ 0xEA6, baudrate is 9600
	USART2->BRR = 0xEA6;
	//Set the TE bit in USART_CR1 to send an idle frame as first transmission
	USART2->CR1 |= 0x08;
}

void Write_Data(char data)
{
	//Write the data to send in the USART_DR register (this clears the TXE bit). Repeat this for each data to be transmitted in case of single buffer
	//write data
	USART2->DR = data;
	//After writing the last data into the USART_DR register, wait until TC=1
	while(!(USART2->SR & 0x40));
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

void Write_String(uint8_t *str)
{
	uint32_t index = 0;
	while(str[index])
	{
		Write_Data(str[index]);
		index++;
	}
}


