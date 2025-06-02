#pragma once
#include "timers.h"

#include <hwd.h>
#include <modules/light.h>
#include <modules/dht.h>
#include <modules/stepper.h>

ISR(TIMER2_COMPA_vect) {
    VALUE_T2++;
    if (COUNT_T2() % 1000 == 0) {
        setPortPinD(&HWD_PORT_B, HWD_PIN_5, !getPortPinD(&HWD_PORT_B, HWD_PIN_5));
    }

    {
        if (WAIT_DHT == 0) {
            READ_DHT();
        } else WAIT_DHT--;
    }

    {
        if (WAIT_STEPPER > 0) {
            WAIT_STEPPER--;
        } else {
            RUN_STEPPER();
            WAIT_STEPPER = 2;
        }
    }

    {
        RUN_LIGHT();
    }
}
