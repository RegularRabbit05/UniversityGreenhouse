#include <hwd.h>
#include <serial.h>
#include <timers.h>

#include <modules/light.h>
#include <modules/fan.h>
#include <modules/dht.h>
#include <modules/stepper.h>

int main() {
    cli();
    INIT_UART();
    INIT_T2();

    INIT_LIGHT();
    INIT_FAN();
    INIT_STEPPER();
    INIT_DHT();

    setPinMode(&HWD_PORT_B, HWD_PIN_5, HWD_PIN_OUT);
    sei();
    printf("Welcome! Type \"help\" for help\n");

    while (1) { // Send the cpu into IDLE sleep so timers will run but power consumption will be lower than a while(1);
        _SLEEP_CONTROL_REG = _SLEEP_CONTROL_REG & ~(_BV(SM0) | _BV(SM1) | _BV(SM2)) | SLEEP_MODE_IDLE;
        _SLEEP_CONTROL_REG |= (uint8_t) _SLEEP_ENABLE_MASK;
        asm __volatile__ ("sleep");
        _SLEEP_CONTROL_REG &= (uint8_t) ~_SLEEP_ENABLE_MASK;
    }
    asm __volatile__("break");
    asm __volatile__("jmp 0");
}