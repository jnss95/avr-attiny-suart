#include <avr/io.h>
#include "suart.h"


int main(void)
{
	suart_init();
	//suart_get_c();
	while(1)
	{
		//char test[10];
		//suart_get_str(test,10);
		//suart_put_str(test);
		//suart_put_c(suart_get_c());
		//suart_put_c('\n');
	}
	return 1;
}
