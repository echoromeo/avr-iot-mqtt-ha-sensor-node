/*
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

/*
 The aim here is to set up a Home Assistant MQTT Light with MQTT Discovery 
 - https://www.home-assistant.io/docs/mqtt/discovery/
 - https://www.home-assistant.io/integrations/light.mqtt/#implementations
*/


#include <xc.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include "main.h"

#include "mcc_generated_files/config/IoT_Sensor_Node_config.h"
#include "mcc_generated_files/config/conf_winc.h"
#include "mcc_generated_files/config/mqtt_config.h"
#include "mcc_generated_files/config/cloud_config.h"

#include "mcc_generated_files/application_manager.h"
#include "mcc_generated_files/led.h"
#include "mcc_generated_files/debug_print.h"

#include "mcc_generated_files/sensors_handling.h"
#include "mcc_generated_files/credentials_storage/credentials_storage.h"
#include "mcc_generated_files/time_service.h"
#include "mcc_generated_files/cloud/cloud_service.h"
#include "mcc_generated_files/cloud/mqtt_service.h"

static char mqttSubscribeTopic[SUBSCRIBE_TOPIC_SIZE];
static char lightandtemperaturetopic[PUBLISH_TOPIC_SIZE];
static char json[PAYLOAD_SIZE];

static uint8_t holdCount = 0;

int main(void)
{
	application_init();

	while (1)
	{ 
		runScheduler();  
	}
   
	return 0;
}

// This will get called every CFG_SEND_INTERVAL while we have a valid Cloud connection
void sendToCloud(void)
{
	int rawTemperature = 0;
	int light = 0;
	int len = 0;

	if (shared_networking_params.haveAPConnection)
	{
		rawTemperature = SENSORS_getTempValue();
		light = SENSORS_getLightValue();
		len = sprintf(json,"{\"light\":%d,\"temp\":%d.%02d}", light,rawTemperature/100,abs(rawTemperature)%100);

		sprintf(lightandtemperaturetopic, "%s/sensor", eeprom->mqttCID); // Can optimize this a lot if never changing CID
		CLOUD_publishData((uint8_t*)lightandtemperaturetopic ,(uint8_t*)json, len);
		if (holdCount)
		{
			holdCount--;
		}
		else
		{
			ledParameterYellow.onTime = LED_BLIP;
			ledParameterYellow.offTime = LED_BLIP;
			LED_control(&ledParameterYellow);
		}
	}
}

void subscribeToCloud(void)
{
	sprintf(mqttSubscribeTopic, "%s/test", eeprom->mqttCID); // Can optimize this a lot if never changing CID
	CLOUD_registerSubscription((uint8_t*)mqttSubscribeTopic,receivedFromCloud);
}

//This handles messages published from the MQTT server when subscribed
void receivedFromCloud(uint8_t *topic, uint8_t *payload)
{
	// TODO: this is where to handle stuff..
	
	ledParameterYellow.onTime = SOLID_ON;
	ledParameterYellow.offTime = SOLID_OFF;
	LED_control(&ledParameterYellow);
	holdCount = 2;
	
	debug_printIoTAppMsg("topic: %s", topic);
	debug_printIoTAppMsg("payload: %s", payload);
}
