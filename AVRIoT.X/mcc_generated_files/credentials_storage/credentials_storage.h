/*
    \file   credentials_storage.h

    \brief  Credential Storage header file.

    (c) 2018 Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip software and any
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party
    license terms applicable to your use of third party software (including open source software) that
    may accompany Microchip software.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS
    FOR A PARTICULAR PURPOSE.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS
    SOFTWARE.
*/


#ifndef CREDENTIALS_STORAGE_H
#define CREDENTIALS_STORAGE_H

#include <xc.h>
#include "../winc/m2m/m2m_types.h"

#define MQTT_HOST_IP 0
#define MQTT_HOST_DNS 1

#define MAX_NTP_SERVER_LENGTH	20
#define MAX_MQTT_CREDENTIALS_LENGTH (EEPROM_PAGE_SIZE/2)


typedef struct wifi_credentials
{
	char ssid[M2M_MAX_SSID_LEN];
	char pass[M2M_MAX_PSK_LEN];
	char authType[2];
} wifi_credentials_t;

typedef struct eeprom_data
{
	char mqttUser[MAX_MQTT_CREDENTIALS_LENGTH];
	char mqttPassword[MAX_MQTT_CREDENTIALS_LENGTH];
	char mqttCID[MAX_MQTT_CREDENTIALS_LENGTH];
	char mqttAddress[MAX_MQTT_CREDENTIALS_LENGTH];
	uint16_t mqttPort;
	uint8_t mqttAddressType;
	uint8_t reserved[(MAX_MQTT_CREDENTIALS_LENGTH*2)-3]; // Future eeprom page alignment
} eeprom_data_t;

extern wifi_credentials_t wifi;
extern eeprom_data_t *eeprom;
//extern userrow_data_t *userrow;
char *mqttCIDptr;

extern char ntpServerName[MAX_NTP_SERVER_LENGTH];

void CREDENTIALS_STORAGE_clearWifiCredentials(void);
void CREDENTIALS_STORAGE_writeMQTTCredentials(char *username, char *password,char *cid);
void CREDENTIALS_STORAGE_clearMQTTCredentials(void);
void CREDENTIALS_STORAGE_writeMQTTBroker(char *address, uint16_t port, uint8_t type);
void CREDENTIALS_STORAGE_clearMQTTBroker(void);
void CREDENTIALS_STORAGE_readNTPServerName(char *serverNameBuffer);
void CREDENTIALS_STORAGE_writeNTPServerName(char *serverNameBuffer);

#endif /* CREDENTIALS_STORAGE_H */