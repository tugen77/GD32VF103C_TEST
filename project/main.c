#include <string.h>

#include "gd32vf103.h"
#include "systick.h"
#include "uart.h"
#include "led.h"
#include "adc.h"
#include "misc.h"
#include "can.h"

extern uint16_t adc_value[8];

int main(void)
{
	unsigned long		Voltage;
	CanMessage 			message;
	unsigned char		i, cnt;
	
	ledInit();
	uartInit();
	adcInit();
	canInit(0x150);

	message.identifier = 0x1;
	message.dlc = 8;
	cnt = 0;
	while(1)
	{
		/* ADC software trigger enable */
//        adc_software_trigger_enable(ADC0, ADC_REGULAR_CHANNEL);
		LED_RED_TOGGLE();
		delay_ms(500);
//		Voltage = (unsigned long)adc_value[0] * 3000 / 4096;
//		qprintf("ADC0 regular channel 0 data = %d \r\n",Voltage);
//		qprintf("ADC0 regular channel 0 data = %d \r\n",adc_value[0]);
//		qprintf("ADC0 regular channel 1 data = %d \r\n",adc_value[1]);
//		qprintf("ADC0 regular channel 2 data = %d \r\n",adc_value[2]);
//		qprintf("ADC0 regular channel 3 data = %d \r\n",adc_value[3]);

		for(i = 0; i < 8; i++)
			message.dta[i] = cnt + i;
		canSend(message);

		cnt++;
	}
	
	return 0;
}




