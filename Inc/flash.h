#pragma once

typedef struct _FlashStore_t
{
    void (*WriteStoreData)(const uint32_t* data, uint32_t size);
    void (*ReadStoreData)(uint32_t* data, uint32_t size);
    void (*EraseStore)(void);
} FlashStore_t;

FlashStore_t* GetInstanceOfFlashStorage(void);
