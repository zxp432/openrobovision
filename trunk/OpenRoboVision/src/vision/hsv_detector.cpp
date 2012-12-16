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

#ifdef USE_OPENCV

#include "orv/vision/hsv_detector.h"

HSVDetector::HSVDetector()
{
	setup();
}

HSVDetector::~HSVDetector() 
{
	releaseImages();
}

// инициализация
void HSVDetector::setup()
{
	img=0;
	hsv=0;
	h_plane=0;
	s_plane=0;
	v_plane=0;
	and_img=0;
	temp=0;
	morth=0;
	Kern=0;

	useSrange = true;
	useMorthology = true;

	result=0;
}

void HSVDetector::update()
{
	
}

void HSVDetector::make()
{
	
}

// получить центр масс объекта на изображении
bool HSVDetector::getCenter(IplImage* image, CvPoint2D32f &center, float threshold)
{
	bool res = false;

	center = cvPoint2D32f(0, 0);

	if(!image)
	{
		printf("[!][HSVDetector] Error: iamge is null!\n");
		return res;
	}

	int x=0, y=0;

	// если вызывается первый раз - выделяем память под изображения
	if(img==0)
	{
		initImages(image);
	}
	else
	{
		// проверим - не изменился ли размер картинки
		CvSize s1 = cvGetSize(image);
		CvSize s2 = cvGetSize(img);

		// если размер изменился - выделяем память снова
		if(s1.height!=s2.height || s1.width!=s2.width)
		{
			printf("[!][HSVDetector] Hmmmm: image have new size!\n");
			releaseImages();
			initImages(image);
		}
	}

	//  конвертируем в HSV 
	cvCvtColor(image, hsv, CV_BGR2HSV); 
		
	// разбиваем на плоскости
	cvCvtPixToPlane(hsv, h_plane, s_plane, v_plane, 0);

#if HSVDETECTOR_SHOW_HSV
	cvNamedWindow("H", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("S", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("V", CV_WINDOW_AUTOSIZE);

	cvShowImage( "H", h_plane );
	cvShowImage( "S", s_plane );
	cvShowImage( "V", v_plane );

	// заодно покажем минимальное и максимальное значение слоя
	double framemin=0;
	double framemax=0;

	cvMinMaxLoc(h_plane, &framemin, &framemax);
	printf("[i][HSVDetector] H %f x %f\n", framemin, framemax );
	cvMinMaxLoc(s_plane, &framemin, &framemax);
	printf("[i][HSVDetector] S %f x %f\n", framemin, framemax );
	cvMinMaxLoc(v_plane, &framemin, &framemax);
	printf("[i][HSVDetector] V %f x %f\n", framemin, framemax );
#endif //#if HSVDETECTOR_SHOW_HSV

	// бинаризация H-слоя - получаем чёрно-белую картинку с белыми областями подходящими под заданные параметры (+шум)
	cvInRangeS(h_plane, cvScalar(Hmin), cvScalar(Hmax), h_plane);
	// аналогично для S- и V- слоёв - выбираем данные в заданном интервале
	if(useSrange){
		cvInRangeS(s_plane, cvScalar(Smin), cvScalar(Smax), s_plane);
	}
	cvInRangeS(v_plane, cvScalar(Vmin), cvScalar(Vmax), v_plane);

#if HSVDETECTOR_SHOW_HSV_THRESHOLD
	cvNamedWindow("treshold H", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("treshold S", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("treshold V", CV_WINDOW_AUTOSIZE);

	cvShowImage( "treshold H", h_plane );
	cvShowImage( "treshold S", s_plane );
	cvShowImage( "treshold V", v_plane );
#endif //#if HSVDETECTOR_SHOW_HSV_THRESHOLD

	// выполняем AND изображений
	if(useSrange)
	{
		cvAnd(h_plane, s_plane, and_img);
		cvAnd(and_img, v_plane, and_img);
	}
	else
	{
		cvAnd(h_plane, v_plane, and_img);
	}

#if HSVDETECTOR_SHOW_HSV_AND
	cvNamedWindow("treshold AND",CV_WINDOW_AUTOSIZE);
	cvShowImage( "treshold AND", and_img);
#endif //#if HSVDETECTOR_SHOW_HSV_AND

	result = 0;
	if(useMorthology)
	{
		// осуществляем морфологическое преобразование
		cvMorphologyEx(and_img, morth, temp, Kern, CV_MOP_OPEN, 1);
		result = morth;
	}
	else
	{
		result = and_img;
	}

#if HSVDETECTOR_SHOW_HSV_MORTH
	cvNamedWindow("morth", CV_WINDOW_AUTOSIZE);
	cvShowImage( "morth", morth);
#endif //#if HSVDETECTOR_SHOW_HSV_MORTH

	//
	// находим центр масс
	//

	int Xc = 0;
	int Yc = 0;
	int counter = 0; // счётчик числа белых пикселей

	// пробегаемся по пикселям изображения
	for(y=0; y<result->height; y++)
	{
		uchar* ptr = (uchar*) (result->imageData + y * result->widthStep);
		for(x=0; x<result->width; x++)
		{
			if( ptr[x]>0 )
			{
				Xc += x;
				Yc += y;
				counter++; 
			}
		}
	}
	
	if(counter!=0)
	{
		center.x = float(Xc)/counter;
		center.y = float(Yc)/counter;
	}

	if(counter>threshold)
	{
		res = true;
	}

	return res;
}

// выделение памяти под изображения
void HSVDetector::initImages(IplImage* image)
{
	if(!image)
	{
		printf("[!][HSVDetector] Error: iamge is null!\n");
		return;
	}

	img = cvCreateImage( cvGetSize(image), IPL_DEPTH_8U, 3);
	hsv = cvCreateImage( cvGetSize(image), IPL_DEPTH_8U, 3);
	h_plane = cvCreateImage( cvGetSize(image), IPL_DEPTH_8U, 1 );
	s_plane = cvCreateImage( cvGetSize(image), IPL_DEPTH_8U, 1 );
	v_plane = cvCreateImage( cvGetSize(image), IPL_DEPTH_8U, 1 );
	and_img = cvCreateImage( cvGetSize(image), IPL_DEPTH_8U, 1 );
	temp = cvCreateImage( cvGetSize(image), IPL_DEPTH_8U, 1 );
	morth = cvCreateImage( cvGetSize(image), IPL_DEPTH_8U, 1 );

	int radius = 1;
	IplConvKernel* Kern = cvCreateStructuringElementEx(radius*2+1, radius*2+1, radius, radius, CV_SHAPE_ELLIPSE);
}

// освобождение изображений
void HSVDetector::releaseImages()
{
	cvReleaseImage(&img);
	cvReleaseImage(&hsv);
	cvReleaseImage(&h_plane);
	cvReleaseImage(&s_plane);
	cvReleaseImage(&v_plane);
	cvReleaseImage(&and_img);
	cvReleaseImage(&temp);
	cvReleaseImage(&morth);
	if(Kern!=0)
	{
		cvReleaseStructuringElement(&Kern);
		Kern=0;
	}
}

#endif //#ifdef USE_OPENCV
