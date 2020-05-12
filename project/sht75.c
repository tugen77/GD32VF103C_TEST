#include <math.h>

#include "gd32vf103.h"
#include "systick.h"

#include "sht75.h"

#define PIN_CLK		GPIO_PIN_12
#define PIN_DAT		GPIO_PIN_15

#define noACK           0
#define ACK             1
								//adr  command  r/w

#define MEASURE_TEMP    0x03   	//000   0001    1
#define MEASURE_HUMI    0x05   	//000   0010    1
#define STATUS_REG_W    0x06   	//000   0011    0
#define STATUS_REG_R    0x07   	//000   0011    1
#define SHT_RESET       0x1E   	//000   1111    0

void sht75Init(void)
{
	rcu_periph_clock_enable(RCU_GPIOA);
	
	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, PIN_CLK | PIN_DAT);
	gpio_bit_reset(GPIOA, PIN_CLK | PIN_DAT);
}

//----------------------------------------------------------------------------------
unsigned char s_write_byte(unsigned char value)
//----------------------------------------------------------------------------------
// writes a byte on the Sensibus and checks the acknowledge
//This code correctly releases (make it an input) data pin of the PIC18uC while not in use
{
	unsigned char   i, error = 0;
	
	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, PIN_DAT);

	for(i = 0x80; i > 0; i /= 2)             //shift bit for masking
	{
		if(i & value)
			gpio_bit_set(GPIOA, PIN_DAT);			//masking value with i , write to SENSI-BUS
		else
			gpio_bit_reset(GPIOA, PIN_DAT);

		gpio_bit_set(GPIOA, PIN_CLK);		//clk for SENSI-BUS
		delay_us(10);//Delayus(5);        //pulswith approx. 5 us
		gpio_bit_reset(GPIOA, PIN_CLK);
		delay_us(1);
	}
	
	gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, PIN_DAT);
	
	gpio_bit_set(GPIOA, PIN_CLK);		//clk #9 for ack
	delay_us(3);//Delayus(3);
	error = gpio_input_bit_get(GPIOA, PIN_DAT);		//check ack (DATA will be pulled down by SHT11)
	gpio_bit_reset(GPIOA, PIN_CLK);
	delay_us(3);//Delayus(3);

	return error;                     //error=1 in case of no acknowledge
}

//----------------------------------------------------------------------------------
unsigned char s_read_byte(unsigned char ack)
//----------------------------------------------------------------------------------
// reads a byte form the Sensibus and gives an acknowledge in case of "ack=1"
{
	unsigned char     i, val = 0;
	
	gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, PIN_DAT);

	for(i = 0x80; i > 0; i /= 2)             //shift bit for masking
	{
		gpio_bit_set(GPIOA, PIN_CLK);
		delay_us(5);//Delayus(5);					//clk for SENSI-BUS
		if(gpio_input_bit_get(GPIOA, PIN_DAT))
			val = (val | i);        //read bit
		gpio_bit_reset(GPIOA, PIN_CLK);
		delay_us(3);//Delayus(3);	
	}
	
	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, PIN_DAT);
	if(ack)
	{
//		gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, PIN_DAT);
		gpio_bit_reset(GPIOA, PIN_DAT);
//		gpio_bit_set(GPIOA, PIN_DAT);
	} else {
		gpio_bit_set(GPIOA, PIN_DAT);
//		gpio_bit_reset(GPIOA, PIN_DAT);
	}
	delay_us(5);
	

	gpio_bit_set(GPIOA, PIN_CLK);			//clk #9 for ack
	delay_us(5);//Delayus(5);				//pulswith approx. 5 us
	gpio_bit_reset(GPIOA, PIN_CLK);
	
//	gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, PIN_DAT);
	
	return val;
}

