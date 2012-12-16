//
// OpenRoboVision
//
// считывание кадров по списку и нахождение разницы между кадрами
//

#include "orv/orv.h"
#include "orv/vision/frame_reader.h"
#include "orv/vision/optical_flow.h"

// число пикселей маски, превышение которого означает "движение"
#define MOVEMENT_THRESHOLD 1000

//
// based on code (chapter 9) from book:
//   Learning OpenCV: Computer Vision with the OpenCV Library
//     by Gary Bradski and Adrian Kaehler
//     Published by O'Reilly Media, October 3, 2008
// 
class BackgroundFromMotion
{
public:
	BackgroundFromMotion():
		count(0),
		img_32f(0),
		img_prev(0),
		img_avg(0),
		img_absdiff(0),
		img_diff(0),
		img_hi(0),
		img_low(0),
		img_hi1(0), img_hi2(0), img_hi3(0),
		img_low1(0), img_low2(0), img_low3(0),
		img_mask(0),
		img_gray1(0), img_gray2(0), img_gray3(0)
	{
	}

	~BackgroundFromMotion()
	{
		if(img_32f)
			cvReleaseImage(&img_32f);
		if(img_prev)
			cvReleaseImage(&img_prev);
		if(img_avg)
			cvReleaseImage(&img_avg);
		if(img_absdiff)
			cvReleaseImage(&img_absdiff);
		if(img_diff)
			cvReleaseImage(&img_diff);
		if(img_hi)
			cvReleaseImage(&img_hi);
		if(img_low)
			cvReleaseImage(&img_low);
		if(img_low1)
			cvReleaseImage(&img_low1);
		if(img_low2)
			cvReleaseImage(&img_low2);
		if(img_low3)
			cvReleaseImage(&img_low3);
		if(img_hi1)
			cvReleaseImage(&img_hi1);
		if(img_hi2)
			cvReleaseImage(&img_hi2);
		if(img_hi3)
			cvReleaseImage(&img_hi3);
		if(img_mask)
			cvReleaseImage(&img_mask);
		if(img_gray1)
			cvReleaseImage(&img_gray1);
		if(img_gray2)
			cvReleaseImage(&img_gray2);
		if(img_gray3)
			cvReleaseImage(&img_gray3);
	}

	void init(IplImage *src)
	{
		if(src){
			CvSize sz = cvGetSize( src );
			img_32f = cvCreateImage( sz, IPL_DEPTH_32F, 3 );
			img_prev = cvCreateImage( sz, IPL_DEPTH_32F, 3 );
			img_avg = cvCreateImage( sz, IPL_DEPTH_32F, 3 );
			img_absdiff = cvCreateImage( sz, IPL_DEPTH_32F, 3 );
			img_diff = cvCreateImage( sz, IPL_DEPTH_32F, 3 );
			img_hi = cvCreateImage( sz, IPL_DEPTH_32F, 3 );
			img_low = cvCreateImage( sz, IPL_DEPTH_32F, 3 );
			img_hi1 = cvCreateImage( sz, IPL_DEPTH_32F, 1 );
			img_hi2 = cvCreateImage( sz, IPL_DEPTH_32F, 1 );
			img_hi3 = cvCreateImage( sz, IPL_DEPTH_32F, 1 );
			img_low1 = cvCreateImage( sz, IPL_DEPTH_32F, 1 );
			img_low2 = cvCreateImage( sz, IPL_DEPTH_32F, 1 );
			img_low3 = cvCreateImage( sz, IPL_DEPTH_32F, 1 );
			img_mask = cvCreateImage( sz, IPL_DEPTH_8U, 1 );
			img_gray1 = cvCreateImage( sz, IPL_DEPTH_32F, 1 );
			img_gray2 = cvCreateImage( sz, IPL_DEPTH_32F, 1 );
			img_gray3 = cvCreateImage( sz, IPL_DEPTH_32F, 1 );
			if(!img_32f || !img_prev || !img_avg || !img_absdiff || !img_diff ||
				!img_hi || !img_low || !img_hi1 || !img_hi2 || !img_hi3 || 
				!img_low1 || !img_low2 || !img_low3 || !img_mask || 
				!img_gray1 || !img_gray2 || !img_gray3)
			{
				fprintf(stderr, "[!] Error: cant allocate memory!\n");
			}
		}
	}

	void accumulateBackground( IplImage *src )
	{
		if(!src)
			return;

		bool first = false;
		if(!img_32f)
		{
			init(src);
			first = true;
		}

		cvConvertScale( src, img_32f, 1, 0 ); // convert to float
		if(!first)
		{
			cvAcc( img_32f, img_avg );
			cvAbsDiff( img_32f, img_prev, img_absdiff );
			cvAcc( img_absdiff, img_diff );
			count += 1.0;
		}
		cvCopy( img_32f, img_prev );
	}

	void createModelsfromStats()
	{
		cvConvertScale( img_avg, img_avg, (double)(1.0/count) );
		cvConvertScale( img_diff, img_diff, (double)(1.0/count) );
		//Make sure diff is always something
		//
		cvAddS( img_diff, cvScalar( 1.0, 1.0, 1.0), img_diff );
		setHighThreshold( 7.0 );
		setLowThreshold( 6.0 );
	}

