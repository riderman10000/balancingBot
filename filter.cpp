#ifndef FILTER_CPP_
#define FILTER_CPP_

#include <avr/io.h>
#include <math.h>

#include "mpu6050.cpp"

#define SMPLRT 	100 //hz
#define CFLP	260   //hz
#define CFHP	256   //hz
#define PI 		3.14159265



class Filter
{
public:
	struct Value
	{
		volatile float ax;
		volatile float ay;
		volatile float az;
		volatile float gx;
		volatile float gy;
		volatile float gz;
		volatile float pitch;
	}vv;
	
	float accel_Angle, gyro_previousAngle, gyro_presentAngle,alphaCof,taulp,tauhp;
	
	float alphalp,alphahp;
	
	float thetalp0,thetalp1,thetahp0,thetahp1;
	
	unsigned int previousTime, presentTime;
	
	Filter(void)
	{
		taulp= (1/(2*PI*CFLP));
		tauhp= (1/(2*PI*CFHP));
		alphalp = (SMPLRT/(taulp+SMPLRT));
		alphahp = (tauhp/(tauhp+SMPLRT));
		alphaCof = 0.29; //0.2
		TCCR0 |= (1<<CS00);
		TCNT0 = 0;
		
		gyro_previousAngle = 0;
		previousTime = 0;
		thetalp0 = 0;
		thetahp0 = 0;
		
		//for calculation of alphaCoF 
		//A = (CFLP +CFHP)/2
		// alphaCoF = (A/(A+SMPLRT))
		//it may give us some errors so it's better to tune it yourself i think 
	}
	
	float getAngle(void)
	{
		
		calculateAngles();
		
		vv.pitch = getCofAngle();
		update();
		/*if(TIFR & 0x01)
		{
			TIFR &= ~(1<<TOV0);
			TCNT0 = 0;
			previousTime = 0;
		}*/
		return vv.pitch;
	}
	void calculateAngles()
	{
		//update of present time remaining
		presentTime += TCNT0;//=timer1 value
		
		//accel_Angle = (-1*vv.ax)/(sqrt(pow(vv.ay,2)+pow(vv.az,2)));
		
		//accel_Angle =  atan2(vv.ax,vv.az)*180/ PI;
		
		//accel_Angle = atan2((-1*vv.ax),sqrt((vv.ay*vv.ay)+(vv.az*vv.az)));
		//accel_Angle = atan2(vv.ay,vv.az);
		//accel_Angle =  ((accel_Angle) * 180 )/ PI;
		
		//gyro_presentAngle = gyro_previousAngle +( vv.gx*(presentTime - previousTime));
		//gyro_presentAngle = (atan(gyro_presentAngle)*180) / PI;
		//gyro_presentAngle = gyro_previousAngle +( (vv.gx*180/ PI)*(presentTime - previousTime));
		
		
		//accel_Angle = atan2(vv.ay,vv.az)*180/PI;
		//gyro_presentAngle = gyro_previousAngle +( vv.gy*(presentTime - previousTime));
		//gyro_presentAngle = (atan(gyro_presentAngle)*180) / PI;
		
		accel_Angle = atan2(vv.ax,vv.az)*180/PI;
		gyro_presentAngle = gyro_previousAngle +( vv.gy*(presentTime - previousTime));
		//gyro_presentAngle = ((gyro_presentAngle)*180) / PI;
		
	}
	void update()
	{
		gyro_previousAngle = gyro_presentAngle;
		previousTime = presentTime;
	}
	
	float getlpAngle()
	{
		thetalp1 = ((accel_Angle * alphalp)+(thetalp0*(1-alphalp)));
		thetalp0 = thetalp1;
		return thetalp1;
	}
	float gethpAngle()
	{
		thetahp1 = ((thetahp0+gyro_presentAngle-gyro_previousAngle)*alphahp);
		thetahp0 = thetahp1;
		return thetahp1;
	}
	float getCofAngle()
	{
		
		return ((alphaCof*getlpAngle())+((1-alphaCof)*gethpAngle()));
	}
	void operator=(Mpu6050::Value v)
	{
		vv.ax = v.ax;
		vv.ay = v.ay;
		vv.az = v.az;
		vv.gx = v.gx;
		vv.gy = v.gy;
		vv.gz = v.gz;
		vv.pitch = v.pitch;
	}
	float standardDeviation(float a[])
	{
		float mean = 0,sum = 0;
		int i = 0;
		for(i = 0;i<4;i++)
			mean += a[i];
		for(i = 0 ; i<4;i++)
			a[i] -= mean;
		for(i = 0 ; i<4;i++)
			a[i] = pow(a[i],2);
		for(i = 0 ; i<4;i++)
			sum += a[i];
		sum = sqrt(sum/4);
		return sum;
	}
};
#endif