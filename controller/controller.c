#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/time.h>

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
#define LEFT		'a'
#define RIGHT		'd'
#define FORWARD		's'
#define BACKWARD	'w'
#define PRINT		' '
#define ESCAPE		27
#define CURSOR		'1'
#define TEXT		'2'
#define WORMS		'3'

#define WORM_DELAY	500 // half a second

void read_from_cube(int device_nr);
void write_to_cube(int device_nr, uint8_t[]);
void clear(uint8_t[]);
void setpixel(uint8_t, uint8_t, uint8_t, uint8_t[]);
int getpixel(uint8_t, uint8_t, uint8_t, uint8_t[]);
void move(char);
void dump(uint8_t[]);
void text(int device_nr);
void cursor(int device_nr);
void worms(int device_nr);
void print_main_menu();
void wrap_cursor();
void block_cursor();
long get_time();

static char get_keystroke(void);

int8_t cursor_x = 0;
int8_t cursor_y = 0;
int8_t cursor_z = 0;

int8_t dir_x = 0;
int8_t dir_y = 0;
int8_t dir_z = 0;

uint8_t buf[8 * 8];
uint8_t cube[8 * 8];

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
		while ((c = get_keystroke()) >= 32) {
			if (c == TEXT) {
				text(device_nr);
				print_main_menu();
			} else if (c == WORMS) {
				worms(device_nr);
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
	printf("Use 3 to play worms\n");
}

void dump(uint8_t buf[]) {
	printf("\n");
	for (uint8_t y = 0; y < 8; y++) {
		for (uint8_t z = 0; z < 8; z++) {
			for (uint8_t x = 0; x < 8; x++) {
				if (getpixel(x, y, z, buf) == 0) {
					printf(". ");
				} else {
					printf("()");
				}
			}
			printf(" ");
		}
		printf("\n");
	}
}

void clear(uint8_t buf[]) {
	for (int i = 0; i < 64; i++) {
		buf[i] = 0;
	}
}

int getpixel(uint8_t x, uint8_t y, uint8_t z, uint8_t buf[]) {
	return buf[z * 8 + y] & (1 << (7 - x));
}

void setpixel(uint8_t x, uint8_t y, uint8_t z, uint8_t buf[]) {
	buf[z * 8 + y] |= (1 << (7 - x));
}

void move(char c) {
	int i;
	switch (c) {
	case UP:
		cursor_z++;
		break;
	case DOWN:
		cursor_z--;
		break;
	case LEFT:
		cursor_x++;
		break;
	case RIGHT:
		cursor_x--;
		break;
	case FORWARD:
		cursor_y++;
		break;
	case BACKWARD:
		cursor_y--;
		break;
	default:
		i = c;
		printf("\nunknown key '%c' (%d)\n", c, i);
		break;
	}
	wrap_cursor();
}

void wrap_cursor() {
	if (cursor_z > 7)
		cursor_z = 0;
	if (cursor_z < 0)
		cursor_z = 7;
	if (cursor_x > 7)
		cursor_x = 0;
	if (cursor_x < 0)
		cursor_x = 7;
	if (cursor_y > 7)
		cursor_y = 0;
	if (cursor_y < 0)
		cursor_y = 7;
}

void block_cursor() {
	if (cursor_z > 7)
		cursor_z = 7;
	if (cursor_z < 0)
		cursor_z = 0;
	if (cursor_x > 7)
		cursor_x = 7;
	if (cursor_x < 0)
		cursor_x = 0;
	if (cursor_y > 7)
		cursor_y = 7;
	if (cursor_y < 0)
		cursor_y = 0;
}

void move_dir() {
	cursor_x += dir_x;
	cursor_y += dir_y;
	cursor_z += dir_z;
	block_cursor();
}

void change_dir(char c) {
	int i;
	switch (c) {
	case UP:
		dir_x = 0;
		dir_y = 0;
		dir_z = 1;
		break;
	case DOWN:
		dir_x = 0;
		dir_y = 0;
		dir_z = -1;
		break;
	case LEFT:
		dir_x = -1;
		dir_y = 0;
		dir_z = 0;
		break;
	case RIGHT:
		dir_x = 1;
		dir_y = 0;
		dir_z = 0;
		break;
	case FORWARD:
		dir_x = 0;
		dir_y = 1;
		dir_z = 0;
		break;
	case BACKWARD:
		dir_x = 0;
		dir_y = -1;
		dir_z = 0;
		break;
	default:
		i = c;
		printf("\nunknown key '%c' (%d)\n", c, i);
		break;
	}
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

void worms(int device_nr) {
	printf("\n\nWORMS:\n");
	printf("press esc to quit\n");
	printf("use w/a/s/d/o/l to change direction the worm to quit\n");
	char c; // pressed key

	cursor_x = 3;
	cursor_y = 3;
	cursor_z = 3;
	dir_x = 0;
	dir_y = 0;
	dir_z = 1;

	long time = get_time();
	for(;;) {
		c = get_keystroke();

		if (c == ESCAPE) {
			return;
		} else if (c > 0) {
			change_dir(c);
		}

		if (get_time() - time > WORM_DELAY) {
			time = get_time();
			move_dir();
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

static char get_keystroke(void) {
	if (_kbhit()) {
		char c = _getch();
		return c;
	}
	return 0;
}

long get_time() {
	struct timeval tp;
	gettimeofday(&tp, 0);
	return tp.tv_sec * 1000 + tp.tv_usec / 1000;
}
