#include "main.h"
#include "w25qxx.h"

#define SpiFlashCsLow() LL_GPIO_ResetOutputPin(FLASH_CS_GPIO_Port, FLASH_CS_Pin)
#define SpiFlashCsHigh() LL_GPIO_SetOutputPin(FLASH_CS_GPIO_Port, FLASH_CS_Pin)

/* --------------- extern variables ---------------- */
extern SPI_HandleTypeDef hspi2;

/* ---------------- Static variables ------------------------ */
/**
 * @brief W25QXX flash control commands definition
 */
enum _W25QXX_COMMAND
{
    W25QXX_WRITE_ENABLE =       0x06,
    W25QXX_WRITE_DISABLE =      0x04,
    W25QXX_READ_STATUS_REG =    0x05,
    W25QXX_WRITE_STATUS_REG =   0x01,
    W25QXX_READ_DATA =          0x03,
    W25QXX_FAST_READ_DATA =     0x0B,
    W25QXX_FAST_READ_DUAL =     0x3B,
    W25QXX_PAGE_PROGRAM =       0x02,
    W25QXX_BLOCK_ERASE =        0xD8,
    W25QXX_SECTOR_ERASE =       0x20,
    W25QXX_CHIP_ERASE =         0xC7,
    W25QXX_POWER_DOWN =         0xB9,
    W25QXX_RELEASE_POWER_DOWN = 0xAB,
    W25QXX_DEVICE_ID =          0xAB,
    W25QXX_MANUFACT_DEVICE_ID = 0x90,
    W25QXX_JEDEC_DEVICE_ID =    0x9F
};

/**
 * @brief W25QXX flash type definition
 */
enum _W25QXX_TYPE
{
    W25Q80 = 0xEF13,
    W25Q16 = 0xEF14,
    W25Q32 = 0xEF15,
    W25Q64 = 0xEF16,
    W25Q128 = 0xEF17
};

static const uint32_t SECTOR_SIZE = 4096;
static const uint32_t PAGE_SIZE = 256;
static const uint32_t SECTOR_MASK = 0xFFFFF000U;
static const uint32_t PAGE_MASK = 0xFFFFFF00U;
static SPI_HandleTypeDef *spiHandle = NULL;
static W25QXX_t w25qxx = {0, 0, 0, 0};

/* -------------------- Static functions --------------------- */
static void Read(uint8_t *, uint32_t, uint32_t);
static void Write(uint8_t *, uint32_t, uint32_t);
static uint8_t ReadSR(void);
static void WriteSR(uint8_t);
static void EnableWrite(void);
static void DisableWrite(void);
static uint16_t ReadID(void);
static void WaitBusyBit(uint32_t);
void EraseSector(uint32_t sectorAddr);
void ProgramPage(uint8_t *buffer, uint32_t address, uint32_t size);

/**
 * @brief Initialize W25QXX data struct
 * @param w25qqq control struct
 * @return none 
*/
W25QXX_t *W25QXX_Init(void)
{
    if (spiHandle == NULL) // Singleton mode
    {
        SpiFlashCsHigh();
        w25qxx.type = W25Q64;
        w25qxx.Read = Read;
        w25qxx.Write = Write;
        w25qxx.ReadID = ReadID;
        spiHandle = &hspi2;
    }
    return &w25qxx;
}

/**
 * @brief Read W25QXX status register
 * BIT7 6    5    4    3    2    1    0
 * SPR  RV   TB   BP2  BP1  BP0  WEL  BUSY
 * SPR: default 0, protection bit, work with WP
 * TB, BP2, BP1, BP0: Flash area write protection
 * WEL: Write eanble lock
 * BUSY: busy flag 1-busy 0-idle
 * default value: 0x00
 * @param none
 * @return status register value
 */
uint8_t ReadSR(void)
{
    uint8_t txBuff[2] = {W25QXX_READ_STATUS_REG, 0xFF};
    uint8_t rxBuff[2];
    SpiFlashCsLow();
    HAL_SPI_TransmitReceive(spiHandle, txBuff, rxBuff, 2, 5);
    SpiFlashCsHigh();
    return rxBuff[1];
}

/**
 * @brief Write W25QXX status register
 * Only SPR, TB, BP2, BP1, BP0(bit 7,5,4,3,2) can be written.
 * @param status status data to be written
 * @return none
 */
void WriteSR(uint8_t status)
{
    uint8_t txBuff[2] = {W25QXX_WRITE_STATUS_REG, status};
    uint8_t rxBuff[2];
    SpiFlashCsLow();
    HAL_SPI_TransmitReceive(spiHandle, txBuff, rxBuff, 2, 5);
    SpiFlashCsHigh();
}

/**
 * @brief Read chip ID
 * @param none
 * @return chip manufacture ID
 *      @arg @ref W25Q80
 *      @arg @ref W25Q16
 *      @arg @ref W25Q32
 *      @arg @ref W25Q64
 *      @arg @ref W25Q128
 */
