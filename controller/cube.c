#include <stdio.h>
#include <stdlib.h>

#include "cube.h"
#include "misc.h"
#include "hid.h"

uint8_t buf[8 * 8];
uint8_t cube[8 * 8];

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

void clearpixel(uint8_t x, uint8_t y, uint8_t z, uint8_t buf[]) {
	buf[z * 8 + y] &= ~(1 << (7 - x));
}

