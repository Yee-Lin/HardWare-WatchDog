#include "cmsis_os2.h" // CMSIS RTOS header file

/*----------------------------------------------------------------------------
 *      Memory Pool creation & usage
 *---------------------------------------------------------------------------*/

// Size of each memory pool
#define MEMPOOL_128_OBJECTS 64
#define MEMPOOL_256_OBJECTS 32
#define MEMPOOL_512_OBJECTS 16
#define MEMPOOL_1024_OBJECTS 8
#define MEMPOOL_2048_OBJECTS 4
#define MEMPOOL_4096_OBJECTS 2

#define MEMPOOL_TYPES 6

static struct _MEM_POOL
{
    osMemoryPoolId_t memPoolId;
    uint32_t memPoolObjects;
    uint32_t memSize;
} memPool[MEMPOOL_TYPES];

void InitMemoryPool(void)
{
    memPool[0].memPoolObjects = MEMPOOL_128_OBJECTS;
    memPool[1].memPoolObjects = MEMPOOL_256_OBJECTS;
    memPool[2].memPoolObjects = MEMPOOL_512_OBJECTS;
    memPool[3].memPoolObjects = MEMPOOL_1024_OBJECTS;
    memPool[4].memPoolObjects = MEMPOOL_2048_OBJECTS;
    memPool[5].memPoolObjects = MEMPOOL_4096_OBJECTS;
    uint32_t size = 128;
    for (int i = 0; i < MEMPOOL_TYPES; ++i)
    {
        memPool[i].memSize = size;
        size <<= 1;
        memPool[i].memPoolId = osMemoryPoolNew(memPool[i].memPoolObjects, memPool[i].memSize, NULL);
    }
}

void *MemoryAlloc(uint32_t size)
{
    for (int i = 0; i < MEMPOOL_TYPES; ++i)
    {
        if (size <= memPool[i].memSize)
        {
            return osMemoryPoolAlloc(memPool[i].memPoolId, 0);
        }
    }
    return NULL;
}

uint32_t MemoryFree(void *mem)
{
    osStatus_t status;
    for (int i = 0; i < MEMPOOL_TYPES; ++i)
    {
        status = osMemoryPoolFree(memPool[i].memPoolId, mem);
        if (status == osOK)
            break;
        else if (status == osErrorParameter)
        {
            osMemoryPoolFree(memPool[i].memPoolId, mem);
        }
    }
    return status;
}
