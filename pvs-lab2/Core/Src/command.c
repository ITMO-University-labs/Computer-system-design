#include "string.h"
#include "stm32_io.h"
#include "garland.h"
#include "stdlib.h"
// #include ""

#define MAX_LEN 32

static char command[MAX_LEN] = {0};
static int command_step = 0;
static Garland new_garland = {0};

// static void command_new_garland();
static void command_set_garland();
static void command_new_garland();
static void command_new_garland_0();
static void command_new_garland_1();

void command_execute() {
    if (command_step == 0)
        memcpy(command, input, sizeof(command));
    if (!strncmp(command, "set ", 4))
        command_set_garland();
    else if (!strncmp(command, "new ", 4))
        command_new_garland();
    else
        xprintf("Unknown command!\n");
}

static void command_set_garland() {
    if (command[4] < '0' || command[4] - '0' > MODE_MAX) {
        xprintf("Mode number must be from 0 to %d!\n", MODE_MAX - 1);
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
    for (int i = 4; i < 4 + MODE_MAX; i++) {
        if (command[i] == '\0') {
            if (i < 6) {
                xprintf("Counts of diods must be from 2 to 8!\n");
                return;
            }
            break;
        }
        switch (command[i]) {
            case 'g':
                new_garland.garland_steps[new_garland.size++].colors[0] = GREEN;
                break;
            case 'r':
                new_garland.garland_steps[new_garland.size++].colors[0] = RED;
                break;
            case 'y':
                new_garland.garland_steps[new_garland.size++].colors[0] = YELLOW;
                break; 
            case 'n':
                new_garland.garland_steps[new_garland.size].post_delay += 1;
                break;
            default:
                xprintf("Unknown letter '%c' in position %d. Possible letters: g,r,y,n.\n", 
                    command[i], new_garland.size);
                return;
        }
    }
    xprintf("Enter diods duration:\n");
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

    garlands[next_mode++] = new_garland;
    next_mode = next_mode == 8 ? 5 : next_mode;
    mode_max = mode_max < 7 ? mode_max + 1 : 7;

    command_step = 0;
    xprintf("Successfully added new mode.\n");
} 