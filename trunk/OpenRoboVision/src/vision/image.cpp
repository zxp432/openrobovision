//
// OpenRoboVision
//
// object for work with images (OpenCV)
//
// объект для работы с картинками (OpenCV)
//
//
// robocraft.ru
//

#include "orv/vision/image.h"

namespace orv {

Image_type::Image_type():
width(0), height(0), type(0), nChannels(0), size(0), step(0), data(0)
{
}

Image_type::Image_type(int _width, int _height, int _type)
{
	this->create(_width, _height, _type);
}

void Image_type::create(int _width, int _height, int _type)
{
	width = _width;
	height = _height;
	type = _type;
	size = width*height;
	switch(type)
	{
	case CV_8SC1:
		nChannels = 1;
		size *= sizeof(char);
		step = width*nChannels*sizeof(char);
		data = new char[size];
		break;
	case CV_8SC3:
		nChannels = 3;
		size *= nChannels*sizeof(char);
		step = width*nChannels*sizeof(char);
		data = new char[size];
		break;
	case CV_8UC1:
		nChannels = 1;
		size *= sizeof(unsigned char);
		step = width*nChannels*sizeof(unsigned char);
		data = (char*)new unsigned char[size];
		break;
	case CV_16SC1:
		nChannels = 1;
		size *= sizeof(short);
		step = width*nChannels*sizeof(short);
		data = (char*)new short[size];
		break;
	case CV_16SC3:
		nChannels = 3;
		size *= nChannels*sizeof(char);
		step = width*nChannels*sizeof(char);
		data = (char*)new short[size];
		break;
	case CV_32FC1:
		nChannels = 1;
		size *= sizeof(float);
		step = width*nChannels*sizeof(float);
		data = (char*)new float[size];
		break;
	case CV_32FC3:
		nChannels = 3;
		size *= nChannels*sizeof(float);
		step = width*nChannels*sizeof(float);
		data = (char*)new float[size];
		break;
	case CV_8UC3:
	default:
		nChannels = 3;
		size *= nChannels*sizeof(unsigned char);
		step = width*nChannels*sizeof(unsigned char);
		data = (char*)new unsigned char[size];
		break;
	}
}

void Image_type::release()
{
	if(data)
	{
		delete []data;
		data = 0;
	}
	width = 0;
	height = 0;
	nChannels = 0;
	size = 0;
	step = 0;
}

void Image_type::set(int val)
{
	if(this->data && this->size>0)
		memset(this->data, val, this->size);
}

void Image_type::zero()
{
	this->set(0);
}

Image_type* Image_type::clone()
{
	Image_type* clon = new Image_type(this->width, this->height, this->type);
	if(clon)
	{
		if(this->data)
			this->copyTo(clon);
		return clon;
	}
	return 0;
}

void Image_type::copyTo(Image_type* img)
{
	if(img && this->data && this->size>0 &&
		img->width == this->width && img->height == this->height &&
		img->type == this->type)
	{
			memcpy(img->data, this->data, this->size);
	}
}

int Image_type::open(const char* file_name)
{
	return 0;
}

int Image_type::save(const char* file_name)
{
	return 0;
}

#ifdef USE_OPENCV
Image_type::Image_type(IplImage *im)
{
	if(!im)
		return;
	assert(im->nChannels==1 || im->nChannels==3);

	int depth = CV_8UC3;
	switch(im->depth)
	{
	case IPL_DEPTH_8S:
		depth = im->nChannels==1?CV_8SC1:CV_8SC3;
		break;
	case IPL_DEPTH_16S:
		depth = im->nChannels==1?CV_16SC1:CV_16SC3;
		break;
	case IPL_DEPTH_16U:
		depth = im->nChannels==1?CV_16UC1:CV_16UC3;
		break;
	case IPL_DEPTH_32F:
		depth = im->nChannels==1?CV_32FC1:CV_32FC3;
		break;
	case IPL_DEPTH_8U:
	default:
		depth = im->nChannels==1?CV_8UC1:CV_8UC3;
		break;
	}
	this->create(im->width, im->height, depth);
	memcpy(this->data, im->imageData, im->imageSize);
}

IplImage* Image_type::getIplImage()
{
	int depth = IPL_DEPTH_8U;
	switch(this->type)
	{
	case CV_8SC1:
	case CV_8SC3:
		depth = IPL_DEPTH_8S;
		break;
	case CV_16SC1:
	case CV_16SC3:
		depth = IPL_DEPTH_16S;
		break;
	case CV_16UC1:
	case CV_16UC3:
		depth = IPL_DEPTH_16U;
		break;
	case CV_32FC1:
	case CV_32FC3:
		depth = IPL_DEPTH_32F;
		break;
		break;
	case CV_8UC1:
	case CV_8UC3:
	default:
		depth = IPL_DEPTH_8U;
		break;
	}
	IplImage* image = cvCreateImage(cvSize(this->width, this->height), depth, this->nChannels);
	memcpy(image->imageData, this->data, this->size);
	return image;
}

void Image_type::show(const char* window_name)
{
	IplImage* tmp = this->getIplImage();
	if(tmp){
		cvNamedWindow(window_name);
		cvShowImage(window_name, tmp);
		cvReleaseImage(&tmp);
	}
}
#endif //#ifdef USE_OPENCV

//---------------------------------------------------

Image::Image()
{
}

#if defined(USE_OPENCV)
Image::Image(cv::Mat m)
{
	m.copyTo(img);
}

Image::Image(CvMat *mat, bool copydata):
	img(mat, copydata)
{
}

Image::Image(IplImage *image, bool copydata):
	img(image, copydata)
{
}

Image::Image(cv::Size size, int type)	
{
	img.create(size, type);
}

Image::Image(cv::Size size)
{
	img.create(size, CV_8UC3);
}
#else
Image::Image(Image_type* im, bool copydata=false)
{
	if(img_)
	{
		img.create(im->width, im->height, im->type);
		if(copydata)
			im.copyTo(img);
		else
		{
			delete []img.data;
			img.data = im.data;
		}
	}
}
#endif //#if defined(USE_OPENCV)

Image::Image(int width, int height, int type)
{
	img.create(height, width, type);
}

Image::Image(int width, int height)
{
	img.create(height, width, CV_8UC3);
}

Image::~Image()
{
#if defined(USE_OPENCV)
# if CV_MAJOR_VERSION >= 2
	img.release();
# else
	if(img)
	{
		cvReleaseImage(&img);
		img = 0;
	}
# endif //# if CV_MAJOR_VERSION >= 2
#else
	if(img)
	{
		img->release();
		img = 0;
	}
#endif //#if defined(USE_OPENCV)
}

Image& Image::operator=(const Image &other)
{
	if (this == &other) // same object
		return *this;
	other.img.copyTo(img);
	return *this;
}

// Loads an image from a file
bool Image::open(const std::string filename, int iscolor)
{
#if defined(USE_OPENCV)
	img = cv::imread(filename, iscolor);
	return !img.empty();
#else
	img.open(filename.c_str());
#endif //#if defined(USE_OPENCV)
}

// Saves an image to a specified file
bool Image::save(const std::string filename, const std::vector<int>& params)
{
#if defined(USE_OPENCV)
	return cv::imwrite(filename, img, params);
#else
	img.save(filename.c_str());
#endif //#if defined(USE_OPENCV)
}

// clone image
Image Image::clone()
{
	return *(new Image(img.clone()));
}

//
// get image properties
//

#if defined(USE_OPENCV)
cv::Size Image::size() const
{
	return img.size();
}
#endif //#if defined(USE_OPENCV)

int Image::width() const
{
#if defined(USE_OPENCV)
	return img.size().width;
#else
	return img.width;
#endif //#if defined(USE_OPENCV)
}
int Image::height() const
{
#if defined(USE_OPENCV)
	return img.size().height;
#else
	return img.height;
#endif //#if defined(USE_OPENCV)
}
int Image::channels() const
{
#if defined(USE_OPENCV)
	return img.channels();
#else
	return img.nChannels;
#endif //#if defined(USE_OPENCV)
}
int Image::depth() const
{
#if defined(USE_OPENCV)
	return img.depth();
#else
	return img.type;
#endif //#if defined(USE_OPENCV)
}

#if defined(USE_OPENCV)

// Finds edges in an image using Canny algorithm.
void Image::canny(double thresh1, double thresh2, int aperatureSize, bool moreAccurate)
{
	cv::Canny(img, img, thresh1, thresh2, aperatureSize, moreAccurate);
}

// Calculates the first, second, third or mixed image derivatives using an extended Sobel operator
void Image::sobel(int xorder, int yorder, int ksize)
{	
	cv::Sobel(img, img, CV_8U, xorder, yorder, ksize);
}

// Applies a fixed-level threshold to each array element
void Image::threshold(double thresh, double maxVal, int thresholdType)
{
	cv::threshold(img, img, thresh, maxVal, thresholdType); 
}

void Image::adaptiveThreshold(double maxValue, int adaptiveMethod, int thresholdType, int blockSize, double C)
{
	cv::adaptiveThreshold(img, img, maxValue, adaptiveMethod, thresholdType, 5, 5); 
}

// Finds global minimum and maximum in a whole array or sub-array
void Image::minMaxLoc(double* minVal, double* maxVal, cv::Point* minLoc, cv::Point* maxLoc) const
{
	cv::minMaxLoc(img, minVal, maxVal, minLoc, maxLoc);
}

// Converts matrix to another datatype with optional scaling.
void Image::convertTo(int rtype, double alpha, double beta)
{
	img.convertTo( img, rtype, alpha, beta );
}

// Compares a template against overlapped image regions.
void Image::matchTemplate(const Image& templ, Image& result, int method)
{
	cv::matchTemplate(img, templ.img, result.img, method);
}

void Image::grabFrame( cv::VideoCapture &cap )
{
	unsigned char* data = img.data;
	cap >> img;
	
	// Just a check to protect against leaks.  
	// There probably isn't a good reason why the Mat should
	// be making a new data array frequently (like every frame)
	// so if you see this a lot, you are probably doing something wrong.
	if( data != img.data)
	{
		fprintf( stdout, "[i] INFO: Reallocated img in Image::grabFrame()\n");
	}
}

// Displays the image in the specified window
void Image::show(const char* _windowname)
{
	std::string windowname(_windowname);
	cv::imshow(windowname, img);
}

void Image::show(std::string& windowname)
{
	cv::imshow(windowname, img);
}

// Displays the image window in specified point of screen
void Image::show(std::string& windowname, int x, int y)
{
	cv::namedWindow(windowname);
	cvMoveWindow(windowname.c_str(), x, y);
	cv::imshow(windowname, img);
}

// Draws a text string
void Image::addtext(const std::string text, cv::Point point, cv::Scalar color, int fontFace, double fontScale, int thickness)
{
	cv::putText(img, text, point, fontFace, fontScale, color, thickness);
}

// Draws a line segment connecting two points
void Image::addline(cv::Point pt1, cv::Point pt2, const cv::Scalar& color, int thickness, int lineType, int shift)
{
	cv::line(img, pt1, pt2, color, thickness, lineType, shift);
}

// Draws a simple, thick, or filled up-right rectangle
void Image::addrectangle(cv::Point pt1, cv::Point pt2, const cv::Scalar& color, int thickness, int lineType, int shift)
{
	cv::rectangle(img, pt1, pt2, color, thickness, lineType, shift);
}

// Draws a circle
void Image::addcircle(cv::Point center, int radius, const cv::Scalar& color, int thickness, int lineType, int shift)
{
	cv::circle(img, center, radius, color, thickness, lineType, shift);
}

// calc gradient
void Image::grad(Image &magnitude, Image &orientation)
{
	if( this->channels() > 1 )
		return;

	int width = this->width();
	int height = this->height();

	// borders
	magnitude = Image( width+2, height+2, CV_32FC1 );
	orientation = Image( width+2, height+2, CV_32FC1 );

	magnitude.zero();
	orientation.zero();

	double dx, dy, orient;

	for(int j=1; j<height-1; j++) {
		//float* ptrM = (float*) (magnitude.img.data + j * magnitude.img.step);
		//float* ptrO = (float*) (orientation.img.data + j * orientation.img.step);
		for(int i=1; i<width-1; i++) {

			// вычисление градиента
			dx = img.at<uchar>(j, i+1) - img.at<uchar>(j, i-1);
			dy = img.at<uchar>(j+1, i) - img.at<uchar>(j-1, i);

			// сохранение величины
			magnitude.img.at<float>(j, i) = static_cast<float>( sqrt(dx*dx + dy*dy) );
			//ptrM[i] = static_cast<float>( sqrt(dx*dx + dy*dy) );

			// вычисление и сохранение ориентации градиента в радианах
			orient = atan2f(float(dy), float(dx));

			orientation.img.at<float>(j, i) = static_cast<float>( orient );
			//ptrO[i] = static_cast<float>( orient );
		}
	}
}

void Image::copySectionFrom(Image &src, cv::Rect src_rect)
{
	this->copySectionFrom(src, src_rect, src_rect);
}

void Image::copySectionFrom(Image &src, cv::Rect src_rect, cv::Rect dst_rect)
{
	cv::Mat src_roi = src.img(src_rect);
	cv::Mat dst_roi = img(dst_rect);
	src_roi.copyTo(dst_roi);
}

void Image::copyInto(Image &background, int x, int y, Image &mask)
{
	cv::Rect bg = cv::Rect(0, 0, background.width(), background.height());
	cv::Rect fg = cv::Rect(x, y, width(), height());
	cv::Rect intersect = fg & bg;
	
	cv::Mat foreground_roi = img( cv::Rect(0, 0, intersect.width, intersect.height) );
	cv::Mat mask_roi = mask.img( cv::Rect(0, 0, intersect.width, intersect.height) );
	cv::Mat background_roi = background.img(cv::Rect(x, y, intersect.width, intersect.height));
	
	foreground_roi.copyTo(background_roi, mask_roi);
}

void Image::copyInto(Image &background, int x, int y)
{
	Image tmp_mask = Image(cv::Mat::ones(size(), CV_8UC1));
	copyInto(background, x, y, tmp_mask);
}

// Get an image for each channel in this image
std::vector<Image> Image::split()
{
	std::vector<cv::Mat> cvPlanes;
	std::vector<Image> planes;
	// Divides multi-channel array into several single-channel arrays
	cv::split(img, cvPlanes);
	for(unsigned int i=0; i<cvPlanes.size(); i++)
	{
		Image img(cvPlanes[i]);
		planes.push_back( img );
	}
	return planes;
}

// converts matrix to another datatype with optional scalng. See cvConvertScale.
void Image::setDepth(int new_type)
{
	img.convertTo(img, new_type);
}

std::vector<Image> Image::copies(int num)
{
	std::vector<Image> clones;
	for(int i=0; i<num; i++)
	{
		Image aCopy( img );
		clones.push_back( aCopy );
	}
	return clones;
}
#endif //#if defined(USE_OPENCV)

Image Image::getGrayscale()
{
#if defined(USE_OPENCV)
	int code;
	switch(channels())
	{
		case 1:
			return this->clone();
		case 3:
			code=CV_RGB2GRAY;
			break;
		case 4:
			code=CV_RGBA2GRAY;
			break;
	}
	
	cv::Mat gray;
	// Converts image from one color space to another
	cv::cvtColor(img, gray, code);
	Image* grayscale = new Image(gray);
	return *grayscale;
#else
	if(img.nChannels==1)
		return this->clone();
	else{
		Image_type* gray = new Image_type(image.width, image.height, CV_8UC1);
		if(gray){
			// RGB to Gray 
			int counter=0;
			for(int i=0; i<img.size-2; i+=3){
				float temp = 0.2989f * src[i] + 0.5870f * src[i + 1] + 0.1140f * src[i + 2];
				gray.data[counter++] = (unsigned char)temp;
			}
			Image* grayscale = new Image(gray);
			return *grayscale;
		}
		return 0;
	}
#endif //#if defined(USE_OPENCV)
}

uchar* Image::at(int x, int y) //$TODO
{
#if defined(USE_OPENCV)
	uchar* ptr = (uchar*) (img.data + y * img.step);

	if(img.depth() == CV_8UC1)
		return &ptr[x];
	else if(img.depth() == CV_8UC3)
		return &ptr[3*x];
	return NULL;
#endif //#if defined(USE_OPENCV)
}

}; //namespace orv {
