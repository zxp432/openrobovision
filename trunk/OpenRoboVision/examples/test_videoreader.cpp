//
// OpenRoboVision
//
// пример чтения видео
//

#include "orv.h"
#include "vision/videoreader.h"

int main(int argc, char* argv[])
{
	// заводим объект чтения видео
	Videoreader reader("capture.avi");

	IplImage* frame=0;

	printf("[i] press Esc for quit!\n\n");

	int counter=0;
	char filename[512];

	while(true){

		// следующий кадр
		reader.update();
		// считываем кадр
		frame = reader.getFrame();

		if(frame){
			// сохраняем кадр в файл
			sprintf(filename, "frame%05d.jpg", counter);
			printf("[i] write frame: %s\n", filename);
			cvSaveImage(filename, frame);
			counter++;
		}
		else{
			break;
		}
	
		char c = cvWaitKey(33);
		if (c == 27) { // нажата ESC
			break;
		}
	}
	return 0;
}
