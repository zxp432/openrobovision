//
// OpenRoboVision
//
// base object for get frames from video devices
//
// базовый объект для получения видео
//
//
// robocraft.ru
//

#include "orv/vision/video_capture.h"

BaseVideoCapture::BaseVideoCapture():
	window_name(VIDEO_CAPTURE_WINDOW_NAME),
	width(0),
	height(0),
	frame(0),
	frame_size(0),
	show_capture(true)
{
	setup();
}

BaseVideoCapture::BaseVideoCapture(int _width, int _height):
	window_name(VIDEO_CAPTURE_WINDOW_NAME),
	width(_width),
	height(_height),
	frame(0),
	frame_size(0),
	show_capture(true)
{
	setup();
}

BaseVideoCapture::~BaseVideoCapture() 
{

#ifdef USE_OPENCV
	if(intrinsic)
	{
		cvReleaseMat(&intrinsic);
	}
	if(distortion)
	{
		cvReleaseMat(&distortion);
	}
	cvReleaseImage(&mapx);
	cvReleaseImage(&mapy);
	cvReleaseImage(&undist);

	cvDestroyWindow(window_name);
#endif //#ifdef USE_OPENCV
}

void BaseVideoCapture::setup()
{
#ifdef USE_OPENCV
	intrinsic=0;
	distortion=0;
	mapx=0;
	mapy=0;
	undist=0;

	setShowCapture(true);
#else
	dontShowCapture(false);
#endif //#ifdef USE_OPENCV
}

void BaseVideoCapture::remove_frame()
{
#if defined(USE_OPENCV)
	// удаляем предыдущий кадр
	if(frame)
	{
		cvReleaseImage(&frame);
		frame = NULL;
	}
#else
	if(frame)
	{
		delete []frame;
		frame = 0;
	}
	frame_size = 0;
#endif //#if defined(USE_OPENCV)
}

// задаёт показывать ли захваченную картинку
void BaseVideoCapture::setShowCapture(bool show)
{
	show_capture=show;
	if(!show_capture)
	{
#ifdef USE_OPENCV
		cvDestroyWindow(window_name);
#endif //#ifdef USE_OPENCV
	}
	else
	{
#ifdef USE_OPENCV
		cvNamedWindow(window_name);
#endif //#ifdef USE_OPENCV
	}
}

#ifdef USE_OPENCV

// инициализация данных для устранения дисторсии
void BaseVideoCapture::initUndistortion(const char* _intrinsics, const char* _distortion)
{
	if(!frame)
		return;

	if(!_intrinsics || !_distortion)
	{
		printf("[!][BaseVideoCapture] Error: empty filename!\n");
		return;
	}
	
	//
	// загрузка из файлов 
	//
	// калибровочные коэффициенты
	intrinsic = (CvMat*)cvLoad( _intrinsics );
	// коэффициенты дисторсии
	distortion = (CvMat*)cvLoad( _distortion );

	mapx = cvCreateImage( cvGetSize( frame ), IPL_DEPTH_32F, 1 );
	mapy = cvCreateImage( cvGetSize( frame ), IPL_DEPTH_32F, 1 );
	//
	// создание карты для устранения дисторсии
	cvInitUndistortMap( intrinsic, distortion, mapx, mapy );

	// картинка для сохранения исправленной картинки
	undist = cvCloneImage(frame);
}

// получение картинки с устранённой дисторсией
IplImage* BaseVideoCapture::getUndistort()
{
	if(frame && undist && mapx && mapy)
	{
		// устраняем дисторсию через карту
		cvRemap( frame, undist, mapx, mapy);
		return undist;
	}
	return 0;
}

#endif //#ifdef USE_OPENCV
