#ifndef USART_CPP_
#define USART_CPP_

#include <avr/io.h>
#include <string.h>
using namespace std;


class Usart
{
public:
char buffer[30], value[10];
Usart(void)
{
	UCSRB = (1<<TXEN);
	UCSRC = (1<<UCSZ1)|(1<<UCSZ0)|(1<<URSEL);
	UBRRL = 0X67;
}

void send(char ch)
{
	while(!(UCSRA & (1<<UDRE)));
	UDR = ch;
}
void send(char *ch)
{
	/*unsigned int i = 0;
	unsigned int j = 30;//sizeof(ch);
	for(i = 0; i<=j;i++)
		send(ch[i]);*/
	unsigned int i=0;
	while(ch[i]!=0)
	{
		send(ch[i]);
		i++;
	}
}

void send(const char *ch)
{
	unsigned int i=0;
	while(ch[i]!=0)
	{
		send(ch[i]);
		i++;
	}
}

void send(int ch)
{
	char buffer[10];
	itoa(ch,buffer,10);
	send(buffer);
}
void send(unsigned int ch)
{
	char buffer[10];
	itoa(ch,buffer,10);
	send(buffer);
}
void send(float ch)
{
	dtostrf( ch, 3, 2, value );					// Take values in buffer to send all parameters over USART
	sprintf(buffer,"\n\r%s\t",value);
	send(buffer);
}
};

#endif