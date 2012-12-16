//
// OpenRoboVision
//
// работа с камерой - сохранение кадров в BMP-файлы
//

#include "orv/orv.h"
#include "orv/vision/camera.h"

int main(int argc, char* argv[])
{
	Camera camera(CV_CAP_ANY);

	IplImage* frame=0;

	printf("[i] press Esc for quit!\n\n");

	int counter=0;
	char filename[1024];

	while(true){
		// получаем кадр
		camera.update();
		frame = camera.getFrame();
	
		char c = cvWaitKey(33);
		if (c == 27) { // нажата ESC
			break;
		}
		
		// сохраняем кадр в файл
		sprintf(filename, "frame%06d.bmp", counter);
		printf("[i] capture... %s\n", filename);
		cvSaveImage(filename, frame);
		counter++;
	}
	return 0;
}
