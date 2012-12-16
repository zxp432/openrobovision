//
// OpenRoboVision
//
// работа с набором файлов - считывание кадров по списку
//

#include "orv/orv.h"
#include "orv/vision/frame_reader.h"

int main(int argc, char* argv[])
{
	Framereader reader("frames.txt", true);

	IplImage* frame=0;

	printf("[i] press Esc for quit!\n\n");

	while(true){
		// получаем кадр
		reader.update();
		frame = reader.getFrame();
	
		char c = cvWaitKey(33);
		if (c == 27) { // нажата ESC
			break;
		}
	}
	return 0;
}
