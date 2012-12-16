//
// OpenRoboVision
//
// utils
//
// вспомогательные функции
//
//
// robocraft.ru
//

#ifndef _ORV_UTILS_H_
#define _ORV_UTILS_H_

#ifdef USE_OPENCV

#include <cv.h>
#include <highgui.h>

#define CV_PIXEL(type,img,x,y) (((type*)(img->imageData+(y)*img->widthStep))+(x)*img->nChannels)

#endif //#ifdef USE_OPENCV

#endif //#ifndef _ORV_UTILS_H_