//----------------------------------------------------------------------------------
void s_transstart(void)
//----------------------------------------------------------------------------------
// generates a transmission start
//       _____         ________
// DATA:      |_______|
//           ___     ___
// SCK : ___|   |___|   |______
{
	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, PIN_DAT | PIN_CLK);
	
	gpio_bit_set(GPIOA, PIN_DAT);
	gpio_bit_reset(GPIOA, PIN_CLK);
	delay_us(3);//Delayus(3);	
	gpio_bit_set(GPIOA, PIN_CLK);
	delay_us(3);//Delayus(3);
	gpio_bit_reset(GPIOA, PIN_DAT);
	delay_us(3);//Delayus(3);	
	gpio_bit_reset(GPIOA, PIN_CLK);
	delay_us(3);//Delayus(3);	
	gpio_bit_set(GPIOA, PIN_CLK);
	delay_us(3);//Delayus(3);	
	gpio_bit_set(GPIOA, PIN_DAT);
	
//	gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, PIN_DAT);
	
	delay_us(3);//Delayus(3);	
	gpio_bit_reset(GPIOA, PIN_CLK);
	delay_us(3);//Delayus(3);	
}

//----------------------------------------------------------------------------------
void s_connectionreset(void)
//----------------------------------------------------------------------------------
// communication reset: DATA-line=1 and at least 9 SCK cycles followed by transstart
//       _____________________________________________________         ________
// DATA:                                                      |_______|
//          _    _    _    _    _    _    _    _    _        ___     ___
// SCK : __| |__| |__| |__| |__| |__| |__| |__| |__| |______|   |___|   |______
{
	unsigned char   i;
	
	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, PIN_DAT | PIN_CLK);

	gpio_bit_set(GPIOA, PIN_DAT);
	gpio_bit_reset(GPIOA, PIN_CLK);
	delay_us(1);//Delayus(1);	

//	gpio_bit_reset(GPIOA, PIN_CLK);
	for(i = 0; i < 9; i++)                  //9 SCK cycles
	{
		gpio_bit_set(GPIOA, PIN_CLK);
		delay_us(3);//Delayus(3);
		gpio_bit_reset(GPIOA, PIN_CLK);
		delay_us(1);//Delayus(1);
	}
	s_transstart();                   //transmission start
}

//----------------------------------------------------------------------------------
unsigned char s_softreset(void)
//----------------------------------------------------------------------------------
// resets the sensor by a softreset
{
	unsigned char     error = 0;
	s_connectionreset();              //reset communication
	error += s_write_byte(SHT_RESET);       //send RESET-command to sensor

	return error;                     //error=1 in case of no response form the sensor
}

//----------------------------------------------------------------------------------
unsigned char s_read_statusreg(unsigned char *p_value, unsigned char *p_checksum)
//----------------------------------------------------------------------------------
// reads the status register with checksum (8-bit)
{
	unsigned char       error = 0;

	s_transstart();                   //transmission start
	error = s_write_byte(STATUS_REG_R); //send command to sensor
	*p_value = s_read_byte(ACK);        //read status register (8-bit)
	*p_checksum = s_read_byte(noACK);   //read checksum (8-bit)

	return error;                     //error=1 in case of no response form the sensor
}

//----------------------------------------------------------------------------------
unsigned char s_write_statusreg(unsigned char *p_value)
//----------------------------------------------------------------------------------
// writes the status register with checksum (8-bit)
{
	unsigned char   error = 0;
	s_transstart();                   //transmission start
	error += s_write_byte(STATUS_REG_W);//send command to sensor
	error += s_write_byte(*p_value);    //send value of status register

	return error;                     //error>=1 in case of no response form the sensor
}

//----------------------------------------------------------------------------------
unsigned char s_measure(unsigned char *p_value, unsigned char *p_checksum, unsigned char mode)
//----------------------------------------------------------------------------------
// makes a measurement (humidity/temperature) with checksum
{
	unsigned error = 0;
	unsigned int i;

	s_transstart();                   //transmission start
	switch(mode)
	{                     //send command to sensor
		case TEMP	:
			error += s_write_byte(MEASURE_TEMP);
			break;

		case HUMI	:
			error += s_write_byte(MEASURE_HUMI);
			break;
			
		default     :
			break;
	}
	
	gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, PIN_DAT);

	for(i = 0; i < 65535; i++)
    {
			delay_us(3);//Delayus(3);
			if(gpio_input_bit_get(GPIOA, PIN_DAT) == 0)
				break; //wait until sensor has finished the measurement
		}

	if(gpio_input_bit_get(GPIOA, PIN_DAT))
		error += 1;                // or timeout (~2 sec.) is reached
	*(p_value + 1) = s_read_byte(ACK);    //read the first byte (MSB)
	*(p_value) = s_read_byte(ACK);    //read the second byte (LSB)
	*p_checksum = s_read_byte(noACK);  //read checksum

	return error;
}

