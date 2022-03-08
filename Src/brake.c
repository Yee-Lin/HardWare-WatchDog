#include "main.h"
#include "cmsis_os2.h" // ::CMSIS:RTOS2
#include "brake.h"
#include "can.h"


extern osMessageQueueId_t appCan1SendQueueId;
extern osMessageQueueId_t appCan2SendQueueId;
uint8_t brake[8]={0x05,0,0,0,0,0,0,0};
osStatus_t Braking(void)
{
    osStatus_t osStatus = osMessageQueuePut(appCan2SendQueueId,brake,CAN_MSG_LEN,0);
    return osStatus;
}
osStatus_t 

