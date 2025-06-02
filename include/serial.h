#pragma once

#define HASSERIAL
#define HASCONSOLE

typedef struct {
    char* modName;
    char** varNames;
    volatile uint16_t** varPtrs;
    uint8_t varCount;
} ModuleInfo;

#ifdef HASSERIAL
#include <stdio.h>

#define BAUDRATE           115200L
#define SERIAL_FORMAT_8N1  0x06

#ifdef HASCONSOLE
#define OUT_BUF_SIZE 80
volatile uint8_t OUT_BUF_PTR;
volatile uint8_t OUT_BUF_PTR_INTERNAL;
volatile uint8_t OUT_BUF[OUT_BUF_SIZE];

#define IN_BUF_SIZE 64
volatile uint8_t IN_BUF_PTR;
volatile uint8_t IN_BUF_PTR_INTERNAL;
volatile uint8_t IN_BUF[IN_BUF_SIZE];
volatile uint8_t IN_BUF_REALIGN[IN_BUF_SIZE+1];
#define TMP_BUF_SIZE 16

#include <timers.h>
#include <modules/light.h>
#include <modules/fan.h>
#include <modules/dht.h>
#include <modules/stepper.h>
const ModuleInfo* CONSOLE_MODULES[] = {&STRUCT_MODULE_LIGHT, &STRUCT_MODULES_FAN, &STRUCT_MODULE_DHT, &STRUCT_MODULE_STEPPER, &STRUCT_MODULE_T2};
#endif

int PUT_UART(char c, FILE *_);
void CONSOLE_UART();
uint8_t STR_FROM_UART(uint8_t * dst);
static FILE OUTFILE_UART = FDEV_SETUP_STREAM(PUT_UART, NULL, _FDEV_SETUP_WRITE); // NOLINT(*-non-copyable-objects)

#define READY_UART()    (UCSR0A & 1 << UDRE0 ? 1 : 0)

#ifdef HASCONSOLE
#define INIT_CONSOLE() {                                   \
    OUT_BUF_PTR = 0;                                       \
    OUT_BUF_PTR_INTERNAL = 0;                              \
    IN_BUF_PTR = 0;                                        \
    IN_BUF_PTR_INTERNAL = 0;                               \
}
#else
#define INIT_CONSOLE() {}
#endif

#define INIT_UART() {                                       \
    INIT_CONSOLE()                                          \
    UBRR0H = 0;                                             \
    UBRR0L = (uint8_t) (F_CPU / 4 / BAUDRATE - 1) / 2;      \
    UCSR0A |= 1 << U2X0;                                    \
    UCSR0B |= 1 << RXEN0;                                   \
    UCSR0B |= 1 << TXEN0;                                   \
    UCSR0B |= 1 << RXCIE0;                                  \
    UCSR0C  = SERIAL_FORMAT_8N1;                            \
    stdout = &OUTFILE_UART;                                 \
}

#include "serial.c"
#else
#define INIT_UART() {}
#endif

