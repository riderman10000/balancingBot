#ifndef I2C_CPP_
#define I2C_CPP_

#include<avr/io.h>


class Twi
{
public:

	Twi(void)
	{
		TWSR = 0x00;
		TWBR = 1;
		TWCR = (1<<TWEN);
	}
	void start(void)
	{
		TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
		while((TWCR & (1<<TWINT))==0);
	}
	void write(char data)
	{
		TWDR = data;
		TWCR = (1<< TWEN)|(1<<TWINT);
		while((TWCR & (1<<TWINT))==0);
	}
	char read(uint8_t a)
	{
		if(a)												//1 for ACK
			TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);
		else												//2 for NACK
			TWCR = (1<<TWINT)|(1<<TWEN);
		while((TWCR&(1<<TWINT))==0);
		return TWDR;
	}
	void stop(void)
	{
		TWCR = (1<<TWEN)|(1<<TWSTO)|(1<<TWINT);
		while(TWCR & (1<<TWSTO));
	}
};
#endif