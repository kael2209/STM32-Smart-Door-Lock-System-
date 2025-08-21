void lcd_i2c_data(unsigned char data);
void lcd_i2c_cmd(unsigned char data);
void lcd_i2c_init();
void lcd_i2c_send(char str[]);
void lcd_i2c_msg(unsigned char line_1_2, unsigned char pos_0_16, char msg[]);

// Hàm xóa màn hình
void lcd_i2c_clear(void) ;

void lcd_i2c_blink(unsigned char line_1_2, unsigned char pos_0_16, char msg[],int count,int dotre);

