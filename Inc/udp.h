#pragma once

#include <stdint.h>
#include <stdbool.h>

void InitUdp(void);
bool UdpSendData(const uint8_t* buff, uint32_t size);
