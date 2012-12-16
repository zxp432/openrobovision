//
// OpenRoboVision
//
// object for PID-control
//
//
//
// robocraft.ru
//

#include "orv/robo/pid.h"

#include <stdio.h>

Pid::Pid() 
{
	setup();
}

Pid::Pid(float _kp, float _ki, float _kd)
{
	setup();
	kp=_kp;
	ki=_ki;
	kd=_kd;
}

Pid::~Pid() 
{
}

void Pid::setup()
{
	kp = 0;
	ki = 0;
	kd = 0;
	previous_error = 0;
	summ_error = 0; 
	max_error = 0;
}

float Pid::make(float current, float target, int type)
{
	// calc error
	float error = target - current;

	float P = kp * error;
	float I = 0;
	float D = 0;

	// protect from big summ error
	if(max_error>0){
		if (summ_error > max_error){
			summ_error = max_error;
		}
		else if (summ_error < -max_error){
			summ_error = -max_error;
		}
	}

	summ_error += error;

	I = ki * summ_error;
	D = kd * (error - previous_error);

	previous_error = error;

	printf("[i][PID] P: %.3f I: %.3f D: %.3f\n", P, I, D);

	switch(type)
	{
		case PID_TYPE_PID:
			return ( P + I + D);
		case PID_TYPE_P:
			return P;
		case PID_TYPE_PI:
			return ( P + I );
		case PID_TYPE_PD:
			return ( P + D );
		default:
			return ( P + I + D );
	}
	return ( P + I + D );
}
