//
// OpenRoboVision
//
// пример работы с двум€ камерами (стереопара :)
// http://ru.wikipedia.org/wiki/—тереопара
//
// http://robocraft.ru
//

#include "orv/orv.h"
#include "orv/vision/camera.h"

int make_stereo(IplImage* image1, IplImage* image2);

int main(int argc, char* argv[])
{
#if defined(WIN32)
	// DirectShow (via videoInput)
	Camera camera_left(CV_CAP_DSHOW);
	Camera camera_right(CV_CAP_DSHOW+1);
#elif defined(LINUX)
	Camera camera_left(CV_CAP_V4L2);
	Camera camera_right(CV_CAP_V4L2+1);
#endif //#if defined(WIN32)

	camera_left.setShowCapture(false);
	camera_right.setShowCapture(false);
	camera_left.setParam(CV_CAP_PROP_FRAME_WIDTH, 320);
	camera_left.setParam(CV_CAP_PROP_FRAME_HEIGHT, 240);
	camera_right.setParam(CV_CAP_PROP_FRAME_WIDTH, 320);
	camera_right.setParam(CV_CAP_PROP_FRAME_HEIGHT, 240);

	IplImage* frame_left=0, *frame_right=0;

	printf("[i] press Enter for capture image and Esc for quit!\n\n");

	int counter=0;
	char filename[512], filename2[512];

	//cvNamedWindow("left");
	//cvNamedWindow("right");

	while(true){
		// получаем кадр
		camera_left.update();
		camera_right.update();
		frame_left = camera_left.getFrame();
		frame_right = camera_right.getFrame();

		//cvShowImage("left", frame_left);
		//cvShowImage("right", frame_right);

		make_stereo(frame_left, frame_right);
	
		char c = cvWaitKey(33);
		if (c == 27) { // нажата ESC
			break;
		}
		else if(c == 13) { // Enter
			// сохран€ем кадр в файл
			sprintf(filename, "Image_left_%d.jpg", counter);
			sprintf(filename2, "Image_right_%d.jpg", counter);
			printf("[i] capture... %s %s\n", filename, filename2);
			cvSaveImage(filename, frame_left);
			cvSaveImage(filename2, frame_right);
			counter++;
		}
	}

	cvDestroyAllWindows();
	return 0;
}

int make_stereo(IplImage* image1, IplImage* image2)
{
	if(!image1 || ! image2)
		return -1;

	cv::Mat gray1, gray2;

	cv::Mat img1, img2, disp, disp8;

	img1 = cv::Mat(image1);
	img2 = cv::Mat(image2);
	if(img1.empty() || img2.empty())
		return -1;
	// convert to grayscale
	if(img1.channels()>1)
	{
		cv::cvtColor(img1, gray1, CV_RGB2GRAY);
		img1 = gray1;
	}
	if(img2.channels()>1)
	{
		cv::cvtColor(img2, gray2, CV_RGB2GRAY);
		img2 = gray2;
	}

	cv::Size img_size = img1.size();
    
	cv::Rect roi1, roi2;

	cv::StereoBM bm;

	int SADWindowSize = 0, numberOfDisparities = 0;

	numberOfDisparities = numberOfDisparities > 0 ? numberOfDisparities : ((img_size.width/8) + 15) & -16;

	bm.state->roi1 = roi1;
    bm.state->roi2 = roi2;
    bm.state->preFilterCap = 31;
    bm.state->SADWindowSize = SADWindowSize > 0 ? SADWindowSize : 9;
    bm.state->minDisparity = 0;
    bm.state->numberOfDisparities = numberOfDisparities;
    bm.state->textureThreshold = 10;
    bm.state->uniquenessRatio = 15;
    bm.state->speckleWindowSize = 100;
    bm.state->speckleRange = 32;
    bm.state->disp12MaxDiff = 1;

	bm(img1, img2, disp);
	disp.convertTo(disp8, CV_8U);

	cv::namedWindow("left", 1);
	cv::imshow("left", img1);
	cv::namedWindow("right", 1);
	cv::imshow("right", img2);
	cv::namedWindow("disparity", 0);
	cv::imshow("disparity", disp8);
	return 0;
}
