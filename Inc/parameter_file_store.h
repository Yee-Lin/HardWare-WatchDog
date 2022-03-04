#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "main.h"
#include "rl_net.h" // Keil.MDK-Plus::Network:CORE

typedef struct _PARAMETERS
{
    uint8_t ipAddress[NET_ADDR_IP4_LEN];
    uint8_t subnetMask[NET_ADDR_IP4_LEN];
    uint32_t dhcpEnable;
    uint32_t udpPort;
    uint32_t impactProtection;
    uint32_t impactDistance;
    uint32_t sideImpactProtection;
    uint32_t sideImpactDistance;
    uint32_t impactThreshold;
	uint32_t releaseMotorTime;
	uint32_t releaseMotorMethod;
    uint8_t sonicSensorMap[ULTRA_SONIC_SENSOR_NUM];
    uint32_t crc;
} Parameters_t;

typedef struct _PARAM_FILE_STORE
{
    Parameters_t* parameters;
    bool (*Read)(void);
    void (*Write)(void);
    uint32_t (*GetCRC)(void);
} ParameterFileStore_t;

ParameterFileStore_t* InitParameterFileStore(void);
