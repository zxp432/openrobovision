//
// OpenRoboVision
//
// object for work with images (OpenCV)
//
// îáúåêò äëÿ ðàáîòû ñ êàðòèíêàìè (OpenCV)
//
//
// robocraft.ru
//
 
#ifndef _IMAGE_H_
#define _IMAGE_H_

#if defined(USE_OPENCV)

#include <cv.h>

#if CV_MAJOR_VERSION >= 2

#if CV_MAJOR_VERSION == 2 && CV_MINOR_VERSION < 2
# include <highgui.h>
# include <cv.hpp>
#elif CV_MAJOR_VERSION == 2 && CV_MINOR_VERSION >= 2
# include <opencv2/opencv.hpp>
#endif

#endif //#if CV_MAJOR_VERSION >= 2

#endif //#if defined(USE_OPENCV)

namespace orv {

#if !defined(USE_OPENCV)
// image type (for set image without OpenCV)
enum IMAGE_TYPE {CV_8SC1=0, CV_8SC3, CV_8UC1, CV_8UC3, CV_16SC1, CV_16SC3, CV_16UC1, CV_16UC3, CV_32FC1, CV_32FC3};
#endif //#if !defined(USE_OPENCV)

// image type for working without OpenCV
typedef struct Image_type
{
	int width;
	int height;
	int type;
	int nChannels;
	int size;
	int step;
	char* data;

	Image_type();
	Image_type(int width, int height, int type);
	void create(int width, int height, int type);
	void release();
	void set(int val);
	void zero();
	Image_type* clone();
	void copyTo(Image_type* img);

	int open(const char* file_name);
	int save(const char* file_name);

#ifdef USE_OPENCV
	Image_type(IplImage *im);
	IplImage* getIplImage();
	void show(const char* window_name);
#endif //#ifdef USE_OPENCV

} Image_type;

#define ORV_PIXEL(type,img,x,y) ( ( (type*)((img)->data+(y)*(img)->step) ) + (x)*(img)->nChannels )

//---------------------------------------------------

class Image 
{
public:
	
	Image();
#if defined(USE_OPENCV)
	Image(cv::Mat m);
	Image(CvMat *mat, bool copydata=false);
	Image(IplImage *image, bool copydata=false);
	Image(cv::Size size, int type);
	Image(cv::Size size);
#else
	Image(Image_type* im, bool copydata=false);
#endif //#if defined(USE_OPENCV)
	Image(int width, int height, int type);
	Image(int width, int height);
	~Image();
	
	//
	// overload operators
	//

	Image& operator=(const Image &other);

	//
	// methods
	//

	// Loads an image from a file
	bool open(const std::string filename, int iscolor=1);
	// Saves an image to a specified file
	bool save(const std::string filename, const std::vector<int>& params=std::vector<int>());

	// Clone image
	Image clone();
	
	//
	// get image properties
	//
#if defined(USE_OPENCV)
	cv::Size size() const;
#endif //#if defined(USE_OPENCV)
	int width()	const;
	int height() const;
	int channels() const;
	int depth() const;
	
#if defined(USE_OPENCV)
	// Converts image from one color space to another
	void convert(int code)	{	cv::cvtColor(img, img, code);			}
	
	// Computes the per-element sum of two arrays or an array and a scalar
	void add(Image &im)	{	cv::add(im.img, img, img);		}
	void add(const cv::Scalar& sc)	{	cv::add(img, sc, img);			}
	
	// Calculates per-element difference between two arrays or array and a scalar
	void sub(const Image &im)		{	cv::subtract(im.img, img, img);	}
	void sub(const cv::Scalar &sc)	{	cv::subtract(img, sc, img);			}
	
	// Dilates an image by using a specific structuring element
	void dilate(int iter=1)	{	cv::dilate(img, img, cv::Mat(), cv::Point(-1, -1), iter);	}
	// Erodes an image by using a specific structuring element
	void erode(int iter=1)	{	cv::erode(img, img, cv::Mat(), cv::Point(-1, -1), iter);	}
	
	// Smoothes image using normalized box filter
	void blur(int ksize)	{	cv::blur(img, img, cv::Size(ksize,ksize));		}

	// Calculates per-element bit-wise conjunction of two arrays
	void _and(const Image &im)		{	cv::bitwise_and(im.img, img, img);	}
	// Calculates per-element bit-wise disjunction of two arrays
	void _or(const Image &im)		{	cv::bitwise_or(im.img, img, img);	}
	// Calculates per-element bit-wise ”exclusive or” operation on two arrays
	void _xor(const Image &im)		{	cv::bitwise_xor(im.img, img, img);	}
	// Inverts every bit of array
	void invert()			{	cv::bitwise_not(img, img);	}

