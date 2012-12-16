//
// OpenRoboVision
//
// own image type
//
// собственный тип для работы с изображениями
//
//
// robocraft.ru
//

#include "orv/vision/orv_image.h"

#ifndef PNM_READ_BUF_SIZE 
# define PNM_READ_BUF_SIZE 256
#endif

orv_image::orv_image(int w, int h, int channels_number, ORV_IMAGE_TYPE _type): 
width(w), height(h), nChannels(channels_number), type(_type), data(NULL)
{
	int _size = w*h*channels_number;
	switch (type)
	{
	case DEPTH_8U:
		size = _size*sizeof(unsigned char);
		step = width*nChannels*sizeof(unsigned char);
		data = (char*)new unsigned char[_size];
		break;
	case DEPTH_16S:
		size = _size*sizeof(short);
		step = width*nChannels*sizeof(short);
		data = (char*)new short[_size];
		break;
	case DEPTH_16U:
		size = _size*sizeof(unsigned short);
		step = width*nChannels*sizeof(unsigned short);
		data = (char*)new unsigned short[_size];
		break;
	case DEPTH_32F:
		size = _size*sizeof(float);
		step = width*nChannels*sizeof(float);
		data = (char*)new float[_size];
		break;
	case DEPTH_8S:
	default:
		size = _size*sizeof(char);
		step = width*nChannels*sizeof(char);
		data = new char[_size];
		break;
	}
	if(!data)
	{
		fprintf(stderr, "[!] Error: cant allocate memeory!\n");
	}
}

void orv_image::save(const char* file_name)
{
	simple_image* sim = orv_image2simple_image(this);
	if(sim)
	{
		save_pgm_image(sim, file_name);
		release_image(&sim);
	}
	fprintf(stderr, "[!] Warning: cant convert image!\n");
}

void pnm_read(std::ifstream &file, char *buf)
{
	if(!file || !buf)
		return;

	char doc[PNM_READ_BUF_SIZE];
	char c;

	file >> c;
	while (c == '#')
	{
		file.getline(doc, PNM_READ_BUF_SIZE);
		file >> c;
	}
	file.putback(c);

	file.width(PNM_READ_BUF_SIZE);
	file >> buf;
	file.ignore();
}

// load grayscale image from .PGM-file
// http://en.wikipedia.org/wiki/Portable_graymap
image_t<uchar>* load_image_from_PGM(const char *name)
{
	if(!name)
	{
		fprintf(stderr, "[!] Error: Bad file name: %s \n", name);
		return NULL;
	}

	char buf[PNM_READ_BUF_SIZE];

	// read header
	std::ifstream file(name, std::ios::in | std::ios::binary);
	if(!file.is_open())
	{
		fprintf(stderr, "[!] Error: Could not open file: %s \n", name);
		return NULL;
	}

	pnm_read(file, buf);
	if (strncmp(buf, "P5", 2)) 
	{
		fprintf(stderr, "[!] Error: Could not read file: %s \n", name);
		return NULL;
	}

	pnm_read(file, buf);
	int width = atoi(buf);
	pnm_read(file, buf);
	int height = atoi(buf);

	pnm_read(file, buf);
	if (atoi(buf) > UCHAR_MAX)
	{
		fprintf(stderr, "[!] Error: Could not read file: %s \n", name);
		return NULL;
	}

	// read data
	image_t<uchar> *im = new image_t<uchar>(width, height);
	if(!im){
		fprintf( stderr, "[!] Error: Cant allocate memory for image!\n");
		file.close();
		return NULL;
	}

	file.read((char *)imPtr(im, 0, 0), width * height * sizeof(uchar));
	file.close();

	return im;
}