//----------------------------------------------------------------------------------------
void calc_sth11(float *p_humidity ,float *p_temperature)
//----------------------------------------------------------------------------------------
// calculates temperature [癈] and humidity [%RH]
// input :  humi [Ticks] (12 bit)
//          temp [Ticks] (14 bit)
// output:  humi [%RH]
//          temp [癈]
//These constants are for the sensors prior to V4
//Update these with newer values for V4 sensors

{
	const float C1 = -4.0;              // for 12 Bit
	const float C2 = +0.0405;           // for 12 Bit
	const float C3 = -0.0000028;        // for 12 Bit
	const float T1 = +0.01;             // for 14 Bit @ 5V
	const float T2 = +0.00008;           // for 14 Bit @ 5V

	float rh = *p_humidity;             // rh:      Humidity [Ticks] 12 Bit
	float t = *p_temperature;           // t:       Temperature [Ticks] 14 Bit
	float rh_lin;                     // rh_lin:  Humidity linear
	float rh_true;                    // rh_true: Temperature compensated humidity
	float t_C;                        // t_C   :  Temperature [癈]

	t_C = t * 0.01 - 40;                  //calc. temperature from ticks to [癈]
	rh_lin = C3 * rh * rh + C2 * rh + C1;     //calc. humidity from ticks to [%RH]
	rh_true = (t_C - 25) * (T1 + T2 * rh) + rh_lin;   //calc. temperature compensated humidity [%RH]
	if(rh_true > 100)
		rh_true = 100;       //cut if the value is outside of
	if(rh_true < 0.1)
		rh_true = 0.1;       //the physical possible range

	*p_temperature = t_C;               //return temperature [癈]
	*p_humidity = rh_true;              //return humidity[%RH]
}

//从相对温度和湿度计算露点
float calc_dewpoint(float h,float t)
{
       float	logEx, dew_point;
       logEx=0.66077+7.5*t/(237.3+t)+(log10(h)-2);
       dew_point= (logEx - 0.66077)*237.3/(0.66077+7.5-logEx);
       return dew_point;
} 

unsigned char sht75(int *t, unsigned int *h)
{
	sht_value       humi_val, temp_val;
	unsigned char   error, checksum;

	error = 0;
//    uartPutChar(0x5A);
	error += s_measure((unsigned char*)&humi_val.i, &checksum, HUMI);  //measure humidity
	error += s_measure((unsigned char*)&temp_val.i, &checksum, TEMP);  //measure temperature
//    uartPutChar(error);
	if(error != 0)
		s_connectionreset();                 //in case of an error: connection reset
	else
	{
		humi_val.f = (float)humi_val.i;                   //converts integer to float
		temp_val.f = (float)temp_val.i;                   //converts integer to float
		calc_sth11(&humi_val.f, &temp_val.f);            //calculate humidity, temperature
        //dew_point=calc_dewpoint(humi_val.f,temp_val.f);
		//putch('A');
		//putch('\n'); //calculate dew point
//      printf("temp:%5.2fC humi:%5.2f dew point:%5.2f\n",temp_val.f,humi_val.f,dew_point);
	}

	*t = temp_val.f * 10;
	*h = humi_val.f * 10;

	return error;
}

