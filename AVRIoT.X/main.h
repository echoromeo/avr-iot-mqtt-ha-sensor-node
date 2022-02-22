/*
 * main.h
 */ 


#ifndef MAIN_H_
#define MAIN_H_

#include <avr/io.h>

void subscribeToCloud(void);
void receivedFromCloud(uint8_t *topic, uint8_t *payload);
void sendToCloud(void);

#endif /* MAIN_H_ */