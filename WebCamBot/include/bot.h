//
// объект веб-кам-бота
//
// robocraft.ru
//

#ifndef _BOT_H_
#define _BOT_H_

#include <cv.h>
#include <highgui.h>
#include <stdio.h>

#include "orv/vision/camera.h"
#include "orv/vision/hsv_detector.h"
#include "gimbal.h"

// название окна для вывода картинки с камеры
#define BOT_WINDOW_NAME "web-cam-bot"

class Bot
{
	// у бота есть:
	Camera* camera; // камера
	Gimbal* gimbal; // поворотная система
	HSVDetector* detector; // детектор объекта (по цвету)

	// для интерфейса:
	IplImage* image; // изображение для вывода
	const char* window_name; // название окна для отображения
	CvFont font; // для вывода информационных сообщений на картинке

public:
	Bot();
	Bot(int width, int height);
	Bot(int width, int height, int port, int rate);
	Bot(int width, int height, const char* port_name, int rate);
	~Bot();

	// инициализация
	void setup();
	void update();
	// функция получения и обработки кадра с камеры
	void make();

private:
	
};

#endif //#ifndef _BOT_H_
