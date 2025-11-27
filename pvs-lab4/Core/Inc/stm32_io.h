#ifndef IO
#define IO

#include "main.h"

extern char input[32];

HAL_StatusTypeDef start_receive_char_it();
int try_get_received_char(char* buf);
void read_char(void (*enter_callback)(void));
void clear_input();
void xprintf(char* fmt, ...);
void set_interrupts(int val);

#endif