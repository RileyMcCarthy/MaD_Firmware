#ifndef IOBoard_H
#define IOBoard_H

#include "MCP23017.h"
#define SERVO_SRDY 14 //Servo ready status (Input)
#define SERVO_DIR 12  //Step direction (Output)
#define SERVO_PUL 13  //Step pulse (Output)
#define SERVO_ENA 11  //Servo enable (Output)

#define SD_CD 4   //Data SD card CD
#define SD_CMD 5  //Data SD card CMD
#define SD_CLK 6  //Data SD card clock
#define SD_DAT0 7 //Data SD card data 0

#define RTC_SCL 0  //clock for real time clock and IO expander I2C (Output)
#define RTC_SDA 26 //data for real time clock and IO expander I2C (Input/Output)

#define I2C_SCL 28 //clock for general I2C (Output)
#define I2C_SDA 29 //data for general I2C (Input/Output)

#define DST_A 19 //A for distance differential encoder (Input)
#define DST_B 20 //B for distance differential encoder (Input)
#define DST_Z 21 //Z for distance differential encoder (Input)
#define DST_ZI 8 //ZI for distance differential encoder (Input)

#define FORCE_OVLD 17 //overload status of force gauge (Input)
#define FORCE_RX 15   //force gauge data (Input)
#define FORCE_TX 16   //force gauge data (Output)

#define GPIO_SDA 26
#define GPIO_SCL 0
#define GPIO_ADDR 0
#define GPIO_DIR_MASKA 0b11110000
#define GPIO_DIR_MASKB 0b11111111

#define CHARGE_PUMP_OUT 1
#define DISTANCE_LIMIT_MAX 1
#define DISTANCE_LIMIT_MIN 2

void gpioConfig(MCP23017 *gpio);

uint8_t gpi(MCP23017 *gpio, int pin);

#endif