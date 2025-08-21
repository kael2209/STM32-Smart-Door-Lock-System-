#include "i2c.h"
#include "pcf8574.h"
#include "systick_time.h"
#include "lcd-i2c.h"

// I2c adjust 4 wire data tx
void lcd_i2c_data(unsigned char data)
{
	PCF8574_tx_byte(0x09);
	delay_us(10);
	PCF8574_tx_byte(0x0D);
	delay_us(5);
	PCF8574_tx_byte(((data & 0x00f0) | 0x0D));
	delay_us(10);
	PCF8574_tx_byte(((data & 0x00f0) | 0x09));
	
	delay_us(20);
	
	PCF8574_tx_byte(0x0D);
	delay_us(5);
	PCF8574_tx_byte((((data << 4) & 0x00f0) | 0x0D));
	delay_us(10);
	PCF8574_tx_byte((((data << 4) & 0x00f0) | 0x09));
}

void lcd_i2c_cmd(unsigned char data)
{
	PCF8574_tx_byte(0x08);
	delay_us(10);
	PCF8574_tx_byte(0x0C);
	delay_us(5);
	PCF8574_tx_byte(((data & 0x00f0) | 0x0C));
	delay_us(10);
	PCF8574_tx_byte(((data & 0x00f0) | 0x08));
	delay_us(20);
	PCF8574_tx_byte(0x0C);
	delay_us(5);
	PCF8574_tx_byte((((data << 4) & 0x00f0) | 0x0C));
	delay_us(10);
	PCF8574_tx_byte((((data << 4) & 0x00f0) | 0x08));
}

void lcd_i2c_init()
{
  i2c_init();
	delay(20);
	PCF8574_tx_byte(0x08);
	delay_us(10);
	PCF8574_tx_byte(0x0C);
	delay_us(5);
	PCF8574_tx_byte(0x3C);
	delay_us(10);

	PCF8574_tx_byte(0x38);
	
	delay(10);
	

	PCF8574_tx_byte(0x08);
	delay_us(10);
	PCF8574_tx_byte(0x0C);
	delay_us(5);
	PCF8574_tx_byte(0x3C);
	delay_us(10);
	PCF8574_tx_byte(0x38);
	
	delay(1);

	PCF8574_tx_byte(0x08);
	delay_us(10);
	PCF8574_tx_byte(0x0C);
	delay_us(5); 
	PCF8574_tx_byte(0x3C);
	delay_us(10);
	PCF8574_tx_byte(0x38);
	
	delay(1);
	PCF8574_tx_byte(0x08);
	delay_us(10);
	PCF8574_tx_byte(0x0C);
	delay_us(5);
	PCF8574_tx_byte(0x2C);
	delay_us(10);
	PCF8574_tx_byte(0x28);

	lcd_i2c_cmd(0x2C); // 4 bit communication mode / 2 lines
	delay(5);
	lcd_i2c_cmd(0x0C); // Display ON
	delay(5);
	lcd_i2c_cmd(0x01); // Clear Display
	delay(5);
	lcd_i2c_cmd(0x02); // Get back to initial address
	delay(5);
}
void lcd_i2c_send(char str[])
{
	int i = 0;
		while(str[i])
		{
			lcd_i2c_data(str[i]);
			i++;
			delay_us(100);
		}
}
void lcd_i2c_msg(unsigned char line_1_2, unsigned char pos_0_16, char msg[])
{
	short pos = 0;
	if(line_1_2==1)
	{
		pos = 0;
	}
	else if(line_1_2==2)
	{
		pos = 0x40;
	}
	lcd_i2c_cmd(0x80 +pos + pos_0_16);
	delay_us(100);
	lcd_i2c_send(msg);
}
void lcd_i2c_clear(void) 
{
    lcd_i2c_cmd(0x01); // Clear Display
    delay(2);
}
void lcd_i2c_blink(unsigned char line_1_2, unsigned char pos_0_16, char msg[],int count,int dotre)
{
	for (int i=0;i<count;i++)
	{
	lcd_i2c_msg(line_1_2,pos_0_16,msg);
	delay(dotre);
	lcd_i2c_clear();
	delay(dotre);
	}
}

