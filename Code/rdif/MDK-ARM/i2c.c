#include "stm32f10x.h"
#include "i2c.h"

void i2c_init(void) {
    // Enable clock for I2C1 and GPIOB
    RCC->APB1ENR |= (1<<21); // I2C1 clock
    RCC->APB2ENR |= (1<<3);  // GPIOB clock

    // Configure PB6 (SCL) and PB7 (SDA) as Alternate Function Open-Drain
    GPIOB->CRL &= ~((0xF<<24) | (0xF<<28));
    GPIOB->CRL |= (0xF<<24) | (0xF<<28); // AF Open-Drain, 50 MHz

    // Software reset I2C1
    I2C1->CR1 |= (1<<15);
    I2C1->CR1 &= ~(1<<15);

    // Configure I2C (assuming PCLK1 = 8 MHz)
    I2C1->CR2 = 8;    // APB1 = 8 MHz
    I2C1->CCR = 45;   // 100 kHz
    I2C1->TRISE = 9;  // Max rise time
    I2C1->CR1 |= (1<<0); // Enable I2C1
}

void i2c_start(void){
    I2C1->CR1 |= (1<<8); // Generate START
    while (!(I2C1->SR1 & (1<<0))); // Wait for SB (Start Bit)
}

void i2c_stop(void){
    I2C1->CR1 |= (1<<9); // Generate STOP
}

void i2c_address(uint8_t addr, uint8_t RW){
    I2C1->DR = (addr << 0) | RW; // Shift address and add R/W bit /***********/
    while (!(I2C1->SR1 & (1<<1))); // Wait for ADDR
    if (I2C1->SR1 & (1<<10)) { // Check NACK
        I2C1->CR1 |= (1<<9); // Generate STOP
        I2C1->SR1 &= ~(1<<10); // Clear AF
        // Note: Error handling can be customized if needed
    }
    (void)I2C1->SR1; // Clear ADDR by reading SR1
    (void)I2C1->SR2; // and SR2
}

void i2c_write(uint8_t data){
    while (!(I2C1->SR1 & (1<<7))); // Wait for TxE
    I2C1->DR = data; // Write data
    while (!(I2C1->SR1 & (1<<2))); // Wait for BTF (Byte Transfer Finished)
}

void i2c_writereg8(uint8_t addr, uint8_t reg, uint8_t data){
    i2c_start();
    i2c_address(addr, 0); // Write mode
    i2c_write(reg);
    i2c_write(data);
    i2c_stop();
}

void i2c_writereg16(uint8_t addr, uint8_t reg, uint16_t data){
    i2c_start();
    i2c_address(addr, 0); // Write mode
    i2c_write(reg);
    i2c_write((data >> 8) & 0xFF); // High byte
    i2c_write(data & 0xFF); // Low byte
    i2c_stop();
}

uint8_t i2c_readreg8(uint8_t addr, uint8_t reg){
    uint8_t data;
    i2c_start();
    i2c_address(addr, 0); // Write mode
    i2c_write(reg); // Send register address
    i2c_start(); // Repeated start
    i2c_address(addr, 1); // Read mode
    I2C1->CR1 &= ~(1 << 10); // Clear ACK for NACK
    while (!(I2C1->SR1 & (1 << 6))); // Wait for RxNE
    data = I2C1->DR;
    i2c_stop();
    return data;
}

uint16_t i2c_readreg16(uint8_t addr, uint8_t reg){
    uint16_t data;
    i2c_start();
    i2c_address(addr, 0); // Write mode
    i2c_write(reg); // Send register address
    i2c_start(); // Repeated start
    i2c_address(addr, 1); // Read mode
    I2C1->CR1 |= (1 << 10); // Enable ACK for first byte
    while (!(I2C1->SR1 & (1 << 6))); // Wait for first byte
    uint8_t msb = I2C1->DR;
    I2C1->CR1 &= ~(1 << 10); // Clear ACK for NACK
    while (!(I2C1->SR1 & (1 << 6))); // Wait for second byte
    uint8_t lsb = I2C1->DR;
    i2c_stop();
    data = (msb << 8) | lsb;
    return data;
}

void i2c_read_multi(uint8_t addr, uint8_t reg, uint8_t *data, uint8_t length){
    i2c_start();
    i2c_address(addr, 0); // Write mode
    i2c_write(reg); // Send register address
    i2c_start(); // Repeated start
    i2c_address(addr, 1); // Read mode

    for (uint8_t i = 0; i < length; i++) {
        if (i == length - 1) {
            I2C1->CR1 &= ~(1 << 10); // Clear ACK for NACK on last byte
        } else {
            I2C1->CR1 |= (1 << 10); // Enable ACK for all but last byte
        }
        while (!(I2C1->SR1 & (1 << 6))); // Wait for RxNE
        data[i] = I2C1->DR;
    }
    i2c_stop();
}