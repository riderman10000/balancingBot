#include<stddef.h>
#include<avr/io.h>
#include<util/delay.h>
#include<stdlib.h>
#include <inttypes.h>

#include<stdio.h>
#include<math.h>
#include<avr/interrupt.h>

//#include "i2c.cpp"
#include "mpu6050.cpp"
//#include "mpu_define.h"
#include "usart.cpp"
#include "motor.cpp"
#include "filter.cpp"

#define F_CPU 16000000L
#define F_SCL 400000L

#define PI 3.14159265

float angle;
volatile float prevAngle;
volatile int i;
Usart usart;
Mpu6050 mpu;
Filter f;
Motor m;


float checkAngle()
{
	mpu.burst_read();
	f = mpu.v;
	
	return f.getAngle();
}


int main(void)
{	    
	
	/*float rb=0.155,rw=0.035;	//radius of bot and wheel respectively
	float g = 9.8;//m/s^2
	float rps;
	//angle;
	*/
	//float x;
	TCCR2 |= (1 << WGM21) ;
	TCCR2 |= (1 << CS20) | (1<<CS21) | ( 1<<CS22);
	
	TCNT2 = 0;
	
	OCR2 = 75;//75;
	
	TIMSK |= (1 << OCIE2);
	
	sei();
	
	while(1)
	{
		mpu.display(usart);
		usart.send("\r\nmanupulated value\n\r");
		usart.send(m.mv);
		
		usart.send("\r\nangle value\n\r");
		usart.send(angle);
		
		//x= checkAngle();
		
		/*if((angle>=0)&&(x>=0))
		{
			usart.send("\r\nin backward\n\r");
			m.backward();
			//while(angle<=0);
		}
		if((angle<0)&&(x<0))
		{
			usart.send("\r\nin forward\n\r");
			m.forward();
			//while(angle>=0);
		}*/
	}
}

ISR(TIMER2_COMP_vect)
{
	mpu.burst_read();	
	f = mpu.v;
	mpu.v.pitch = f.getAngle();
	angle = mpu.v.pitch;
	m.pid(angle);
	if(angle>0)
	{
		//usart.send("\r\nin backward\n\r");
		m.backward();
		//while(angle<=0);
	}
	if(angle<0)
	{
		//usart.send("\r\nin forward\n\r");
		m.forward();
		//while(angle>=0);
	}
}
