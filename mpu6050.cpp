#ifndef MPU6050_CPP_
#define MPU6050_CPP_

#include <avr/io.h>
#include <util/delay.h>
#include <math.h>

#include "mpu_define.h"
#include "i2c.cpp"
#include "usart.cpp"


#define PI 3.14159265

class Mpu6050: public Twi
{

public:
	
struct Value
{
	volatile float ax;
	volatile float ay;
	volatile float az;
	volatile float t;
	volatile float gx;
	volatile float gy;
	volatile float gz;
	volatile float pitch;
};
Value v;


	//float ax,ay,az,t,gx,gy,gz;

	Mpu6050(void)
	{
		_delay_ms(150);
		start();
		write(MPUW); 					/// passed device address + Write
		write(SMPLRT_DIV); 				//sample rate division register
		write(0x07);					// data to be written in above register
		stop();
		
		start();
		write(MPUW); 					/// passed device address + Write
		write(PWR_MGMT_1); 				//power mgmt  1 register
		write(0x01);					// data to be written in above register
		stop();
		
		start();
		write(MPUW); 					/// passed device address + Write
		write(CONFIG); 					//config register
		write(0x00);					// data to be written in above register
		stop();
		
		start();
		write(MPUW); 					/// passed device address + Write
		write(GYRO_CONFIG); 			//gyo config register
		write(0x10); ///0x18					// data to be written in above register
		stop();
		
		start();
		write(MPUW); 					/// passed device address + Write
		write(ACCEL_CONFIG); 			//ACCEL config register
		write(0x10);		//0x18			// data to be written in above register
		stop();
		
		start();
		write(MPUW); 					/// passed device address + Write
		write(INT_ENABLE); 				//interrupt enable register
		write(0x01);					// data to be written in above register
		stop();
		
		v.pitch=0;
	}

	void burst_read(void)
	{
		start();
		write(MPUW);
		write(ACCEL_XOUT_H);
		
		start();
		write(MPUR);
		
		v.ax = (((int)read(1)<<8)|(int)read(1));
		v.ay = (((int)read(1)<<8)|(int)read(1));
		v.az = (((int)read(1)<<8)|(int)read(1));
		v.t  = (((int)read(1)<<8)|(int)read(1));
		v.gx = (((int)read(1)<<8)|(int)read(1));
		v.gy = (((int)read(1)<<8)|(int)read(1));
		v.gz = (((int)read(1)<<8)|(int)read(0));
		stop();
		
		v.ax = v.ax/4096;		//16384.0;		//gforce
		v.ay = v.ay/4096;		//16384.0;
		v.az = v.az/4096;
		
		v.gx = (((v.gx/32.8)*180)/PI);			//16.4;			//rotation ||||||//angular velocity
		v.gy = (((v.gy/32.8)*180)/PI);
		v.gz = (((v.gz/32.8)*180)/PI);
		


//		v.t = (v.t/340)+36.53;
	}
	
	void display(Usart usart)
	{
		char buffer[30]={0},value[10]={0};
		
		/*
		v.pitch = (-1*v.ax)/(sqrt(pow(v.ay,2)+pow(v.az,2)));
		v.pitch =  atan (v.pitch) * 180 / PI;
		*/
		
		dtostrf( v.ax, 3, 2, value );					// Take values in buffer to send all parameters over USART
		sprintf(buffer,"\n\rAx = %s g\t",value);
		usart.send(buffer);
		
		dtostrf( v.ay, 3, 2, value );
		sprintf(buffer," Ay = %s g \t",value);
		usart.send(buffer);
		
		dtostrf( v.az, 3, 2, value );
		sprintf(buffer," Az = %s g \n\r",value);
		usart.send(buffer);
		/*	
		dtostrf( t, 3, 2, value );
		sprintf(buffer," T = %s%cC\n\r",value,0xF8);           // 0xF8 Ascii value of degree '°' on serial 
		usart.send(buffer);
		*/
		dtostrf( v.gx, 3, 2, value );
		sprintf(buffer," Gx = %s%c/s\t",value,0xF8);
		usart.send(buffer);
		
		dtostrf(v.gy, 3, 2, value );
		sprintf(buffer," Gy = %s%c/s\t",value,0xF8);
		usart.send(buffer);
		
		dtostrf(v.gz, 3, 2, value );
		sprintf(buffer," Gz = %s%c/s\r\n",value,0xF8);
		usart.send(buffer);
		
		dtostrf(v.pitch, 3, 2, value );
		sprintf(buffer," pitch = %s Degrees\r\n",value);
		usart.send(buffer);
	}
};
#endif