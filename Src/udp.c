#include <string.h>
#include "rl_net.h"                     // Keil.MDK-Plus::Network:CORE
#include "udp.h"
#include "parameter_file_store.h"

/* --------------- Const value define ---------------*/
// Network parameters

/* ------------------- static variables ------------ */
static NET_ADDR lastReceivedAddr;
static int udpSocket;
static bool udpConnected = false;

/* ------------------- static functions ------------ */
static unsigned int UdpCallback(int socket, const NET_ADDR *addr, const uint8_t* buf, uint32_t len);

void InitUdp(void)
{
	// Get UDP socket, register listen call back function, listen to the port store in the flash
	ParameterFileStore_t* paramFile = InitParameterFileStore();
	udpSocket = netUDP_GetSocket(UdpCallback);
	netUDP_Open(udpSocket, paramFile->parameters->udpPort);
	memset(&lastReceivedAddr, 0, sizeof(NET_ADDR));
}

unsigned int UdpCallback(int socket, const NET_ADDR *addr, const uint8_t* buf, uint32_t len)
{
	return 0;
}

/**
 * @brief Send out UDP package
 * @param buff pointer to data buff
 * @param size length of data need to be sent
 * @return false - failed true - success
 */
bool UdpSendData(const uint8_t* buff, uint32_t size)
{
	if(!udpConnected) return false;
	if(udpSocket <= 0) return false;
	
	uint8_t* sendBuff = NULL;
	sendBuff = netUDP_GetBuffer(size);
	if(sendBuff == NULL) return false;
	
	memcpy(sendBuff, buff, size);
	netStatus nSt = netUDP_Send(udpSocket, &lastReceivedAddr, sendBuff, size);
	if(nSt == netOK) return true;
	else return false;
}