// save grayscale image to .PGM-file
void save_image_to_PGM(image_t<uchar> *im, const char *name)
{
	if(!im || !name)
	{
		fprintf(stderr, "[!] Error: Bad image or file name: %s \n", name);
		return;
	}
	int width = im->width();
	int height = im->height();
	std::ofstream file(name, std::ios::out | std::ios::binary);

	if(!file.is_open())
	{
		fprintf(stderr, "[!] Error: Could not open file: %s \n", name);
		return;
	}

	file << "P5\n" << width << " " << height << "\n" << UCHAR_MAX << "\n";
	file.write((char *)imPtr(im, 0, 0), width * height * sizeof(uchar));
	file.close();
}

//----------------------------------------------------------------------------
// load grayscale image from .PGM-file
// http://en.wikipedia.org/wiki/Portable_graymap
simple_image* load_pgm_image(const char *name)
{
	if(!name)
	{
		fprintf(stderr, "[!] Error: Bad file name: %s \n", name);
		return NULL;
	}

	char buf[PNM_READ_BUF_SIZE];

	// read header
	std::ifstream file(name, std::ios::in | std::ios::binary);
	if(!file.is_open())
	{
		fprintf(stderr, "[!] Error: Could not open file: %s \n", name);
		return NULL;
	}

	pnm_read(file, buf);
	if (strncmp(buf, "P5", 2)) 
	{
		fprintf(stderr, "[!] Error: Could not read file: %s \n", name);
		return NULL;
	}

	pnm_read(file, buf);
	int width = atoi(buf);
	pnm_read(file, buf);
	int height = atoi(buf);

	pnm_read(file, buf);
	if (atoi(buf) > UCHAR_MAX)
	{
		fprintf(stderr, "[!] Error: Could not read file: %s \n", name);
		return NULL;
	}

	// read data
	simple_image* im = new simple_image(width, height, 1);

	if(!im){
		fprintf( stderr, "[!] Error: Cant allocate memory for image!\n");
		file.close();
		return NULL;
	}

	file.read(im->data, im->size);
	file.close();

	return im;
}

// save grayscale image to .PGM-file
void save_pgm_image(simple_image *im, const char *name)
{
	if(!im || !name)
	{
		fprintf(stderr, "[!] Error: Bad image or file name: %s \n", name);
		return;
	}
	int width = im->width;
	int height = im->height;
	std::ofstream file(name, std::ios::out | std::ios::binary);

	if(!file.is_open())
	{
		fprintf(stderr, "[!] Error: Could not open file: %s \n", name);
		return;
	}

	file << "P5\n" << width << " " << height << "\n" << UCHAR_MAX << "\n";
	file.write(im->data, im->size);
	file.close();
}

void release_image(simple_image** pim)
{
	if(pim && *pim)
	{
		(*pim)->reset();
		(*pim) = NULL;
	}
}

void release_image(orv_image** pim)
{
	if(pim && *pim)
	{
		(*pim)->reset();
		(*pim) = NULL;
	}
}

orv_image* simple_image2orv_image(simple_image *im)
{
	if(!im)
		return NULL;

	orv_image* image = new orv_image(im->width, im->height, im->nChannels, DEPTH_8U);
	memcpy(image->data, im->data, im->size);

	return image;
}

simple_image* orv_image2simple_image(orv_image *im)
{
	if(!im)
		return NULL;

	if(im->type == DEPTH_8S || im->type == DEPTH_8U)
	{
		simple_image* image = new simple_image(im->width, im->height, im->nChannels);
		memcpy(image->data, im->data, im->size);
		return image;
	}
	return NULL;
}

#ifdef USE_OPENCV
IplImage* simple_image2IplImage(simple_image *im)
{
	if(!im)
		return NULL;

	IplImage* image = cvCreateImage(cvSize(im->width, im->height), IPL_DEPTH_8U, im->nChannels);
	memcpy(image->imageData, im->data, im->size);

	return image;
}

