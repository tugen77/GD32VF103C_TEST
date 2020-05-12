#include "gd32vf103.h"

void i2cInit(void)
{
	gpio_pin_remap_config(GPIO_I2C0_REMAP, ENABLE);
	
	/* enable GPIOB clock */
    rcu_periph_clock_enable(RCU_GPIOB);
    /* enable I2C0 clock */
    rcu_periph_clock_enable(RCU_I2C0);
	
	/* I2C0 GPIO ports */
    /* connect PB8 to I2C0_SCL */
    /* connect PB9 to I2C0_SDA */
//	gpio_init(GPIOB, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_8 | GPIO_PIN_9);
    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
	gpio_init(GPIOB, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_9);
	
	/* configure I2C0 clock */
    i2c_clock_config(I2C0, 100000, I2C_DTCY_2);
    /* enable I2C0 */
    i2c_enable(I2C0);
    /* enable acknowledge */
    i2c_ack_config(I2C0, I2C_ACK_ENABLE);
}

void i2cWrite(unsigned char slaveAddress7, unsigned char registerAddress, unsigned char length, unsigned char *data)
{
	int			i;
	
	i2c_mode_addr_config(I2C0, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, (slaveAddress7 << 1));
    i2c_ack_config(I2C0, I2C_ACK_ENABLE);
	
	/* wait until I2C bus is idle */
    while(i2c_flag_get(I2C0, I2C_FLAG_I2CBSY));
	/* send a start condition to I2C bus */
    i2c_start_on_bus(I2C0);
    /* wait until SBSEND bit is set */
    while(!i2c_flag_get(I2C0, I2C_FLAG_SBSEND));
    /* send slave address to I2C bus*/
    i2c_master_addressing(I2C0, (slaveAddress7 << 1), I2C_TRANSMITTER);
	/* wait until ADDSEND bit is set*/
    while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND));
	/* clear ADDSEND bit */
    i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
	/* send a data byte */
	i2c_data_transmit(I2C0, registerAddress);
	/* wait until the transmission data register is empty*/
	while(!i2c_flag_get(I2C0, I2C_FLAG_TBE));
	
	for(i = 0; i < length; i++)
	{
//		if(i == (length - 1))
			i2c_ack_config(I2C0, I2C_ACK_DISABLE);
        /* send a data byte */
		i2c_data_transmit(I2C0, data[i]);
		/* wait until the transmission data register is empty*/
		while(!i2c_flag_get(I2C0, I2C_FLAG_TBE));
    }
	/* send a stop condition to I2C bus*/
    i2c_stop_on_bus(I2C0);
    /* wait until stop condition generate */
    while(I2C_CTL0(I2C0)&0x0200);
//    while(!i2c_flag_get(I2C1, I2C_FLAG_STPDET));
}

void i2cRead(unsigned char slaveAddress7, unsigned char registerAddress, unsigned char length, unsigned char *data)
{
	unsigned char		i;
	
	i2c_mode_addr_config(I2C0, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, (slaveAddress7 << 1));
    i2c_ack_config(I2C0, I2C_ACK_ENABLE);
	
	/* wait until I2C bus is idle */
    while(i2c_flag_get(I2C0, I2C_FLAG_I2CBSY));
	/* send a start condition to I2C bus */
    i2c_start_on_bus(I2C0);
    /* wait until SBSEND bit is set */
    while(!i2c_flag_get(I2C0, I2C_FLAG_SBSEND));
    /* send slave address to I2C bus*/
    i2c_master_addressing(I2C0, (slaveAddress7 << 1), I2C_TRANSMITTER);
	/* wait until ADDSEND bit is set*/
    while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND));
	/* clear ADDSEND bit */
    i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
	/* wait until the transmit data buffer is empty */
    while(!i2c_flag_get(I2C0, I2C_FLAG_TBE));
	/* send a data byte */
	i2c_data_transmit(I2C0, registerAddress);
	/* wait until the transmission data register is empty*/
	while(!i2c_flag_get(I2C0, I2C_FLAG_TBE));
	/* send a stop condition to I2C bus*/
    i2c_stop_on_bus(I2C0);
    /* wait until stop condition generate */
    while(I2C_CTL0(I2C0)&0x0200);
	
////////////////////////////////////////////////////////////////////////////////////	
//    i2c_ack_config(I2C0, I2C_ACK_ENABLE);	
	/* wait until I2C bus is idle */
    while(i2c_flag_get(I2C0, I2C_FLAG_I2CBSY));
	/* send a start condition to I2C bus */
    i2c_start_on_bus(I2C0);
    /* wait until SBSEND bit is set */
    while(!i2c_flag_get(I2C0, I2C_FLAG_SBSEND));
    /* send slave address to I2C bus*/
    i2c_master_addressing(I2C0, (slaveAddress7 << 1), I2C_RECEIVER);
	/* wait until ADDSEND bit is set */
    while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND));
    /* clear ADDSEND bit */
    i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
	
	for(i = 0; i < length; i++)
	{
		if(i == (length - 1))
			i2c_ack_config(I2C0, I2C_ACK_DISABLE);
        /* wait until the RBNE bit is set */
        while(!i2c_flag_get(I2C0, I2C_FLAG_RBNE));//gpio_bit_set(GPIOA, GPIO_PIN_2);
        /* read a data from I2C_DATA */
        data[i] = i2c_data_receive(I2C0);
    }
    /* send a stop condition to I2C bus */
    i2c_stop_on_bus(I2C0);
    /* wait until stop condition generate */
    while(I2C_CTL0(I2C0)&0x0200);
	i2c_data_receive(I2C0);
    i2c_ack_config(I2C0, I2C_ACK_ENABLE);
	i2c_flag_clear(I2C1, I2C_FLAG_AERR);
}



