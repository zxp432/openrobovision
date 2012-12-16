//
// объект веб-кам-бота
//
// robocraft.ru
//

#include "bot.h"
#include "definitions.h"

Bot::Bot():
	camera(0),
	gimbal(0),
	detector(0),
	window_name(BOT_WINDOW_NAME)
{
	camera = new CameraV4L2();
	gimbal = new Gimbal();
	detector = new HSVDetector();
	if(!camera || !gimbal || !detector){
		printf("[!][Bot] Error: cant allocate memory!\n");
		return;
	}
	setup();
}

Bot::Bot(int width, int height):
	window_name(BOT_WINDOW_NAME)
{
	camera = new CameraV4L2(DEFAULT_V4L2_DEV_NAME, width, height);
	gimbal = new Gimbal();
	detector = new HSVDetector();
	if(!camera || !gimbal || !detector){
		printf("[!][Bot] Error: cant allocate memory!\n");
		return;
	}
	setup();
}

Bot::Bot(int width, int height, int port, int rate):
	window_name(BOT_WINDOW_NAME)
{

#if CAMERA_USE_DEFAULT_SIZE
	camera = new CameraV4L2();
#else
	camera = new CameraV4L2(DEFAULT_V4L2_DEV_NAME, width, height);
#endif // #if CAMERA_USE_DEFAULT_SIZE

	gimbal = new Gimbal(port, rate);
	detector = new HSVDetector();
	if(!camera || !gimbal || !detector){
		printf("[!][Bot] Error: cant allocate memory!\n");
		return;
	}
	setup();
}

Bot::Bot(int width, int height, const char* port_name, int rate):
	window_name(BOT_WINDOW_NAME)
{

#if CAMERA_USE_DEFAULT_SIZE
	camera = new CameraV4L2();
#else
	camera = new CameraV4L2(DEFAULT_V4L2_DEV_NAME, width, height);
#endif // #if CAMERA_USE_DEFAULT_SIZE

	gimbal = new Gimbal(port_name, rate);
	detector = new HSVDetector();
	if(!camera || !gimbal || !detector){
		printf("[!][Bot] Error: cant allocate memory!\n");
		return;
	}
	setup();
}

Bot::~Bot() 
{
	if(gimbal){
		delete gimbal;
		gimbal = 0;
	}
	if(camera){
		delete camera;
		camera = 0;
	}
	if(detector){
		delete detector;
		detector = 0;
	}
	cvReleaseImage(&image);
	cvDestroyWindow(window_name);
}

// инициализация
void Bot::setup()
{
	image=0;
	cvNamedWindow(window_name);

	cvInitFont(&font, CV_FONT_HERSHEY_PLAIN, 1.5, 1.5);

	// отключим показ картинки с камеры
	camera->setShowCapture(false);

	// устанавливаем коэффициенты для детектирования объекта по цвету (HSV)
	if(detector){
		detector->Hmin = H_min;
		detector->Hmax = H_max;
		detector->Smin = S_min;
		detector->Smax = S_max;
		detector->Vmin = V_min;
		detector->Vmax = V_max;

		// не будем использовать проверку в S-слое
		detector->useSrange = false;
	}
}

void Bot::update()
{
	
}

void Bot::make()
{
	camera->update();
	if(camera->getFrame()){
		//
		// получаем кадр с камеры
		//
		if(image==0){
			image = cvCloneImage(camera->getFrame());
		}
		else{
			cvCopy(camera->getFrame(), image);
		}

		// для хранения центра объекта
		CvPoint2D32f point;

		//
		// передаём кадр на обработку детектору
		//
		bool result = detector->getCenter( image, point, BOT_THRESHOLD);

		char buf[128]; // для вывода информации

		if(result){
			//
			// объект обнаружен
			//

			printf("[i][Bot] point: %.2f : %.2f \n", point.x, point.y);

			if(gimbal){
				gimbal->make( cvPoint(image->width/2, image->height/2), cvPointFrom32f(point) );
			}

			// отметим центр
			cvCircle(image, cvPointFrom32f(point), 3, CV_RGB(0, 255, 0), 2);

			// выводим сообщение в верхнем левом углу картинки
			sprintf(buf, "Object detected: %.2f : %.2f", point.x, point.y);
			cvPutText(image, buf, cvPoint(10, 20), &font, CV_RGB(0, 255, 0));
		}
		else{
			//
			// объект не обнаружен
			//

			// выводим сообщение в верхнем левом углу картинки
			sprintf(buf, "Cant find object!");
			cvPutText(image, buf, cvPoint(10, 20), &font, CV_RGB(255 , 0, 0));
		}

		cvShowImage(window_name, image);
	}
}