IplImage* orv_image2IplImage(orv_image *im)
{
	if(!im)
		return NULL;

	int depth = IPL_DEPTH_8U;
	switch(im->type)
	{
	case DEPTH_8S:
		depth = IPL_DEPTH_8S;
		break;
	case DEPTH_16S:
		depth = IPL_DEPTH_16S;
		break;
	case DEPTH_16U:
		depth = IPL_DEPTH_16U;
		break;
	case DEPTH_32F:
		depth = IPL_DEPTH_32F;
		break;
	case DEPTH_8U:
	default:
		depth = IPL_DEPTH_8U;
		break;
	}

	IplImage* image = cvCreateImage(cvSize(im->width, im->height), depth, im->nChannels);
	memcpy(image->imageData, im->data, im->size);

	return image;
}

simple_image* IplImage2simple_image(IplImage *img)
{
	if(!img)
		return NULL;

	simple_image* im = new simple_image(img->width, img->height, img->nChannels);
	memcpy(im->data, img->imageData, img->imageSize);

	return im;
}

void show_image(const char* window_name, orv_image* im, int wait_key_ms, bool delete_window, bool save_image, const char* file_name)
{
	if(!window_name || !im)
		return;

	IplImage* tmp = orv_image2IplImage(im);
	if(tmp)
	{ 
		cvNamedWindow(window_name);
		cvShowImage(window_name, tmp);
		cvWaitKey(wait_key_ms);
		if(delete_window)
			cvDestroyWindow(window_name);
		if(save_image && file_name)
			cvSaveImage(file_name, tmp);
		cvReleaseImage(&tmp);
	}
}
#endif //#ifdef USE_OPENCV

// make image border  (cvCopyMakeBorder)
void image_border(orv_image* src, orv_image* dst, orv_point offset, int border_type, orv_scalar value)
{
	if(!src || !dst)
		return;

	if(dst->width - src->width < offset.x ||  dst->height - src->height < offset.y)
	{
		fprintf( stderr, "[!] Error: bad image size or offset!\n");
		return;
	}

	if( (src->type != DEPTH_8S && src->type != DEPTH_8U) ||
		(dst->type != DEPTH_8S && dst->type != DEPTH_8U) )
	{
		fprintf( stderr, "[!] Error: bad image format!\n");
		return;
	}

	int x, y, i, j;

	if(border_type == BORDER_CONSTANT)
	{
		dst->data_set( static_cast<int>(value.val[0]) );
	}
	else if(border_type == BORDER_REPLICATE)
	{
		///@TODO
	}

	for(y=0, j=offset.y; y<src->height; j++, y++)
	{
		for(x=0, i=offset.x; x<src->width; i++, x++)
		{
			((TYPE_DEPTH_8U*)dst->data)[j*dst->step + i] = ((TYPE_DEPTH_8U*)src->data)[y*src->step + x];
		}
	}

}

// calc image gradient (cvSobel)
int image_gradient(orv_image* src, orv_image** magnitude, orv_image** orientation)
{
	if(!src)
		return -1;

	if(src->nChannels!=1)
	{
		fprintf(stderr, "[!] convert to grayscale first!\n");
		return -2;
	}

	orv_image* image = new orv_image(src->width+2, src->height+2, src->nChannels, src->type);
	*magnitude = new orv_image( src->width+2, src->height+2, src->nChannels, DEPTH_32F);
	*orientation = new orv_image( src->width+2, src->height+2, src->nChannels, DEPTH_32F);

	if(!image || !*magnitude || !*orientation)
	{
		fprintf( stderr, "[!] Error: Cant allocate memory for image!\n");
		return -3;
	}

	(*magnitude)->data_zero();
	(*orientation)->data_zero();

	// make borders
	image_border(src, image, orv_point(1, 1));

//	show_image("border", image);
//	image->save("border.pgm");

	int i, j;
	float dx, dy;

	for( j=1; j<image->height-1; j++)
	{
		for( i=1; i<image->width-1; i++)
		{
			// calc grad
			dx = (float)( ORV_PIXEL(TYPE_DEPTH_8U, image, i+1, j)[0] - ORV_PIXEL(TYPE_DEPTH_8U, image, i-1, j)[0] );
			dy = (float)( ORV_PIXEL(TYPE_DEPTH_8U, image, i, j+1)[0] - ORV_PIXEL(TYPE_DEPTH_8U, image, i, j-1)[0] );

			ORV_PIXEL(TYPE_DEPTH_32F, *magnitude, i, j)[0] = sqrt(dx*dx + dy*dy);
			ORV_PIXEL(TYPE_DEPTH_32F, *orientation, i, j)[0] = atan2f(dy, dx);
		}
	}

	release_image(&image);

	return 0;
}

