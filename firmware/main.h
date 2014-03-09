#ifndef MAIN_H
#define MAIN_H

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "cube.h"

void ioinit (void);

#define DATA_BUS        PORTB   // port to write the data for flipflops to
#define LAYER_SELECT    PORTD   // port to select the layer (1 bit set)

#define LATCH_ADDR      PORTF   // port to select the flipflop
#define LATCH_MASK      0xE0    // 11 10 00 00, thus PF7, PF6, PF5      
#define LATCH_MASK_INV  0x1F    // 00 01 11 11

#define OE_PORT         PORTF   // port to handle clock
#define OE_MASK         0x0F    // 00 01 00 00, thus PF4

#define LED_PORT        PORTF   // port for the leds
#define LED0            0x20    // 00 10 00 00
#define LED1            0x40    // 01 00 00 00
#define LED2            0x80    // 10 00 00 00

#define LED_RED		LED0
#define LED_GREEN	LED1
#define LED_YELLOW	LED2



#define BUTTON_PIN      PINF    // pin for the buttons
#define BUTTON0         0x01    // 00 00 00 01
#define BUTTON1         0x02    // 00 00 00 10
 
volatile unsigned char current_layer;

void ioinit (void);
void read_from_usb(void);

#endif

