//
// OpenRoboVision
//
// set of objects and methods for Robots Vision
// based on OpenCV
//
// набор объектов и методов для зрения роботов
// основанный на OpenCV
//
//
// Author: 	Vladimir  aka noonv
// email: 	noonv13@gmail.com
// site:	robocraft.ru
//

OpenRoboVision - это набор С++ классов и методов вокруг библиотеки компьютерного зрения - OpenCV.

Идеология ORV:
* просто
* быстро
* удобно

Основу ORV составляют классы и методы, которые сделаны так чтобы их можно было использовать независимо от других, т.е. просто вырезать и использовать в своих проектах.

Например, чтобы работать в OpenCV с камерой нужно написать:

//
// пример работы с камерой на OpenCV
//

#include <cv.h>
#include <highgui.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
	// получаем любую подключённую камеру
	CvCapture* capture = cvCaptureFromCAM(CV_CAP_ANY);
	assert( capture );

	//cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, 640);
	//cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, 480);

	// узнаем ширину и высоту кадра
	double width = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
	double height = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);
	printf("[i] %.0f x %.0f\n", width, height );

	IplImage* frame=0;

	cvNamedWindow("capture", CV_WINDOW_AUTOSIZE);

	printf("[i] press Enter for capture image and Esc for quit!\n\n");

	int counter=0;
	char filename[512];

	while(true){
		// получаем кадр
		frame = cvQueryFrame( capture );

		// показываем
		cvShowImage("capture", frame);
	
		char c = cvWaitKey(33);
		if (c == 27) { // нажата ESC
			break;
		}
		else if(c == 13) { // Enter
			// сохраняем кадр в файл
			sprintf(filename, "Image%d.jpg", counter);
			printf("[i] capture... %s\n", filename);
			cvSaveImage(filename, frame);
			counter++;
		}
	}
	// освобождаем ресурсы
	cvReleaseCapture( &capture );
	cvDestroyWindow("capture");
	return 0;
}

А вот как будет выглядеть эта же программа с использованием ORV:

//
// пример работы с камерой c OpenRoboVision
//

#include "orv.h"
#include "vision/camera.h"

int main(int argc, char* argv[])
{
	Camera camera(CV_CAP_ANY);

	IplImage* frame=0;

	printf("[i] press Enter for capture image and Esc for quit!\n\n");

	int counter=0;
	char filename[512];

	while(true){
		// получаем кадр
		camera.update();
		frame = camera.getFrame();
	
		char c = cvWaitKey(33);
		if (c == 27) { // нажата ESC
			break;
		}
		else if(c == 13) { // Enter
			// сохраняем кадр в файл
			sprintf(filename, "Image%d.jpg", counter);
			printf("[i] capture... %s\n", filename);
			cvSaveImage(filename, frame);
			counter++;
		}
	}
	return 0;
}

Объект Camera скрывает в себе все те операции, которые нужно было вызывать явным образом. Так же, объект сам освободит за собой используемые ресурсы.


Т.о. OpenRoboVision не подменяет собой OpenCV, а дополняет. Делая его использование более удобным.

Разумеется, в OpenCV, начиная с версии 2.0 уже есть встроенные С++ -обёртки, например:
VideoCapture
VideoWriter 
и можно использовать их, но объект Camera так же содержит функции для устранения дисторсии и может быть простым образом расширен дополнительными функциями.


Первый проект, использующий ORV (из которого собственно он и родился) - это Web-cam-bot. Простейший робот, состоящий из двух серв, контроллера Arduino/CraftDuino и дешёвой web-камеры, подключённой к ПК.

Web-cam-bot управляет сервомашинками чтобы отслеживать указатель лазерной указки.

Не считая ORV, проект состоит из сущностей Bot и Gimbal.
Bot - это собственно объект бота, который включает в себя объекты: 
Camera* camera; // камера
Gimbal* gimbal; // поворотная система
HSVDetector* detector; // детектор объекта (по цвету)


Объект Gimbal управляет сервами, подключённыйми к Arduino, по протоколу Firmata. 
Для рассчёта управляющего действия используется ПИД (объект Pid).

Объект HSVDetector позволяет по заданным параметрам HSV выделить на изображении по его цвету.

