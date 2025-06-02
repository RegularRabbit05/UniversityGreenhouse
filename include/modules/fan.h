#pragma once

volatile uint16_t THRESHOLD_FAN;

const volatile uint16_t *STRUCT_POINTERS_FAN[] = {&THRESHOLD_FAN};
const char* STRUCT_NAMES_FAN[] = {"threshold"};
const ModuleInfo STRUCT_MODULES_FAN = {
    "fan",
    {STRUCT_NAMES_FAN},
    {STRUCT_POINTERS_FAN},
    sizeof(STRUCT_NAMES_FAN) / sizeof(const char*)
};

#define INIT_FAN() {                                        \
    THRESHOLD_FAN = 50;                                     \
    setPinMode(&HWD_PORT_D, HWD_PIN_5, HWD_PIN_OUT);        \
    setPinMode(&HWD_PORT_D, HWD_PIN_4, HWD_PIN_OUT);        \
    setPortPinD(&HWD_PORT_D, HWD_PIN_4, HWD_PIN_LOW);       \
}

void SPEED_FAN() {

}