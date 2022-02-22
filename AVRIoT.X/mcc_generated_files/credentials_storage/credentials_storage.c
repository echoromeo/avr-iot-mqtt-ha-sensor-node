/*
    \file   credentials_storage.c

    \brief  Credential Storage source file.

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

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/eeprom.h>
#include "credentials_storage.h"
#include "../config/mqtt_config.h"

/* Last used wifi credentials are stored in the WINC? */
wifi_credentials_t wifi;

/* ntp server name.. */
char ntpServerName[MAX_NTP_SERVER_LENGTH];

/* Last used mqtt credentials are stored in EEPROM:
 * - Pointer for use in the application
 * - The initial values to put in eeprom for the elf file
 *   Remember to uncheck perserve EEPROM in Tool properties
 */
eeprom_data_t *eeprom = (eeprom_data_t *) EEPROM_START;
eeprom_data_t eeprom_init __attribute((__used__, __section__(".eeprom"))) = {
	.mqttUser = CFG_MQTT_USERNAME,
	.mqttPassword = CFG_MQTT_PASSWORD,
	.mqttCID = "",
	.mqttAddress = CFG_MQTT_HOSTURL,
	.mqttPort = CFG_MQTT_PORT,
	.mqttAddressType = MQTT_HOST_IP,
};

/* Function to check if the NVM can be updated */
static inline bool eeprom_userrow_write_busy(bool wait)
{
	// You did something wrong..
	if (NVMCTRL.STATUS & NVMCTRL_WRERROR_bm) {
		return true;
	}

	// Do you want to wait until ready or keep polling?	
	if (wait) {
		while(NVMCTRL.STATUS & (NVMCTRL_EEBUSY_bm | NVMCTRL_FBUSY_bm));
	} else if (NVMCTRL.STATUS) {
		return true;
	}
	
	// We are ready to write!
	return false;
}

/* Function to write changes to eeprom or user row page buffer
 * Make sure to only change the content of one page at the time 
 *
 * Only the bytes updated in the page buffer will be written or
 * erased in the EEPROM. (Is this true for userrow as well?)
 */
static inline bool eeprom_userrow_write_buffer(bool blocking)
{
	// NVM is busy or you messed up..
	if(eeprom_userrow_write_busy(false)) {
		return true;
	}
	
	// The actual write command
	_PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, NVMCTRL_CMD_PAGEERASEWRITE_gc);
	
	//Do you want to wait until finished?
	if(blocking) {
		while(NVMCTRL.STATUS & NVMCTRL_EEBUSY_bm);
	}

	// We are done!
	return false;
}


void CREDENTIALS_STORAGE_clearWifiCredentials(void)
{
	memset(&wifi, 0, sizeof(wifi));
}

void CREDENTIALS_STORAGE_writeMQTTCredentials(char *username, char *password, char *cid)
{
	eeprom_userrow_write_busy(true);

	if (username != NULL) {
		snprintf(eeprom->mqttUser, sizeof(eeprom->mqttUser), username);
		eeprom_userrow_write_buffer(true);
	}
	
	if (password != NULL) {
		snprintf(eeprom->mqttPassword, sizeof(eeprom->mqttPassword), password);
		eeprom_userrow_write_buffer(true);
	}
	
	if (cid != NULL) {
		snprintf(eeprom->mqttCID, sizeof(eeprom->mqttPassword), cid);
	} else {
		memset(eeprom->mqttCID, 0, sizeof(eeprom->mqttCID));
	}
	eeprom_userrow_write_buffer(false);
}

void CREDENTIALS_STORAGE_clearMQTTCredentials(void)
{
	eeprom_userrow_write_busy(true);

	memset(eeprom->mqttUser, 0, sizeof(eeprom->mqttUser));
	eeprom_userrow_write_buffer(true);

	memset(eeprom->mqttPassword, 0, sizeof(eeprom->mqttPassword));
	eeprom_userrow_write_buffer(true);

	memset(eeprom->mqttCID, 0, sizeof(eeprom->mqttCID));
	eeprom_userrow_write_buffer(false);
}

void CREDENTIALS_STORAGE_writeMQTTBroker(char *address, uint16_t port, uint8_t type)
{
	eeprom_userrow_write_busy(true);

	snprintf(eeprom->mqttAddress, sizeof(eeprom->mqttAddress), address);
	eeprom_userrow_write_buffer(true);
	
	eeprom->mqttPort = port;
	eeprom->mqttAddressType = type;
	eeprom_userrow_write_buffer(false);
}

void CREDENTIALS_STORAGE_clearMQTTBroker(void) {
	eeprom_userrow_write_busy(true);

	memset(eeprom->mqttAddress, 0, sizeof(eeprom->mqttAddress));
	eeprom_userrow_write_buffer(true);
	
	memset(&eeprom->mqttPort, 0, MAX_MQTT_CREDENTIALS_LENGTH-sizeof(eeprom->mqttAddress));
	eeprom_userrow_write_buffer(false);
}



