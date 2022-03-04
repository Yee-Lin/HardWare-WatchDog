/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network:Service
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    HTTP_Server_CGI.c
 * Purpose: HTTP Server CGI Module
 * Rev.:    V7.0.0
 *----------------------------------------------------------------------------*/
//! [code_HTTP_Server_CGI]
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "rl_net.h"
#include "main.h"
#include "update_file_store.h"
#include "parameter_file_store.h"
#include "mem_pool.h"
#include "app_main.h"

/* ---------- Static variables ---------------- */
/**
 * @brief Update program flag.
 * It is used to notify IAP to update program.
 */
static UpdateFileStore_t *updateFile;
static uint32_t updateFileLen;
static bool upgradeFileUploaded = false;
static bool upgradeFileCrced = false;

// \brief Process query string received by GET request.
// \param[in]     qstr          pointer to the query string.
// \return        none.
void netCGI_ProcessQuery(const char *qstr)
{
    char var[40];

    do
    {
        // Loop through all the parameters
        qstr = netCGI_GetEnvVar(qstr, var, sizeof(var));
        // Check return string, 'qstr' now points to the next parameter
        if (var[0] != 0)
        {
            // First character is non-null, string exists

            /* Example of a parameter "ip=192.168.0.100"
            if (strncmp (var, "ip=", 3) == 0) {
              uint8_t ip_addr[NET_ADDR_IP4_LEN];
              // Read parameter IP address submitted by the browser
              netIP_aton (&var[3], NET_ADDR_IP4, ip_addr);
              ...
            }
            */
        }
    } while (qstr);
}

