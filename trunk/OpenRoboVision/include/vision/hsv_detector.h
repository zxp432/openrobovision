//
// OpenRoboVision
//
// for detect objects by HSV-color
//
// для детектирования по цвету в пространстве HSV
//
//
// robocraft.ru
//

#ifndef _HSV_DETECTOR_H_
#define _HSV_DETECTOR_H_

#ifdef USE_OPENCV

#include <cv.h>
#include <highgui.h>
#include <stdio.h>

//
// параметры для отладки
//

// показывать изображения HSV-слоёв
#define HSVDETECTOR_SHOW_HSV 0 
// показывать изображение после порогового преобразования
#define HSVDETECTOR_SHOW_HSV_THRESHOLD 0
// показывать изображение после сложения слоёв
#define HSVDETECTOR_SHOW_HSV_AND 0
// показать изображение после морфологического преобразования
#define HSVDETECTOR_SHOW_HSV_MORTH 1

class HSVDetector
{
	IplImage* img;
	IplImage* hsv;
	IplImage* h_plane;
	IplImage* s_plane;
	IplImage* v_plane;
	IplImage* and_img;
	IplImage* temp;
	IplImage* morth;
	IplConvKernel* Kern;

public:
	HSVDetector();
	~HSVDetector();

	// инициализация
	void setup();
	void update();
	void make();

	// получить центр масс объекта на изображении
	bool getCenter(IplImage* image, CvPoint2D32f &center, float threshold=5);

	IplImage* result; // указатель на изображение с результатом операции

	int Hmin;
	int Hmax;

	int Smin;
	int Smax;

	int Vmin;
	int Vmax;

	// флаг, определящий использовать ли значения S-слоя
	bool useSrange;
	// флаг, определящий использовать ли морфологическое преобразование
	bool useMorthology;

private:
	// выделение памяти под изображения
	void initImages(IplImage* image);
	// освобождение изображений
	void releaseImages();
	
};

#endif //#ifdef USE_OPENCV

#endif //#ifndef _HSV_DETECTOR_H_
