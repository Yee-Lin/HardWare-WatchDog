#ifndef _CAN_H
#define _CAN_H

#include <stdbool.h>

/* ---------------- CAN identify define ------------------- */


/* ---------------- CAN identify define ------------------- */
#define CAN_ID_SRS_RESP                 0x31F  //SRS: security redundant system, hardware watchdog.
#define CAN_ID_RESP_SRS                 0x30F
#define CAN_ID_VCU_LEFT_FRONT_MOTOR     0x4A4
#define CAN_ID_VCU_LEFT_REAR_MOTOR      0x4A3
#define CAN_ID_VCU_RIGHT_FRONT_MOTOR    0x4A2
#define CAN_ID_VCU_RIGHT_REAR_MOTOR     0x4A1
#define CAN_ID_VCU_FEPS                 0x416
#define CAN_ID_VCU_BEPS                 0x426
#define CAN_ID_FEPS_VCU                 0x410
#define CAN_ID_BEPS_VCU                 0x420
#define CAN_ID_VCU_BMS                  0x781
#define CAN_ID_BMU_VLOTAGE              0x782   
#define CAN_ID_BMU_CAPACITY             0x783
#define CAN_ID_BMS_TEMPERATURE          0x788
#define CAN_ID_BMS_1to4_BATTERY         0x789
#define CAN_ID_BMS_5to8_BATTERY         0x78a
#define CAN_ID_BMS_9to12_BATTERY        0x78b
#define CAN_ID_BMS_13to16_BATTERY       0x78c
#define CAN_ID_BMS_17to20_BATTERY       0x78d




#define APP_CAN_MSG_Q_LEN 32
#define CAN_MSG_LEN 8

void SendCanMessageToRemoteController(uint8_t *message);
void SendCanMessageToMotor(uint16_t cobId, uint8_t *message, uint32_t msgLen);
void SendCanNmtMessage(uint16_t motorId, uint8_t *message);
void CanTransmit(uint32_t id, uint8_t *message, uint32_t msgLen);
void app_can1(void const *arg);
void app_can2(void const *arg);
void app_can1_send(void* arg);
void app_can2_send(void* arg);
#endif
