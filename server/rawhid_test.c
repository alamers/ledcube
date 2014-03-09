#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#if defined(OS_LINUX) || defined(OS_MACOSX)
#include <sys/ioctl.h>
#include <termios.h>
#elif defined(OS_WINDOWS)
#include <conio.h>
#endif

#include "controller.h"

#include "hid.h"
#include "font.h"

#define UP		'o' 
#define DOWN		'l'
#define LEFT		'd'
#define RIGHT		'a'
#define FORWARD		's'
#define BACKWARD	'w'
#define PRINT		' '
#define TEXT		't'


void read_from_cube(int device_nr);
void write_to_cube(int device_nr, uint8_t[]);
void clear(uint8_t[]);
void setpixel(uint8_t,uint8_t,uint8_t,uint8_t[]);
int getpixel(uint8_t,uint8_t,uint8_t,uint8_t[]);
void move(char);
void dump(uint8_t[]);
void text();

static char get_keystroke(void);

int8_t cursor_x = 0;
int8_t cursor_y = 0;
int8_t cursor_z = 0;

uint8_t buf[8*8];
uint8_t cube[8*8];

int main()
{
	char c;

	int device_nr = 0;
	int opened = rawhid_open(1, 0x16C0, 0x0480, 0xFFAB, 0x0200);
	if (opened <= 0) {
		printf("no rawhid device found\n");
		return -1;
	}
	printf("found rawhid device %d\n", device_nr);
	printf("Use a/s/w/d/o/l to move cursor around\n");
	printf("Use t to print text\n");

	while (1) {
		// check if any Raw HID packet has arrived
		read_from_cube(device_nr);

		// check if any input on stdin
		while ((c = get_keystroke()) >= 32) {
			if (c==PRINT) {
				dump(buf);
			} else if (c==TEXT) {
				text(device_nr);
			} else {
				move(c);
				clear(buf);
				setpixel(cursor_x, cursor_y, cursor_z, buf);
				//buf[cursor_z * 8 + cursor_y] = (1<<cursor_x);
				dump(buf);
				rawhid_send(device_nr, buf, 64, 100);
			}

		}
	}
}

void dump(uint8_t buf[]) {
	printf("\n");
	for (uint8_t y=0; y<8; y++) {
		for (uint8_t z=0; z<8; z++) {
			for (uint8_t x=0; x<8; x++) {
				if (getpixel(x,y,z,buf)==0) {
					printf(".");
				} else {
					printf("X");
				}
			}
			printf(" ");
		}
		printf("\n");
	}
}

void clear(uint8_t buf[]) {
	for (int i=0; i<64; i++) {
		buf[i] = 0;
	}
}

int getpixel(uint8_t x, uint8_t y, uint8_t z, uint8_t buf[]) {
	return buf[z*8 + y] & (1<<x);
}

void setpixel(uint8_t x, uint8_t y, uint8_t z, uint8_t buf[]) {
        buf[z*8 + y] |= (1 << x);
}

void move(char c) {
	int i;
	switch(c) {
		case UP: cursor_z++; if (cursor_z>7) cursor_z = 0; break;
		case DOWN: cursor_z--; if (cursor_z<0) cursor_z = 7; break;
		case LEFT: cursor_x++; if (cursor_x>7) cursor_x = 0; break;
		case RIGHT: cursor_x--; if (cursor_x<0) cursor_x = 7; break;
		case FORWARD: cursor_y++; if (cursor_y>7) cursor_y = 0; break;
		case BACKWARD: cursor_y--; if (cursor_y<0) cursor_y = 7; break;
		default: i = c; printf("\nunknown key '%c' (%d)\n", c, i); break;
	}
}

void text(int device_nr) {
	printf("press backspace to quit");
	char c; // pressed key
	unsigned char chr[5];
	while ((c = get_keystroke()) != 127) {
		clear(buf);
		font_getchar(c, chr);

		// Put a character on the back of the cube
		for (int x = 0; x < 5; x++) {
			for (int y = 0; y < 8; y++) {
				if ((chr[x] & (0x80 >> y))) {
					setpixel(7, x + 2, y, buf);
				}
			}
		}
		dump(buf);
		rawhid_send(device_nr, buf, 64, 100);
	}

}


void read_from_cube(int device_nr) {
	int size = rawhid_recv(device_nr, cube, 64, 220);
	if (size < 0) {
		printf("\nerror reading, device went offline\n");
		rawhid_close(device_nr);
		exit(1);
	}

	if (size > 0) {
		printf("Cube state:\n");
		dump(cube);
	}
}

void write_to_cube(int device_nr, uint8_t buf[]) {
	rawhid_send(device_nr, buf, 64, 100);
}

#if defined(OS_LINUX) || defined(OS_MACOSX)
// Linux (POSIX) implementation of _kbhit().
// Morgan McGuire, morgan@cs.brown.edu
static int _kbhit() {
	static const int STDIN = 0;
	static int initialized = 0;
	int bytesWaiting;

	if (!initialized) {
		// Use termios to turn off line buffering
		struct termios term;
		tcgetattr(STDIN, &term);
		term.c_lflag &= ~ICANON;
		tcsetattr(STDIN, TCSANOW, &term);
		setbuf(stdin, NULL);
		initialized = 1;
	}
	ioctl(STDIN, FIONREAD, &bytesWaiting);
	return bytesWaiting;
}
static char _getch(void) {
	char c;
	if (fread(&c, 1, 1, stdin) < 1) return 0;
	return c;
}
#endif


static char get_keystroke(void)
{
	if (_kbhit()) {
		char c = _getch();
		if (c >= 32) return c;
	}
	return 0;
}


