#include <avr/io.h>

#define SUART_RX
#define SUART_TX

void suart_init(void);

#ifdef SUART_TX
	#define SUART_TX_PORT PORTB
	#define SUART_TX_PIN PB3
	#define SUART_TX_DDR DDRB
	static volatile uint16_t m_outbuffer;
	void suart_put_c(char data);
	void suart_put_str(char *data);
#endif

#ifdef SUART_RX
	#define SUART_RX_PIN PINB
	#define SUART_RX_PORT PORTB
	#define SUART_RX_BIT PB2
	#define SUART_RX_DDR DDRB

	static volatile uint8_t m_inbuffer;
	static volatile uint8_t m_inbits;

	char suart_get_c(void);
	void suart_get_str(char *buff,uint8_t maxLen);
#endif


