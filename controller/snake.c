#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "cursor.h"
#include "misc.h"
#include "cube.h"

#define MAX_SNAKE_LENGTH 512
#define SNAKE_DELAY	500 // half a second
#define CANDY_DELAY	133

struct point {
	uint8_t x;
	uint8_t y;
	uint8_t z;
};

struct point candy;
struct point snake_buffer[MAX_SNAKE_LENGTH];
int snake_head;
int snake_tail;

void init();
int draw_snake(int device);
int move_snake();
int snake_length();
void game_over();
void place_candy();

void snake(int device_nr) {
	printf("\nSNAKE:\n");
	printf("press esc to quit\n");
	printf("use w/a/s/d/o/l to change direction the snake to quit\n");
	char c; // pressed key

	init();

	long time = get_time();
	long candy_time = get_time();
	int candy_ticker = 0;
	for (;;) {
		c = get_keystroke();

		if (c == ESCAPE) {
			return;
		} else if (c > 0) {
			change_dir(c);
		}

		if (get_time() - candy_time > CANDY_DELAY) {
			candy_time = get_time();
			candy_ticker++;
			if (candy_ticker % 2 == 0) {
				setpixel(candy.x, candy.y, candy.z, buf);
			} else {
				clearpixel(candy.x, candy.y, candy.z, buf);
			}
			write_to_cube(device_nr, buf);
		}

		if (get_time() - time > SNAKE_DELAY) {
			time = get_time();
			if (move_snake()) {
				break;
			}
			if (draw_snake(device_nr)) {
				break;
			}
		}
	}

	game_over();
}

int snake_length() {
	return (snake_head > snake_tail ?
			snake_head - snake_tail :
			snake_head + (MAX_SNAKE_LENGTH - snake_tail)) + 1;
}

void game_over() {
	printf("Game over! Length = %d", snake_length());
}

int draw_snake(int device_nr) {
	clear(buf);
	if (snake_head > snake_tail) {
		for (int i = snake_head; i >= snake_tail; i--) {
			if (getpixel(snake_buffer[i].x, snake_buffer[i].y,
					snake_buffer[i].z, buf)) {
				return -1;
			}
			setpixel(snake_buffer[i].x, snake_buffer[i].y, snake_buffer[i].z,
					buf);
		}
	} else {
		for (int i = 0; i <= snake_head; i++) {
			if (getpixel(snake_buffer[i].x, snake_buffer[i].y,
					snake_buffer[i].z, buf)) {
				return -1;
			}
			setpixel(snake_buffer[i].x, snake_buffer[i].y, snake_buffer[i].z,
					buf);
		}
		for (int i = MAX_SNAKE_LENGTH - 1; i >= snake_tail; i--) {
			if (getpixel(snake_buffer[i].x, snake_buffer[i].y,
					snake_buffer[i].z, buf)) {
				return -1;
			}
			setpixel(snake_buffer[i].x, snake_buffer[i].y, snake_buffer[i].z,
					buf);
		}
	}
	write_to_cube(device_nr, buf);
	return 0;
}

int move_snake() {
	struct point old_head = snake_buffer[snake_head];

	snake_head++;

	snake_buffer[snake_head].x = old_head.x + dir_x;
	snake_buffer[snake_head].y = old_head.y + dir_y;
	snake_buffer[snake_head].z = old_head.z + dir_z;

	if (candy.x == snake_buffer[snake_head].x
			&& candy.y == snake_buffer[snake_head].y
			&& candy.z == snake_buffer[snake_head].z) {
		printf("increasing length to %d", snake_length());
		place_candy();
	} else {
		snake_tail++;
	}

	if (snake_buffer[snake_head].x < 0 || snake_buffer[snake_head].x > 7
			|| snake_buffer[snake_head].y < 0 || snake_buffer[snake_head].y > 7
			|| snake_buffer[snake_head].z < 0
			|| snake_buffer[snake_head].z > 7) {
		return -1;
	} else {
		return 0;
	}
}

void place_candy() {
	candy.x = rand() % 8;
	candy.y = rand() % 8;
	candy.z = rand() % 8;
	if (getpixel(candy.x, candy.y, candy.z, buf)) {
		place_candy();
	}
}

void init() {
	srand(time(NULL));
	place_candy();
	clear(buf);
	snake_head = 1;
	snake_tail = 0;
	snake_buffer[0].x = 3;
	snake_buffer[0].y = 3;
	snake_buffer[0].z = 0;
	snake_buffer[1].x = 3;
	snake_buffer[1].y = 3;
	snake_buffer[1].z = 1;
	dir_x = 0;
	dir_y = 0;
	dir_z = 1;
}

