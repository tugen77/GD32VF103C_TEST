#include "gd32vf103.h"
#include "systick.h"

#define BACKLIGHT		GPIO_PIN_13
#define PIN_E			GPIO_PIN_12
#define PIN_WR			GPIO_PIN_13
#define PIN_RS			GPIO_PIN_14
#define PIN_DATA		(GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | \
						 GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7)

unsigned char lcd1602ReadStatus(void)
{
	unsigned char		status;
	
	gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, PIN_DATA);
	
	gpio_bit_reset(GPIOB, PIN_RS);			delay_us(5);
	gpio_bit_set(GPIOB, PIN_WR);			delay_us(5);
	gpio_bit_set(GPIOB, PIN_E);				delay_us(5);
	delay_us(50);
	gpio_bit_reset(GPIOB, PIN_E);
	
	while(gpio_input_port_get(GPIOB) & 0x0080) delay_us(5);
	status = gpio_input_port_get(GPIOB) & 0x00FF;
	
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, PIN_DATA);	
	return status;
}

void lcd1602WriteData(unsigned char data)
{
	lcd1602ReadStatus();
	
	gpio_bit_set(GPIOB, PIN_RS);		delay_us(5);
	gpio_bit_reset(GPIOB, PIN_WR);		delay_us(5);
	gpio_bit_reset(GPIOB, PIN_E);		delay_us(5);
	gpio_port_write(GPIOB, (gpio_output_port_get(GPIOB) & 0xFF00) | data);
	delay_us(50);
	gpio_bit_set(GPIOB, PIN_E);
	delay_us(80);
	gpio_bit_reset(GPIOB, PIN_E);
}

void lcd1602WriteCommand(unsigned char command, unsigned char BusyC)
{
	if(BusyC)
		lcd1602ReadStatus();
	
	gpio_bit_reset(GPIOB, PIN_RS);		delay_us(5);
	gpio_bit_reset(GPIOB, PIN_WR);		delay_us(5);
	gpio_bit_reset(GPIOB, PIN_E);		delay_us(5);
	gpio_port_write(GPIOB, (gpio_output_port_get(GPIOB) & 0xFF00) | command);
	delay_us(50);
	gpio_bit_set(GPIOB, PIN_E);
	delay_us(80);
	gpio_bit_reset(GPIOB, PIN_E);
}
				

void lcd1602Init(void)
{
	rcu_periph_clock_enable(RCU_GPIOB);
	rcu_periph_clock_enable(RCU_GPIOC);
	
    gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, BACKLIGHT);
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, PIN_E | PIN_RS | PIN_WR);
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, PIN_DATA);

    gpio_bit_set(GPIOC, BACKLIGHT);
	gpio_port_write(GPIOB, gpio_output_port_get(GPIOB) & 0xFF00);
	gpio_bit_reset(GPIOB, PIN_E);
	gpio_bit_reset(GPIOB, PIN_RS);
	gpio_bit_reset(GPIOB, PIN_WR);
	
	
//	gpio_bit_reset(GPIOB, PIN_E);
//	gpio_bit_reset(GPIOB, PIN_RS);
//	gpio_bit_reset(GPIOB, PIN_WR);
//	gpio_port_write(GPIOB, (gpio_output_port_get(GPIOB) & 0xFF00) | 0xAA);
//	while(1);
	
	lcd1602WriteCommand(0x38, 0);
	delay_ms(5);
	lcd1602WriteCommand(0x38, 0);
	delay_ms(5);
	lcd1602WriteCommand(0x38, 0);
	delay_ms(5);
	
	lcd1602WriteCommand(0x38, 1);
	delay_ms(5);
	lcd1602WriteCommand(0x08, 1);
	delay_ms(5);
	lcd1602WriteCommand(0x01, 1);
	delay_ms(5);
	lcd1602WriteCommand(0x06, 1);
	delay_ms(5);
	lcd1602WriteCommand(0x0C, 1);	
}

void lcd1602DisplayChar(unsigned char X, unsigned char Y, unsigned char data)
{
	Y &= 0x01;
	X &= 0x0F;
	if(Y)
		X |= 0x40;
	X |= 0x80;
	if(data > 0x19)
	{
		lcd1602WriteCommand(X, 0);
		lcd1602WriteData(data);
	}
}

//void lcd1602DisplayLine(unsigned char line, unsigned char *data)
//{
//	int 	i;
//	
//	for(i = 0; i < 16; i++)
//		lcd1602DisplayChar(i, 0, data[i]);
//}


void lcd1602DisplayString(unsigned char X, unsigned char Y, unsigned char *data)
{
	unsigned char		length;
	
	length = 0;
	Y &= 0x01;
	X &= 0x0F;
	while(data[length] > 0x19)
	{
		if(X <= 0x0F)
		{
			lcd1602DisplayChar(X, Y, data[length]);
			length++;
			X++;
		}
	}
}