// \brief Process data received by POST request.
// \param[in]     code          callback context:
//                              - 0 = www-url-encoded form data,
//                              - 1 = filename for file upload (null-terminated string),
//                              - 2 = file upload raw data,
//                              - 3 = end of file upload (file close requested),
//                              - 4 = any other type of POST data (single or last stream),
//                              - 5 = the same as 4, but with more data to follow.
// \param[in]     data          pointer to POST data.
// \param[in]     len           length of POST data.
// \return        none.
void netCGI_ProcessData(uint8_t code, const char *data, uint32_t len)
{
    char var[40], str[4];
    uint8_t ip_addr[8];
    ParameterFileStore_t *paramFile = InitParameterFileStore();
    Parameters_t *parameters = paramFile->parameters;

    switch (code)
    {
    case 0:
        // Url encoded form data received
        do
        {
            // Parse all parameters
            data = netCGI_GetEnvVar(data, var, sizeof(var));
            if (var[0] != 0)
            {
                if (strncmp(var, "dhcp=o", 6) == 0)
                {
                    if (var[6] == 'f')
                    {
                        parameters->dhcpEnable = false;
                        netDHCP_Disable(NET_IF_CLASS_ETH);
                    }
                    else if (var[6] == 'n')
                    {
                        parameters->dhcpEnable = true;
                        netDHCP_Enable(NET_IF_CLASS_ETH);
                    }
                }
                else if (strncmp(var, "ipad=", 5) == 0)
                {
                    netIP_aton(&var[5], NET_ADDR_IP4, ip_addr);
                    memcpy(parameters->ipAddress, ip_addr, NET_ADDR_IP4_LEN);
                    netIF_SetOption(NET_IF_CLASS_ETH, netIF_OptionIP4_Address, ip_addr, NET_ADDR_IP4_LEN);
                }
                else if (strncmp(var, "mask=", 5) == 0)
                {
                    netIP_aton(&var[5], NET_ADDR_IP4, ip_addr);
                    memcpy(parameters->subnetMask, ip_addr, NET_ADDR_IP4_LEN);
                    netIF_SetOption(NET_IF_CLASS_ETH, netIF_OptionIP4_SubnetMask, ip_addr, NET_ADDR_IP4_LEN);
                }
                else if (strncmp(var, "port=", 5) == 0)
                {
                    char str[5];
                    memcpy(str, &var[5], sizeof(str));
                    parameters->udpPort = atoi(str);
                    paramFile->Write();
                }
                else if (strncmp(var, "impact_", 7) == 0)
                {
                    switch (var[7])
                    {
                    case 'e':
                        if (var[10] == 'f')
                            parameters->impactProtection = false;
                        else if (var[10] == 'n')
                            parameters->impactProtection = true;
                        break;

                    case 'd':
                        memcpy(str, &var[9], sizeof(str));
                        parameters->impactDistance = atoi(str);
                        break;

                    case 'g':
                        if (var[10] == 'f')
                            parameters->sideImpactProtection = false;
                        else if (var[10] == 'n')
                            parameters->sideImpactProtection = true;
                        break;

                    case 'f':
                        memcpy(str, &var[9], sizeof(str));
                        parameters->sideImpactDistance = atoi(str);
                        break;

                    case 't':
                        memcpy(str, &var[9], sizeof(str));
                        parameters->impactThreshold = atoi(str);
                        break;

                    default:
                        break;
                    }
                }
                else if (strncmp(var, "sensor_", 7) == 0)
                {
                    char str[4];
                    memcpy(str, &var[9], sizeof(str));
                    int8_t sensorId = var[7] - '0';
                    if (sensorId >= 0 && sensorId < ULTRA_SONIC_SENSOR_NUM)
                        parameters->sonicSensorMap[sensorId] = atoi(str);
                    if (sensorId == ULTRA_SONIC_SENSOR_NUM - 1)
                        paramFile->Write();
                }
                else if (strncmp(var, "upgrade=", 8) == 0)
                {
                    upgradeFileCrced = false;
                    upgradeFileUploaded = false;
                    APP_MAIN_MSG_t msg;
                    msg.msgType = MSG_TYPE_UPGRADE;
                    SendMessageToAppMain(&msg);
                }
            }
        } while (data);
        break;

    case 1:
    // Filename for file upload received
		/* Example
		if (data[0] != 0) {
			// Open a file for writing
			file = fopen (var, "w");
		}
		*/
        updateFile = InitUpdateFileStore();
        updateFileLen = 0;
        upgradeFileCrced = false;
        upgradeFileUploaded = false;
        break;

    case 2:
    // File content data received
        /* Example
		if (file != NULL) {
			// Write data to a file
			fwrite (data, 1, len, file);
		}
		*/
        // Write received data to outer flash
        updateFile->Write((uint8_t *)data, len);
        updateFileLen += len;
        break;

    case 3:
    // File upload finished
        /* Example
		if (file != NULL) {
			// Close a file
			fclose (file);
		}
		*/
        {
            // Get file length at the start of received file.
            uint32_t receivedFileLen;
            updateFile->SetReadPos(0);
            updateFile->Read((uint8_t *)&receivedFileLen, sizeof(receivedFileLen));
            // Check if length of recevied equals the length described at file start.
            upgradeFileUploaded = true;
            upgradeFileCrced = false;
            if (receivedFileLen != updateFileLen)
                break;
            upgradeFileUploaded = true;
            // Caculate CRC, check if file corrupt
            uint32_t crc = updateFile->GetCRC();
            if (!crc)
                upgradeFileCrced = true;
        }
        break;

    case 4:
    // Other content type data, last packet
		/* Example
		if (strcmp (netHTTPs_GetContentType(), "text/xml; charset=utf-8") == 0) {
        // Content type xml, utf-8 encoding
         ...
		}
		*/
        break;

    case 5:
        // Other content type data, more to follow
        // ... Process data
        break;

    default:
        // Ignore all other codes
        break;
    }
}