const unsigned char CRC_Table[256] =
{
	   0,  49,  98,  83, 196, 245, 166, 151, 185, 136, 219, 234, 125,  76,  31,  46,
	  67, 114,  33,  16, 135, 182, 229, 212, 250, 203, 152, 169,  62,  15,  92, 109,
	 134, 183, 228, 213,  66, 115,  32,  17,  63,  14,  93, 108, 251, 202, 153, 168,
	 197, 244, 167, 150,   1,  48,  99,  82, 124,  77,  30,  47, 184, 137, 218, 235,
	  61,  12,  95, 110, 249, 200, 155, 170, 132, 181, 230, 215,  64, 113,  34,  19,
	 126,  79,  28,  45, 186, 139, 216, 233, 199, 246, 165, 148,   3,  50,  97,  80,
	 187, 138, 217, 232, 127,  78,  29,  44,   2,  51,  96,  81, 198, 247, 164, 149,
	 248, 201, 154, 171,  60,  13,  94, 111,  65, 112,  35,  18, 133, 180, 231, 214,
	 122,  75,  24,  41, 190, 143, 220, 237, 195, 242, 161, 144,   7,  54, 101,  84,
	  57,   8,  91, 106, 253, 204, 159, 174, 128, 177, 226, 211,  68, 117,  38,  23,
	 252, 205, 158, 175,  56,   9,  90, 107,  69, 116,  39,  22, 129, 176, 227, 210,
	 191, 142, 221, 236, 123,  74,  25,  40,   6,  55, 100,  85, 194, 243, 160, 145,
	  71, 118,  37,  20, 131, 178, 225, 208, 254, 207, 156, 173,  58,  11,  88, 105,
	   4,  53, 102,  87, 192, 241, 162, 147, 189, 140, 223, 238, 121,  72,  27,  42,
	 193, 240, 163, 146,   5,  52, 103,  86, 120,  73,  26,  43, 188, 141, 222, 239,
	 130, 179, 224, 209,  70, 119,  36,  21,  59,  10,  89, 104, 255, 206, 157, 172
};

//
// Compute CRC of command byte and 2 rcvd bytes, and checks it against CRC stored in global struct
//
unsigned char SHT75_CheckCRC( unsigned char b1, unsigned char b2, unsigned char b3)
{
	unsigned char crc = 0;
	unsigned char tmp = 0;
	unsigned char bits= 8;

	crc = CRC_Table[crc ^ b1];
	crc = CRC_Table[crc ^ b2];
	crc = CRC_Table[crc ^ b3];

	while(bits--)
	{
		tmp >>=1;
		if( crc & 0b10000000)
			tmp |= 0b10000000;
		crc <<=1;
	}

	return tmp;

//	return (tmp==SHT75.crc);
}

unsigned int sht75Get(unsigned char mode)
{
	unsigned int        i;
	int                 error;
	unsigned char       byte0, byte1, checksum;
    
	gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, PIN_DAT);
	
	error = 0;
	s_transstart();                   //transmission start
	switch(mode)
	{                     //send command to sensor
		case TEMP :
			error += s_write_byte(MEASURE_TEMP);
			break;

		case HUMI :
			error += s_write_byte(MEASURE_HUMI);
			break;
        
		default :
			break;
	}
	for(i = 0; i < 100; i++)
	{
//		vTaskDelay(1);
		delay_ms(10);//Delayus(3);
		if(gpio_input_bit_get(GPIOA, PIN_DAT) == 0)
			break; //wait until sensor has finished the measurement
	}

	if(gpio_input_bit_get(GPIOA, PIN_DAT))
		error++;                // or timeout (~2 sec.) is reached
	byte1 = s_read_byte(ACK);    //read the first byte (MSB)
	byte0 = s_read_byte(ACK);    //read the second byte (LSB)
//    checksum = s_read_byte(ACK);    //read the second byte (LSB)
	checksum = s_read_byte(noACK);  //read checksum

	switch(mode)
	{                     //send command to sensor
		case TEMP :
			if(checksum != SHT75_CheckCRC(MEASURE_TEMP, byte1, byte0))
				error++;
			break;

		case HUMI :
			if(checksum != SHT75_CheckCRC(MEASURE_HUMI, byte1, byte0))
				error++;
			break;

		default :
			break;
	}
	if(error)
	{
		s_connectionreset();
		return 0;
	} else {
		return (byte1 << 8) | byte0;
	}

//    return error;
}

int sht75Val(unsigned int *h, int *t)
{
	float               humi_val, temp_val;

//	qprintf("%DEBUG - sht75Val \n");
	
	humi_val = (float)sht75Get(HUMI);
	temp_val = (float)sht75Get(TEMP);
//	humi_val = (float)sht75Get(HUMI);

	if((humi_val != 0) && (temp_val != 0))
	{
		calc_sth11(&humi_val, &temp_val);            //calculate humidity, temperature
		*t = temp_val * 100;
		*h = humi_val * 100;
		return 0;
	} else {
		return -1;
	}
}





