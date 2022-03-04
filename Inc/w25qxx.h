#pragma once

#include <stdint.h>

typedef struct
{
    uint32_t type;
    uint16_t (* ReadID)();
    void (* Read)(uint8_t*, uint32_t, uint32_t);
    void (* Write)(uint8_t*, uint32_t, uint32_t);
} W25QXX_t;

W25QXX_t* W25QXX_Init(void);
