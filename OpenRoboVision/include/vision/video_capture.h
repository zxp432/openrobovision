//
// OpenRoboVision
//
// base object for get frames from video devices
//
// базовый объект для получения видео
//
//
// robocraft.ru
//

#ifndef _VIDEO_CAPTURE_H_
#define _VIDEO_CAPTURE_H_

#ifdef USE_OPENCV
# include <cv.h>
# include <highgui.h>
#endif //#ifdef USE_OPENCV

// название окна для вывода картинки с камеры
#define VIDEO_CAPTURE_WINDOW_NAME "capture"

class BaseVideoCapture
{
public:
	BaseVideoCapture();
	BaseVideoCapture(int width, int height);
	virtual ~BaseVideoCapture();

	// инициализация
	virtual void init() = 0;
	// получить следующий кадр
	virtual void update() = 0;
	// прекратить видеозахват
	virtual void end() = 0;

	// высота/ширина кадра
	virtual int getWidth() = 0;
	virtual int getHeight() = 0;
	
	// возвращает кадр с камеры
#ifdef USE_OPENCV
	IplImage* getFrame() {return frame;}
#else
	char* getFrame(int& size) { size = frame_size; return frame;}
#endif //#ifdef USE_OPENCV

	// задаёт показывать ли захваченную картинку
	void setShowCapture(bool show=false);

	//
	// для устранения дисторсии
	//

#ifdef USE_OPENCV

	// инициализация данных для устранения дисторсии
	void initUndistortion(const char* intrinsics="intrinsics.xml", const char* distortion="distortion.xml");
	// получение картинки с устранённой дисторсией
	IplImage* getUndistort();

#endif //#ifdef USE_OPENCV

protected:
	void setup();

	int width;	// ширина
	int height; // высота

#ifdef USE_OPENCV
	IplImage* frame;

	//
	// для устранения дисторсии
	//

	// матрицы с коэффициентами дисторсии
	// калибровочные коэффициенты
	CvMat* intrinsic;
	// коэффициенты дисторсии
	CvMat* distortion;
	// матрицы для устранения дисторсии
	IplImage* mapx;
	IplImage* mapy;
	// картинка с устранённой дисторсией
	IplImage* undist;

#else
	char* frame;
#endif //#ifdef USE_OPENCV
	int frame_size;

	// название окна
	const char* window_name;
	// флаг задающий показывать картинку или нет
	bool show_capture;
};

#endif //#ifndef _VIDEO_CAPTURE_H_
