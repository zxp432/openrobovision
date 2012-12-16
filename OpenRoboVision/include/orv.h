//
// OpenRoboVision
//
// set of objects and methods for Robots Vision
// based on OpenCV
//
//
// robocraft.ru
//

#ifndef _OPENROBOVISION_H_
#define _OPENROBOVISION_H_

#include <stdlib.h>
#include <stdio.h>

#ifdef USE_OPENCV
# include <cv.h>
# include <highgui.h>
#endif //#ifdef USE_OPENCV

//
// ORV version
// версия ORV
//
#define ORV_MAJOR_VERSION    0
#define ORV_MINOR_VERSION    2
#define ORV_SUBMINOR_VERSION 9

#define ORV_INCLUDE_ALL 0

#if ORV_INCLUDE_ALL
# include "system/mutex.h"
# include "system/semaphore.h"
# include "system/thread.h"
# include "system/serial.h"
# include "robo/pid.h"
# include "robo/arduino_firmata.h"
# include "vision/orv_utils.h"
# include "vision/hsv_detector.h"
# include "vision/rgb_detector.h"
# include "vision/camera.h"
# include "vision/camera_vi.h"
# include "vision/camera_v4l2.h"
# include "vision/video_reader.h"
# include "vision/video_writer.h"
#endif //#if ORV_INCLUDE_ALL 

//
// add OpenCV library for MSVC linker
//
#ifdef USE_OPENCV
#ifdef _MSC_VER
# if CV_MAJOR_VERSION == 2 && CV_MINOR_VERSION == 0
// OpenCV 2.0
#  pragma comment(lib, "cxcore200.lib")
#  pragma comment(lib, "cv200.lib")
#  pragma comment(lib, "highgui200.lib")
#  pragma comment(lib, "ml200.lib")

#  pragma comment(lib, "cvaux200.lib")

# elif CV_MAJOR_VERSION == 2 && CV_MINOR_VERSION == 1
// OpenCV 2.1
#  if defined(_DEBUG)
#  pragma comment(lib, "cxcore210d.lib")
#  pragma comment(lib, "cv210d.lib")
#  pragma comment(lib, "highgui210d.lib")
#  pragma comment(lib, "ml210d.lib")

#  pragma comment(lib, "cvaux210d.lib")
#  else
#  pragma comment(lib, "cxcore210.lib")
#  pragma comment(lib, "cv210.lib")
#  pragma comment(lib, "highgui210.lib")
#  pragma comment(lib, "ml210.lib")

#  pragma comment(lib, "cvaux210.lib")
#  endif //#  if defined(_DEBUG)

# elif CV_MAJOR_VERSION == 2 && CV_MINOR_VERSION == 2
// OpenCV 2.2
#  pragma comment(lib, "opencv_core220.lib")
#  pragma comment(lib, "opencv_highgui220.lib")
#  pragma comment(lib, "opencv_imgproc220.lib")
#  pragma comment(lib, "opencv_video220.lib")
#  pragma comment(lib, "opencv_ml220.lib")

#  pragma comment(lib, "opencv_objdetect220.lib")
#  pragma comment(lib, "opencv_features2d220.lib")
#  pragma comment(lib, "opencv_contrib220.lib")
#  pragma comment(lib, "opencv_ts220.lib")
#  pragma comment(lib, "opencv_legacy220.lib")
#  pragma comment(lib, "opencv_flann220.lib")
#  pragma comment(lib, "opencv_ffmpeg220.lib")
#  pragma comment(lib, "opencv_gpu220.lib")

# elif CV_MAJOR_VERSION == 2 && CV_MINOR_VERSION == 3 && CV_SUBMINOR_VERSION == 1
// OpenCV 2.3.1
#  if defined(_DEBUG)
#  pragma comment(lib, "opencv_core231d.lib")
#  pragma comment(lib, "opencv_highgui231d.lib")
#  pragma comment(lib, "opencv_imgproc231d.lib")
#  pragma comment(lib, "opencv_video231d.lib")
#  pragma comment(lib, "opencv_ml231d.lib")

#  pragma comment(lib, "opencv_calib3d231d.lib")
#  pragma comment(lib, "opencv_objdetect231d.lib")
#  pragma comment(lib, "opencv_features2d231d.lib")
#  pragma comment(lib, "opencv_contrib231d.lib")
#  pragma comment(lib, "opencv_ts231d.lib")
#  pragma comment(lib, "opencv_legacy231d.lib")
#  pragma comment(lib, "opencv_flann231d.lib")
#  pragma comment(lib, "opencv_gpu231d.lib")
#else
#  pragma comment(lib, "opencv_core231.lib")
#  pragma comment(lib, "opencv_highgui231.lib")
#  pragma comment(lib, "opencv_imgproc231.lib")
#  pragma comment(lib, "opencv_video231.lib")
#  pragma comment(lib, "opencv_ml231.lib")

#  pragma comment(lib, "opencv_calib3d231.lib")
#  pragma comment(lib, "opencv_objdetect231.lib")
#  pragma comment(lib, "opencv_features2d231.lib")
#  pragma comment(lib, "opencv_contrib231.lib")
#  pragma comment(lib, "opencv_ts231.lib")
#  pragma comment(lib, "opencv_legacy231.lib")
#  pragma comment(lib, "opencv_flann231.lib")
#  pragma comment(lib, "opencv_gpu231.lib")
# endif //#  if defined(_DEBUG)

# endif //# if CV_MAJOR_VERSION == 2 && CV_MINOR_VERSION == 0
#endif //#ifdef _MSC_VER
#endif //#ifdef USE_OPENCV

#endif //#ifndef _OPENROBOVISION_H_
