#define PIN_LED_RED			GPIO_PIN_8
#define PIN_LED_GREEN		GPIO_PIN_9

#define LED_RED_ON()			gpio_bit_set(GPIOB, PIN_LED_RED)
#define LED_RED_OFF()			gpio_bit_reset(GPIOB, PIN_LED_RED)
#define LED_RED_TOGGLE()		gpio_bit_write(GPIOB, PIN_LED_RED, !gpio_output_bit_get(GPIOB, PIN_LED_RED))

#define LED_GREEN_ON()			gpio_bit_set(GPIOB, PIN_LED_GREEN)
#define LED_GREEN_OFF()			gpio_bit_reset(GPIOB, PIN_LED_GREEN)
#define LED_GREEN_TOGGLE()		gpio_bit_write(GPIOB, PIN_LED_GREEN, !gpio_output_bit_get(GPIOB, PIN_LED_GREEN))

void ledInit(void);






