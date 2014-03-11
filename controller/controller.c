#include <stdio.h>
#include <stdlib.h>

#include "controller.h"
#include "misc.h"
#include "hid.h"
#include "font.h"
#include "cursor.h"
#include "cube.h"
#include "snake.h"

void text(int device_nr);
void cursor(int device_nr);
void print_main_menu();

int main() {
	char c;
	int device_nr = 0;

	int opened = rawhid_open(1, 0x16C0, 0x0480, 0xFFAB, 0x0200);
	if (opened <= 0) {
		printf("no rawhid device found\n");
		return -1;
	}
	printf("found rawhid device %d\n", device_nr);

	print_main_menu();

	while (1) {
		// check if any Raw HID packet has arrived
		read_from_cube(device_nr);

		// check if any input on stdin
		while ((c = get_keystroke())) {
			if (c == TEXT) {
				text(device_nr);
				print_main_menu();
			} else if (c == SNAKE) {
				snake(device_nr);
				print_main_menu();
			} else if (c == CURSOR) {
				cursor(device_nr);
				print_main_menu();
			} else if (c == ESCAPE) {
				return 0;
			}
		}
	}
}

void print_main_menu() {
	printf("\n\nCHOOSE PROGRAM:\n");
	printf("Use 1 to move a cursor\n");
	printf("Use 2 to show text\n");
	printf("Use 3 to play snake\n");
}


void cursor(int device_nr) {
	printf("\n\nCURSOR\n");
	printf("press esc to quit\n");
	printf("Use a/s/w/d/o/l to move cursor around\n");

	char c;
	while (1) {
		while ((c = get_keystroke())) {
			if (c == ESCAPE) {
				return;
			}

			move(c);
			clear(buf);
			setpixel(cursor_x, cursor_y, cursor_z, buf);
			//buf[cursor_z * 8 + cursor_y] = (1<<cursor_x);
			dump(buf);
			write_to_cube(device_nr, buf);
		}
	}
}

void text(int device_nr) {
	printf("\n\nTEXT:\n");
	printf("press backspace to quit\n");
	printf("press keys to show that character\n");
	char c; // pressed key
	unsigned char chr[5];

	for (;;)
		while ((c = get_keystroke()) > 0) {

			if (c >= '0' && c <= 'z') {
				clear(buf);
				font_getchar(c, chr);

				// Put a character on the back of the cube
				for (int x = 0; x < 5; x++) {
					for (int y = 0; y < 8; y++) {
						if ((chr[x] & (0x80 >> y))) {
							setpixel(x + 2, 7 - y, 0, buf);
						}
					}
				}
				dump(buf);
				write_to_cube(device_nr, buf);
			}
			if (c == ESCAPE) {
				return;
			}
		}

}



