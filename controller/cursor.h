/*
 * cursor.h
 *
 *  Created on: 11 mrt. 2014
 *      Author: arjanl
 */

#ifndef CURSOR_H_
#define CURSOR_H_

#include "misc.h"

#define UP		'o'
#define DOWN		'l'
#define LEFT		'a'
#define RIGHT		'd'
#define FORWARD		's'
#define BACKWARD	'w'

void move(char);
void wrap_cursor();
void block_cursor();
void change_dir(char);
void move_dir();


extern int8_t cursor_x;
extern int8_t cursor_y;
extern int8_t cursor_z;

extern int8_t dir_x;
extern int8_t dir_y;
extern int8_t dir_z;

#endif /* CURSOR_H_ */
