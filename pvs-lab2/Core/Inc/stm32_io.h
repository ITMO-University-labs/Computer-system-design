#ifndef IO
#define IO

#include "main.h"

extern char input[32];

void read_char(void (*enter_callback)(void));
void clear_input();
void xprintf(char* fmt, ...);
void set_interrupts(int val);

#endif