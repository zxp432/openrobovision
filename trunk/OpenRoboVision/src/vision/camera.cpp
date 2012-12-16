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

#include "orv/vision/camera.h"

Camera::Camera():
	capture(0),
	device(CV_CAP_ANY)
{
	init();
}

Camera::Camera(int camera_index):
	capture(0),
	device(CV_CAP_ANY)
{
	show_capture = true;
	device = camera_index;
	init();
}

Camera::Camera(int _width, int _height):
	capture(0),
	device(CV_CAP_ANY)
{
	show_capture = true;
	init();

	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, _width);
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, _height);
}

Camera::Camera(int camera_index, int _width, int _height):
	capture(0),
	device(CV_CAP_ANY)
{
	show_capture = true;
	device = camera_index;
	init();

	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, _width);
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, _height);
}

Camera::~Camera() 
{
	this->end();
}

// инициализация
void Camera::init()
{
	frame = 0;
	width = 0;
	height = 0;

	this->capture = cvCreateCameraCapture(device);
	if (!capture)
	{
		printf("[!][Camera] Error: cant open camera!\n");
		return;
	}

	// узнаем ширину и высоту кадра
	double _width = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
	double _height = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);
	double _rate = cvGetCaptureProperty(capture, CV_CAP_PROP_FPS); // 0
	printf("[i][Camera] width: %.0f height: %.0f\n", _width, _height );
	width = (int)_width;
	height = (int)_height;
	rate = (int)_rate;
}

void Camera::update()
{
	frame = cvQueryFrame( capture );
	if (!frame)
	{
		printf("[!][Camera][update] Error: cant get frame!\n");
		return;
	}
	if(show_capture)
	{
		cvShowImage(window_name, frame);
	}
}

// прекратить видеозахват
void Camera::end()
{
	cvReleaseCapture(&capture);
}

int Camera::getWidth()
{
	if(capture)
	{
		double _width = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
		width = (int)_width;
	}
	return width;
}

int Camera::getHeight()
{
	if(capture)
	{
		double _height = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);
		height = (int)_height;
	}
	return height;
}

// получить параметр 
double Camera::getParam(int paramId)
{
	if (!capture)
	{
		printf("[!][Camera] Error: empty capture!\n");
		return -1;
	}
	return cvGetCaptureProperty(capture, paramId);
}

// установить параметр
int Camera::setParam(int paramId, double value)
{
	if (!capture)
	{
		printf("[!][Camera] Error: empty capture!\n");
		return -1;
	}
	return cvSetCaptureProperty(capture, paramId, value);
}
