//
// OpenRoboVision
//
// own image type
//
//
// robocraft.ru
//

#ifndef _ORV_IMAGE_H_
#define _ORV_IMAGE_H_

#ifdef USE_OPENCV
# include <cv.h>
# include <highgui.h>
#endif //#ifdef USE_OPENCV

#include <stdio.h>
#include <stdlib.h>

#include <fstream>

#ifndef CV_PI
# define CV_PI 3.1415926535897932384626433832795
#endif

#ifndef DEG_TO_RAD
# define DEG_TO_RAD(x) ( (x) * (float)CV_PI/180.0f)
#endif

#ifndef RAD_TO_DEG
# define RAD_TO_DEG(x) ( (x) * 180.0f/(float)CV_PI)
#endif

typedef unsigned char uchar;

// simple struct for store ordinary RGB image
typedef struct simple_image
{
	int width;
	int height;
	int nChannels;
	int size;
	char* data;

	simple_image():
		width(0), height(0), nChannels(0), size(0), data(NULL)
	{
	}

	simple_image(int w, int h, int channels_number=3): 
		width(w), height(h), nChannels(channels_number), size(w*h*channels_number), data(NULL)
	{
		data = new char[size];
		if(!data)
		{
			fprintf(stderr, "[!] Error: cant allocate memeory!\n");
		}
	}

	void reset()
	{
		width = 0;
		height = 0;
		nChannels = 0;
		size = 0;
		if(data)
		{
			delete [] data;
			data = NULL;
		}
	}
} simple_image;

typedef struct orv_point
{
	int x;
	int y;
	orv_point():
		x(0), y(0)
	{
	}
	orv_point(int _x, int _y):
		x(_x), y(_y)
	{
	}

} orv_point;

typedef struct orv_scalar
{
	float val[4];

	orv_scalar()
	{
		val[0]=val[1]=val[2]=val[3]=0;
	}
	orv_scalar(float val0, float val1=0, float val2=0, float val3=0)
	{
		val[0]=val0; val[1]=val1; val[2]=val2; val[3]=val3;
	}

}orv_scalar;

enum ORV_IMAGE_TYPE { DEPTH_8S, DEPTH_8U, DEPTH_16S, DEPTH_16U, DEPTH_32F};

typedef struct orv_image
{
	int width;
	int height;
	int nChannels;
	int size;
	ORV_IMAGE_TYPE type;
	int step;
	char* data;

	orv_image():
		width(0), height(0), nChannels(0), size(0), type(DEPTH_8S), step(0), data(NULL)
	{
	}

	orv_image(int w, int h, int channels_number=3, ORV_IMAGE_TYPE _type=DEPTH_8S);

	void reset()
	{
		width = 0;
		height = 0;
		nChannels = 0;
		size = 0;
		step = 0;
		if(data)
		{
			delete [] data;
			data = NULL;
		}
	}

	void data_set(int val)
	{
		if(data)
		{
			memset(data, val, size);
		}
	}

	void data_zero()
	{
		data_set(0);
	}

	void save(const char* file_name);

} orv_image;

typedef char			TYPE_DEPTH_8S;
typedef unsigned char	TYPE_DEPTH_8U;
typedef short			TYPE_DEPTH_16S;
typedef unsigned short	TYPE_DEPTH_16U;
typedef float			TYPE_DEPTH_32F;

#define ORV_PIXEL(type,img,x,y) ( ( (type*)((img)->data+(y)*(img)->step) ) + (x)*(img)->nChannels )

//----------------------------------------------------------------------------

//
// basic image I/O, based on Andreas Geiger's code (libelas)
//

// use imRef to access image data.
#define imRef(im, x, y) (im->access[y][x])

// use imPtr to get pointer to image data.
#define imPtr(im, x, y) &(im->access[y][x])

typedef struct { uchar r, g, b; } rgb;

inline bool operator==(const rgb &a, const rgb &b)
{
  return ((a.r == b.r) && (a.g == b.g) && (a.b == b.b));
} 

// image template class
template <class T> class image_t
{
public:

	// create image
	image_t(const int width, const int height, const bool init = false);

	// delete image
	~image_t();

	// init image
	void init(const T &val);

	// deep copy
	image_t<T> *copy() const;

	// get image width/height
	int width() const { return w; }
	int height() const { return h; }

	// image data
	T *data;

	// row pointers
	T **access;

private:
	int w, h;
};

template <class T> image_t<T>::image_t(const int width, const int height, const bool init)
{
	w = width;
	h = height;
	data = new T[w * h];  // allocate space for image data
	access = new T*[h];   // allocate space for row pointers

	// initialize row pointers
	for (int i = 0; i < h; i++)
		access[i] = data + (i * w);  

	// init to zero
	if (init)
		memset(data, 0, w * h * sizeof(T));
}

template <class T> image_t<T>::~image_t()
{
	if(data)
	{
		delete [] data; 
		data = NULL;
	}
	if(access)
	{
		delete [] access;
		access = NULL;
	}
}

template <class T> void image_t<T>::init(const T &val)
{
	T *ptr = imPtr(this, 0, 0);
	T *end = imPtr(this, w-1, h-1);
	while (ptr <= end)
		*ptr++ = val;
}


template <class T> image_t<T> *image_t<T>::copy() const
{
	image_t<T> *im = new image_t<T>(w, h, false);
	memcpy(im->data, data, w * h * sizeof(T));
	return im;
}

void pnm_read(std::ifstream &file, char *buf);
// load grayscale image from .PGM-file
image_t<uchar>* load_image_from_PGM(const char *name);
// save grayscale image to .PGM-file
void save_image_to_PGM(image_t<uchar> *im, const char *name);

//----------------------------------------------------------------------------
// load grayscale image from .PGM-file
simple_image* load_pgm_image(const char *name);
// save grayscale image to .PGM-file
void save_pgm_image(simple_image *im, const char *name);

void release_image(simple_image** im);
void release_image(orv_image** im);

orv_image* simple_image2orv_image(simple_image *im);
simple_image* orv_image2simple_image(orv_image *im);

#ifdef USE_OPENCV
IplImage* simple_image2IplImage(simple_image *im);
IplImage* orv_image2IplImage(orv_image *im);
simple_image* IplImage2simple_image(IplImage *img);
void show_image(const char* window_name, orv_image* im, int wait_key_ms=0, bool delete_window=false, bool save_image=false, const char* file_name="image.bmp");
#endif //#ifdef USE_OPENCV

enum ORV_BORDER_TYPE {BORDER_CONSTANT, BORDER_REPLICATE};

// make image border  (cvCopyMakeBorder)
void image_border(orv_image* src, orv_image* dst, orv_point offset, int border_type=BORDER_CONSTANT, orv_scalar value=orv_scalar(0));

// calc image gradient (cvSobel)
int image_gradient(orv_image* src, orv_image** magnitude, orv_image** orientation);

// calc thin borders (cvCanny)
int image_thin_borders(orv_image* src, orv_image** dst);

#endif //#ifndef _ORV_IMAGE_H_
