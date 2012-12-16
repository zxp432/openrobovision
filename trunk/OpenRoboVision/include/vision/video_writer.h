//
// OpenRoboVision
//
// object for writes frames to video-file
//
// объект дл€ записи видео-файлов
//
//
// robocraft.ru
//

#ifndef _VIDEO_WRITER_H_
#define _VIDEO_WRITER_H_

#ifdef USE_OPENCV

#include <cv.h>
#include <highgui.h>
#include <stdio.h>

// название окна дл€ вывода картинки из файла
#define VIDEOWRITER_CAPTURE_WINDOW_NAME "video write"
// частота по-умолчанию
#define VIDEOWRITER_DEFAULT_FPS 15
// кодек по-умолчанию
#define VIDEOWRITER_DEFAULT_CODEC CV_FOURCC('X','V','I','D')
//
// 4-буквенный код кодека дл€ обработки видео:
//
// CV_FOURCC('X','V','I','D')	 = кодек XviD
// CV_FOURCC('P','I','M','1')    = MPEG-1
// CV_FOURCC('M','J','P','G')    = motion-jpeg  (does not work well)
// CV_FOURCC('M', 'P', '4', '2') = MPEG-4.2 
// CV_FOURCC('D', 'I', 'V', '3') = MPEG-4.3 
// CV_FOURCC('D', 'I', 'V', 'X') = MPEG-4 
// CV_FOURCC('U', '2', '6', '3') = H263 
// CV_FOURCC('I', '2', '6', '3') = H263I 
// CV_FOURCC('F', 'L', 'V', '1') = FLV1 

class Videowriter
{
	CvVideoWriter *writer;
	
	const char* window_name; // название окна дл€ отображени€ картинки
	bool show_capture; // флаг определ€ющий показывать картинку или нет
public:
	Videowriter();
	Videowriter(char* filename);
	Videowriter(char* filename, int width, int height);
	Videowriter(char* filename, CvSize size, double fps=VIDEOWRITER_DEFAULT_FPS, int codec=VIDEOWRITER_DEFAULT_CODEC, int is_color=true);
	~Videowriter();

	// инициализаци€
	void setup();

	// записать кадр в файл
	void write(IplImage* image);
	// завершить запись
	void end();

	// задаЄт показывать ли захваченную картинку
	void setShowCapture(bool show=false);

	CvSize size;	// размер кадра
	double fps;		// частота
	int codec;		// кодек (fourcc)
	char* filename;	// название файла
	bool isColor;	// цветна€ картинка

private:
};

#endif //#ifdef USE_OPENCV

#endif //#ifndef _VIDEO_WRITER_H_
