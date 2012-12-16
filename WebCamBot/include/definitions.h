//
// определения
//

#ifndef _DEFINITIONS_H_
#define _DEFINITIONS_H_

#include "orv/orv.h"

// использовать размер кадра по-умолчанию
#define CAMERA_USE_DEFAULT_SIZE 1
// размер картинки от камеры
#define CAMERA_WIDTH 640 
#define CAMERA_HEIGHT 480

// COM-порт к которому подключён контроллер
#if defined(WIN32)
# define GIMBAL_PORT "COM5"
#elif defined(LINUX)
# define GIMBAL_PORT "/dev/ttyS0"
#endif
// скорость обмена
#define GIMBAL_RATE 9600

// номера пинов контроллера Arduino/CraftDuino к которым подключены сервы
#define GYMBAL_SERVO_PIN_AZ 8
#define GYMBAL_SERVO_PIN_EL 7

// начальные углы положения серв
#define GYMBAL_SERVO_INIT_AZ 30
#define GYMBAL_SERVO_INIT_EL 40

//
// параметры для обнаружения объекта по цвету (в пространcтве HSV)
//
#if 1
#define H_min 13
#define H_max 85
#define S_min 0
#define S_max 100
#define V_min 220
#define V_max 256
#else
#define H_min 0
#define H_max 256
#define S_min 0
#define S_max 256
#define V_min 0
#define V_max 256
#endif

// минимальное число пикселей объекта
#define BOT_THRESHOLD 5

// коэффициенты PID
#define K_P 0.05f
#define K_I 0.001f
#define K_D -0.001f
#define PID_MAXERROR 1

#endif //#ifndef _DEFINITIONS_H_