Т.о. алгоритм работы Web-cam-bot-а довольно прост:
* получить картинку с камеры ( cvCopy(camera->getFrame(), image); )
* найти центр пятна от лазерной указки ( bool result=detector->getCenter( image, point, BOT_THRESHOLD); )
* если объект обнаружен, то повернуть камеру т.о. чтобы пятно было в центре картинки ( gimbal->make( cvPoint(image->width/2, image->height/2), cvPointFrom32f(point) ); )


Т.о. набор ORV-классов позволяет быстро реализовывать задачи компьютерного зрения роботов.


Установка:
распаковать OpenRoboVision в удобную директорию.
прописать в Visual Studio или настройках проекта путь до директории с orv.h и директории с библиотекой OpenRoboVision.lib
Написать заветную строчку 
#include "orv.h"
можно начинать использовать :)
Обратите внимание, что библиотека OpenCV должна быть установлена, а Visual Studio уже настроена на работу с ней.
На данный момент поддерживаются(проверены в работе) версии OpenCV 2.0 и страше


Пример использования OpenRoboVision можно посмотреть в проекте WebCamBot.
Проекты WebCamBot и OpenRoboVision находятся в одной директории, поэтому в настройках проекта WebCamBot прописаны относительные пути:

AdditionalIncludeDirectories="../OpenRoboVision/include;"
AdditionalDependencies="./Release/orv.lib"


-= TODO =-

- добавить методы обработки изображений без OpenCV
- перенести функционал orv_image в Image
- оставить комментарии на русском только там, где это необходимо,
  в остальном - заменить их на английский
- использовать _единую_ структуру данных для работы с буферами

-= HISTORY =-

2012-06-26 - v.0.2.12
	+ в Stream и Socket добавлены методы available() и waitInput()
	+ в Serial добавлены: настройка числа бит данных, чётности, стоп-бит,
		под Linux добавлен метод setLowLatency() // нужно вызывать перед открытием порта
2012-05-18 - v.0.2.11
	+ добавлен класс для работы с сокетами (system/network.h)
	+ добавлен класс видеозахвата MJPG, способный получать данные 
		от mjpg-streamer (video/camera_mjpg.h)
2012-05-02 - v.0.2.10
	! заголовочные файлы перенесены в include/orv/
2012-04-06 - v.0.2.9
    ! начало реализации протокола ORCP для Arduino (docs/orcp)
	! robo/orv_cp.h -> robo/orcp.h
	! docs/pp-protocol.txt -> docs/orcp.txt
	! в ORCP изменён алгоритм рассчёта контрольной суммы
	(вместо CRC16 - теперь простой XOR)
2012-04-05 - v.0.2.8 
	! начало реализации ORCP - ORV Communication Protocol (robo/orv_cp.h)
2012-04-04 - v.0.2.8 
	+ добавлены методы для рассчёта CRC (system/crypto.h)
	+ добавлены методы кодирования/декодирования SLIP (system/slip.h)
	+ незначительные правки
2012-04-03 - v.0.2.7 
	+ добавлен ConfigParser (system/config_parser.h) - простой парсер 
		конфигурационных файлов вида (key=value)
	+ добавлены методы для записи логов (system/log.h)
	+ незначительные правки
2012-03-28 - v.0.2.6 
	+ добавлен абстрактный класс Stream, от которого теперь наследуется Serial
	! удалены методы Serial: Open, Close, Write, Read 
	 (оставлены только open close write read)
	- удалено поле Serial.port_number
	+ добавлен метод Serial.getLSR() (реализовано только под Linux!)
	+ незначительные правки
2012-01-01 - v.0.2.5 
	+ добавление robo/roomba system/types system/times
2011-12-29 - v.0.2.4 
	начало переноса orv_image в Image
2011-12-15 - v.0.2.3 
	+ добавлен orv::Image (vision/image.h)
2011-12-15 - v.0.2.2 
	+ добавлен Framereader (vision/frame_reader.h)
2011-11-17 - v.0.2.1 
	+ добавлен CameraV4L2 (vision/camera_v4l2.h)
2011-10-25 - v.0.2.0 
	! спустя год О_о изменена структура дерева исходных кодов
2010-10-17 - v.0.1.0 
	! начало работы непосредственно над OpenRoboVision

