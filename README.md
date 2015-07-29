# ATtiny25/45/85 softuart

This is a simple,small and blocking USART implementation using the
ATtiny's Timer/Counter1 and INT0 Interrupt.
So if you use this library those functions will be blocked.
The footprint with RX and TX chanel activated is around 500 bytes.

By default the library is configured to run at 9600 baud with a CPU Clock
of 8MHz.

> - PIN2 -> RX
> - PIN3 -> TX
