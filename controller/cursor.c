
#include "cursor.h"

int8_t cursor_x = 0;
int8_t cursor_y = 0;
int8_t cursor_z = 0;

int8_t dir_x = 0;
int8_t dir_y = 0;
int8_t dir_z = 0;


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


