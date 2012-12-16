//
// OpenRoboVision
//
// object for get frames from camera (via OpenCV)
//
// объект для получения картинки с видеокамеры (через OpenCV)
//
//
// robocraft.ru
//

#ifndef _CAMERA_H_
#define _CAMERA_H_

#ifdef USE_OPENCV

#include "video_capture.h"

class Camera : public BaseVideoCapture
{
public:
	Camera();
	Camera(int camera_index);
	Camera(int width, int height);
	Camera(int camera_index, int width, int height);
	~Camera();

	// инициализация
	void init();
	// получить следующий кадр
	void update();
	// прекратить видеозахват
	void end();

	// высота/ширина кадра
	int getWidth();
	int getHeight();

	// получить параметр 
	double getParam(int paramId);
	// установить параметр
	int setParam(int paramId, double value);

private:
	CvCapture* capture;

	int device; // номер камеры в системе
	int rate;	// частота кадров
};

#endif //#ifdef USE_OPENCV

#endif //#ifndef _CAMERA_H_
