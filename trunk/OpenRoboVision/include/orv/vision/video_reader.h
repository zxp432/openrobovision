//
// OpenRoboVision
//
// object for get frames from video-file
//
// объект дл€ чтени€ видео-файлов
//
//
// robocraft.ru
//

#ifndef _VIDEO_READER_H_
#define _VIDEO_READER_H_

#ifdef USE_OPENCV

#include <cv.h>
#include <highgui.h>
#include <stdio.h>

// название окна дл€ вывода картинки из файла
#define VIDEOREADER_CAPTURE_WINDOW_NAME "video read"

class Videoreader
{
	CvCapture* capture;
	IplImage* frame;
	const char* window_name; // название окна дл€ отображени€ картинки
	bool show_capture; // флаг определ€ющий показывать картинку или нет
public:
	Videoreader();
	Videoreader(char* filename);
	Videoreader(char* filename, int width, int height);
	~Videoreader();

	// инициализаци€
	void setup();
	// получить следующий кадр
	void update();
	// возвращает кадр из файла
	IplImage* getFrame() {return frame;}

	// задаЄт показывать ли захваченную картинку
	void setShowCapture(bool show=false);

	// название файла
	char* filename;

private:
};

#endif //#ifdef USE_OPENCV

#endif //#ifndef _VIDEO_READER_H_
