#ifndef _APP_MAIN_H
#define _APP_MAIN_H

#include <stdint.h>

#define FIRMWARE_VERSION "1.23"

#define APP_MAIN_MSG_Q_LEN	16
#define APP_MAIN_MSG_LEN	8

enum APP_MAIN_MSG {
	MSG_TYPE_REMOTE_RECEIVED = 1,
	MSG_TYPE_MOTOR_TO_VCU,
	MSG_TYPE_MOTOR_REPORT_SPD_POS,
	MSG_TYPE_MOTOR_REPORT_VOL_ERR,
	MSG_TYPE_IO_TO_VCU,
	MSG_TYPE_TEMPERATURE,
	MSG_TYPE_UPGRADE
};

typedef struct {
	uint16_t msgType;
	uint16_t msgSubType;
	uint8_t msg[APP_MAIN_MSG_LEN];
} APP_MAIN_MSG_t;

void SendMessageToAppMain(APP_MAIN_MSG_t*);
int GetDriveMode(void);
int16_t GetTemperature(void);
void InitAppMain(void);

#endif
