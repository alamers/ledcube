/*
 * Code to control an 8x8x8 ledcube using avr
 * http://www.instructables.com/id/Led-Cube-8x8x8/
 * See license.txt for license.
 */

#include "main.h"
#include "effect.h"
#include "launch_effect.h"
#include "draw.h"
#include "usb_rawhid.h"
#include "analog.h"

void launch_test_effect();

int main (void)
{
	default_programming();
}

void default_programming() {
	ioinit();
	current_layer = 0;	
	sei();

	usb_init();

	while (!usb_configured()) {
		launch_effect(rand()%EFFECTS_TOTAL);
	}
	read_from_usb();
}

uint8_t usb_buffer[64];
void read_from_usb() {
        for(;;) {
                int8_t r = usb_rawhid_recv(usb_buffer, 0);
                if (r>0) {
			for (int z=0; z<8; z++) {
				for (int y=0; y<8; y++) {
					fb[z][y] = usb_buffer[z*8 + y];
				}
			}
                        fb2cube();
                }
        }
}


/*
 * Multiplexer/framebuffer routine
 * This function is called by an interrupt generated by timer 2.
 * Every time it runs, it does the following:
 * 1) Disable the output for the multiplexer array.
 * 2) Turn of all layers.
 * 3) Load the current layer from the cube buffer onto the
 *    multiplexer array.
 * 4) Enable output from the multiplexer array.
 * 5) Turn on the current layer.
 * 6) Increment the current_layer variable, so the next layer is
 *    drawn the next time this function runs.
*/

ISR(TIMER0_COMPA_vect)
{
	// why not loop over current_layer here as well? too much for this interrupt maybe?

	LAYER_SELECT = 0x00; // Turn all cathode layers off. (all transistors off)
	OE_PORT |= OE_MASK; // Set OE high, disabling all outputs on latch array

	// y coordinates are mixed up so correct here by unfolding the y-loop

	DATA_BUS = cube[current_layer][0];
	unsigned char flipflop_addr = (0) << 5;
        LATCH_ADDR = (LATCH_ADDR & LATCH_MASK_INV) | (LATCH_MASK & flipflop_addr);

	DATA_BUS = cube[current_layer][7];
	flipflop_addr = (4) << 5;
        LATCH_ADDR = (LATCH_ADDR & LATCH_MASK_INV) | (LATCH_MASK & flipflop_addr);

	DATA_BUS = cube[current_layer][6];
	flipflop_addr = (1) << 5;
        LATCH_ADDR = (LATCH_ADDR & LATCH_MASK_INV) | (LATCH_MASK & flipflop_addr);

	DATA_BUS = cube[current_layer][5];
	flipflop_addr = (5) << 5;
        LATCH_ADDR = (LATCH_ADDR & LATCH_MASK_INV) | (LATCH_MASK & flipflop_addr);

	DATA_BUS = cube[current_layer][4];
	flipflop_addr = (2) << 5;
        LATCH_ADDR = (LATCH_ADDR & LATCH_MASK_INV) | (LATCH_MASK & flipflop_addr);

	DATA_BUS = cube[current_layer][3];
	flipflop_addr = (6) << 5;
        LATCH_ADDR = (LATCH_ADDR & LATCH_MASK_INV) | (LATCH_MASK & flipflop_addr);

	DATA_BUS = cube[current_layer][2];
	flipflop_addr = (3) << 5;
        LATCH_ADDR = (LATCH_ADDR & LATCH_MASK_INV) | (LATCH_MASK & flipflop_addr);

	DATA_BUS = cube[current_layer][1];
	flipflop_addr = (7) << 5;
        LATCH_ADDR = (LATCH_ADDR & LATCH_MASK_INV) | (LATCH_MASK & flipflop_addr);

	OE_PORT &= ~OE_MASK; // Set OE low, enabling outputs on the latch array

	LAYER_SELECT = (0x01 << current_layer); // Transistor ON for current layer

	// Increment the curren_layer counter so that the next layer is
	// drawn the next time this function runs.
	current_layer++;
	// We want to count from 0-7, so set it to 0 when it reaches 8.
	if (current_layer == 8)
		current_layer = 0;
}


void ioinit (void)
{

        DDRB = 0xff;    // DATA bus output,     1111 1111
        DDRD = 0xff;    // Layer select output
        DDRF = 0xfc;    // Buttons 0/1 on PF0/PF1, LED's 0-2 on PF5-7,  1111 1100
        
        DATA_BUS = 0x00;        // Set data bus off
        LAYER_SELECT = 0x00;    // Set layer select off
        LATCH_ADDR = 0x00;      // Set latch off

        // use timer 0 for drawing led cube
        // The Timer/Counter (TCNT0) and Output Compare Registers (OCR0A and OCR0B) are 8-bit registers. 
	// Interrupt request (abbreviated to Int.Req. in the figure) signals are all visible in the Timer Interrupt Flag Register (TIFR0).

        TCNT0 = 0x00; // initial counter value

	TCCR0A =  0x2; 		// 00 00 -- 10 disable OC0A, set CTC
	TCCR0B = 0x4;		// 00 -- 01 00 select ctc, clk/256

        TIMSK0 |= (1<<1); // enable timer/counter0
        OCR0A = 10; // post scaler
}



