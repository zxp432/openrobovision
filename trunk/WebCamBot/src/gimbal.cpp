//
// объект управления системы наведения
//
// robocraft.ru
//

#include "gimbal.h"
#include "definitions.h"

Gimbal::Gimbal()
{
	arduino = new ArduinoFirmata(GIMBAL_PORT, GIMBAL_RATE);
	setup();
}

Gimbal::Gimbal(int port, int rate)
{
	arduino = new ArduinoFirmata(port, rate);
	setup();

}

Gimbal::Gimbal(const char* port_name, int rate)
{
	arduino = new ArduinoFirmata(port_name, rate);
	setup();
}

Gimbal::~Gimbal() 
{
	initServoPosition();

	if(arduino){
		delete arduino;
		arduino = 0;
	}
	if(pidAz){
		delete pidAz;
		pidAz=0;
	}
	if(pidEl){
		delete pidEl;
		pidEl=0;
	}
}

// инициализация
void Gimbal::setup()
{
	pidAz = new Pid(K_P, K_I, K_D);
	pidEl = new Pid(K_P, K_I, K_D);
	if(!pidAz || !pidEl){
		printf("[!][Gimbal] Error: cant allocate memory!\n");
	}

	pinAz = GYMBAL_SERVO_PIN_AZ;
	pinEl = GYMBAL_SERVO_PIN_EL;

	initServoPosition();
}

void Gimbal::update()
{
	
}

void Gimbal::make(CvPoint current, CvPoint target)
{
	float targetX = pidAz->make((float)target.x, (float)current.x, PID_TYPE_PID);
	float targetY = pidEl->make((float)target.y, (float)current.y, PID_TYPE_PID);

	printf("[i][Gimbal] x: %d  y: %d\n", (int)targetX, (int)targetY);

	currentAz-=(int)targetX;
	currentEl-=(int)targetY;

	printf("[i][Gimbal] x: %d  y: %d\n", currentAz, currentEl);

	// поворачиваем сервы на угол
	if(arduino){
		arduino->analogWrite(pinAz, currentAz);
		arduino->analogWrite(pinEl, currentEl);
	}
}

void Gimbal::initServoPosition()
{
	if(arduino){
		arduino->analogWrite(pinAz, GYMBAL_SERVO_INIT_AZ);
		currentAz=GYMBAL_SERVO_INIT_AZ;
		arduino->analogWrite(pinEl, GYMBAL_SERVO_INIT_EL);
		currentEl=GYMBAL_SERVO_INIT_EL;
	}
}
