#include <avr/io.h>
#include <avr/interrupt.h>
#include "suart.h"
#include <util/delay.h>

#define nop() __asm volatile ("nop")

void suart_init(void)
{
	cli(); // Disable Interrupts
	//SETUP TX
#ifdef SUART_TX
	SUART_TX_PORT |= (1<<SUART_TX_PIN); //Pull TX High
	SUART_TX_DDR |= (1<<SUART_TX_PIN); //Enable TX as Output
	OCR1A = (F_CPU/16)/BAUD;
#endif

	TCCR1 |= (1<<CTC1) | //Reset timer1 on OCR1C Match
		(1 << CS10) | (1 << CS12); // Prescaler Fosc/16
	OCR1C = (F_CPU/16)/BAUD;

	//SETUP RX
#ifdef SUART_RX
	SUART_RX_PORT |= (1<<SUART_RX_PIN); //Pull RX High
	SUART_RX_DDR &= ~(1<<SUART_RX_PIN); //Set RX as Input
	MCUCR |= (1<<ISC01); //Trigger INT0 on falling Edge
#endif
	sei(); //Enable Interrupts
}

#ifdef SUART_TX
SIGNAL (SIG_OUTPUT_COMPARE1A)
{
	uint16_t data = m_outbuffer;
	if (data & 0x1)
	{
		SUART_TX_PORT |= (1<<SUART_TX_PIN);
	}
	else
	{
		SUART_TX_PORT &= ~(1<<SUART_TX_PIN);
	}
	if(data == 0x1)
	{
		TIMSK &= ~(1<<OCIE1A);
	}
	m_outbuffer = data >> 1;
}

void suart_put_c(char data)
{
	//Wait for current transmission to finish
	do{
		sei(); nop(); cli();
	} while(m_outbuffer);
	m_outbuffer = (3 << 9) | (((uint8_t)data) << 1); //1S 8D 1S
	TIFR = (1<< OCF1A); //Reset OCF1A
	TIMSK |= (1<<OCIE1A); //Enable OCIE1A Interrupt
	do{
		sei(); nop(); cli();
	} while(m_outbuffer);
}


void suart_put_str(char *data)
{
	while(*data != '\0'){
		suart_put_c(*data);
		++data;
	}
}
#endif

#ifdef SUART_RX
SIGNAL (SIG_OUTPUT_COMPARE1B)
{
	uint8_t bits = m_inbits;
	if (bits > 0 && bits < 0x9){ //Skip start and stop bits
		uint8_t data = m_inbuffer >> 1;
		if (SUART_RX_PIN & (1<<SUART_RX_BIT))
			data |= (1<<7);
		m_inbuffer = data;
	}
	bits +=1;
	if (bits == 0xA)
	{
		TIMSK &= ~(1<<OCIE1B);
	}
	m_inbits = bits;
}

SIGNAL (SIG_INTERRUPT0)
{
	uint8_t bitTim = OCR1C;

	uint8_t ocr1b = TCNT1 + bitTim/2;
	if(ocr1b >= bitTim)
		ocr1b -= bitTim;
	OCR1B = ocr1b; //Set OCR1B trigger value
	GIMSK &= ~(1<<INT0); //Disable INT0 interrupt
	TIFR = (1<<OCF1B); //Reset OC1B interrupt flag
	TIMSK |= (1<<OCIE1B); //Enable OCIE1B interrupt
}


char suart_get_c(void)
{
	m_inbuffer = 0x00; //Rest inbuffer
	m_inbits = 0x00; //Rest read bits
	GIFR = (1<<INTF0); //Reset INT0 Interrupt Flag
	GIMSK |= (1<<INT0); //Enable INT0 Interrupt
	do
	{
		sei(); nop();cli();
	} while(m_inbits < 0xA); //Wait for char
	return (char)m_inbuffer; //Return read char
}

void suart_get_str(char* buff, uint8_t maxLen)
{
	uint8_t nextChar;
	uint8_t strLen = 0;

	nextChar = suart_get_c();
	while(nextChar != '\n' && strLen < maxLen -1){
		*buff++ = nextChar;
		strLen++;
		nextChar = suart_get_c();
	}
	*buff = '\0';
}
#endif

