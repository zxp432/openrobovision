//
// OpenRoboVision
//
// object for get frames from video-file
//
// объект для чтения видео-файлов
//
//
// robocraft.ru
//

#ifdef USE_OPENCV

#include "orv/vision/video_reader.h"

Videoreader::Videoreader():
window_name(VIDEOREADER_CAPTURE_WINDOW_NAME),
filename(0)
{
	show_capture = true;
	setup();
}

Videoreader::Videoreader(char* _filename):
window_name(VIDEOREADER_CAPTURE_WINDOW_NAME),
filename(0)
{
	filename = _filename;
	show_capture = true;
	setup();
}

Videoreader::Videoreader(char* _filename, int width, int height):
window_name(VIDEOREADER_CAPTURE_WINDOW_NAME),
filename(0)
{
	filename = _filename;
	show_capture = true;
	setup();

	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, width);
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, height);
}

Videoreader::~Videoreader() 
{
	cvReleaseCapture(&capture);
	cvDestroyWindow(window_name);
}

// инициализация
void Videoreader::setup()
{
	frame = 0;

	if(!filename)
	{
		printf("[!][Videoreader] Error: empty filename!\n");
		return;
	}

	this->capture = cvCreateFileCapture(filename);
	if (!capture)
	{
		printf("[!][Videoreader] Error: cant open file!\n");
		return;
	}

	// узнаем ширину и высоту кадра
	double _width = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
	double _height = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);
	printf("[i][Videoreader] width: %.0f height: %.0f\n", _width, _height );

	// получаем число кадров
	double framesCount = cvGetCaptureProperty( capture, CV_CAP_PROP_FRAME_COUNT);
	printf("[i][Videoreader] frames count: %.0f\n", framesCount);


	frame = 0;
	cvNamedWindow(window_name);
}

void Videoreader::update()
{
	frame = cvQueryFrame( capture );
	if (!frame)
	{
		printf("[!][Videoreader] Error: cant get frame!\n");
		return;
	}
	if(show_capture)
	{
		cvShowImage(window_name, frame);
	}
}

void Videoreader::setShowCapture(bool show)
{
	show_capture=show;
	if(!show_capture)
	{
		cvDestroyWindow(window_name);
	}
	else
	{
		cvNamedWindow(window_name);
	}
}

#endif //#ifdef USE_OPENCV
