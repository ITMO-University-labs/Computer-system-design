#include "string.h"
#include "stm32_io.h"
#include "stdlib.h"
#include "stm32f427xx.h"
#include "tim.h"

#define RED GPIO_PIN_15
#define YELLOW GPIO_PIN_14
#define GREEN GPIO_PIN_13

static void led_on(int pos);
static void led_off();

static int pult_step = 0;

typedef struct {
  int color;
  int brightness;
} LEDSettings;

static LEDSettings led_settings[] = {
  {GREEN, 10},
  {GREEN, 40},
  {GREEN, 100},
  {YELLOW, 10},
  {YELLOW, 40},
  {YELLOW, 100},
  {RED, 10},
  {RED, 40},
  {RED, 100},
};

void pult_execute(char button) {
  static LEDSettings buffer_led_settings = {0};
  static int pos = 0;

  switch (pult_step) {
    case 0:
      if (button >= '1' && button <= '9') {
        xprintf("Set mode %c.\n", button);
        led_on(button - '1');
      }
      else if (button == '0') {
        xprintf("All leds switched off.\n");
        led_off();
      }
      else if (button == '#') {
        pult_step++;
        xprintf("Switch to settings mode. Enter from 1 to 9.\n");
      }
      break;
  
    case 1:
      if (button >= '1' && button <= '9') {
        pos = button - '1';
        pult_step++;
        xprintf("Enter 1 (green), 2 (yellow) or 3 (red).\n");
      }
      break;
     
    case 2:
      if (button >= '1' && button <= '3') {
        switch (button) {
          case '1': buffer_led_settings.color = GREEN; break;
          case '2': buffer_led_settings.color = YELLOW; break;
          case '3': buffer_led_settings.color = RED; break;
        }
        buffer_led_settings.brightness = led_settings[pos].brightness;
        pult_step++;
        xprintf("Enter 1 to increase brightness by 10, or 2 to decrease by 10.\n");
      }
      break;

    case 3:
      if (button == '1' && buffer_led_settings.brightness < 100) {
        buffer_led_settings.brightness += 10;
        xprintf("Set %d%%. Press 'Enter' to save.\n", buffer_led_settings.brightness);
      }
      else if (button == '2' && buffer_led_settings.brightness > 0) {
        buffer_led_settings.brightness -= 10;
        xprintf("Set %d%%. Press 'Enter' to save.\n", buffer_led_settings.brightness);
      }
      else if (button == '#') {
        led_settings[pos] = buffer_led_settings;
        pult_step = 0;
        xprintf("Saved to mode %d.\n", pos + 1);
      }
  }
  
}

static void led_off() {
  htim4.Instance->CCR2 = 0;
  htim4.Instance->CCR3 = 0;
  htim4.Instance->CCR4 = 0;
}

static void led_on(int pos) {
  led_off();

  LEDSettings current = led_settings[pos];
  switch (current.color) {
    case GREEN: htim4.Instance->CCR2 = current.brightness; break;
    case YELLOW: htim4.Instance->CCR3 = current.brightness; break;
    case RED: htim4.Instance->CCR4 = current.brightness; break;
  }
}
