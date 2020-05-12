/* select CAN baudrate */
/* 1MBps */
/* #define CAN_BAUDRATE  1000 */
/* 500kBps */
/* #define CAN_BAUDRATE  500 */
/* 250kBps */
#define CAN_BAUDRATE  250 
/* 125kBps */
//#define CAN_BAUDRATE  125 
/* 100kBps */ 
/* #define CAN_BAUDRATE  100 */
/* 50kBps */ 
/* #define CAN_BAUDRATE  50 */
/* 20kBps */ 
/* #define CAN_BAUDRATE  20 */

#define CAN_MAX_CHAR_IN_MESSAGE (8)

typedef struct {
	// identifier CAN_xxxID
	unsigned char		extended_identifier;
	// either extended (the 29 LSB) or standard (the 11 LSB)
	unsigned long		identifier;
	// data length:
	unsigned char  	dlc;
	unsigned char  	dta[CAN_MAX_CHAR_IN_MESSAGE];

	// used for receive only:
	// Received Remote Transfer Bit
	//  (0=no... 1=remote transfer request received)
	unsigned char  	rtr;
	// Acceptence Filter that enabled the reception
	unsigned char  	filhit;
}CanMessage;

void canInit(const unsigned short filter);
void canSend(CanMessage message);






