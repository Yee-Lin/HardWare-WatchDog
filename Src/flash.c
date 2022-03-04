#include "main.h"
#include "flash.h"

#if defined(STM32F405xx) || defined(STM32F415xx) || defined(STM32F407xx) || defined(STM32F417xx) || defined(STM32F412Zx) || \
	defined(STM32F412Vx) || defined(STM32F412Rx) || defined(STM32F412Cx)
static const uint32_t FLASH_SECTOR_ADDR[FLASH_SECTOR_TOTAL] = {
	0x08000000U, // FLASH_SECTOR_0
	0x08004000U, // FLASH_SECTOR_1
	0x08008000U, // FLASH_SECTOR_2
	0x0800C000U, // FLASH_SECTOR_3
	0x08010000U, // FLASH_SECTOR_4
	0x08020000U, // FLASH_SECTOR_5
	0x08040000U, // FLASH_SECTOR_6
	0x08060000U, // FLASH_SECTOR_7
	0x08080000U, // FLASH_SECTOR_8
	0x080A0000U, // FLASH_SECTOR_9
	0x080C0000U, // FLASH_SECTOR_10
	0x080E0000U	 // FLASH_SECTOR_11
};
#endif /* STM32F405xx || STM32F415xx || STM32F407xx || STM32F417xx || STM32F412Zx || STM32F412Vx || STM32F412Rx || STM32F412Cx */

/* ---------------- Static variables ------------------- */
/**
 * @brief Flash storage data control structure
 */
static FlashStore_t flashStore;

/* ---------------- Static functions ------------------- */
static uint32_t EraseSector(uint32_t sectorNo);
static uint32_t EraseSectors(uint32_t sectorNo, uint32_t nbSectors);
static void WriteStoreData(const uint32_t* data, uint32_t size);
static void ReadStoreData(uint32_t* data, uint32_t size);
static void EraseStore(void);

/**
 * @brief Get the storage stucture pointer and initialize its function pointers
 * @param none
 * @return 
 */
FlashStore_t* GetInstanceOfFlashStorage(void)
{
	flashStore.EraseStore = EraseStore;
	flashStore.ReadStoreData = ReadStoreData;
	flashStore.WriteStoreData = WriteStoreData;
	return &flashStore;
}

/**
 * @brief Write storage data to last sector of the flash
 * @param data pointer to datas need to be written.
 * @param size how many words need to be written.
 */
void WriteStoreData(const uint32_t* data, uint32_t size)
{
	// Unlock flash
	HAL_FLASH_Unlock();
	// Erase last sector
	EraseSector(FLASH_SECTOR_11);
	// Write to last sector of the flash.
	for(int i = 0; i < size; ++i)
	{
		uint32_t dataToBeWritten = *(data + i);
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_SECTOR_ADDR[FLASH_SECTOR_11] + i*4, dataToBeWritten);
	}
	// Lock flash.
	HAL_FLASH_Lock();
}

/**
 * @brief Read storage data from last sector of the flash
 * @param data pinter to buffer which will receive the data.
 * @param size how many words need to be read.
 */
void ReadStoreData(uint32_t* data, uint32_t size)
{
	for(int i = 0; i < size; ++i)
	{
		*(data + i) = *(__IO uint32_t*)(FLASH_SECTOR_ADDR[FLASH_SECTOR_11] + i*4);
	}
}

/**
 * @brief Erase storage data area
 * Erase the last sector of the flash
 * @param none
 * @return none
 */
void EraseStore(void)
{
	HAL_FLASH_Unlock();
	EraseSector(FLASH_SECTOR_11);
	HAL_FLASH_Lock();
}

/**
 * @brief Erase a sector
 * @param sectorNo No of sector need to be erased
 * @return sectorError Error no.
 */
uint32_t EraseSector(uint32_t sectorNo)
{
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_PGSERR | FLASH_FLAG_PGPERR | 
                            FLASH_FLAG_PGAERR | FLASH_FLAG_WRPERR |
                            FLASH_FLAG_OPERR | FLASH_FLAG_EOP);
	// Setup erase parameters.
	FLASH_EraseInitTypeDef flashErase;
	flashErase.NbSectors = 1;
	flashErase.Sector = sectorNo;
	flashErase.TypeErase = FLASH_TYPEERASE_SECTORS;
	flashErase.Banks = FLASH_BANK_1;
	flashErase.VoltageRange = FLASH_VOLTAGE_RANGE_3;
	// Erase
	uint32_t sectorError;
	HAL_FLASHEx_Erase(&flashErase, &sectorError);
	return sectorError;
}

/**
 * @brief Erase sectors
 * @param sectorNo No of sector need to be erased
 * @param nbSectors Number of sectors need to be erased
 * @return sectorError Error no.
 */
uint32_t EraseSectors(uint32_t sectorNo, uint32_t nbSectors)
{
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_PGSERR);
	// Setup erase parameters.
	FLASH_EraseInitTypeDef flashErase;
	flashErase.NbSectors = nbSectors;
	flashErase.Sector = sectorNo;
	flashErase.TypeErase = FLASH_TYPEERASE_SECTORS;
	flashErase.Banks = FLASH_BANK_1;
	flashErase.VoltageRange = FLASH_VOLTAGE_RANGE_3;
	// Erase
	uint32_t sectorError;
	HAL_FLASHEx_Erase(&flashErase, &sectorError);
	return sectorError;
}
