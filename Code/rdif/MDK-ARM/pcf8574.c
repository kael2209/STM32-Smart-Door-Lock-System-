#include "i2c.h"
#include "pcf8574.h"

void PCF8574_tx_byte(char data)
{
	i2c_start();
	i2c_address(0x4E,0);
	i2c_write(data);
	i2c_stop();
}
