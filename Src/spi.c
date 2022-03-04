#include "main.h"
#include "spi.h"
#include <string.h>

/* -------------- External variables ---------------- */
extern SPI_HandleTypeDef hspi2;

/* -------------- Static variables ------------------ */
uint8_t spiTxBuffer[SPI_BUF_LEN + 2] = {0x00, 0x00, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00};
uint8_t spiRxBuffer[SPI_BUF_LEN + 2];

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
	HAL_GPIO_WritePin(FLASH_CS_GPIO_Port, FLASH_CS_Pin, GPIO_PIN_SET);
	uint8_t buffer[SPI_BUF_LEN];
	memcpy(buffer, spiRxBuffer, SPI_BUF_LEN);
}

void SonicSpiTransmitReceive(void)
{
	HAL_GPIO_WritePin(FLASH_CS_GPIO_Port, FLASH_CS_Pin, GPIO_PIN_RESET);
	HAL_SPI_TransmitReceive_DMA(&hspi2, spiTxBuffer, spiRxBuffer, SPI_BUF_LEN + 1);
}

uint32_t errorCode;
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
	errorCode = HAL_SPI_GetError(hspi);
	HAL_SPI_TransmitReceive_DMA(hspi, spiTxBuffer, spiRxBuffer, SPI_BUF_LEN + 1);
}
