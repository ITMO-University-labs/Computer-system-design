/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

typedef struct {
  int colors[2];
  int continue_ignite[2];
  int colors_size;
  int delay_on;
  int post_delay
} GarlandStep;

typedef struct {
  GarlandStep garland_steps[16];
  int size;
  int current_step;
} Garland;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define RED GPIO_PIN_15
#define YELLOW GPIO_PIN_14
#define GREEN GPIO_PIN_13
#define BTN_PIN_NUM GPIO_PIN_15
#define MODE_MAX 4

#define digital_write(GPIO, pin, value) HAL_GPIO_WritePin(GPIO, pin, value)
#define digital_read(GPIO, pin) !HAL_GPIO_ReadPin(GPIO, pin)

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static int prev_state = 0, mode = 0;
static int steps[MODE_MAX] = {0};

static Garland mode_0 = {
  .garland_steps = {
    {.colors = {GREEN}, .continue_ignite = {0}, .colors_size = 1, .delay_on = 500},
    {.colors = {YELLOW}, .continue_ignite = {0}, .colors_size = 1, .delay_on = 500},
    {.colors = {GREEN}, .continue_ignite = {0}, .colors_size = 1, .delay_on = 500},
    {.colors = {RED}, .continue_ignite = {0}, .colors_size = 1, .delay_on = 500}
  },
  .size = 4,
};

static Garland mode_1 = {
  .garland_steps = {
    {.colors = {GREEN, YELLOW}, .continue_ignite = {0, 0}, .colors_size = 2, .delay_on = 250, .post_delay = 250},
    {.colors = {GREEN, YELLOW}, .continue_ignite = {0, 0}, .colors_size = 2, .delay_on = 250, .post_delay = 250},
    {.colors = {GREEN, RED}, .continue_ignite = {0, 0}, .colors_size = 2, .delay_on = 250, .post_delay = 250},
    {.colors = {GREEN, RED}, .continue_ignite = {0, 0}, .colors_size = 2, .delay_on = 250, .post_delay = 250},
    {.colors = {GREEN}, .continue_ignite = {0}, .colors_size = 1, .delay_on = 250},
    {.colors = {YELLOW}, .continue_ignite = {0}, .colors_size = 1, .delay_on = 250},
    {.colors = {GREEN}, .continue_ignite = {0}, .colors_size = 1, .delay_on = 250},
    {.colors = {RED}, .continue_ignite = {0}, .colors_size = 1, .delay_on = 250}
  },
  .size = 8,
};

static Garland mode_2 = {
  .garland_steps = {
    {.colors = {GREEN}, .continue_ignite = {0}, .colors_size = 1, .delay_on = 500, .post_delay = 250},
    {.colors = {GREEN}, .continue_ignite = {0}, .colors_size = 1, .delay_on = 250, .post_delay = 125},
    {.colors = {GREEN}, .continue_ignite = {1}, .colors_size = 1, .delay_on = 250, .post_delay = 500},
    {.colors = {RED}, .continue_ignite = {0}, .colors_size = 1, .delay_on = 500, .post_delay = 250},
    {.colors = {RED}, .continue_ignite = {0}, .colors_size = 1, .delay_on = 250, .post_delay = 125},
    {.colors = {RED}, .continue_ignite = {1}, .colors_size = 1, .delay_on = 250, .post_delay = 500},
    {.colors = {GREEN, RED}, .continue_ignite = {0, 0}, .colors_size = 2, .post_delay = 500},
  },
  .size = 7,
};

static Garland mode_3 = {
  .garland_steps = {
    {.colors = {GREEN, RED}, .continue_ignite = {0, 0}, .colors_size = 2, .delay_on = 75, .post_delay = 75},
    {.colors = {GREEN, RED}, .continue_ignite = {0, 0}, .colors_size = 2, .delay_on = 75, .post_delay = 75},
    {.colors = {GREEN, RED}, .continue_ignite = {0, 0}, .colors_size = 2, .delay_on = 75, .post_delay = 75},
    {.colors = {GREEN, RED}, .continue_ignite = {0, 0}, .colors_size = 2, .delay_on = 75, .post_delay = 75},
    {.colors = {GREEN, YELLOW}, .continue_ignite = {0, 0}, .colors_size = 2, .delay_on = 75, .post_delay = 75},
    {.colors = {GREEN, YELLOW}, .continue_ignite = {0, 0}, .colors_size = 2, .delay_on = 75, .post_delay = 75},
    {.colors = {GREEN,YELLOW}, .continue_ignite = {0, 0}, .colors_size = 2, .delay_on = 75, .post_delay = 75},
    {.colors = {GREEN, YELLOW}, .continue_ignite = {0, 0}, .colors_size = 2, .delay_on = 75, .post_delay = 75}
  },
  .size = 7,
};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
void garland_ignite(Garland* garland);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

int button_processing() {
  if (digital_read(GPIOC, BTN_PIN_NUM)) {
    if (!prev_state) {
      HAL_Delay(20);                                 // задержка с целью избавиться от дребезга контактов
      if (digital_read(GPIOC, BTN_PIN_NUM)) {   // кнопки. 20 мс оказалось достаточно
        mode = mode < MODE_MAX ? mode + 1 : 0;
        prev_state = 1;
        digital_write(GPIOD, GREEN, 0);
        digital_write(GPIOD, RED, 0);
        digital_write(GPIOD, YELLOW, 0);
        return 1;
      }
    }
  } else
      prev_state = 0;
  
  return 0;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    /* USER CODE END WHILE */
    button_processing();

    switch (mode) {
      case 0: garland_ignite(&mode_0); break;
      case 1: garland_ignite(&mode_1); break;
      case 2: garland_ignite(&mode_2); break;
      case 3: garland_ignite(&mode_3); break;
      default: break;
    }
  }
  /* USER CODE END 3 */
}

#define CHECK_TIME(label, delay) \
  state++;        \
  last_time = HAL_GetTick(); \
  label:      \
  if (HAL_GetTick() - last_time < delay) return; \

void garland_ignite(Garland* garland) {
  static int state = 0;
  static uint32_t last_time;
  int i = garland->current_step;
  
  if (state == 1) goto delay_on;
  else if (state == 2) goto post_delay;

  for(int j = 0; j < garland->garland_steps[i].colors_size; j++)
    digital_write(GPIOD, garland->garland_steps[i].colors[j], 1);

  CHECK_TIME(delay_on, garland->garland_steps[i].delay_on)

  for(int j = 0; j < garland->garland_steps[i].colors_size; j++)
    digital_write(GPIOD, garland->garland_steps[i].colors[j], garland->garland_steps[i].continue_ignite[j]);

  CHECK_TIME(post_delay, garland->garland_steps[i].post_delay)

  garland->current_step = garland->current_step >= garland->size ? 0 : garland->current_step + 1;
  state = 0;
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC15 */
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PD13 PD14 PD15 */
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
