#ifndef MOTOR_CPP_
#define MOTOR_CPP_

#include<avr/io.h>
#include<util/delay.h>
#include<math.h>

#define PI 		3.14159265

class Motor
{
public:
	volatile float kp,ki,kd,p,i,d;
	volatile float last_error,proportional,derivative,integral;
	volatile float mv;
	volatile float time;
	volatile float minout,maxout,imin,imax;
	//float thres;
	
	Motor(void)
	{
		DDRB |= 0xFF;
		DDRD |= 0xFF;
		
		PORTB = 0x00;
		PORTD = 0x00;
		
		TCCR1A |= (1<<WGM11) | (1<<COM1A1)| (1<<COM1A0)| (1<<COM1B1)| (1<<COM1B0);
		TCCR1B |= (1<<WGM12) | (1<<WGM13)|(1<<CS10);
		ICR1 =255;
		//thres = ICR1 - 150;
		OCR1A = 0;
		OCR1B = 0;
		
		
		kp = 1.62;      //1.62
		ki = 1024;		//1024
		kd = 455;		//285
		time = 0.005;
		
		minout = 100;
		maxout = 255;
		
		imax =  0.1*(maxout-minout)/ki;
		imin = -0.1*(maxout-minout)/ki;
		
		last_error = 0;
		integral = 0;
	}
	void forward()
	{
		PORTB &= ~(1<<PB1);
		PORTB &= ~(1<<PB3);
		PORTB |= (1<<PB0) | (1<<PB2);
	}
	void backward()
	{	
		PORTB &= ~(1<<PB0);
		PORTB &= ~(1<<PB2);
		PORTB |= (1<<PB1) | (1<<PB3);
	}
	void stop(void)
	{
		PORTB = 0x00;
	}
	void pid(float error)
	{
		
		if(error < 0)
			error = -1*error;
		
		if(error<9)
		{
			error = (((maxout - minout)/20)*error)+minout;
			
			
			
			proportional = error;
			derivative = error - last_error;
			//integral = integral+proportional;
			last_error = error;
			////////////////conditions////////////////
			///completly hawa bakwas tori logics//////
			//////////////////////////////////////////
			
			p = kp * proportional;
			
			//p = (((maxout - minout)/20)*p)+minout;
			
			if(p>=maxout)
				p = maxout;
			else if(p<=minout)
				p = minout;
			
			if((integral+error)>imax)
				integral = imax;
			else if((integral+error)<imin)
				integral = imin;
			else
				integral = integral+error;
			
			i = ki * integral;
			d = kd * derivative;
			
			mv = (p + i *time + d*time);
			
			//mv = (15*kp/20)* mv;
		}
		else 
			mv = 255;
		
		OCR1A = ICR1 -mv; //-minout-mv;
		OCR1B = ICR1 -mv; //-minout-mv;
	}
};
#endif
