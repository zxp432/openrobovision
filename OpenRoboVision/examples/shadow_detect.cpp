//
// OpenRoboVision
//
// детектирование тени
// (по мотивам статьи на хабре)
//

#include "orv/orv.h"
#include "orv/vision/image.h"
#include <string>

#define SUB_THRESHOLD 25
#define GRAD_ORIENT_SUB_THRESHOLD 2

// show image with 2 maskes
void show_mask( std::string windowname, orv::Image& image, orv::Image& mask1, orv::Image& mask2, cv::Scalar color1,  cv::Scalar color2);

int main(int argc, char* argv[])
{
	orv::Image image1, image2, mask_obj, mask_shadow;
	orv::Image gray1, gray2;

	// default image names
	char filename01[] = "shadow1.jpg";
	char filename02[] = "shadow2.jpg";

	std::string filename1 = argc >=2 ? argv[1] : filename01;
	std::string filename2 = argc >=3 ? argv[2] : filename02;

	// load images
	if( !image1.open( filename1 ) )
		printf("[!] Error: cant open image: %s \n", filename1.c_str());

	if( !image2.open( filename2 ) )
		printf("[!] Error: cant open image: %s \n", filename2.c_str());

	if( image1.empty() || image2.empty() )
		return -1;

	// show images
	image1.show("1");
	image2.show("2");

	// convert to grayscale
	gray1 = image1.getGrayscale();
	gray2 = image2.getGrayscale();

	//--------------------------------------------
	//
	// subtraction
	//

	orv::Image sub = gray2.clone();
	sub.sub(gray1);
	sub.show("sub");

	// use threshold
	orv::Image sub_m = sub.clone();
	sub_m.threshold( SUB_THRESHOLD );
	//sub_m.erode(); 
	//sub_m.dilate();
	sub_m.show("sub_m");

	mask_shadow = sub_m.clone();
	mask_shadow.zero();
	mask_obj = sub_m.clone();

//	Image adiff = gray2.clone();
//	adiff.absdiff(gray1);
//	adiff.show("absdiff");

	orv::Image res0 = image2.clone();
	show_mask("res sub", res0, mask_obj, mask_shadow, cv::Scalar(52, 255, 52), cv::Scalar(255, 49, 51));

	//--------------------------------------------
	//
	// grad
	//

	orv::Image grad_m1, grad_o1;
	orv::Image grad_m2, grad_o2;

	// calc gradient
	gray1.grad(grad_m1, grad_o1);
	gray2.grad(grad_m2, grad_o2);

#if 0
	grad_m1.setDepth( CV_8UC1 );
	grad_m2.setDepth( CV_8UC1 );
	grad_m1.show("grad_m1");
	grad_o1.show("grad_o1");
	grad_m2.show("grad_m2");
	grad_o2.show("grad_o2");
#endif

	// compare gradient orientation
	orv::Image grad_orientation_sub = grad_o2.clone();
	grad_orientation_sub.sub(grad_o1);
	//grad_orientation_sub.show("grad_absdiff");
	//cv::waitKey(0);

	//grad_orientation_sub.threshold( GRAD_ORIENT_SUB_THRESHOLD );
	//grad_orientation_sub.show("grad_absdiff_threshold");
	//cv::waitKey(0);

	double omin, omax;
	// get max value
	grad_orientation_sub.minMaxLoc( &omin, &omax );
	printf("[i] orientation sub: min: %0.2f max: %0.2f \n", omin, omax);

	// convert to CV_8U
	grad_orientation_sub.convertTo( CV_8UC1, 255.0/omax );
	grad_orientation_sub.show("grad_orientation_sub");

	// set to image size
	mask_obj.copySectionFrom( grad_orientation_sub, cv::Rect(1, 1, image1.width(), image1.height()), cv::Rect(0, 0, image1.width(), image1.height()) );
	mask_obj._and( sub_m );
	mask_obj.show("mask_obj grad and sub");

	mask_shadow = sub_m.clone();

	mask_shadow.show("mask_shadow grad and sub");
//	cv::waitKey(0);
	mask_shadow.sub(mask_obj);
	mask_shadow.show("mask_shadow grad and sub");
//	cv::waitKey(0);

	orv::Image res1 = image2.clone();
	show_mask("res sub grad", res1, mask_obj, mask_shadow, cv::Scalar(52, 255, 52), cv::Scalar(255, 49, 51));

	//--------------------------------------------

//	IplImage t = IplImage( res.cv_img );
//	cvNamedWindow("IplImage");
//	cvShowImage("IplImage", &t);

	cv::waitKey(0);

	return 0;
}

// show image with 2 maskes
void show_mask( std::string windowname, orv::Image& image, orv::Image& mask1, orv::Image& mask2, cv::Scalar color1,  cv::Scalar color2)
{
	if(image.empty())
		return;
	if(image.size() != mask1.size() || mask1.size() != mask2.size() )
		return;

	int width, height;
	width =image.width(); 
	height =image.height(); 
	for(int y=0; y<height; y++)
	{
		uchar* ptr1 = (uchar*) (mask1.img.data + y * mask1.img.step);
		uchar* ptr2 = (uchar*) (mask2.img.data + y * mask2.img.step);
		uchar* ptrI = (uchar*) (image.img.data + y * image.img.step);
		for(int x=0; x<width; x++)
		{
			// object
			if( ptr1[x]>0 )
			{
				ptrI[3*x]	= (uchar)color1[0]; //B
				ptrI[3*x+1] = (uchar)color1[1]; //G
				ptrI[3*x+2] = (uchar)color1[2]; //R
			}
			// shadow
			if( ptr2[x]>0 )
			{
				ptrI[3*x]	= (uchar)color2[0]; //B
				ptrI[3*x+1] = (uchar)color2[1]; //G
				ptrI[3*x+2] = (uchar)color2[2]; //R
			}
		}
	}

	image.show(windowname);
}

