//
// OpenRoboVision
//
// object for PID-control
//
//
//
// robocraft.ru
//

#ifndef _PID_H_
#define _PID_H_

#define PID_TYPE_PID	0
#define PID_TYPE_P		1
#define PID_TYPE_PI		2
#define PID_TYPE_PD		3

class Pid
{
public:
	Pid();
	Pid(float kp, float ki, float kd);
	~Pid();

	void setup(); 

	// коэффициенты PID
	float kp;
	float ki;
	float kd;

	// последнее значение контролируемой величины (для дифференциальной составляющей ПИД)
	float previous_error; 

	// суммарная ошибка для интегральной составляющей PID 
	float summ_error; 

	// для ограничения интегральной ошибки
	float max_error;

	// подсчёт ПИД
	// current - текущее значение
	// target - целевое значение
	// type - тип подсчёта (по-умочанию PID_TYPE_PID)
	float make(float current, float target, int type=PID_TYPE_PID);
	
private:
};

#endif //#ifndef _PID_H_