	void setHighThreshold( float scale )
	{
		cvConvertScale( img_diff, img_32f, scale );
		cvAdd( img_32f, img_avg, img_hi );
		cvSplit( img_hi, img_hi1, img_hi2, img_hi3, 0 );
	}
	void setLowThreshold( float scale )
	{
		cvConvertScale( img_diff, img_32f, scale );
		cvSub( img_avg, img_32f, img_low );
		cvSplit( img_low, img_low1, img_low2, img_low3, 0 );
	}

	// Create a binary: 0,255 mask where 255 means foreground pixel
	// I Input image, 3-channel, 8u
	// Imask Mask image to be created, 1-channel 8u
	//
	void backgroundDiff( IplImage *src, IplImage *mask) {
		if(!src || !mask)
			return;

		cvCvtScale(src, img_32f, 1, 0); // To float;
		cvSplit( img_32f, img_gray1 ,img_gray2, img_gray3, 0 );
		//Channel 1
		//
		cvInRange(img_gray1, img_low1, img_hi1, mask);
		//Channel 2
		//
		cvInRange(img_gray2, img_low2, img_hi2, img_mask);
		cvOr(mask, img_mask, mask);
		//Channel 3
		//
		cvInRange(img_gray3, img_low3, img_hi3, img_mask);
		cvOr(mask, img_mask, mask);
		//Finally, invert the results
		//
		cvSubRS(mask, cvScalar(255), mask);
	}

	int getMaskCenter(IplImage *src, CvPoint &center)
	{
		center.x=0;
		center.y=0;
		int counter = 0;
		if(src)
		{
			for(int y=0; y<src->height; y++)
			{
				unsigned char* ptr = (unsigned char*) (src->imageData + y * src->widthStep);
				for(int x=0; x<src->width; x++)
				{
					if( ptr[x]>0 )
					{
						center.x += x;
						center.y += y;
						++counter; 
					}
				}
			}
			if(counter!=0)
			{
				center.x = cvRound((float)center.x/counter);
				center.y = cvRound((float)center.y/counter);
			}
		}
		return counter;
	}

	float count;
	IplImage* img_32f;
	IplImage* img_prev;
	IplImage* img_avg;
	IplImage* img_absdiff;
	IplImage* img_diff;
	IplImage* img_hi;
	IplImage* img_low;
	IplImage *img_hi1, *img_hi2, *img_hi3;
	IplImage *img_low1, *img_low2, *img_low3;
	IplImage *img_gray1, *img_gray2, *img_gray3;
	IplImage* img_mask;
};

int main(int argc, char* argv[])
{
	Framereader reader("frames.txt", true);

	IplImage *frame=0, *frame1=0, *frame2=0, *dst=0, *image=0, *mask=0;

	cvNamedWindow("cvAbsDiff");
	cvNamedWindow("bg");

	printf("[i] press Esc for quit!\n\n");

	BackgroundFromMotion bg;
	OpticalFlowLK of;

	int last_count=0; // for check next loop

	while(true){
		// получаем кадр
		reader.update();
		frame = reader.getFrame();

		if(!frame1 && !frame2 && !dst){
			frame1 = cvCloneImage(frame);
			frame2 = cvCloneImage(frame);
			dst = cvCloneImage(frame);
			image = cvCloneImage(frame);
			mask = cvCreateImage( cvGetSize(frame), IPL_DEPTH_8U, 1);
		}
		else{
			cvCopy(frame1, frame2);
			cvCopy(frame, frame1);
			// абсолютрая разница между кадрами
			//	dst(x,y,c) = abs( src1(x,y,c) - src2(x,y,c) )
			cvAbsDiff(frame1, frame2, dst);
			cvShowImage("cvAbsDiff", dst);

			//of.setA(frame1);
			//of.setB(frame2);
			//of.make();
			//of.show();

		}

		int count = reader.getFrameCounter();

		if(count>last_count){	// last_count только растёт, поэтому он равен count,
			last_count = count; // только при первом чтении фреймов 
		}
		if(count<50 && last_count==count){
			bg.accumulateBackground( frame ); // накопление данных
		}
		else if(count==50 && last_count==count){
			bg.createModelsfromStats(); // вычисление модели
		}
		else{
			// получение маски движения
			cvZero(mask); //cvSet(mask, cvScalar(255));
			bg.backgroundDiff(frame, mask);
			cvShowImage("bg", mask);
			CvPoint center;
			int thr = bg.getMaskCenter(mask, center);
			printf("[i] cout= %d (%dx%d)\n", thr, center.x, center.y);
			if(thr>MOVEMENT_THRESHOLD)
			{
				printf("[i] !!!!!!!!!!!!!!!! MOVEMENT !!!!!!!!!!!\n");
			}
		}
	
		char c = cvWaitKey(33);
		if (c == 27) { // нажата ESC
			break;
		}
	}

	cvReleaseImage(&frame1);
	cvReleaseImage(&frame2);
	cvReleaseImage(&dst);
	cvReleaseImage(&image);
	cvReleaseImage(&mask);
	cvDestroyAllWindows();

	return 0;
}
