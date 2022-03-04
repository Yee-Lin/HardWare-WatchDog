#include "main.h"
#include "update_file_store.h"
#include "w25qxx.h"
#include "mem_pool.h"
#include "crc32.h"

/* --------------- extern variables ---------------- */
extern CRC_HandleTypeDef hcrc;

/* --------------- Static variables ---------------- */
static W25QXX_t *w25qxx = NULL;
static UpdateFileStore_t updateFileStore;
static uint32_t writePos, readPos;

/* ---------------- Static functions ---------------- */
static void Write(uint8_t *, uint32_t);
static void Read(uint8_t *, uint32_t);
static void SetWritePos(uint32_t pos);
static void SetReadPos(uint32_t pos);
static uint32_t GetCRC(void);

UpdateFileStore_t *InitUpdateFileStore(void)
{
    if (w25qxx == NULL)
    {
        w25qxx = W25QXX_Init();
        updateFileStore.Read = Read;
        updateFileStore.Write = Write;
        updateFileStore.GetCRC = GetCRC;
        updateFileStore.SetWritePos = SetWritePos;
        updateFileStore.SetReadPos = SetReadPos;
    }
    readPos = 0;
    writePos = 0;
    return &updateFileStore;
}

void Write(uint8_t *buffer, uint32_t size)
{
    w25qxx->Write(buffer, writePos, size);
    writePos += size;
}

void Read(uint8_t *buffer, uint32_t size)
{
    w25qxx->Read(buffer, readPos, size);
    readPos += size;
}

uint32_t GetCRC(void)
{
    uint8_t* buff = MemoryAlloc(2048);
    uint32_t crc = 0xFFFFFFFF;
    uint32_t fileLen;
	readPos = 0;
    Read((uint8_t*)&fileLen, sizeof(fileLen));
	readPos = 0;
    while (fileLen)
    {
        uint32_t len = (fileLen < 2048) ? fileLen : 2048;
        Read(buff, len);
		crc = Crc32(crc, buff, len);
        fileLen -= len;
    }
	MemoryFree(buff);
	return crc;
}

void SetWritePos(uint32_t pos)
{
    writePos = pos;
}

void SetReadPos(uint32_t pos)
{
    readPos = pos;
}
