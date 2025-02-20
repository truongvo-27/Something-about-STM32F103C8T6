#include <stm32f10x.h>

void GPIO_SPI_Conf(void);
void SPI_Conf(void);
uint8_t Spi_TransmitAndReceive(uint8_t u8data);
void Erase_Sector_ExternalFlash(uint32_t u32Address);
void Write_Page_ExternalFlash(uint32_t u32Address, uint8_t *pu8Data);
void Read_ExternalFlash(uint32_t u32Address, uint8_t *pu8Data, uint32_t len);
uint8_t ReadStatusRegisters1(void);

static uint8_t u8buff[256] = {0u};
static uint8_t u8buff_read[256] = {0u};

int main(void)
{
	GPIO_SPI_Conf();
	SPI_Conf();
	
	uint32_t u32count = 0;
	for(u32count = 0; u32count < 256; u32count++)
	{
		u8buff[u32count] = u32count;
	}
	Erase_Sector_ExternalFlash(0x00000000);
	Write_Page_ExternalFlash(0x00000000, u8buff);
	Read_ExternalFlash(0x00000000, u8buff_read, 256);
	while(1)
	{
	
	}
}

void GPIO_SPI_Conf(void)
{
	//enable clock GPIOB
	RCC->APB2ENR |= 0x08;
	
	//NSS
	GPIOB->CRH &= 0xFFF0FFFF;
	GPIOB->CRH |= 0x00030000;
	GPIOB->ODR |= 0x00001000;
	
	//SCK
	GPIOB->CRH &= 0xFF0FFFFF;
	GPIOB->CRH |= 0x00B00000;
	
	//MISO
	GPIOB->CRH &= 0xF0FFFFFF;
	GPIOB->CRH |= 0x0B000000;
	
	//MOSI
	GPIOB->CRH &= 0x0FFFFFFF;
	GPIOB->CRH |= 0xB0000000;
}

void SPI_Conf(void)
{
	//enable clock SPI2
	RCC->APB1ENR |= 0x4000;
	
	//Select the BR[2:0] bits to define the serial clock baud rate (see SPI_CR1 register), fPCLK / 32, [5:3] = 100
	SPI2->CR1 |= 0x20;
	
	//Select the CPOL and CPHA bits to define one of the four relationships between the data transfer and the serial clock, CPOL = 0 and CPHA = 0
	SPI2->CR1 |= 0x00;
	
	//Set the DFF bit to define 8- or 16-bit data frame format, 8 bit
	SPI2->CR1 |= 0x00;
	
	//Configure the LSBFIRST bit in the SPI_CR1 register to define the frame format, MSB transmitted first
	SPI2->CR1 |= 0x00;
	
	/*If the NSS pin is required in input mode, in hardware mode, connect the NSS pin to a
	high-level signal during the complete byte transmit sequence. In NSS software mode,
	set the SSM and SSI bits in the SPI_CR1 register. If the NSS pin is required in output
	mode, the SSOE bit only should be set.*/
	SPI2->CR1 |= 0x300;
	
	//The MSTR and SPE bits must be set
	SPI2->CR1 |= 0x44;
}

uint8_t Spi_TransmitAndReceive(uint8_t u8data)
{
	//Data received or to be transmitted
	SPI2->DR = u8data; 
	//wait until SPI (or I2S) not busy
	while((SPI2->SR & 0x80) != 0);
	//return data
	return SPI2->DR;
}

//This function use to check busy bit
uint8_t ReadStatusRegisters1(void)
{
	uint8_t u8ReadStatusRegisters1;
	
	GPIOB->ODR &= ~0x1000u;
	
	Spi_TransmitAndReceive(0x05);
	u8ReadStatusRegisters1 = Spi_TransmitAndReceive(0x00); //dummy data(du lieu ko co y nghia)
	
	GPIOB->ODR |= 0x1000;
	
	/*BUSY is a read only bit in the status register (S0) that is set to a 1 state when the device is executing a
	Page Program, Sector Erase, Block Erase, Chip Erase or Write Status Register instruction. */
	return (u8ReadStatusRegisters1 & 0x1); 
}

void Erase_Sector_ExternalFlash(uint32_t u32Address)
{
	//Write enable
	GPIOB->ODR &= ~0x1000u;
	Spi_TransmitAndReceive(0x06);
	GPIOB->ODR |= 0x1000;
	
	while(ReadStatusRegisters1() == 1);
	
	//erase sector
	GPIOB->ODR &= ~0x1000u;
	
	Spi_TransmitAndReceive(0x20);
	
	Spi_TransmitAndReceive((uint8_t)(u32Address >> 16));
	Spi_TransmitAndReceive((uint8_t)(u32Address >> 8));
	Spi_TransmitAndReceive((uint8_t)u32Address);
	
	GPIOB->ODR |= 0x1000;
	
	while(ReadStatusRegisters1() == 1);
}

void Write_Page_ExternalFlash(uint32_t u32Address, uint8_t *pu8Data)
{
	//Write enable
	GPIOB->ODR &= ~0x1000u;
	Spi_TransmitAndReceive(0x06);
	GPIOB->ODR |= 0x1000;
	
	while(ReadStatusRegisters1() == 1);
	
	//Write Page
	GPIOB->ODR &= ~0x1000u;
	
	Spi_TransmitAndReceive(0x02);
	
	Spi_TransmitAndReceive((uint8_t)(u32Address >> 16));
	Spi_TransmitAndReceive((uint8_t)(u32Address >> 8));
	Spi_TransmitAndReceive((uint8_t)u32Address);
	
	uint16_t u16count = 0;
	for(u16count = 0; u16count < 256; u16count++)
	{
		Spi_TransmitAndReceive(pu8Data[u16count]);	
	}
	
	GPIOB->ODR |= 0x1000;
	
	while(ReadStatusRegisters1() == 1);
}

void Read_ExternalFlash(uint32_t u32Address, uint8_t *pu8Data, uint32_t len)
{
	GPIOB->ODR &= ~0x1000u;
	
	Spi_TransmitAndReceive(0x03);
	Spi_TransmitAndReceive((uint8_t)(u32Address >> 16));
	Spi_TransmitAndReceive((uint8_t)(u32Address >> 8));
	Spi_TransmitAndReceive((uint8_t)u32Address);
	
	uint32_t u32count = 0;
	for(u32count = 0; u32count < len; u32count++)
	{
		pu8Data[u32count] = Spi_TransmitAndReceive(0x00);
	}
	
	GPIOB->ODR |= 0x1000;
}

