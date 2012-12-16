//
// OpenRoboVision
//
// monocular visual odometry
//

#include "orv/orv.h"
#include "orv/vision/camera.h"
#include "orv/vision/optical_flow.h"
#include "orv/vision/image.h"

int main(int argc, char* argv[])
{
	BaseVideoCapture* capture = NULL;

	capture = new Camera(CV_CAP_ANY);
	if(!capture)
	{
		printf("[!] Error: cant create capture!\n");
		return -1;
	}

	IplImage* frame=0;
	orv::Image frameA;
	orv::Image frameB;

	printf("[i] press Enter for capture image and Esc for quit!\n\n");

	int counter=0;
	char filename[512];

	while(true)
	{
		// получаем кадр
		capture->update();

		frame = capture->getFrame();

		if(frame)
		{
			frameA = frameB.clone();
			frameB = orv::Image(frame, true);
		}
	
		char c = cvWaitKey(33);
		if (c == 27) // ESC
			break;
		else if(c == 13)  // Enter
		{
			// сохраняем кадр в файл
			sprintf(filename, "Image%d.jpg", counter);
			printf("[i] capture... %s\n", filename);
			cvSaveImage(filename, frame);
			counter++;
		}
	}

	if(capture)
	{
		delete capture;
		capture = NULL;
	}

	return 0;
}
