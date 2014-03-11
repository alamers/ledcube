/*
 * cube.h
 *
 *  Created on: 11 mrt. 2014
 *      Author: arjanl
 */

#ifndef CUBE_H_
#define CUBE_H_

#include "misc.h"

void read_from_cube(int device_nr);
void write_to_cube(int device_nr, uint8_t[]);
void clear(uint8_t[]);
void setpixel(uint8_t, uint8_t, uint8_t, uint8_t[]);
void clearpixel(uint8_t, uint8_t, uint8_t, uint8_t[]);
int getpixel(uint8_t, uint8_t, uint8_t, uint8_t[]);
void dump(uint8_t[]);


extern uint8_t buf[8 * 8];
extern uint8_t cube[8 * 8];

#endif /* CUBE_H_ */
