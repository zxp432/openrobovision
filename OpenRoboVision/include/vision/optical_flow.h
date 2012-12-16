//
// OpenRoboVision
//
// optical flow (piramid Lucas-Kanade)
//
// оптический поток (пирамидальный Лукас-Канаде)
//
//
// robocraft.ru
//

#ifndef _OPTICAL_FLOW_H_
#define _OPTICAL_FLOW_H_

#ifdef USE_OPENCV

# include <cv.h>
# include <highgui.h>

#define LK_MAX_CORNERS 100
#define LK_WINDOW_SIZE 10

// максимальной число итераций
#define LK_ITER_COUNT 20
#define LK_MAX_FEATURE_ERROR 500 //550

class OpticalFlowLK
{
	// первое изображение
	IplImage* imgA;
	// второе изображение
	IplImage* imgB;
	// временные изображения для нахождения точек для отслеживания
	IplImage* eig_image;
	IplImage* tmp_image;
	// для пирамиды первого изображения
	IplImage* pyrA;
	// для пирамиды второго изображения
	IplImage* pyrB;

public:
	OpticalFlowLK();
	~OpticalFlowLK();

	// инициализация
	void init();

	// установка изображений для сравнения
	void setA(IplImage* src);
	void setB(IplImage* src);
	IplImage* getA(){return imgA;}
	IplImage* getB(){return imgB;}
	void releaseAB();

	// поиск оптического потока
	void make();

	// показать результат
	void show();

	// для хранения координат углов
	CvPoint2D32f* cornersA;
	CvPoint2D32f* cornersB;

	// статус точки - найдена или нет
	char* featuresFound;
	// ошибка поиска
	float* featureErrors;

	// количество найденных углов
	int cornerCount;

	// центр масс нового положения
	CvPoint2D32f center;
	// количество точек, использованных для подсчёта центра масс
	int cornerCountGood;

private:

};

#endif //#ifdef USE_OPENCV

#endif //#ifndef _OPTICAL_FLOW_H_
