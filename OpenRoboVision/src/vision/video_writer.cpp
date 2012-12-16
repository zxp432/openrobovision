//
// OpenRoboVision
//
// object for writes frames to video-file
//
// объект для записи видео-файлов
//
//
// robocraft.ru
//

#ifdef USE_OPENCV

#include "orv/vision/video_writer.h"

Videowriter::Videowriter():
window_name(VIDEOWRITER_CAPTURE_WINDOW_NAME),
filename(0),
writer(0),
fps(VIDEOWRITER_DEFAULT_FPS),
codec(VIDEOWRITER_DEFAULT_CODEC),
isColor(true)
{
	show_capture = true;
	setup();
}

Videowriter::Videowriter(char* _filename):
window_name(VIDEOWRITER_CAPTURE_WINDOW_NAME),
filename(0),
writer(0),
fps(VIDEOWRITER_DEFAULT_FPS),
codec(VIDEOWRITER_DEFAULT_CODEC),
isColor(true)
{
	filename = _filename;
	show_capture = true;
	setup();
}

Videowriter::Videowriter(char* _filename, int width, int height):
window_name(VIDEOWRITER_CAPTURE_WINDOW_NAME),
filename(0),
writer(0),
fps(VIDEOWRITER_DEFAULT_FPS),
codec(VIDEOWRITER_DEFAULT_CODEC),
isColor(true)
{
	filename = _filename;
	show_capture = true;

	size = cvSize(width, height);

	setup();
}

Videowriter::Videowriter(char* _filename, CvSize _size, double _fps, int _codec, int is_color):
window_name(VIDEOWRITER_CAPTURE_WINDOW_NAME),
filename(0),
writer(0)
{
	filename = _filename;
	show_capture = true;

	size = _size;
	fps = _fps;
	codec = _codec;

	isColor=false;
	if(is_color)
	{
		isColor=true;
	}

	setup();
}

Videowriter::~Videowriter() 
{
	this->end();
	cvDestroyWindow(window_name);
}

// инициализация
void Videowriter::setup()
{
	if(!filename)
	{
		printf("[!][Videowriter] Error: empty filename!\n");
		return;
	}

	int is_color = 0;
	if(isColor)
	{
		is_color = 1;
	}

	// инициализация записи видео в файл
	writer = cvCreateVideoWriter(filename, codec, fps, size, is_color);
	if (!writer)
	{
		printf("[!][Videowriter] Error: cant create writer!\n");
		return;
	}

	cvNamedWindow(window_name);
}

// записать кадр в файл
void Videowriter::write(IplImage* image)
{
	if(!image)
	{
		printf("[!][Videowriter] Error: empty image!\n");
		return;
	}

	if (!writer)
	{
		printf("[!][Videowriter] Error: empty writer!\n");
		return;
	}

	// добавляем кадр в видео-файл
	cvWriteFrame(writer, image);
}

// завершить запись
void Videowriter::end()
{
	// закрываем видео-файл
	cvReleaseVideoWriter(&writer); 
	writer = 0;
}

void Videowriter::setShowCapture(bool show)
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
