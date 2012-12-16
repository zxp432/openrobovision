//
// OpenRoboVision
//
// object for get frames from list of files
//
// объект для чтения изображений по списку файлов
//
//
// robocraft.ru
//

#ifndef _FRAME_READER_H_
#define _FRAME_READER_H_

#ifdef USE_OPENCV

#include "video_capture.h"

#include <fstream>
#include <string>

// default file name with frames list
#define FRAMEREADER_DEFAULT_FILE_NAME "frames.txt"

class Framereader : public BaseVideoCapture
{
public:
	Framereader();
	Framereader(const char* file_name, bool is_loop=false, int iscolor=CV_LOAD_IMAGE_COLOR);
	~Framereader();

	// инициализация
	void init();
	// получить следующий кадр
	void update();
	// прекратить видеозахват
	void end();

	// высота/ширина кадра
	int getWidth();
	int getHeight();

	int getFrameCounter() const;
	const char* getCurrentFileName() const;

private:
	// file name of frames list
	std::string file_name;
	std::string current_file;
	std::ifstream fs;
	int frame_counter;

	bool is_loop;
	int is_color;
};

#endif //#ifdef USE_OPENCV

#endif //#ifndef _FRAME_READER_H_
