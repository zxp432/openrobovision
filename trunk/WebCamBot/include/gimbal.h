//
// объект управления системы наведения
//
// robocraft.ru
//

#ifndef _GIMBAL_H_
#define _GIMBAL_H_

#include <cv.h>
#include <stdio.h>

#include "orv/robo/arduino_firmata.h"
#include "orv/robo/pid.h"

class Gimbal
{
	// ПИД для обоих серв
	Pid* pidAz;
	Pid* pidEl;

	// для работы с контроллером Arduino/CraftDuino
	ArduinoFirmata* arduino;

public:
	Gimbal();
	Gimbal(int port, int rate);
	Gimbal(const char* port_name, int rate);
	~Gimbal();

	// инициализация
	void setup();
	void update();
	void make(CvPoint current, CvPoint target);

	// начальная установка серв в заданное положение
	void initServoPosition();

	// номера пинов, к которым подключены сервы
	int pinAz;
	int pinEl;

	// текущее значение угла
	int currentAz;
	int currentEl;

private:
	
};

#endif //#ifndef _GIMBAL_H_
