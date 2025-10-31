#ifndef IO
#define IO

#include "main.h"

extern char input[32];

void read_char(void (*enter_callback)(void));
void xprintf(char* fmt, ...);

#endif