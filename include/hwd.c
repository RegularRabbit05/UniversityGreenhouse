#pragma once
#include "hwd.h"

// Sets the pin data direction
void setPinMode(volatile uint8_t *port, const uint8_t pin, const uint8_t mode) {
    if (port == &HWD_PORT_B) port = &DDRB;          // Translate from PORTx definition to DDRx definition
    else if (port == &HWD_PORT_C) port = &DDRC;     // (only needed to set the correct direction)
    else if (port == &HWD_PORT_D) port = &DDRD;
    else return;

    if (mode == HWD_PIN_IN) *port &= ~HWD_SHIFT(pin); else *port |= HWD_SHIFT(pin);
}

// Sets the pin to high or low (digital)
void setPortPinD(volatile uint8_t *port, const uint8_t pin, const uint8_t mode) {
    if (mode == HWD_PIN_LOW) *port &= ~HWD_SHIFT(pin); else *port |= HWD_SHIFT(pin);
}

// Gets the pin state from output pins (digital)
uint8_t getPortPinD(const volatile uint8_t *port, const uint8_t pin) {
    return (*port & HWD_SHIFT(pin)) != 0 ? 1: 0;
}

// Reads the pin state for external input pins (digital)
uint8_t readPortPinD(const volatile uint8_t *port, const uint8_t pin) {
    if (port == &HWD_PORT_B) port = &PINB;          // Translate from PORTx definition to PINx definition
    else if (port == &HWD_PORT_C) port = &PINC;     // (only needed when reading in input mode)
    else if (port == &HWD_PORT_D) port = &PIND;
    else return 0;

    return (*port & HWD_SHIFT(pin)) != 0 ? 1 : 0;
}

// Reads the pin state for external input pins (analog)
uint16_t readPinA(const uint8_t pin) {
    ADMUX = (ADMUX & 0xF0) | (pin & 0x0F);
    ADCSRA |= 1 << ADSC;
    while (ADCSRA & 1 << ADSC) asm __volatile("nop");
    return ADC;
}

// Reads the pin state for external input pins (analog) but the read will be delayed and the interrupt will fetch it
void beginReadPinA(const uint8_t pin) {
    ADMUX = (ADMUX & 0xF0) | (pin & 0x0F);
    ADCSRA |= 1 << ADSC;
}