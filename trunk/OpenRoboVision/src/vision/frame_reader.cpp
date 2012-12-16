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

#include "orv/vision/frame_reader.h"

Framereader::Framereader():
	file_name(FRAMEREADER_DEFAULT_FILE_NAME),
	frame_counter(0),
	is_loop(false),
	is_color(CV_LOAD_IMAGE_COLOR)
{
	show_capture = true;
	init();
}

Framereader::Framereader(const char* _file_name, bool _is_loop, int iscolor):
	file_name(_file_name),
	frame_counter(0),
	is_loop(_is_loop),
	is_color(iscolor)
{
	show_capture = true;
	init();
}

Framereader::~Framereader() 
{
	this->end();
}

// инициализация
void Framereader::init()
{
	frame = 0;
	width = 0;
	height = 0;

	if(file_name.c_str()){
		fs.open(file_name.c_str());
	}
	if (!fs)
	{
		printf("[!][Framereader] Error: cant open file: %s!\n", file_name.c_str());
		return;
	}
}

void Framereader::update()
{
	current_file = "";

	if(fs.eof())
	{
		if(is_loop)
		{
			// return to begin
			fs.seekg(0, std::ios_base::beg);
			frame_counter = 0;
		}
		else
		{
			printf("[!][Framereader] End of file!\n");
			return;
		}
	}

	// read image file name
	fs >> current_file;
	printf("[i] Read frame %d from %s\n", ++frame_counter, current_file.c_str());

	if(frame)
	{
		cvReleaseImage(&frame);
		frame = 0;
	}
	// load image
	frame = cvLoadImage(current_file.c_str(), is_color);
	if (!frame)
	{
		printf("[!][Framereader] Error: cant get frame!\n");
		return;
	}
	width = frame->width;
	height = frame->height;
	if(show_capture)
	{
		cvShowImage(window_name, frame);
	}
}

// прекратить видеозахват
void Framereader::end()
{
	if(fs)
	{
		fs.close();
	}
	if(frame)
	{
		cvReleaseImage( &frame );
	}
}

int Framereader::getWidth()
{
	return width;
}

int Framereader::getHeight()
{
	return height;
}

int Framereader::getFrameCounter() const
{
	return frame_counter;
}

const char* Framereader::getCurrentFileName() const
{
	return current_file.c_str();
}
