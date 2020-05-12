#include "gd32vf103.h"

#include "misc.h"

extern unsigned char			action, samples;
extern unsigned short			Caliration;
unsigned char						uartLen, uartIdx, uartDat[16];

unsigned char			dd;

void process(unsigned char data);

void TIMER3_IRQHandler(void)
{
	if(SET == timer_interrupt_flag_get(TIMER3, TIMER_INT_FLAG_UP))
	{
		uartIdx = 0;
		/* clear channel 0 interrupt bit */
		timer_interrupt_flag_clear(TIMER3, TIMER_INT_FLAG_UP);
		timer_disable(TIMER3);
	}
}

void uartInit(void)
{
	timer_parameter_struct timer_initpara;
	
	rcu_periph_clock_enable(RCU_TIMER3);
	
	/* enable GPIO clock */
	rcu_periph_clock_enable(RCU_GPIOA);

	/* enable USART clock */
	rcu_periph_clock_enable(RCU_USART0);

	/* connect port to USARTx_Tx */
	gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);

	/* connect port to USARTx_Rx */
	gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_10);

	/* USART configure */
	usart_deinit(USART0);
	usart_baudrate_set(USART0, 9600U);
	usart_word_length_set(USART0, USART_WL_8BIT);
	usart_stop_bit_set(USART0, USART_STB_1BIT);
	usart_parity_config(USART0, USART_PM_NONE);
	usart_hardware_flow_rts_config(USART0, USART_RTS_DISABLE);
	usart_hardware_flow_cts_config(USART0, USART_CTS_DISABLE);
	usart_receive_config(USART0, USART_RECEIVE_ENABLE);
	usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
	
	/* USART interrupt configuration */
    eclic_global_interrupt_enable();
    eclic_priority_group_set(ECLIC_PRIGROUP_LEVEL3_PRIO1);
    eclic_irq_enable(USART0_IRQn, 1, 0);
	
	usart_interrupt_enable(USART0, USART_INT_RBNE);

	usart_enable(USART0);
	
	uartIdx = uartLen = 0;
	
	timer_deinit(TIMER3);
    timer_struct_para_init(&timer_initpara);
    timer_initpara.prescaler         = 9599;	//108M/10800 = 10KHz, 96MHz/9600 = 10KHz
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = (uint32_t)10 * 2; //time_interval_us;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_init(TIMER3, &timer_initpara);

	eclic_set_nlbits(ECLIC_GROUP_LEVEL3_PRIO1);
	eclic_irq_enable(TIMER3_IRQn, 1, 0);
	
    timer_interrupt_enable(TIMER3, TIMER_INT_UP);	//update interrupt
	timer_counter_value_config(TIMER3, 0);
	timer_disable(TIMER3);

	dd = 0;
}

void uartPutchar(unsigned char ch)
{
    usart_data_transmit(USART0, (unsigned char) ch );
    while(usart_flag_get(USART0, USART_FLAG_TBE)== RESET);
}

void uartPutstring(unsigned char *st)
{
	while(*st != '\0')
        uartPutchar(*(st++));
}

void USART0_IRQHandler(void)
{
	unsigned char				getCh;
	
    if(RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_RBNE))
	{
        /* receive data */
        getCh = usart_data_receive(USART0);

		timer_counter_value_config(TIMER3, 0);
		timer_enable(TIMER3);
		
		process(getCh);
    }
}

void process(unsigned char data)
{
	;

}



