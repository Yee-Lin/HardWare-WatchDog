#include "main.h"
#include "parameter_file_store.h"
#include "w25qxx.h"
#include "mem_pool.h"
#include "rl_net.h"
#include "crc32.h"
#include "Net_Config_ETH_0.h"
#include <string.h>

#define PARAMETER_FILE_POSITION	0x700000U
#define PARAMETER_FILE_LENGTH	256

/* --------------- extern variables ---------------- */
extern CRC_HandleTypeDef hcrc;

/* --------------- Static variables ---------------- */
static W25QXX_t *w25qxx = NULL;
static ParameterFileStore_t fileStore;
static Parameters_t parameters;

/* ---------------- Static functions ---------------- */
static void Write(void);
static bool Read(void);
static uint32_t GetCRC(void);

ParameterFileStore_t *InitParameterFileStore(void)
{
    if (w25qxx == NULL)
    {
        w25qxx = W25QXX_Init();
        fileStore.parameters = &parameters;
        fileStore.Read = Read;
        fileStore.Write = Write;
        fileStore.GetCRC = GetCRC;

        parameters.dhcpEnable = false;
        uint8_t ipAddr[4];
        netIP_aton(ETH0_IP4_ADDR, NET_ADDR_IP4, ipAddr);
        memcpy(parameters.ipAddress, ipAddr, 4);
        netIP_aton(ETH0_IP4_MASK, NET_ADDR_IP4, ipAddr);
        memcpy(parameters.subnetMask, ipAddr, 4);
        parameters.impactProtection = true;
        parameters.impactDistance = 250;
        parameters.sideImpactProtection = true;
        parameters.sideImpactDistance = 50;
		parameters.impactThreshold = 500;
        parameters.udpPort = 19000;
		parameters.releaseMotorMethod = 0;
		parameters.releaseMotorTime = 20000;
        for(int i = 0; i < ULTRA_SONIC_SENSOR_NUM; ++i) parameters.sonicSensorMap[i] = i;
    }
    return &fileStore;
}

void Write(void)
{
    parameters.crc = GetCRC();
    w25qxx->Write((uint8_t*)&parameters, PARAMETER_FILE_POSITION, sizeof(parameters));
}

bool Read(void)
{
	uint8_t* paramBuff = MemoryAlloc(sizeof(parameters));
    w25qxx->Read(paramBuff, PARAMETER_FILE_POSITION, sizeof(parameters));
	Parameters_t* paramPtr = (Parameters_t*)paramBuff;
    if(paramPtr->crc == Crc32(0xFFFFFFFF, paramPtr, sizeof(parameters)-sizeof(parameters.crc)))
	{
		memcpy(&parameters, paramPtr, sizeof(parameters));
		MemoryFree(paramBuff);
		return true;
	}
    else
	{
		MemoryFree(paramBuff);
		return false;
	}
}

uint32_t GetCRC(void)
{
	return Crc32(0xFFFFFFFF, &parameters, sizeof(parameters)-sizeof(parameters.crc));
}
