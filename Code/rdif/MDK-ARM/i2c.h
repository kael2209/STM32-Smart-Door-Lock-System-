#ifndef I2C_H_
#define I2C_H_

#include "stm32f10x.h"

void i2c_init(void);
void i2c_start(void);
void i2c_stop(void);
void i2c_address(uint8_t addr, uint8_t RW);
void i2c_write(uint8_t data);

/* Writing Data Functions */ 
void i2c_writereg8(uint8_t addr, uint8_t reg, uint8_t data);
void i2c_writereg16(uint8_t addr, uint8_t reg, uint16_t data);

/* Reading Data Functions */ 
uint8_t i2c_readreg8(uint8_t addr, uint8_t reg);
uint16_t i2c_readreg16(uint8_t addr, uint8_t reg);
void i2c_read_multi(uint8_t addr, uint8_t reg, uint8_t *data, uint8_t length);



#endif