	// Equalizes the histogram of a grayscale image.
	void equalize()			{	cv::equalizeHist(img, img);		}

	bool empty()			{	return img.empty();		}

	void setTo(cv::Scalar &sc)	{		img.setTo(sc);					} 
	void setTo(cv::Scalar sc)	{		img.setTo(sc);					}
	void zero()					{	img.setTo( cv::Scalar(0, 0, 0, 0));	}

	// Computes absolute value of each matrix element
	void abs()	{	cv::abs(img);	}
	// Computes per-element absolute difference between 2 arrays
	void absdiff(const Image &im) {	cv::absdiff(im.img, img, img);	}

	// Finds edges in an image using Canny algorithm.
	void canny(double thresh1, double thresh2, int aperatureSize=3, bool moreAccurate=false);
	// Calculates the first, second, third or mixed image derivatives using an extended Sobel operator
	void sobel(int xorder=0, int yorder=0, int ksize=3);
	
	// Applies a fixed-level threshold to each array element
	void threshold(double thresh, double maxVal=255.0, int thresholdType=cv::THRESH_BINARY);
	// Applies an adaptive threshold to an array.
	void adaptiveThreshold(double maxValue=255.0, int adaptiveMethod=cv::ADAPTIVE_THRESH_GAUSSIAN_C, int thresholdType=cv::THRESH_BINARY, int blockSize=5.0, double C=5.0);

	// Finds global minimum and maximum in a whole array or sub-array
	void minMaxLoc(double* minVal, double* maxVal=0, cv::Point* minLoc=0, cv::Point* maxLoc=0) const;

	// Converts matrix to another datatype with optional scaling.
	void convertTo(int rtype, double alpha=1, double beta=0);

	// Compares a template against overlapped image regions.
	//	result - A map of comparison results; will be single-channel 32-bit floating-point. If image is
	//	W x H and templ is w x h then result will be (W - w + 1) x (H - h + 1)
	void matchTemplate(const Image& templ, Image& result, int method);
	
	// Grab a frame from a VideoCapture object
	void grabFrame( cv::VideoCapture &cap );
	
	//
	// GUI (HighGUI)
	//

	// Displays the image in the specified window
	void show(const char* windowname);
	void show(std::string& windowname);
	// Displays the image window in specified point of screen
	void show(std::string& windowname, int x, int y);
	// Draws a text string
	void addtext(const std::string text, cv::Point point, cv::Scalar color, int fontFace=cv::FONT_HERSHEY_PLAIN, double fontScale=1.0, int thickness=1);
	// Draws a line segment connecting two points
	void addline(cv::Point pt1, cv::Point pt2, const cv::Scalar& color, int thickness=1, int lineType=8, int shift=0);
	// Draws a simple, thick, or filled up-right rectangle
	void addrectangle(cv::Point pt1, cv::Point pt2, const cv::Scalar& color, int thickness=1, int lineType=8, int shift=0);
	// Draws a circle
	void addcircle(cv::Point center, int radius, const cv::Scalar& color, int thickness=1, int lineType=8, int shift=0);

	//
	// additionals
	//

	// calc gradient
	void grad(Image &magnitude, Image &orientation);

	// Get a section from another image
	void copySectionFrom(Image &src, cv::Rect src_rect);
	void copySectionFrom(Image &src, cv::Rect src_rect, cv::Rect dst_rect);
	
	// Copy this image into another at a particular location
	void copyInto(Image &background, int x, int y, Image &mask);
	void copyInto(Image &background, int x, int y);
	
	// Get an image for each channel in this image
	std::vector<Image> split();
	
	// converts matrix to another datatype with optional scalng. See cvConvertScale()
	void setDepth(int new_type);
	
	// Get some copies of this image
	std::vector<Image> copies(int num);
#endif //#if defined(USE_OPENCV)
	
	// get a grayscale version of this image.
	Image getGrayscale();

#if defined(USE_OPENCV)
# if CV_MAJOR_VERSION >= 2
	void setImage( cv::Mat m )	{	img = m;		}
	cv::Mat& getMat()			{	return img;	}
	cv::Mat* getImage()			{	return &img;	}
# endif //# if CV_MAJOR_VERSION >= 2
#else
	Image_type* getImage()		{	return &img;	}
#endif //#if defined(USE_OPENCV)

	uchar* at(int x, int y); //$TODO
	
	//
	// data
	//

#if defined(USE_OPENCV)
# if CV_MAJOR_VERSION >= 2
	cv::Mat img;
# else
	IplImage* img;
# endif //# if CV_MAJOR_VERSION >= 2
#else
	Image_type img;
#endif //#if defined(USE_OPENCV)
};

}; //namespace orv {

#endif //#ifndef _IMAGE_H_
