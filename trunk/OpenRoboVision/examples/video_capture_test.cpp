//
// OpenRoboVision
//
// example of working with video capture
//

#include "orv/orv.h"
#include "orv/vision/camera.h"
#include "orv/vision/camera_mjpg.h"

int main(int argc, char* argv[])
{

	orv::network::init();

	BaseVideoCapture* capture = NULL;

#if 0
	capture = new Camera(CV_CAP_ANY);
#elif 1
	capture = new CameraMJPG("127.0.0.1", 8080, CAMERA_MJPG_COMMAND_MJPG_STREAMER_GET_STREAM);
#endif
	if(!capture)
	{
		printf("[!] Error: cant create capture!\n");
		return -1;
	}

	IplImage* frame=0;

	printf("[i] press Enter for capture image and Esc for quit!\n\n");

	int counter=0;
	char filename[512];

	while(true){
		// получаем кадр
		capture->update();
		frame = capture->getFrame();
	
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

	if(capture){
		delete capture;
		capture = NULL;
	}

	orv::network::end();

	return 0;
}
