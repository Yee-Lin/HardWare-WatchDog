/**
  ******************************************************************************
  * @file           : app_main.c
  * @brief          : Main thread, handle commands
  ******************************************************************************
  * Main thread which handle commands and messages from remote controller and
  * central computer. Calulate detail datas for hub moters.
  * 
  ******************************************************************************
  */
#include "rl_net.h" // Keil.MDK-Plus::Network:CORE
#include "main.h"
#include "string.h"
#include "can.h"
#include "app_main.h"
#include "udp.h"
#include "flash.h"
#include "battery.h"
#include "mem_pool.h"
#include "parameter_file_store.h"
#include "Net_Config_ETH_0.h"

extern IWDG_HandleTypeDef hiwdg;

/* --------------- Static functions ---------------- */
static void periodicTimerCallback(void *argument);
void LoadStoredParametersAndSetupNetwork(void);

/* --------------- Extern functions ----------------- */

/* --------------- Static variables ---------------- */
static const uint32_t UPDATE_FLAG = 0x55555555U;
static osMessageQueueId_t appMainMsgQueueId;
static osThreadId_t threadId;
// Periodic timer with a period of 100ms
static osTimerId_t periodicTimer;
// Network parameters


/**
 * @brief Main thread, handle commands from remote controller and upper computer
 * @param arg Pointer to an incoming parameter when creating the thread.
 * @return none
 */
void app_main(void *arg)
{
	// Initialize network interface
	netStatus netSatus = netInitialize();
	// We have to initialize memory pool first, then we can use the memory allocation.
	InitMemoryPool();
    LoadStoredParametersAndSetupNetwork();
	InitUdp();
	osThreadId_t osThIdCan1 = osThreadNew(app_can1, NULL, NULL);
	osThreadId_t osThIdCan2 = osThreadNew(app_can2, NULL, NULL);
	osThreadId_t osThIdCan1Send = osThreadNew(app_can1_send, NULL, NULL);
	osThreadId_t osThIdCan2Send = osThreadNew(app_can2_send, NULL, NULL);
	appMainMsgQueueId = osMessageQueueNew(APP_MAIN_MSG_Q_LEN, sizeof(APP_MAIN_MSG_t), NULL);
	// Create timer callbacks
	periodicTimer = osTimerNew(periodicTimerCallback, osTimerPeriodic, NULL, NULL);
	osTimerStart(periodicTimer, 100);
	while (true)
	{
		osDelay(100);
	}
}

/**
 * @brief Message interface for other threads
 * Provide and interface to other threads for sending thread message to
 * this thread.
 * @param msgType Message type
 * @param msg 8 bytes message array
 * @return none
 */
void SendMessageToAppMain(APP_MAIN_MSG_t *msg)
{
	osMessageQueuePut(appMainMsgQueueId, msg, NULL, 0);
}

/**
 * @brief Periodiclly callback
 * Callback every 50ms, sends a timed message to the app_main
 * @param argument pointer to an argument from rtos
 * @return none
 */
void periodicTimerCallback(void *argument)
{
	HAL_IWDG_Refresh(&hiwdg);
}

/**
 * @brief Initalize app_main
 * Create thrade and set its priority.
 */
void InitAppMain(void)
{
	threadId = osThreadNew(app_main, NULL, NULL);
	osThreadSetPriority(threadId, osPriorityNormal1);
}

/**
 * @brief Load stored parameters.
 * Load stored parameters from outter flash setup network parameters as the data stored.
 */
void LoadStoredParametersAndSetupNetwork(void)
{
	ParameterFileStore_t* paramFile = InitParameterFileStore();
	netStatus status;
	if(paramFile->Read())
	{
		Parameters_t* parameters = paramFile->parameters;
		if(parameters->dhcpEnable) netDHCP_Enable(NET_IF_CLASS_ETH);
		else
		{
			status = netDHCP_Disable (NET_IF_CLASS_ETH);
			status = netIF_SetOption (NET_IF_CLASS_ETH, netIF_OptionIP4_Address, parameters->ipAddress, NET_ADDR_IP4_LEN);
			status = netIF_SetOption (NET_IF_CLASS_ETH, netIF_OptionIP4_SubnetMask, parameters->subnetMask, NET_ADDR_IP4_LEN);
		}
	}
    else
	{
		status = netDHCP_Disable (NET_IF_CLASS_ETH);
		uint8_t ip4_addr[NET_ADDR_IP4_LEN];
		netIP_aton(ETH0_IP4_ADDR, NET_ADDR_IP4, ip4_addr);
		status = netIF_SetOption (NET_IF_CLASS_ETH, netIF_OptionIP4_Address, ip4_addr, NET_ADDR_IP4_LEN);
		netIP_aton(ETH0_IP4_MASK, NET_ADDR_IP4, ip4_addr);
		status = netIF_SetOption (NET_IF_CLASS_ETH, netIF_OptionIP4_SubnetMask, ip4_addr, NET_ADDR_IP4_LEN);
	}
}