uint16_t ReadID(void)
{
    uint16_t id;
    uint8_t txBuff[6] = {W25QXX_MANUFACT_DEVICE_ID, 0x00, 0x00, 0x00};
    uint8_t rxBuff[2];
    SpiFlashCsLow();
    HAL_SPI_Transmit(spiHandle, txBuff, 4, 4);
    HAL_SPI_Receive(spiHandle, rxBuff, 2, 2);
    SpiFlashCsHigh();
    id = rxBuff[0] << 8 | rxBuff[1];
    return id;
}

/**
 * @brief Read datas from flash
 * @param buffer read buffer pointer
 * @param addr flash address to be read, 24 bits address
 * @param size how many datas need to be read
 * @return none
 */
void Read(uint8_t *buffer, uint32_t addr, uint32_t size)
{
    uint8_t txBuff[4] = {W25QXX_READ_DATA, (uint8_t)(addr >> 16), (uint8_t)(addr >> 8), (uint8_t)addr};
    SpiFlashCsLow();
    HAL_SPI_Transmit(spiHandle, txBuff, sizeof(txBuff), sizeof(txBuff) * 2);
    HAL_SPI_Receive(spiHandle, buffer, size, size * 2);
    SpiFlashCsHigh();
}

/**
 * @brief Write datas to flash
 * @param buffer write buffer pointer
 * @param addrress flash address to be written, 24 bits address
 * @param size how many datas need to be written
 * @return none
 */
void Write(uint8_t *buffer, uint32_t address, uint32_t size)
{
    // Erasing, if nessary, sectors
    if (0 == (address & (~SECTOR_MASK)))
    { // Write to a new sector, need to erase it firstly
        EraseSector(address);
    }
    uint32_t endAddress = address + size - 1;
    uint32_t startSectorAddr = (address & SECTOR_MASK) + SECTOR_SIZE;
    uint32_t endSectorAddr = endAddress & SECTOR_MASK;
    while (startSectorAddr <= endSectorAddr)
    { // Erase all needed sectors if the data cross multiple sectors.
        EraseSector(startSectorAddr);
        startSectorAddr += SECTOR_SIZE;
    }

    // Writing data
    uint32_t remainSize = size, writeAddress = address, pageRemainSize;
    uint8_t *readPtr = buffer;
    while (remainSize)
    {
        // Check if this page already has data and calculate its remain size;
        pageRemainSize = PAGE_SIZE - (writeAddress & (~PAGE_MASK));
        uint32_t writeSize = (remainSize < pageRemainSize) ? remainSize : pageRemainSize;
        ProgramPage(readPtr, writeAddress, writeSize);
        remainSize -= writeSize;
        readPtr += writeSize;
        writeAddress += writeSize;
    }
}

/**
 * @brief Enalbe flash write
 * @param none
 * @return none
 */
void EnableWrite(void)
{
    uint8_t txBuff[1] = {W25QXX_WRITE_ENABLE};
    uint8_t rxBuff[1];
    SpiFlashCsLow();
    HAL_SPI_TransmitReceive(spiHandle, txBuff, rxBuff, sizeof(txBuff), sizeof(txBuff) * 2);
    SpiFlashCsHigh();
}

/**
 * @brief Disable flash write
 * @param none
 * @return none
 */
void DisableWrite(void)
{
    uint8_t txBuff[1] = {W25QXX_WRITE_DISABLE};
    uint8_t rxBuff[1];
    SpiFlashCsLow();
    HAL_SPI_TransmitReceive(spiHandle, txBuff, rxBuff, sizeof(txBuff), sizeof(txBuff) * 2);
    SpiFlashCsHigh();
}

/** 
 * @brief Wait busy bit clear
 * @param period The time interval to check the busy bit again
 * @return none
 */
void WaitBusyBit(uint32_t period)
{
    while (ReadSR() & 0x01)
        osDelay(period);
}

/**
 * @brief Erase sector
 * @param sectorAddr Sector address in 24bits
 * @return none
 */
void EraseSector(uint32_t sectorAddr)
{
    uint8_t txBuff[] = {W25QXX_SECTOR_ERASE, (uint8_t)(sectorAddr >> 16), (uint8_t)(sectorAddr >> 8), (uint8_t)sectorAddr};
    EnableWrite();
    WaitBusyBit(5);
    SpiFlashCsLow();
    HAL_SPI_Transmit(spiHandle, txBuff, sizeof(txBuff), sizeof(txBuff) * 2);
    SpiFlashCsHigh();
    WaitBusyBit(5);
}

/**
 * @brief Program a page, 256 bytes length
 * Write a page, only one page can be written at a time.
 * @param buffer Pointer to the datas to be written.
 * @param address Flash address to be written
 * @param size Data length in bytes
 * @return none
 */
void ProgramPage(uint8_t *buffer, uint32_t address, uint32_t size)
{
    uint8_t txBuff[] = {W25QXX_PAGE_PROGRAM, (uint8_t)(address >> 16), (uint8_t)(address >> 8), (uint8_t)address};
    EnableWrite();
    SpiFlashCsLow();
    HAL_SPI_Transmit(spiHandle, txBuff, sizeof(txBuff), sizeof(txBuff) * 2);
    HAL_SPI_Transmit(spiHandle, buffer, size, size * 2);
    SpiFlashCsHigh();
    WaitBusyBit(1);
}
