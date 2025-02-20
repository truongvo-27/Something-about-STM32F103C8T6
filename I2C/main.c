#include <stm32f10x.h>

//I2C & DS1307 config
void I2C_Setup(void);
void I2C_SentData(uint8_t SlaveAdd, uint8_t reg, uint8_t data);
uint8_t I2C_Receive_Data(uint8_t SlaveAdd, uint8_t reg);

uint8_t BCD_to_DEC(uint8_t bcd);
void SetTime(uint8_t Sec, uint8_t Min, uint8_t Hour);
void GetTime(uint8_t *Sec, uint8_t *Min, uint8_t *Hour);

//UART config
void UART_Conf(void);
void Write_Data(char data);
void Write_String(uint8_t *str);

//Delay function
void Delay_Ms(uint32_t u32Count);

//GPIO Config
void GPIO_Conf(void);

/*THIS IS MAIN FUNCTION*/
int main(void)
{
	I2C_Setup();
	UART_Conf();
	GPIO_Conf();
	uint8_t Hour = 10;
	uint8_t Min = 30;
	uint8_t Sec = 0;
	SetTime(Sec, Min, Hour);
	while(1)
	{
		GetTime(&Sec, &Min, &Hour);
		char buffer[50];
		sprintf(buffer, "Time: %02d:%02d:%02d\r\n", Hour, Min, Sec);
		Write_String(buffer);
		Delay_Ms(1000); //Delay 1 se
	}
}

void I2C_Setup(void)
{
	//enable clock I2C1
	RCC->APB1RSTR |= 0x200000;
	
	//I2C config in master mode
	//Program the peripheral input clock in I2C_CR2 register in order to generate correct timings, select TCLK = 36MHz
	I2C1->CR2 |= 36u;
	//Configure the clock control register, select fSCL = 100000, CCR = fCLK / (2 x fSCL)
	I2C1->CCR |= 180u;
	//Configure the rise time register, TRISE = 10^-6 / TCLK
	I2C1->TRISE |= 37u;
	//Program the I2C_CR1 register to enable the peripheral
	I2C1->CR1 |= 0x01;
	//Set the START bit in the I2C_CR1 register to generate a Start condition
	I2C1->CR1 |= 0x100;
}

void I2C_SentData(uint8_t SlaveAdd, uint8_t reg, uint8_t data)
{
	//make sure that bit busy is cleared
	while(I2C1->SR2 & 0x02);
	
	//start transmit
	//Set the START bit in the I2C_CR1 register to generate a Start condition
	I2C1->CR1 |= 0x100;
	//EV5: SB=1, cleared by reading SR1 register followed by writing DR register with Address.
	while(!(I2C1->SR1 & 0x01));
	I2C1->DR = SlaveAdd << 1;
	//EV6: ADDR=1, cleared by reading SR1 register followed by reading SR2.
	while(!(I2C1->SR1 & 0x02));
	I2C1->SR2;
	//EV8_1: TxE=1, shift register empty, data register empty, write Data1 in DR.
	while(!(I2C1->SR1 & 0x80));
	I2C1->DR = reg;
	//EV8: TxE=1, shift register not empty, data register empty, cleared by writing DR register .
	while(!(I2C1->SR1 & 0x80));
	I2C1->DR = data;
	//EV8_2: TxE=1, BTF = 1, Program Stop request. TxE and BTF are cleared by hardware by the Stop condition
	while(!((I2C1->SR1 & 0x80) && (I2C1->SR1 & 0x04)));
	//Stop condition
	I2C1->CR1 |= 0x200;
}

uint8_t I2C_Receive_Data(uint8_t SlaveAdd, uint8_t reg)
{
	uint8_t data;
	
	//make sure that bit busy is cleared
	while(I2C1->SR2 & 0x02);
	
	//Write SlaveAdd and reg
	//Set the START bit in the I2C_CR1 register to generate a Start condition
	I2C1->CR1 |= 0x100;
	//EV5: SB=1, cleared by reading SR1 register followed by writing DR register with Address.
	while(!(I2C1->SR1 & 0x01));
	I2C1->DR = SlaveAdd << 1;
	//EV6: ADDR=1, cleared by reading SR1 register followed by reading SR2.
	while(!(I2C1->SR1 & 0x02));
	I2C1->SR2;
	//EV8_1: TxE=1, shift register empty, data register empty, write Data1 in DR.
	while(!(I2C1->SR1 & 0x80));
	I2C1->DR = reg;
	
	//Start Read
	//Set the START bit in the I2C_CR1 register to generate a Start condition
	I2C1->CR1 |= 0x100;
	//EV5: SB=1, cleared by reading SR1 register followed by writing DR register with Address.
	while(!(I2C1->SR1 & 0x01));
	I2C1->DR = (SlaveAdd << 1) | 0x01;
	//EV6: ADDR=1, cleared by reading SR1 register followed by reading SR2. In 10-bit master receiver mode, this sequence should be followed by writing CR2 with START = 1.
	while(!(I2C1->SR1 & 0x02));
	I2C1->SR2;
	//EV6_1: no associated flag event, used for 1 byte reception only. The Acknowledge disable and Stop condition generation are made just after EV6, that is after ADDR is cleared
	//The Acknowledge disable
	I2C1->CR1 &= ~0x400;
	//Stop condition generation are made just after EV6
	I2C1->CR1 |= 0x200;
	//EV7: RxNE=1 cleared by reading DR register.
	while(!(I2C1->SR1 & 0x40));
	data = I2C1->DR;
	return data;
}

uint8_t BCD_to_DEC(uint8_t bcd)
{
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

void SetTime(uint8_t Sec, uint8_t Min, uint8_t Hour)
{
	I2C_SentData(0x68, 0x00, BCD_to_DEC(Sec));
	I2C_SentData(0x68, 0x01, BCD_to_DEC(Min));
	I2C_SentData(0x68, 0x02, BCD_to_DEC(Hour));
}	

void GetTime(uint8_t *Sec, uint8_t *Min, uint8_t *Hour)
{
	*Sec = BCD_to_DEC(I2C_Receive_Data(0x68, 0x00));
	*Min = BCD_to_DEC(I2C_Receive_Data(0x68, 0x01));
	*Hour = BCD_to_DEC(I2C_Receive_Data(0x68, 0x02));
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

void Write_String(uint8_t *str)
{
	uint32_t index = 0;
	while(str[index])
	{
		Write_Data(str[index]);
		index++;
	}
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

void GPIO_Conf(void)
{
	//Config for UART
	//PA2 as TX
	//enable clock GPIOA
	RCC->APB2ENR |= 0x04;
	//reset pin2
	GPIOA->CRL &= 0xFFFFF0FF;
	//output,  Alternate function output Push-pul, max speed 50 MHz
	GPIOA->CRL |= 0x0B00;
	
	//Config for I2C
	//PB7 as SDA and PB6 as SDL
	//enable clock GPIOB
	RCC->APB2ENR |= 0x08;
	//reset pin6, 7
	GPIOB->CRL &= 0x00FFFFFF;
	//output,  Alternate function output Push-pul, max speed 50 MHz
	GPIOB->CRL |= 0xBB000000;
}
