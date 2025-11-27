#include "main.h"
#include "stm32_io.h"
#include "i2c.h"
#include <string.h>

#define KBRD_ADDR 0xE2

enum {
    COL1 = 96,
    COL2 = 80,
    COL3 = 48
};

typedef enum {
	INPUT_PORT = 0x00, //Read byte XXXX XXXX
	OUTPUT_PORT = 0x01, //Read/write byte 1111 1111
	POLARITY_INVERSION = 0x02, //Read/write byte 0000 0000
	CONFIG = 0x03 //Read/write byte 1111 1111
} pca9538_regs_t;

static int rows[] = {0xFE, 0xFD, 0xFB, 0xF7};
static char keys[4][3];
static int prev_button_state = 0;

static HAL_StatusTypeDef PCA9538_Read_Register(uint16_t addr, pca9538_regs_t reg, uint8_t* buf) {
	return HAL_I2C_Mem_Read(&hi2c1, addr | 1, reg, 1, buf, 1, 100);
}


static HAL_StatusTypeDef PCA9538_Write_Register(uint16_t addr, pca9538_regs_t reg, uint8_t* buf) {
	return HAL_I2C_Mem_Write(&hi2c1, addr & 0xFFFE, reg, 1, buf, 1, 100);
}

static HAL_StatusTypeDef PCA9538_Read_Config(uint16_t addr, uint8_t* buf) {
	HAL_StatusTypeDef ret = HAL_OK;
	uint8_t *buf_ptr = buf;

	for(uint8_t i = 0; i < 4; i++ ) {
		ret = PCA9538_Read_Register(addr, i, buf_ptr++);

		if(ret != HAL_OK)
			return ret;
	}

	return ret;
}

static HAL_StatusTypeDef PCA9538_Check_DefaultConfig(uint16_t addr) {
	HAL_StatusTypeDef ret = HAL_OK;
	uint8_t buf[4];

	ret = PCA9538_Read_Config(addr, buf);

	return ret;
}

static HAL_StatusTypeDef PCA9538_Read_Inputs(uint16_t addr, uint8_t* buf) {
	return PCA9538_Read_Register(addr, INPUT_PORT, buf);
}

static HAL_StatusTypeDef Set_Keyboard( void ) {
	uint8_t buf = 0;

	HAL_StatusTypeDef ret = PCA9538_Write_Register(KBRD_ADDR, POLARITY_INVERSION, &buf);
	if(ret != HAL_OK ) {
		return ret;
	}
	HAL_Delay(10);
	buf = 0;
	ret = PCA9538_Write_Register(KBRD_ADDR, OUTPUT_PORT, &buf);
	if( ret != HAL_OK ) {
		return ret;
	}
	HAL_Delay(10);

	return HAL_OK;
}

void keyboard_set_keys(char matrix[4][3]) {
    memcpy(keys, matrix, sizeof(keys));
}

uint8_t keyboard_button_processing() {
    uint8_t buf;
    if (PCA9538_Read_Inputs(KBRD_ADDR, &buf) == HAL_OK && (buf & 0x70) != 0x70) {
        if (!prev_button_state) {
            HAL_Delay(20);
            if (PCA9538_Read_Inputs(KBRD_ADDR, &buf) == HAL_OK && (buf & 0x70) != 0x70) {
                prev_button_state = 1;
                return buf & 0x70;
            }
        }
  } else
      prev_button_state = 0;
  
  return 0;
}

char keyboard_get_key() {
    uint8_t buf;
     if (prev_button_state && PCA9538_Read_Inputs(KBRD_ADDR, &buf) == HAL_OK && (buf & 0x70) != 0x70)
         return '\0';
     else
         prev_button_state = 0;

    for (int row = 0; row < 4; row++) {
        if(Set_Keyboard() != HAL_OK)
		    return '\0';

        buf = rows[row];
        if(PCA9538_Write_Register(KBRD_ADDR, CONFIG, &buf) != HAL_OK )
            return '\0';

        uint8_t kbd_in = keyboard_button_processing();
        switch (kbd_in) {
            case COL1: return keys[row][0];
            case COL2: return keys[row][1];
            case COL3: return keys[row][2];
        }
    }
    
    return '\0';
}
