#pragma once
#define PIN_LIGHT       0
#include <modules/dht.h>
#include <modules/stepper.h>

volatile uint16_t VALUE_LIGHT;
volatile uint16_t WAS_LIGHT;

const volatile uint16_t *STRUCT_POINTERS_LIGHT[] = {&VALUE_LIGHT, &WAS_LIGHT};
const char* STRUCT_NAMES_LIGHT[] = {"brightness", "previous"};
const ModuleInfo STRUCT_MODULE_LIGHT = {
    "light",
    {STRUCT_NAMES_LIGHT},
    {STRUCT_POINTERS_LIGHT},
    sizeof(STRUCT_NAMES_LIGHT) / sizeof(const char*),
};

#define INIT_LIGHT() {                                                  \
    VALUE_LIGHT = 0;                                                    \
    WAS_LIGHT = 2;                                                      \
    ADMUX = (1 << REFS0); /* Initialize ADC for pin A0 */               \
    ADMUX &= 0xF0;                                                      \
    ADCSRA = (1 << ADEN)  /* Enable ADC           */                    \
           | (1 << ADIE)  /* Enable ADC Interrupt */                    \
           | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);                \
}

#define RUN_LIGHT()     beginReadPinA(PIN_LIGHT)
#define LIGHT_ISDAY()   (VALUE_LIGHT > THRESHOLD_LIGHT_STEPPER ? 1 : 0)

// Light sensor is the only device using analog pins, so we can use the ADC interrupt exclusively for it
ISR (ADC_vect) {
    VALUE_LIGHT = ADC;
    const uint8_t isDay = LIGHT_ISDAY() && TEMP_DHT > THRESHOLD_TEMP_STEPPER ? 1 : 0;
    if (DIRECTION_STEPPER == 1024 && isDay != WAS_LIGHT) {
        WAS_LIGHT = isDay;
        DIRECTION_STEPPER = isDay ? 2048 : 0;
    }
}