#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#if defined(OS_LINUX) || defined(OS_MACOSX)
#include <sys/ioctl.h>
#include <termios.h>
#elif defined(OS_WINDOWS)
#include <conio.h>
#endif

#include "hid.h"

#define UP		'o' 
#define DOWN		'l'
#define LEFT		'a'
#define RIGHT		'd'
#define FORWARD		'w'
#define BACKWARD	's'
#define PRINT		' '

typedef unsigned char uint8_t; 
void read_from_cube(int, uint8_t[]);
void write_to_cube(uint8_t[]);
void clear(uint8_t[]);
void setpixel(uint8_t,uint8_t,uint8_t,uint8_t[]);
int getpixel(uint8_t,uint8_t,uint8_t,uint8_t[]);
void move(char);
void dump(uint8_t[]);

static char get_keystroke(void);

uint8_t cursor_x = 3;
uint8_t cursor_y = 3;
uint8_t cursor_z = 3;

int main()
{
	char c;
	uint8_t buf[64];

	int device_nr = rawhid_open(1, 0x16C0, 0x0480, 0xFFAB, 0x0200);
	if (device_nr <= 0) {
		printf("no rawhid device found\n");
		return -1;
	}
	printf("found rawhid device\n");

	while (1) {
		// check if any Raw HID packet has arrived
		int size = rawhid_recv(0, buf, 64, 220);
		if (size < 0) {
			printf("\nerror reading, device went offline\n");
			rawhid_close(0);
			return 0;
		}

		read_from_cube(size, buf);

		// check if any input on stdin
		while ((c = get_keystroke()) >= 32) {
			if (c==PRINT) {
				dump(buf);
			} else {
				move(c);
				clear(buf);
				setpixel(cursor_x, cursor_y, cursor_z, buf);
				//buf[cursor_z * 8 + cursor_y] = (1<<cursor_x);
				dump(buf);
				rawhid_send(0, buf, 64, 100);
			}

		}
	}
}

void dump(uint8_t buf[]) {
	printf("\n");
	for (uint8_t y=0; y<8; y++) {
		for (uint8_t x=0; x<8; x++) {
			for (uint8_t z=0; z<8; z++) {
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
	int i;
	for (i=0; i<64; i++) {
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
		case LEFT: cursor_y++; if (cursor_y>7) cursor_y = 0; break;
		case RIGHT: cursor_y--; if (cursor_y<0) cursor_y = 7; break;
		case FORWARD: cursor_x++; if (cursor_x>7) cursor_x = 0; break;
		case BACKWARD: cursor_x--; if (cursor_x<0) cursor_x = 7; break;
		default: i = c; printf("\nunknown key '%c' (%d)\n", c, i); break;
	}
}


void read_from_cube(int size, uint8_t buf[]) {
	int i;
	if (size > 0) {
		printf("\nrecv %d bytes:\n", size);
		for (i=0; i<size; i++) {
			printf("%02X ", buf[i] & 255);
			if (i % 16 == 15 && i < size-1) printf("\n");
		}
		printf("\n");
	}
}

void write_to_cube(uint8_t buf[]) {
	rawhid_send(0, buf, 64, 100);
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


