#ifndef MISC_H_
#define MISC_H_

typedef unsigned char uint8_t;
typedef signed char int8_t;

#define PRINT		' '
#define ESCAPE		27
#define CURSOR		'1'
#define TEXT		'2'
#define SNAKE		'3'

char get_keystroke(void);
long get_time();

#endif


