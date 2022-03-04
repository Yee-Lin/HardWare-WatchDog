/**
  ******************************************************************************
  * @file           : can.c
  * @brief          : CAN functions and thread
  ******************************************************************************
  * 
  *
  ******************************************************************************
  */
#include "cmsis_os2.h" // ::CMSIS:RTOS2
#include <stdbool.h>
#include <main.h>
#include <string.h>
#include "can.h"
#include "app_main.h"

// ----------- CAN handle ----------------------------
extern CAN_HandleTypeDef hcan1;
extern CAN_HandleTypeDef hcan2;
/* --------------- static variables ----------------- */
// ----------- Message queue ------------------------
static osMessageQueueId_t appCan1MsgQueueId;
static osMessageQueueId_t appCan1SendQueueId;
static osMessageQueueId_t appCan2MsgQueueId;
static osMessageQueueId_t appCan2SendQueueId;
// ----------- Tx and Rx Buffer ----------------------
typedef struct
{
	CAN_RxHeaderTypeDef rxMsgHead;
	uint8_t payload[CAN_MSG_LEN];
} AppCanMsg_t;

typedef struct
{
	CAN_TxHeaderTypeDef txMsgHead;
	uint8_t payload[CAN_MSG_LEN];
} AppCanSendMsg_t;

/* ----------------- static functions ----------------*/
static HAL_StatusTypeDef Can1ConfigAndStart(void);
void app_can_send(void* arg);

/**
 * @brief Thread of CAN
 * The function of this thread is to parse messages received from the CAN bus and distribute
 * them to other threads.
 * @param arg Pointer to an incoming parameter when creating the thread.
 * @return none
 */
void app_can(void const *arg)
{
	appCan1MsgQueueId = osMessageQueueNew(APP_CAN_MSG_Q_LEN, sizeof(AppCanMsg_t), NULL);
	Can1ConfigAndStart();
	
	while (true)
	{
		AppCanMsg_t canMessage;
		osStatus_t osStatus = osMessageQueueGet(appCan1MsgQueueId, &canMessage, NULL, osWaitForever);
		if (osOK != osStatus) continue;
		switch (canMessage.rxMsgHead.FilterMatchIndex)
		{
		case 0:	// CAN_ID_REMOTE_VCU
		{ // Received a message from remote controller, forwarding it to thread app_main.
			APP_MAIN_MSG_t msg;
			memcpy(msg.msg, canMessage.payload, CAN_MSG_LEN);
			msg.msgType = MSG_TYPE_REMOTE_RECEIVED;
			SendMessageToAppMain(&msg);
		}
		break;

		default:
		break;
		}
	}
}

/**
 * @brief Thread of CAN send
 * The thread is for sending can message for multiple thread calling, 
 * providing a message queue to store the messages which need to be sent,
 * and send out the messages by CAN one by one.
 * @param arg Pointer to an argument which can be passed in while the thread was created.
 */
void app_can_send(void *arg)
{
	appCan1SendQueueId = osMessageQueueNew(APP_CAN_MSG_Q_LEN, sizeof(AppCanSendMsg_t), NULL);
	while (true)
	{
		AppCanSendMsg_t canSendMsg;
		osStatus_t osStatus = osMessageQueueGet(appCan1SendQueueId, &canSendMsg, NULL, osWaitForever);
		if (osOK != osStatus) continue;
		HAL_CAN_StateTypeDef canState = HAL_CAN_GetState(&hcan1);
		if (HAL_CAN_STATE_READY != canState && HAL_CAN_STATE_LISTENING != canState) continue;
		while (true)
		{
			uint32_t txMailBox;
			HAL_StatusTypeDef result = HAL_CAN_AddTxMessage(&hcan1, &canSendMsg.txMsgHead, canSendMsg.payload, &txMailBox);
			if (HAL_OK == result) break;
			else osDelay(2);
		}
	}
}

/**
  * @brief  Configure the CAN bus, configure the CAN ID filter, start CAN1 and enable the callback function.
  * @param  None
  * @retval HAL_StatusTypeDef HAL Status
  */
HAL_StatusTypeDef Can1ConfigAndStart()
{
	CAN_FilterTypeDef sFilterConfig;
	HAL_StatusTypeDef result;
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	sFilterConfig.FilterIdLow = CAN_ID_STD | CAN_RTR_DATA;
	sFilterConfig.FilterActivation = ENABLE;
	// Filter 0 go fifo 0
	sFilterConfig.FilterBank = 0;
	sFilterConfig.FilterIdHigh = (uint16_t)CAN_ID_VCU_REMOTE << 5;
	sFilterConfig.FilterMaskIdHigh = 0xFF00;
	sFilterConfig.FilterMaskIdLow = 0x0000;
	sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
	result = HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig);
	if (result != HAL_OK)
		return result;

	result = HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
	if (result != HAL_OK)
		return result;

	result = HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO1_MSG_PENDING);
	if (result != HAL_OK)
		return result;

	result = HAL_CAN_Start(&hcan1);
	return result;
}



/**
  * @brief  Rx FIFO_0 Pending callback function.
  *         Rx FIFO_0 Pending callback function, get a can message at canRxMessage 
  *         and the can head at rxMsgHead. Set the os event flags to notify the
  *         thread app_can.
  * @param  hcan CAN_HandleTypeDef*
  * @retval None
  */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	if (hcan->Instance == CAN1)
	{
		AppCanMsg_t canMessage;
		if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &canMessage.rxMsgHead, canMessage.payload) == HAL_OK)
			osMessageQueuePut(appCan1MsgQueueId, &canMessage, NULL, 0);
		else 
		{
			hcan->State = HAL_CAN_STATE_READY;
			HAL_CAN_ResetError(hcan);
		}
	}
	else if(hcan->Instance == CAN2)
	{
		
	}
}

/**
  * @brief  Rx FIFO_1 Pending callback function.
  *         Rx FIFO_1 Pending callback function, get a can message at canRxMessage 
  *         and the can head at rxMsgHead. Set the os event flags to notify the
  *         thread app_can.
  * @param  hcan CAN_HandleTypeDef*
  * @retval None
  */
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	if (hcan->Instance == CAN1)
	{
		AppCanMsg_t canMessage;
		if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO1, &canMessage.rxMsgHead, canMessage.payload) == HAL_OK)
		{
			canMessage.rxMsgHead.FilterMatchIndex += 3;	// Match index is indepent for each FIFO
			osMessageQueuePut(appCan1MsgQueueId, &canMessage, NULL, 0);
		}
		else 
		{
			hcan->State = HAL_CAN_STATE_READY;
			HAL_CAN_ResetError(hcan);
		}
	}
}

/**
  * @brief  Transmit message by CAN
  * @param  id CAN message ID
  * @param  message 8 bytes message send by CAN
  * @retval none
  */
void CanTransmit(uint32_t id, uint8_t *message, uint32_t msgLen)
{
	AppCanSendMsg_t canSendMsg;
	canSendMsg.txMsgHead.IDE = CAN_ID_STD;
	canSendMsg.txMsgHead.RTR = CAN_RTR_DATA;
	canSendMsg.txMsgHead.TransmitGlobalTime = DISABLE;
	canSendMsg.txMsgHead.DLC = msgLen;
	canSendMsg.txMsgHead.StdId = id;
	memcpy(canSendMsg.payload, message, CAN_MSG_LEN);
	osMessageQueuePut(appCan1SendQueueId, &canSendMsg, NULL, 0);
}

