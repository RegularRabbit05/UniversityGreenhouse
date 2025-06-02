#pragma once
#ifdef HASSERIAL
#include "serial.h"

#include <stdlib.h>

int PUT_UART(const char c, FILE *_) {
#ifdef HASCONSOLE
    if (!READY_UART() || OUT_BUF_PTR_INTERNAL != OUT_BUF_PTR) { //If the queue is not empty
        OUT_BUF[OUT_BUF_PTR] = c;                               //Queue new item
        if (++OUT_BUF_PTR >= OUT_BUF_SIZE) OUT_BUF_PTR = 0;
        UCSR0B |= 1 << UDRIE0;
        return c;
    }
#endif
    UDR0 = (uint8_t) c;     //Nothing is in the queue, prin now
    UCSR0B |= 1 << UDRIE0;
    return c;
}

#ifdef HASCONSOLE
uint8_t STR_FROM_UART(uint8_t * dst) {
    uint8_t realigned_size = 0;
    while (IN_BUF_PTR != IN_BUF_PTR_INTERNAL) {
        dst[realigned_size++] = IN_BUF[IN_BUF_PTR];
        IN_BUF_PTR++;
        if (IN_BUF_PTR >= IN_BUF_SIZE) IN_BUF_PTR = 0;
    }
    dst[realigned_size] = 0;
    return realigned_size;
}

uint8_t streq(const char* a, const unsigned char* b) {
    while (*a && *b) {
        if (*a != *b) return 0;
        a++;
        b++;
    }
    if (*a == *b) return 1;
    return 0;
}

void strgetvarat(const unsigned char* str, unsigned char* buffer, const int sz, int var) {
    int i, ptr;
    for (ptr = 0; ptr < IN_BUF_SIZE && str[ptr] != 0 && var > 0; ptr++) {
        if (str[ptr] == ' ') var--;
    }
    if (var != 0) {
        buffer[0] = 0;
        return;
    }
    for (i = 0; i < sz - 1 && str[ptr] != ' ' && str[ptr] != 0; i++, ptr++) {
        buffer[i] = str[ptr];
    }
    buffer[i] = 0;
}

void CONSOLE_UART() {
    uint8_t *realigned = IN_BUF_REALIGN, buffer[16], argCount = 0;
    unsigned int i = 0;
    STR_FROM_UART(realigned);
    for (i = 0; i < IN_BUF_SIZE && realigned[i] != 0; i++) if (realigned[i] == ' ') argCount++;
    strgetvarat(realigned, buffer, TMP_BUF_SIZE, 0);
    if (streq("reset", buffer)) {
        printf("\n");
        asm __volatile__("jmp 0");
        return;
    }

    printf(">> %s\n", (char*) realigned);

    if (streq("help", buffer)) {
        printf("Commands: help, modlist, varlist, set, get, reset\n");
        return;
    }
    if (streq("modlist", buffer)) {
        printf("Modules:");
        for (i = 0; i < sizeof(CONSOLE_MODULES) / sizeof(ModuleInfo*); i++) {
            if (i != 0) printf(",");
            printf(" %s", CONSOLE_MODULES[i]->modName);
        }
        printf("\n");
        return;
    }
    if (streq("varlist", buffer)) {
        if (argCount != 1) {
            printf("Usage: varlist <module>\n");
            return;
        }
        strgetvarat(realigned, buffer, TMP_BUF_SIZE, 1);
        printf("Vars:");
        for (i = 0; i < sizeof(CONSOLE_MODULES) / sizeof(ModuleInfo*); i++) {
            if (streq(CONSOLE_MODULES[i]->modName, buffer)) {
                for (unsigned int j = 0; j < CONSOLE_MODULES[i]->varCount; j++) {
                    if (j != 0) printf(",");
                    printf(" %s", CONSOLE_MODULES[i]->varNames[j]);
                }
                break;
            }
        }
        printf("\n");
        return;
    }
    if (streq("set", buffer)) {
        if (argCount != 3) {
            printf("Usage: set <module> <variable> <value>\n");
            return;
        }
        strgetvarat(realigned, buffer, TMP_BUF_SIZE, 1);
        for (i = 0; i < sizeof(CONSOLE_MODULES) / sizeof(ModuleInfo*); i++) {
            if (streq(CONSOLE_MODULES[i]->modName, buffer)) {
                strgetvarat(realigned, buffer, TMP_BUF_SIZE, 2);
                for (unsigned int j = 0; j < CONSOLE_MODULES[i]->varCount; j++) {
                    if (streq(CONSOLE_MODULES[i]->varNames[j], buffer)) {
                        strgetvarat(realigned, buffer, TMP_BUF_SIZE, 3);
                        *CONSOLE_MODULES[i]->varPtrs[j] = (volatile uint16_t) atoi(buffer);
                        printf("Done!\n");
                        return;
                    }
                }
                printf("Unknown variable name: %s\n", (char *) buffer);
                return;
            }
        }
        printf("Unknown module name: %s\n", (char *) buffer);
        return;
    }
    if (streq("get", buffer)) {
        if (argCount != 2) {
            printf("Usage: set <module> <variable>\n");
            return;
        }
        strgetvarat(realigned, buffer, TMP_BUF_SIZE, 1);
        for (i = 0; i < sizeof(CONSOLE_MODULES) / sizeof(ModuleInfo*); i++) {
            if (streq(CONSOLE_MODULES[i]->modName, buffer)) {
                strgetvarat(realigned, buffer, TMP_BUF_SIZE, 2);
                for (unsigned int j = 0; j < CONSOLE_MODULES[i]->varCount; j++) {
                    if (streq(CONSOLE_MODULES[i]->varNames[j], buffer)) {
                        printf("%s = %u\n", CONSOLE_MODULES[i]->varNames[j], *CONSOLE_MODULES[i]->varPtrs[j]);
                        return;
                    }
                }
                printf("Unknown variable name: %s\n", (char *) buffer);
                return;
            }
        }
        printf("Unknown module name: %s\n", (char *) buffer);
        return;
    }
    printf("Unknown command: %s\n", (char *) buffer);
}

ISR (USART_UDRE_vect) {
    if (OUT_BUF_PTR_INTERNAL != OUT_BUF_PTR) {
        UDR0 = (uint8_t) OUT_BUF[OUT_BUF_PTR_INTERNAL++]; //Serial printer is ready, send next character from the buffer
        if (OUT_BUF_PTR_INTERNAL >= OUT_BUF_SIZE) OUT_BUF_PTR_INTERNAL = 0;
        if (OUT_BUF_PTR_INTERNAL == OUT_BUF_PTR) UCSR0B &= ~(1 << UDRIE0);
    } else {
        UCSR0B &= ~(1 << UDRIE0);
    }
}

ISR (USART_RX_vect) {
    const uint8_t ch = UDR0;
    if (ch == '\r') return;
    if (ch == '\n') {
        printf("\n"); //Command is ready to be processed
        if (IN_BUF_PTR == IN_BUF_PTR_INTERNAL) return;
        CONSOLE_UART();
        return;
    }
    IN_BUF[IN_BUF_PTR_INTERNAL++] = ch;
    if (IN_BUF_PTR_INTERNAL >= IN_BUF_SIZE) IN_BUF_PTR_INTERNAL = 0;
    printf("%c", ch);
}
#endif
#endif
