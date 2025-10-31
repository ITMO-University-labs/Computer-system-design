#include "usart.h"
#include "string.h"
#include "stdarg.h"

#define MAX_INPUT_LEN 32

int start_receive, receive_finished;
int transmit_ongoing;
char input[32];

static uint8_t uart_buf;
static int ptr = 0;

HAL_StatusTypeDef start_receive_char_it() {
  receive_finished = 0;
  return HAL_UART_Receive_IT(&huart6, &uart_buf, 1);
}

int try_get_received_char(uint8_t* buf) {
  if (receive_finished) {
    *buf = uart_buf;
    start_receive = 0;
    return 1;
  }

  return 0;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
  if (huart->Instance == USART6) {
    receive_finished = 1;
    // rb_write(&rx_rb, uart_buf);
  }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
  if (huart->Instance == USART6) {
    transmit_ongoing = 0;
  }
}

int input_char_add(char c) {
  if (ptr >= MAX_INPUT_LEN)
      return 0;
  input[ptr++] = c;
  input[ptr] = '\0';

  return 1;
}

int input_char_remove() {
  if (ptr == 0)
      return 0;
  
  input[--ptr] = '\0';
  return 1;
}

void read_char(void (*enter_callback)(void)) {
  static char c;

  if (try_get_received_char((uint8_t*) &c)) {
    start_receive_char_it();
    xprintf("%c", c);

    switch (c) {
    case 127:
      input_char_remove();
      break;
      
    case('\r'):
      xprintf("\n");
      if (enter_callback != NULL)
        enter_callback();
      ptr = 0;
      input[ptr] = '\0';
      break;

    default:
      input_char_add(c);
    }
  }
}

void xprintf(char* fmt, ...) {
  static char printf_buf[128];

  while(transmit_ongoing);

  va_list arg;
  va_start(arg, fmt);
  vsprintf(printf_buf, fmt, arg);
  transmit_ongoing = 1;

  HAL_UART_Transmit_IT(&huart6, (uint8_t*) printf_buf, strlen(printf_buf));

  va_end(arg);
}