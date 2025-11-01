#ifndef GARLAND
#define GARLAND

#define RED GPIO_PIN_15
#define YELLOW GPIO_PIN_14
#define GREEN GPIO_PIN_13
#define MODE_COUNT 8

typedef struct {
  int colors[2];
  int freeze[2];
  int colors_size;
  int delay_on;
  int post_delay
} GarlandStep;

typedef struct {
  GarlandStep garland_steps[16];
  int size;
  int current_step;
} Garland;

extern int cur_mode;
extern int mode_max;
extern int prev_mode;
extern Garland garlands[MODE_COUNT];

int garland_button_processing();
void garland_ignite();
int get_mode_max();

#endif