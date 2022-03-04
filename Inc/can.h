#ifndef _CAN_H
#define _CAN_H

#include <stdbool.h>

/* ---------------- CAN identify define ------------------- */
#define CAN_ID_VCU_REMOTE 0x190

#define APP_CAN_MSG_Q_LEN 32
#define CAN_MSG_LEN 8

void SendCanMessageToRemoteController(uint8_t *message);
void SendCanMessageToMotor(uint16_t cobId, uint8_t *message, uint32_t msgLen);
void SendCanNmtMessage(uint16_t motorId, uint8_t *message);
void CanTransmit(uint32_t id, uint8_t *message, uint32_t msgLen);

#endif
