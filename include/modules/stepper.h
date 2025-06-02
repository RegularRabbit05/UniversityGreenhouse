#pragma once

uint16_t DIRECTION_STEPPER;
uint16_t WAIT_STEPPER;
uint16_t THRESHOLD_TEMP_STEPPER;
uint16_t THRESHOLD_LIGHT_STEPPER;

const volatile uint16_t *STRUCT_POINTERS_STEPPER[] = {&DIRECTION_STEPPER, &WAIT_STEPPER, &THRESHOLD_TEMP_STEPPER, &THRESHOLD_LIGHT_STEPPER};
const char* STRUCT_NAMES_STEPPER[] = {"direction", "wait", "tthreshold", "lthreshold"};
const ModuleInfo STRUCT_MODULE_STEPPER = {
    "stepper",
    {STRUCT_NAMES_STEPPER},
    {STRUCT_POINTERS_STEPPER},
    sizeof(STRUCT_NAMES_STEPPER) / sizeof(const char*),
};

#define BASE_PIN_STEPPER HWD_PIN_1
#define BASE_PORT_STEPPER HWD_PORT_B
const uint8_t STATES_STEPPER[4][4] = {
    {HWD_PIN_HIGH, HWD_PIN_LOW, HWD_PIN_LOW, HWD_PIN_LOW},
    {HWD_PIN_LOW, HWD_PIN_HIGH, HWD_PIN_LOW, HWD_PIN_LOW},
    {HWD_PIN_LOW, HWD_PIN_LOW, HWD_PIN_HIGH, HWD_PIN_LOW},
    {HWD_PIN_LOW, HWD_PIN_LOW, HWD_PIN_LOW, HWD_PIN_HIGH},
};

#define INIT_STEPPER() {                                    \
    DIRECTION_STEPPER = 1024;                               \
    WAIT_STEPPER = 0;                                       \
    THRESHOLD_TEMP_STEPPER = 10;                            \
    THRESHOLD_LIGHT_STEPPER = 700;                          \
    setPinMode(&HWD_PORT_B, HWD_PIN_2, HWD_PIN_OUT);        \
    setPinMode(&HWD_PORT_B, HWD_PIN_3, HWD_PIN_OUT);        \
    setPinMode(&HWD_PORT_B, HWD_PIN_4, HWD_PIN_OUT);        \
    setPinMode(&HWD_PORT_B, HWD_PIN_5, HWD_PIN_OUT);        \
}

void RUN_STEPPER() {
    if (DIRECTION_STEPPER == 1024) {
        for (int i = 0; i < 4; i++) {
            setPortPinD(&BASE_PORT_STEPPER, BASE_PIN_STEPPER + i, HWD_PIN_LOW);
        }
        return;
    }

    for (int i = 0; i < 4; i++) {
        setPortPinD(&BASE_PORT_STEPPER, BASE_PIN_STEPPER + i, STATES_STEPPER[DIRECTION_STEPPER % 4][i]);
    }
    if (DIRECTION_STEPPER > 1024) {
        DIRECTION_STEPPER--;
    } else {
        DIRECTION_STEPPER++;
    }
}