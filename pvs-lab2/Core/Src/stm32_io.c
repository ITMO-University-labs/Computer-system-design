 #include "usart.h"
#include "string.h"
#include "stdarg.h"
#include "stdio.h"
#include "ring_buffer.h"

#define MAX_INPUT_LEN 32

int start_receive, receive_finished;
int interrupts_on = 1;
char input[MAX_INPUT_LEN] = {0};

static RingBuffer rx_rb = {0};
static RingBuffer tx_rb = {0};
static uint8_t uart_buf;
static int ptr = 0;

void xprintf(char* fmt, ...);

HAL_StatusTypeDef start_receive_char_it() {
  if (interrupts_on) {
    receive_finished = 0;
    return HAL_UART_Receive_IT(&huart6, &uart_buf, 1);
  }

  return HAL_OK;
}

int try_get_received_char(uint8_t* buf) {
  if (!interrupts_on) {
    if (HAL_OK == HAL_UART_Receive(&huart6, &uart_buf, 1, 1)) {
      *buf = uart_buf;
      return 1;
    }
  }
  if (receive_finished) {
    start_receive = 0;
    return rb_read(&rx_rb, buf);
  }

  return 0;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
  if (huart->Instance == USART6) {
    receive_finished = 1;
    rb_write(&rx_rb, uart_buf);
  }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
  if (huart->Instance == USART6) {
    char c;
    if (rb_read(&tx_rb, &c))
      HAL_UART_Transmit_IT(&huart6, (uint8_t*) &c, 1);
  }
}

int input_char_add(char c) {
  if (ptr >= MAX_INPUT_LEN)
      return 0;
  input[ptr++] = c;
  input[ptr] = '\0';

  return 1;
}

void clear_input() {
  memset(input, '\0', sizeof(input));
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
  if (strlen(fmt) == 0)
    return;
  char printf_buf[MAX_BUFFER] = {0};

  va_list arg;
  va_start(arg, fmt);
  vsprintf(printf_buf, fmt, arg);
  
  if (!interrupts_on)
    HAL_UART_Transmit(&huart6, (uint8_t*) printf_buf, strlen(printf_buf), 10);
  else {
    while (!rb_write_array(&tx_rb, printf_buf, strlen(printf_buf)));
    if (huart6.gState == HAL_UART_STATE_READY) {
      char c;
      if (rb_read(&tx_rb, &c)) {
        HAL_UART_Transmit_IT(&huart6, (uint8_t*)&c, 1);
      }
    }
  }

  va_end(arg);
}

void set_interrupts(int val) {
  interrupts_on = val;

  if (val) {
    __HAL_UART_ENABLE_IT(&huart6, UART_IT_RXNE);
    __HAL_UART_ENABLE_IT(&huart6, UART_IT_ERR);
    HAL_UART_Receive_IT(&huart6, &uart_buf, 1);
  } else {
    __HAL_UART_DISABLE_IT(&huart6, UART_IT_RXNE);
    __HAL_UART_DISABLE_IT(&huart6, UART_IT_ERR);
    huart6.RxState = HAL_UART_STATE_READY;
    huart6.gState = HAL_UART_STATE_READY;
  }
}

