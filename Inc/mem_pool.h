#pragma once

#include <stdint.h>

void InitMemoryPool (void);
void* MemoryAlloc(uint32_t size);
uint32_t MemoryFree(void* mem);
