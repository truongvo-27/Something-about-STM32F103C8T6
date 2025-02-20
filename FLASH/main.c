#include "stm32f10x.h"
#include <stdint.h>

typedef enum
{
	FLASH_ERRORS = 0U,    /* There is a error */
	FLASH_NO_ERRORS,      /* There is no errors */
} FlashStatus;

void Flash_Unlock(void);
FlashStatus Flash_Erase(volatile uint32_t u32StartAddr);
FlashStatus Flash_Write(volatile uint32_t u32StartAddr, uint8_t* u8BufferWrite, uint32_t u32Length);
FlashStatus Flash_Read(volatile uint32_t u32StartAddr, uint8_t* u8BufferRead, uint32_t u32Length);

static uint8_t BUFFER_DATA[8] = {1, 2, 3, 4, 5, 6, 7, 8};

int main(void)
{
	Flash_Erase(0x08001000);
	if(Flash_Write(0x08001000, BUFFER_DATA, (uint32_t)8) !=  FLASH_NO_ERRORS)
	{
		while(1);
	}
	if(Flash_Read(0x08001000, BUFFER_DATA, (uint32_t)8) != FLASH_NO_ERRORS)
	{
		while(1);
	}
	Flash_Erase(0x08001000);
}

void Flash_Unlock(void)
{
	FLASH->KEYR = 0x45670123;
	FLASH->KEYR = 0xCDEF89AB;
}

FlashStatus Flash_Erase(volatile uint32_t u32StartAddr)
{
	//Check that no main Flash memory operation is ongoing by checking the BSY bit in the FLASH_SR register.
	while((FLASH->SR & FLASH_SR_BSY) == FLASH_SR_BSY);
	//Read FLASH_CR_LOCK
	if((FLASH->CR & FLASH_CR_LOCK) == FLASH_CR_LOCK)
	{
		Flash_Unlock();
	}
	//Write FLASH_CR_PER to 1
	FLASH->CR |= FLASH_CR_PER;
	//Write into FAR an address within the page to erase
	FLASH->AR = u32StartAddr;
	//Write FLASH_CR_STRT to 1
	FLASH->CR |= FLASH_CR_STRT;
	while((FLASH->SR & FLASH_SR_BSY) == FLASH_SR_BSY);
	//Check the page is erased by reading all the addresses in the page
	FLASH->CR &= ~(uint32_t)FLASH_CR_STRT;
	FLASH->CR &= ~(uint32_t)FLASH_CR_PER;
	//Turn on LOCK
	FLASH->CR |= FLASH_CR_LOCK;
	return FLASH_NO_ERRORS;
}

FlashStatus Flash_Write(volatile uint32_t u32StartAddr, uint8_t* u8BufferWrite, uint32_t u32Length)
{
	uint32_t count = 0u;
	if((u8BufferWrite == 0x00u) || (u32Length == 0u) || (u32Length%2U != 0U))
	{
		return FLASH_ERRORS;
	}
	//Check that no main Flash memory operation is ongoing by checking the BSY bit in the FLASH_SR register.
	while((FLASH->SR & FLASH_SR_BSY) == FLASH_SR_BSY);
	//Read FLASH_CR_LOCK
	if((FLASH->CR & FLASH_CR_LOCK) == FLASH_CR_LOCK)
	{
		Flash_Unlock();
	}
	//Write FLASH_CR_PG to 1
	FLASH->CR |= FLASH_CR_PG;
	//Perform half-word write at the desired address, A halfword is 2 consecutive bytes
	for(count = 0u; count < u32Length/2u; count++)
	{
		*(uint16_t*)(u32StartAddr + count * 2u) = *(uint16_t*)((uint32_t)u8BufferWrite + count * 2u);
	}
	//check busy
	while((FLASH->SR & FLASH_SR_BSY) == FLASH_SR_BSY);
	//Out Write Mode
	FLASH->CR &= ~(uint32_t)FLASH_CR_PG;
	//Turn on LOCK
	FLASH->CR |= FLASH_CR_LOCK;
	return FLASH_NO_ERRORS;
}

FlashStatus Flash_Read(volatile uint32_t u32StartAddr, uint8_t* u8BufferRead, uint32_t u32Length)
{
	uint32_t count = 0u;
	if((u8BufferRead == 0x00u) || (u32Length == 0u))
	{
		return FLASH_ERRORS;
	}
	while((FLASH->SR & FLASH_SR_BSY) == FLASH_SR_BSY);
	//read data
	for(count = 0; count < u32Length/2u; count++)
	{
		*(u8BufferRead + count) = *(uint8_t*)(u32StartAddr + count);
	}
	return FLASH_NO_ERRORS;
}


