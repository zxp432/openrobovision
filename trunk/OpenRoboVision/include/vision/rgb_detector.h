//
// OpenRoboVision
//
// for detect objects by RGB-color
//
// для детектирования по RGB-цвету
//
//
// robocraft.ru
//

#ifndef _RGBDETECTOR_H_
#define _RGBDETECTOR_H_

#ifdef USE_OPENCV

#include <cv.h>
#include <highgui.h>
#include <stdio.h>

// показывать RGB-слои
#define RGBDETECTOR_SHOW_RGB 0
// показывать изображение после порогового преобразования
#define RGBDETECTOR_SHOW_RGB_THRESHOLD 0
// показывать изображение после сложения слоёв
#define RGBDETECTOR_SHOW_RGB_AND 0
// показать изображение после морфологического преобразования
#define RGBDETECTOR_SHOW_RGB_MORTH 0

class RGBDetector
{
	IplImage* img;
	IplImage* r_plane;
	IplImage* g_plane;
	IplImage* b_plane;
	IplImage* and_img;
	IplImage* temp;
	IplImage* morth;
	IplConvKernel* Kern;

public:
	RGBDetector();
	~RGBDetector();

	// инициализация
	void setup();
	void update();
	void make();

	// получить центр масс объекта на изображении
	bool getCenter(IplImage* image, CvPoint2D32f &center, float threshold=5);

	IplImage* result; // указатель на изображение с результатом операции

	int Rmin;
	int Rmax;

	int Gmin;
	int Gmax;

	int Bmin;
	int Bmax;

	// флаг, определящий использовать ли морфологическое преобразование
	bool useMorthology;

private:
	// выделение памяти под изображения
	void initImages(IplImage* image);
	// освобождение изображений
	void releaseImages();
	
};

#endif //#ifdef USE_OPENCV

#endif //#ifndef _RGBDETECTOR_H_
