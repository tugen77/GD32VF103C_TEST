#include "gd32vf103.h"
#include "led.h"

void ledInit(void)
{
	rcu_periph_clock_enable(RCU_GPIOB);
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, PIN_LED_RED | PIN_LED_GREEN);
	LED_RED_ON();
	LED_GREEN_ON();
}





