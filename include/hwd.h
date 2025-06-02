#pragma once
#include <avr/interrupt.h>
#include <avr/sleep.h>

#define HWD_PIN_0       0
#define HWD_PIN_1       1
#define HWD_PIN_2       2
#define HWD_PIN_3       3
#define HWD_PIN_4       4
#define HWD_PIN_5       5
#define HWD_PIN_6       6
#define HWD_PIN_7       7

#define HWD_PORT_B      PORTB
#define HWD_PORT_C      PORTC
#define HWD_PORT_D      PORTD

#define HWD_PIN_IN      0
#define HWD_PIN_OUT     1

#define HWD_PIN_HIGH    1
#define HWD_PIN_LOW     0

#define HWD_SHIFT(t)    (1 << t)

void setPinMode(volatile uint8_t *port, uint8_t pin, uint8_t mode);
void setPortPinD(volatile uint8_t *port, uint8_t pin, uint8_t mode);
uint8_t getPortPinD(const volatile uint8_t *port, uint8_t pin);
uint8_t readPortPinD(const volatile uint8_t *port, uint8_t pin);
uint16_t readPinA(uint8_t pin);

#include "hwd.c"