// \brief Generate dynamic web data based on a CGI script.
// \param[in]     env           environment string.
// \param[out]    buf           output data buffer.
// \param[in]     buf_len       size of output buffer (from 536 to 1440 bytes).
// \param[in,out] pcgi          pointer to a session's local buffer of 4 bytes.
//                              - 1st call = cleared to 0,
//                              - 2nd call = not altered by the system,
//                              - 3rd call = not altered by the system, etc.
// \return        number of bytes written to output buffer.
//                - return len | (1U<<31) = repeat flag, the system calls this function
//                                          again for the same script line.
//                - return len | (1U<<30) = force transmit flag, the system transmits
//                                          current packet immediately.
uint32_t netCGI_Script(const char *env, char *buf, uint32_t buf_len, uint32_t *pcgi)
{
    uint32_t len = 0;

    // Analyze a 'c' script line starting position 2
    // Example script
    // c a i <td><input type=text name=ip value="%s" size=18 maxlength=18></td>
    char ip_ascii[20];
    uint8_t ip_addr[8];
    ParameterFileStore_t *paramFile = InitParameterFileStore();
    Parameters_t *parameters = paramFile->parameters;

    switch (env[0])
    {
    case 'n':
        switch (env[2])
        {
        case 'd': // DHCP Enable
            len = sprintf(buf, &env[4], (parameters->dhcpEnable) ? "" : "checked", (parameters->dhcpEnable) ? "checked" : "");
            break;

        case 'i':
            netIF_GetOption(NET_IF_CLASS_ETH, netIF_OptionIP4_Address, ip_addr, sizeof(ip_addr));
            netIP_ntoa(NET_ADDR_IP4, ip_addr, ip_ascii, sizeof(ip_ascii));
            len = sprintf(buf, &env[4], ip_ascii);
            break;

        case 'm':
            netIF_GetOption(NET_IF_CLASS_ETH, netIF_OptionIP4_SubnetMask, ip_addr, sizeof(ip_addr));
            netIP_ntoa(NET_ADDR_IP4, ip_addr, ip_ascii, sizeof(ip_ascii));
            len = sprintf(buf, &env[4], ip_ascii);
            break;

        case 'p':
            len = sprintf(buf, &env[4], parameters->udpPort);
            break;

        default:
            break;
        }
        break;

    case 'p':
        switch (env[2])
        {
        case 'e':
            len = sprintf(buf, &env[4], (parameters->impactProtection) ? "on" : "off", (parameters->impactProtection) ? "checked" : "");
            break;

        case 'd':
            len = sprintf(buf, &env[4], parameters->impactDistance);
            break;

        case 'g':
            len = sprintf(buf, &env[4], (parameters->sideImpactProtection) ? "" : "checked", (parameters->sideImpactProtection) ? "checked" : "");
            break;

        case 'f':
            len = sprintf(buf, &env[4], parameters->sideImpactDistance);
            break;

        case 't':
            len = sprintf(buf, &env[4], parameters->impactThreshold);
            break;

        case '0':
            len = sprintf(buf, &env[4], parameters->sonicSensorMap[0]);
            break;

        case '1':
            len = sprintf(buf, &env[4], parameters->sonicSensorMap[1]);
            break;

        case '2':
            len = sprintf(buf, &env[4], parameters->sonicSensorMap[2]);
            break;

        case '3':
            len = sprintf(buf, &env[4], parameters->sonicSensorMap[3]);
            break;

        case '4':
            len = sprintf(buf, &env[4], parameters->sonicSensorMap[4]);
            break;

        case '5':
            len = sprintf(buf, &env[4], parameters->sonicSensorMap[5]);
            break;

        case '6':
            len = sprintf(buf, &env[4], parameters->sonicSensorMap[6]);
            break;

        case '7':
            len = sprintf(buf, &env[4], parameters->sonicSensorMap[7]);
            break;

        default:
            break;
        }
        break;

    case 'u':
        switch (env[2])
        {
        case 'l':
            if (upgradeFileUploaded)
                len = sprintf(buf, &env[4], (upgradeFileCrced) ? "Upgrade file uploaded and CRC correct." : "CRC failed!");
            else
                len = sprintf(buf, &env[4], " ");
            break;

        case 'g':
            len = sprintf(buf, &env[4], (upgradeFileCrced && upgradeFileUploaded) ? "visible" : "hidden");
            break;

        default:
            break;
        }
        break;

    case 'v':
        len = sprintf(buf, &env[2], FIRMWARE_VERSION);

    default:
        break;
    }
    return (len);
}

// \brief Override default Content-Type for CGX script files.
// \return        pointer to user defined Content-Type.
const char *netCGX_ContentType(void)
{
    /* Example
  return ("text/xml; charset=utf-8");
  */
    return ("text/xml; charset=utf-8");
}

// \brief Override default character encoding in html documents.
// \return        pointer to user defined character set type.
const char *netCGI_Charset(void)
{
    /* Example
  return ("utf-8");
  */
    return ("utf-8");
}
//! [code_HTTP_Server_CGI]
