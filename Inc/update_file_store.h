#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct _UPATE_FILE_STORE
{
    void (*Read)(uint8_t*, uint32_t);
    void (*Write)(uint8_t*, uint32_t);
    uint32_t (*GetCRC)(void);
    void (*SetWritePos)(uint32_t);
    void (*SetReadPos)(uint32_t);
} UpdateFileStore_t;

UpdateFileStore_t* InitUpdateFileStore(void);
