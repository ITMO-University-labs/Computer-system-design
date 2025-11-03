#include "string.h"
#include "stm32_io.h"
#include "garland.h"
#include "stdlib.h"
#include "stm32f427xx.h"

#define MAX_LEN 32

static char command[MAX_LEN] = {0};
static int command_step = 0;
static Garland new_garland = {0};

static void command_set_garland();
static void command_new_garland();
static void command_new_garland_0();
static void command_new_garland_1();
static void command_set_interrupts();

void command_execute() {
  if (input[0] == '\0')
    return;

  if (command_step == 0)
    memcpy(command, input, sizeof(command));
    
  if (!strncmp(command, "set", 3)) {
    if (!strncmp(command + 3, " interrupts", 11))
        command_set_interrupts();
    else
        command_set_garland();
  }
  else if (!strncmp(command, "new", 3))
    command_new_garland();
  else
    xprintf("Unknown command!\n");

  if (command_step == 0)
    clear_input();
}

static void command_set_garland() {
  if (command[4] < '0' || command[4] - '0' > MODE_COUNT - 1) {
    xprintf("Mode number must be from 0 to %d!\n", MODE_COUNT - 1);
    return;
  }
  int set_mode = command[4] - '0';
  if (set_mode > mode_max) {
    xprintf("Mode %d does not exist!\n", set_mode);
    return;
  }

  cur_mode = command[4] - '0';
  xprintf("Successfully set %d\n", set_mode);
}

static void command_new_garland() {
  if (command_step == 0)
    command_new_garland_0();
  else
    command_new_garland_1();
}

static void command_new_garland_0() {
  memset(&new_garland, 0, sizeof(Garland));
  for (int i = 4; i < 4 + MODE_COUNT; i++) {
    if (command[i] == '\0') {
      if (i < 6) {
        xprintf("Counts of diodes must be from 2 to 8!\n");
        return;
      }
      break;
    }
    switch (command[i]) {
      case 'g':
        new_garland.garland_steps[new_garland.size].colors[0] = GREEN;
        break;
      case 'r':
        new_garland.garland_steps[new_garland.size].colors[0] = RED;
        break;
      case 'y':
        new_garland.garland_steps[new_garland.size].colors[0] = YELLOW;
        break; 
      case 'n':
        new_garland.garland_steps[new_garland.size - 1].post_delay += 1;
        break;
      default:
        xprintf("Unknown letter '%c' in position %d. Possible letters: g,r,y,n.\n", 
          command[i], new_garland.size);
        return;
    }

    new_garland.garland_steps[new_garland.size].colors_size = 1;
    if (command[i] != 'n')
      new_garland.size++;
  }
  xprintf("Enter diodes duration:\n");
  command_step++;
}

static void command_new_garland_1() {
  static int next_mode = 4;

  char local_input[4];
  memcpy(local_input, input, sizeof(local_input));
  int duration = strtol(local_input, NULL, 10);

  if (duration != 200 && duration != 500 && duration != 1000) {
    xprintf("Duration can be 200, 500 or 1000ms!\n");
    command_step = 0;
    return;
  }

  for (int i = 0; i < new_garland.size; i++) {
    new_garland.garland_steps[i].post_delay *= duration;
    new_garland.garland_steps[i].delay_on = duration;
  }

  next_mode = next_mode == MODE_COUNT ? 4 : next_mode;
  mode_max = mode_max < MODE_COUNT - 1 ? mode_max + 1 : MODE_COUNT - 1;
  garlands[next_mode++] = new_garland;
  prev_mode = -1;

  command_step = 0;
  xprintf("Successfully added new mode.\n");
} 

static void command_set_interrupts () {
    if (!strcmp(command + 15, "on")) {
        set_interrupts(1);
        xprintf("Enable interupts.\n");
    }
    else if (!strcmp(command + 15, "off")) {
        set_interrupts(0);
        xprintf("\nDisable interupts.\n");
    }
    else
        xprintf("Unknown parameter '%s'. Possible: on, off.\n", command + 15);
}