// определение к какому из 4 вариантов относится угол
int _what_angle(float val)
{
	float grad = RAD_TO_DEG(val);

	grad /= 2;

	if( (grad<=22.5 && grad> 337.5) /*|| (grad>157.5 && grad<=202.5)*/ ){
		return 0;
	}
	else if( (grad>22.5 && grad<=67.5) /*|| (grad>202.5 && grad<=247.5)*/ ){
		return 45;
	}
	else if( (grad>67.5 && grad<=112.5) /*|| (grad>247.5 && grad<=292.5)*/ ){
		return 90;
	}
	else{
		return 135;
	}

	return 0;
}

// calc thin borders
int image_thin_borders(orv_image* src, orv_image** dst)
{
	if(!src)
		return -1;

	if(src->nChannels!=1)
	{
		fprintf(stderr, "[!] convert to grayscale first!\n");
		return -2;
	}

	orv_image* magnitude = NULL;
	orv_image* orientation = NULL;

	image_gradient(src, &magnitude, &orientation);

	if(!magnitude || !orientation)
	{
		fprintf( stderr, "[!] Error: calc gradients!\n");
		return -2;
	}

//	show_image("magnitude", magnitude, 1, false, false, "magnitude.bmp");
//	show_image("orientation", orientation, 1, false, false, "orientation.bmp");

	*dst = new orv_image( src->width, src->height, 1, DEPTH_8U);
	if(!*dst)
	{
		fprintf( stderr, "[!] Error: Cant allocate memory for image!\n");
		return -3;
	}

	(*dst)->data_zero();

	int i,j, x, y;

	for( j=1, y=0; j<magnitude->height-1; j++, y++)
	{	
		for( i=1, x=0; i<magnitude->width-1; i++, x++)
		{
			float m = ORV_PIXEL(TYPE_DEPTH_32F, magnitude, i, j)[0];
			float ori = ORV_PIXEL(TYPE_DEPTH_32F, orientation, i, j)[0];
			float m1=0, m2=0;

			unsigned char val = 0;

			// в зависимости от угла градиента 
			// проверяем соответствующих соседей
			switch( _what_angle(ori) )
			{
			case 0:
				m1 = ORV_PIXEL(TYPE_DEPTH_32F, magnitude, i, j-1)[0];
				m2 = ORV_PIXEL(TYPE_DEPTH_32F, magnitude, i, j+1)[0];
				if(m>m1 && m>m2){
					val=255;
				}
				break;
			case 45:
				m1 = ORV_PIXEL(TYPE_DEPTH_32F, magnitude, i+1, j-1)[0];
				m2 = ORV_PIXEL(TYPE_DEPTH_32F, magnitude, i-1, j+1)[0];
				if(m>m1 && m>m2){
					val=255;
				}
				break;
			case 90:
				m1 = ORV_PIXEL(TYPE_DEPTH_32F, magnitude, i+1, j)[0];
				m2 = ORV_PIXEL(TYPE_DEPTH_32F, magnitude, i-1, j)[0];
				if(m>m1 && m>m2){
					val=255;
				}
				break;
			case 135:
				m1 = ORV_PIXEL(TYPE_DEPTH_32F, magnitude, i-1, j-1)[0];
				m2 = ORV_PIXEL(TYPE_DEPTH_32F, magnitude, i+1, j+1)[0];
				if(m>m1 && m>m2){
					val=255;
				}
				break;
			default:
				break;
			}

			ORV_PIXEL(TYPE_DEPTH_8U, *dst, x, y)[0] = val;
		}
	}

	release_image(&magnitude);
	release_image(&orientation);

	return 0;
}
