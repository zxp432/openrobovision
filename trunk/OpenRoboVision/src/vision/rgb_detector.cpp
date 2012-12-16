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

#ifdef USE_OPENCV

#include "orv/vision/rgb_detector.h"

RGBDetector::RGBDetector()
{
	setup();
}

RGBDetector::~RGBDetector() 
{
	releaseImages();
}

// инициализация
void RGBDetector::setup()
{
	img=0;
	r_plane=0;
	g_plane=0;
	b_plane=0;
	and_img=0;
	temp=0;
	morth=0;
	Kern=0;

	useMorthology = true;

	result=0;
}

void RGBDetector::update()
{
	
}

void RGBDetector::make()
{
	
}

// получить центр масс объекта на изображении
bool RGBDetector::getCenter(IplImage* image, CvPoint2D32f &center, float threshold)
{
	bool res = false;

	center = cvPoint2D32f(0, 0);

	if(!image)
	{
		printf("[!][RGBDetector] Error: iamge is null!\n");
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
			printf("[!][RGBDetector] Hmmmm: image have new size!\n");
			releaseImages();
			initImages(image);
		}
	}

	// разбиваем на плоскости
	cvSplit( image, b_plane, g_plane, r_plane, 0 );

#if RGBDETECTOR_SHOW_RGB
	cvNamedWindow("R", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("G", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("B", CV_WINDOW_AUTOSIZE);

	cvShowImage( "R", r_plane );
	cvShowImage( "G", g_plane );
	cvShowImage( "B", b_plane );

	// заодно покажем минимальное и максимальное значение слоя
	double framemin=0;
	double framemax=0;

	cvMinMaxLoc(r_plane, &framemin, &framemax);
	printf("[i][RGBDetector] R %f x %f\n", framemin, framemax );
	cvMinMaxLoc(g_plane, &framemin, &framemax);
	printf("[i][RGBDetector] G %f x %f\n", framemin, framemax );
	cvMinMaxLoc(b_plane, &framemin, &framemax);
	printf("[i][RGBDetector] B %f x %f\n", framemin, framemax );
#endif //#if RGBDETECTOR_SHOW_RGB

	// бинаризация R-слоя - получаем чёрно-белую картинку с белыми областями подходящими под заданные параметры (+шум)
	cvInRangeS(r_plane, cvScalar(Rmin), cvScalar(Rmax), r_plane);
	// аналогично для G- и B- слоёв - выбираем данные в заданном интервале
	cvInRangeS(g_plane, cvScalar(Gmin), cvScalar(Gmax), g_plane);
	cvInRangeS(b_plane, cvScalar(Bmin), cvScalar(Bmax), b_plane);

# if RGBDETECTOR_SHOW_RGB_THRESHOLD
	cvNamedWindow("treshold R", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("treshold G", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("treshold B", CV_WINDOW_AUTOSIZE);

	cvShowImage( "treshold R", r_plane );
	cvShowImage( "treshold G", g_plane );
	cvShowImage( "treshold B", b_plane );
# endif //#if RGBDETECTOR_SHOW_RGB_THRESHOLD

	// выполняем AND изображений
	cvAnd(r_plane, g_plane, and_img);
	cvAnd(and_img, b_plane, and_img);

#if RGBDETECTOR_SHOW_RGB_AND
	cvNamedWindow("treshold AND",CV_WINDOW_AUTOSIZE);
	cvShowImage( "treshold AND", and_img);
#endif //#if RGBDETECTOR_SHOW_RGB_AND

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

#if RGBDETECTOR_SHOW_RGB_MORTH
	cvNamedWindow("morth", CV_WINDOW_AUTOSIZE);
	cvShowImage( "morth", morth);
#endif //#if RGBDETECTOR_SHOW_RGB_MORTH

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
void RGBDetector::initImages(IplImage* image)
{
	if(!image)
	{
		printf("[!][RGBDetector] Error: iamge is null!\n");
		return;
	}

	img = cvCreateImage( cvGetSize(image), IPL_DEPTH_8U, 3);
	r_plane = cvCreateImage( cvGetSize(image), IPL_DEPTH_8U, 1 );
	g_plane = cvCreateImage( cvGetSize(image), IPL_DEPTH_8U, 1 );
	b_plane = cvCreateImage( cvGetSize(image), IPL_DEPTH_8U, 1 );
	and_img = cvCreateImage( cvGetSize(image), IPL_DEPTH_8U, 1 );
	temp = cvCreateImage( cvGetSize(image), IPL_DEPTH_8U, 1 );
	morth = cvCreateImage( cvGetSize(image), IPL_DEPTH_8U, 1 );

	int radius = 1;
	IplConvKernel* Kern = cvCreateStructuringElementEx(radius*2+1, radius*2+1, radius, radius, CV_SHAPE_ELLIPSE);
}

// освобождение изображений
void RGBDetector::releaseImages()
{
	cvReleaseImage(&img);
	cvReleaseImage(&r_plane);
	cvReleaseImage(&g_plane);
	cvReleaseImage(&b_plane);
	cvReleaseImage(&and_img);
	cvReleaseImage(&temp);
	cvReleaseImage(&morth);
	if(Kern!=0
		){
		cvReleaseStructuringElement(&Kern);
		Kern=0;
	}
	result=0;
}

#endif //#ifdef USE_OPENCV
