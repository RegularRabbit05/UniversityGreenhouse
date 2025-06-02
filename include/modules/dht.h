#pragma once

#include <modules/fan.h>

volatile uint16_t STATE_DHT;
volatile uint16_t BYTES_READ;
volatile uint16_t BITS_READ;
volatile uint16_t TEMP_DHT;
volatile uint16_t HUMID_DHT;
volatile uint16_t CHK_DHT;
volatile uint16_t WAIT_DHT;
volatile uint8_t BUFFER_DHT[5];

const volatile uint16_t *STRUCT_POINTERS_DHT[] = {&STATE_DHT, &BYTES_READ, &BITS_READ, &TEMP_DHT, &HUMID_DHT, &CHK_DHT, &WAIT_DHT};
const char* STRUCT_NAMES_DHT[] = {"state", "bytes", "bits", "temp", "hum", "csum", "wait"};
const ModuleInfo STRUCT_MODULE_DHT = {
    "dht",
    {STRUCT_NAMES_DHT},
    {STRUCT_POINTERS_DHT},
    sizeof(STRUCT_NAMES_DHT) / sizeof(const char*),
};

#define DHT_HUMID() HUMID_DHT

void RUN_DHT();
void READ_DHT();

#define INIT_DHT() {            \
    TIMSK1 = 0;                 \
    STATE_DHT = UINT16_MAX;     \
    TEMP_DHT = 0;               \
    HUMID_DHT = 0;              \
    CHK_DHT = 0;                \
    WAIT_DHT = 100;             \
    RUN_DHT();                  \
}

uint8_t saved_TIMSK0, saved_TIMSK2, saved_PCICR, saved_EIMSK;
void RUN_DHT() {
    switch (STATE_DHT) {
        case 0: {   //Save states and disable most interrupts
            saved_TIMSK0 = TIMSK0;
            saved_TIMSK2 = TIMSK2;
            saved_PCICR  = PCICR;
            saved_EIMSK  = EIMSK;

            TIMSK0 = 0;
            TIMSK2 = 0;
            PCICR  = 0;
            EIMSK  = 0;

            for (int i = 0; i < 5; i++) BUFFER_DHT[i] = 0;
            BYTES_READ = 0;
            BITS_READ = 0;
            CHK_DHT = 0;

            TCNT1 = 0;
            TCCR1A = 0;
            TCCR1B = 1 << WGM12;
            OCR1A = 4499;
            TIMSK1 = 1 << OCIE1A;
            TCCR1B |= 1 << CS11 | 1 << CS10;        // 18 millis delay

            STATE_DHT++;
            setPinMode(&HWD_PORT_D, HWD_PIN_2, HWD_PIN_OUT);
            setPortPinD(&HWD_PORT_D, HWD_PIN_2, HWD_PIN_LOW);
        } break;
        case 1: {   //Send handshake
            STATE_DHT++;

            TCCR1A = 0;
            TCCR1B = 1 << WGM12;
            OCR1A = 319;
            TIMSK1 = 1 << OCIE1A;
            TCCR1B |= 1 << CS10;                    // 20 micros delay
            setPortPinD(&HWD_PORT_D, HWD_PIN_2, HWD_PIN_LOW);
        } break;
        case 2: {   //Read ack handshake
            setPinMode(&HWD_PORT_D, HWD_PIN_2, HWD_PIN_IN);
            STATE_DHT+=2;
            while (readPortPinD(&HWD_PORT_D, HWD_PIN_2)) {}
            while (readPortPinD(&HWD_PORT_D, HWD_PIN_2) == 0) {}
            while (readPortPinD(&HWD_PORT_D, HWD_PIN_2)) {}
            while (readPortPinD(&HWD_PORT_D, HWD_PIN_2) == 0) {}
            TCCR1A = 0;
            TCCR1B = 1 << WGM12;
            OCR1A = 479;
            TIMSK1 = 1 << OCIE1A;
            TCCR1B |= 1 << CS10;                    // 30 micros delay
        } break;
        case 4: {   //Read 40 bits repeating this state 40 times
            if (readPortPinD(&HWD_PORT_D, HWD_PIN_2)) BUFFER_DHT[BYTES_READ] |= 1 << (7 - BITS_READ);
            while (readPortPinD(&HWD_PORT_D, HWD_PIN_2)) {}
            while (readPortPinD(&HWD_PORT_D, HWD_PIN_2) == 0) {}

            BITS_READ++;
            if (BITS_READ == 8) {
                BITS_READ = 0;
                BYTES_READ++;
            }
            if (BYTES_READ == 5) {
                STATE_DHT = 5;
            }

            TCCR1A = 0;
            TCCR1B = 1 << WGM12;
            OCR1A = 479;
            TIMSK1 = 1 << OCIE1A;
            TCCR1B |= 1 << CS10;                    // 30 micros delay
        } break;
        case 5: {   //Validate and perform calculations
            TIMSK0 = saved_TIMSK0;
            TIMSK2 = saved_TIMSK2;
            PCICR  = saved_PCICR;
            EIMSK  = saved_EIMSK;

            CHK_DHT = (uint8_t) (BUFFER_DHT[0] + BUFFER_DHT[1] + BUFFER_DHT[2] + BUFFER_DHT[3] & 0xFF);
            if (CHK_DHT == BUFFER_DHT[4]) {
                TEMP_DHT = BUFFER_DHT[2];
                HUMID_DHT = BUFFER_DHT[0];

                if (DHT_HUMID() < THRESHOLD_FAN) {  //Start fan if needed
                    int fan_speed = (THRESHOLD_FAN - DHT_HUMID()) * 2;
                    fan_speed = fan_speed * 255 / 100;
                    if (fan_speed < 0) fan_speed = 0;
                    if (fan_speed > 255) fan_speed = 255;
                    DDRD |= 1 << DDD5;                                  //PWM enable with fan_speed
                    TCCR0A |= 1 << COM0B1 | 1 << WGM00 | 1 << WGM01;
                    TCCR0B |= 1 << CS01;
                    OCR0B = fan_speed;
                } else {
                    TCCR0A &= ~(1 << COM0B1 | 1 << COM0B0);             //PWM disable
                    setPortPinD(&HWD_PORT_D, HWD_PIN_5, HWD_PIN_LOW);
                }
            }

            CHK_DHT = CHK_DHT << 8 | BUFFER_DHT[4];
        }
        default: {
            setPinMode(&HWD_PORT_D, HWD_PIN_2, HWD_PIN_OUT);
            setPortPinD(&HWD_PORT_D, HWD_PIN_2, HWD_PIN_HIGH);
            STATE_DHT = 0;
        }
    }
}

void READ_DHT() {   //Only read DHT every 5 seconds
    WAIT_DHT = 5000;
    if (STATE_DHT == 0) {
        RUN_DHT();
    }
}

ISR (TIMER1_COMPA_vect) {
    TCCR1B = 0;
    TIMSK1 = 0;
    RUN_DHT();
}