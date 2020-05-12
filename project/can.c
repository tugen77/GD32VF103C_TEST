#include <string.h>

#include "gd32vf103.h"

#include "can.h"

#define MAX_MAIL_NUM  3

static unsigned char CAN_msg_num[MAX_MAIL_NUM]; 

void canInit(const unsigned short filter)
{
	can_parameter_struct 			can_parameter; 
	can_filter_parameter_struct 	can_filter;
	
	/* enable CAN clock */
	rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_CAN0);
 //   rcu_periph_clock_enable(RCU_AF);
	
	/* configure CAN0 GPIO */
    gpio_init(GPIOA, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_11);
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12);
//    gpio_pin_remap_config(GPIO_CAN0_FULL_REMAP, ENABLE);
	
	eclic_global_interrupt_enable();
	eclic_priority_group_set(ECLIC_PRIGROUP_LEVEL2_PRIO2);
    /* configure CAN0 CLIC */
    eclic_irq_enable(CAN0_RX0_IRQn, 1, 0);
	
	can_struct_para_init(CAN_INIT_STRUCT, &can_parameter);
    can_struct_para_init(CAN_INIT_STRUCT, &can_filter);
    /* initialize CAN register */
    can_deinit(CAN0);
	
	/* initialize CAN parameters */
    can_parameter.time_triggered = DISABLE;
    can_parameter.auto_bus_off_recovery = DISABLE;
    can_parameter.auto_wake_up = DISABLE;
    can_parameter.no_auto_retrans = DISABLE;
    can_parameter.rec_fifo_overwrite = DISABLE;
    can_parameter.trans_fifo_order = DISABLE;
    can_parameter.working_mode = CAN_NORMAL_MODE;
    can_parameter.resync_jump_width = CAN_BT_SJW_1TQ;
    can_parameter.time_segment_1 = CAN_BT_BS1_13TQ;		//CAN_BT_BS1_5TQ;
    can_parameter.time_segment_2 = CAN_BT_BS2_2TQ;		//CAN_BT_BS2_3TQ;
    
    /* 1MBps */
#if CAN_BAUDRATE == 500
    can_parameter.prescaler = 6;
    /* 500KBps */
#elif CAN_BAUDRATE == 250
    can_parameter.prescaler = 12;
    /* 250KBps */
#elif CAN_BAUDRATE == 125
    can_parameter.prescaler = 24;
#else
    #error "please select list can baudrate in private defines in main.c "
#endif  
	
    /* initialize CAN */
    can_init(CAN0, &can_parameter);
	
	/* initialize filter */ 
	can_filter.filter_number = 0;
    can_filter.filter_mode = CAN_FILTERMODE_MASK;
    can_filter.filter_bits = CAN_FILTERBITS_32BIT;
    can_filter.filter_list_high = (((unsigned long)filter << 21) & 0xFFFF0000) >> 16;
    can_filter.filter_list_low = (((unsigned long)filter << 21)) & 0xFFFF;
    can_filter.filter_mask_high = 0xFFFF;
    can_filter.filter_mask_low = 0xFFFF;
    can_filter.filter_fifo_number = CAN_FIFO0;
    can_filter.filter_enable = ENABLE; 
    can_filter_init(&can_filter);
	
	can_interrupt_enable(CAN0, CAN_INT_RFNE0);	
	
	memset(CAN_msg_num,0,MAX_MAIL_NUM);
}

void canSend(CanMessage message)
{
	can_trasnmit_message_struct transmit_message;
	unsigned char				qz;
	uint8_t 					TransmitMailbox = 0;
	
	/* initialize transmit message */
    transmit_message.tx_sfid = message.identifier;
    transmit_message.tx_efid = 0;
    transmit_message.tx_ft = CAN_FT_DATA;
    transmit_message.tx_ff = CAN_FF_STANDARD;
    transmit_message.tx_dlen = message.dlc;
	memset(transmit_message.tx_data, 0x00, sizeof(transmit_message.tx_data));
	for(qz = 0; qz < transmit_message.tx_dlen; qz++)
		transmit_message.tx_data[qz] = message.dta[qz];
	
	TransmitMailbox = can_message_transmit(CAN0, &transmit_message);
	
	if(CAN_NOMAILBOX == TransmitMailbox)
		return 0;
	else        
		CAN_msg_num[TransmitMailbox] = 1;
	
	return 1;
}

void CAN0_RX0_IRQHandler(void)
{
	can_receive_message_struct 	receive_message;
	CanMessage      			message;
	unsigned char				qz;
	
    /* check the receive message */
    can_message_receive(CAN0, CAN_FIFO0, &receive_message);
    
//    if((0x300>>1 == receive_message.rx_sfid)&&(CAN_FF_STANDARD == receive_message.rx_ff)&&(2 == receive_message.rx_dlen))
	if(CAN_FF_STANDARD == receive_message.rx_ff)
	{
        ;//can0_receive_flag = SET; 
		message.identifier = receive_message.rx_sfid;
        message.dlc = receive_message.rx_dlen;
        for(qz = 0; qz < message.dlc; qz++)
            message.dta[qz] = receive_message.rx_data[qz];
//        commRecv(message);
//uartPutchar(receive_message);
    }else{
        ;//can0_error_flag = SET; 
    }
}




