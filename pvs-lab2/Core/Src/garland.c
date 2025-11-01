#include "main.h"
#include "garland.h"

#define BTN_PIN_NUM GPIO_PIN_15

#define digital_write(GPIO, pin, value) HAL_GPIO_WritePin(GPIO, pin, value)
#define digital_read(GPIO, pin) !HAL_GPIO_ReadPin(GPIO, pin)

static int prev_button_state = 0;
int cur_mode = 0, mode_max = 3, prev_mode = 0;

Garland garlands[MODE_COUNT] = {
  {
    .garland_steps = {
      {.colors = {GREEN}, .freeze = {0}, .colors_size = 1, .delay_on = 500},
      {.colors = {YELLOW}, .freeze = {0}, .colors_size = 1, .delay_on = 500},
      {.colors = {GREEN}, .freeze = {0}, .colors_size = 1, .delay_on = 500},
      {.colors = {RED}, .freeze = {0}, .colors_size = 1, .delay_on = 500}
    },
    .size = 4,
  },
  {
    .garland_steps = {
      {.colors = {GREEN, YELLOW}, .freeze = {0, 0}, .colors_size = 2, .delay_on = 250, .post_delay = 250},
      {.colors = {GREEN, YELLOW}, .freeze = {0, 0}, .colors_size = 2, .delay_on = 250, .post_delay = 250},
      {.colors = {GREEN, RED}, .freeze = {0, 0}, .colors_size = 2, .delay_on = 250, .post_delay = 250},
      {.colors = {GREEN, RED}, .freeze = {0, 0}, .colors_size = 2, .delay_on = 250, .post_delay = 250},
      {.colors = {GREEN}, .freeze = {0}, .colors_size = 1, .delay_on = 250},
      {.colors = {YELLOW}, .freeze = {0}, .colors_size = 1, .delay_on = 250},
      {.colors = {GREEN}, .freeze = {0}, .colors_size = 1, .delay_on = 250},
      {.colors = {RED}, .freeze = {0}, .colors_size = 1, .delay_on = 250}
    },
    .size = 8,
  },
  {
    .garland_steps = {
      {.colors = {GREEN}, .freeze = {0}, .colors_size = 1, .delay_on = 500, .post_delay = 250},
      {.colors = {GREEN}, .freeze = {0}, .colors_size = 1, .delay_on = 250, .post_delay = 125},
      {.colors = {GREEN}, .freeze = {1}, .colors_size = 1, .delay_on = 250, .post_delay = 500},
      {.colors = {RED}, .freeze = {0}, .colors_size = 1, .delay_on = 500, .post_delay = 250},
      {.colors = {RED}, .freeze = {0}, .colors_size = 1, .delay_on = 250, .post_delay = 125},
      {.colors = {RED}, .freeze = {1}, .colors_size = 1, .delay_on = 250, .post_delay = 500},
      {.colors = {GREEN, RED}, .freeze = {0, 0}, .colors_size = 2, .post_delay = 500},
    },
    .size = 7,
  },

  {
    .garland_steps = {
      {.colors = {GREEN, RED}, .freeze = {0, 0}, .colors_size = 2, .delay_on = 75, .post_delay = 75},
      {.colors = {GREEN, RED}, .freeze = {0, 0}, .colors_size = 2, .delay_on = 75, .post_delay = 75},
      {.colors = {GREEN, RED}, .freeze = {0, 0}, .colors_size = 2, .delay_on = 75, .post_delay = 75},
      {.colors = {GREEN, RED}, .freeze = {0, 0}, .colors_size = 2, .delay_on = 75, .post_delay = 75},
      {.colors = {GREEN, YELLOW}, .freeze = {0, 0}, .colors_size = 2, .delay_on = 75, .post_delay = 75},
      {.colors = {GREEN, YELLOW}, .freeze = {0, 0}, .colors_size = 2, .delay_on = 75, .post_delay = 75},
      {.colors = {GREEN,YELLOW}, .freeze = {0, 0}, .colors_size = 2, .delay_on = 75, .post_delay = 75},
      {.colors = {GREEN, YELLOW}, .freeze = {0, 0}, .colors_size = 2, .delay_on = 75, .post_delay = 75}
    },
    .size = 7,
  }
};


int garland_button_processing() {
  if (digital_read(GPIOC, BTN_PIN_NUM)) {
    if (!prev_button_state) {
      HAL_Delay(20);                                 // задержка с целью избавиться от дребезга контактов
      if (digital_read(GPIOC, BTN_PIN_NUM)) {   // кнопки. 20 мс оказалось достаточно
        cur_mode = cur_mode < mode_max ? cur_mode + 1 : 0;
        prev_button_state = 1;
        digital_write(GPIOD, GREEN, 0);
        digital_write(GPIOD, RED, 0);
        digital_write(GPIOD, YELLOW, 0);
        return 1;
      }
    }
  } else
      prev_button_state = 0;
  
  return 0;
}

#define CHECK_TIME(delay) if (HAL_GetTick() - last_time < delay) return;
#define LEDS_OFF \
  digital_write(GPIOD, RED, 0);   \
  digital_write(GPIOD, GREEN, 0); \
  digital_write(GPIOD, YELLOW, 0);

void garland_ignite() {
  Garland* garland = &garlands[cur_mode];
  if (prev_mode != cur_mode) {
    LEDS_OFF
    prev_mode = cur_mode;
  }

  static int state = 0;
  static uint32_t last_time;
  int i = garland->current_step;
  
  switch (state){
    case 0:
      for(int j = 0; j < garland->garland_steps[i].colors_size; j++)
    	  digital_write(GPIOD, garland->garland_steps[i].colors[j], 1);
      state++;
      last_time = HAL_GetTick();

    case 1:
      CHECK_TIME(garland->garland_steps[i].delay_on)
      for(int j = 0; j < garland->garland_steps[i].colors_size; j++)
        digital_write(GPIOD, garland->garland_steps[i].colors[j], garland->garland_steps[i].freeze[j]);
      state++;
      last_time = HAL_GetTick();

    case 2:
      CHECK_TIME(garland->garland_steps[i].post_delay)
      garland->current_step = garland->current_step >= garland->size ? 0 : garland->current_step + 1;
      state = 0;

    default:
      break;
  }
}
