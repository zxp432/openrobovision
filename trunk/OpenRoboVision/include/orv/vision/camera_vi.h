//
// OpenRoboVision
//
// object for get frames from camera via videoInput library
//
// объект для получения картинки с видеокамеры
// через библиотеку videoInput
//
//
// robocraft.ru
//

#ifndef _CAMERAVI_H_
#define _CAMERAVI_H_

#if defined(USE_OPENCV) && defined(WIN32) && defined(USE_VIDEOINPUT_LIB)

#include "video_capture.h"

#include <videoInput.h>

//
// для линковки добавить  файл библиотеки: videoInput.lib 
// в командную строку линковщика добавить строчку: /NODEFAULTLIB:"atlthunk"
// 
// добавить для линковки файл библиотеки: videoInput.lib 
#ifdef _MSC_VER
# pragma message ("Using library: videoInput.lib")
# pragma comment(lib, "videoInput.lib")
# pragma message ("Add to linker string:  /NODEFAULTLIB:\"atlthunk\"")
#endif

class CameraVI : public BaseVideoCapture
{
public:
	CameraVI();
	CameraVI(int width, int height, int rate=0, int device=0);
	~CameraVI();

	// инициализация
	void init();
	// получение следующего кадра
	void update();
	// прекратить видеозахват
	void end();

	// высота/ширина кадра
	int getWidth();
	int getHeight();

	// показать окошко с настройками камеры
	void showCameraSettings();

private:

	videoInput VI;

	int device; // номер камеры в системе
	int rate;	// частота кадров
};

#endif //#if defined(USE_OPENCV) && defined(WIN32) && defined(USE_VIDEOINPUT_LIB)

#endif // #ifndef _CAMERAVI_H_
