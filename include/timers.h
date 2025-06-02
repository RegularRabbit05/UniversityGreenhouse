#pragma once
#include <stdint.h>

volatile uint64_t VALUE_T2;

const volatile uint64_t *STRUCT_POINTERS_T2[] = {&VALUE_T2};
const char* STRUCT_NAMES_T2[] = {"value"};
const ModuleInfo STRUCT_MODULE_T2 = {
    "timer2",
    {STRUCT_NAMES_T2},
    {STRUCT_POINTERS_T2},
    sizeof(STRUCT_NAMES_T2) / sizeof(const char*)
};

#define PRESCALER_T2 64
#define OCR2A_VALUE_T2 ((uint64_t) F_CPU / ((uint64_t) PRESCALER_T2 * (uint64_t) 1000))
#define COUNT_T2() VALUE_T2

#define INIT_T2()                                                       \
{                                                                       \
    VALUE_T2 = 0;                       /* Firmware time at 0ms  */     \
    TCCR2A = 1 << WGM21;                /* Timer: Clear&Compare  */     \
    TCCR2B = 0;                                                         \
    OCR2A = (uint8_t) OCR2A_VALUE_T2;   /* Frequency: 1KHz       */     \
    TCCR2B |= 1 << CS22;                /* Prescaler: 64         */     \
    TIMSK2 |= 1 << OCIE2A;              /* Mode: Compare Match A */     \
}

#include "timers.c"