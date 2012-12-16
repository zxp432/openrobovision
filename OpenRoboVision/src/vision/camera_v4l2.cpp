//
// OpenRoboVision
//
// object for get frames from camera under Linux (via V4L2)
//
// объект для получения картинки с видеокамеры под Linux-ом (через V4L2)
//
//
// robocraft.ru
//

//
// based on V4L2 video capture example
// http://v4l2spec.bytesex.org/spec/capture-example.html
// http://code.google.com/p/libv4l2cam/
// OpenCV/modules/highgui/src/cap_v4l.cpp
//
// V4L2 API Specification
// http://www.linuxtv.org/downloads/legacy/video4linux/API/V4L2_API/spec-single/v4l2.html
// http://v4l2spec.bytesex.org/
//

# if defined(LINUX) && defined(USE_V4L2_LIB)

#include "orv/vision/camera_v4l2.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <fcntl.h>              // low-level i/o
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h> // stat()
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <asm/types.h>          // for videodev2.h

#include <linux/videodev2.h>

// IOCTL handling for V4L2
static int xioctl( int fd, int request, void *arg)
{
	int r, itt = 0;

	do
	{
		r = ioctl (fd, request, arg);
		itt++;
	}
	while ((-1 == r) && (EINTR == errno) && (itt<100));

	return r;
}

static void errno_exit (const char* s)
{
	fprintf ( stderr, "[!] %s error %d, %s\n", s, errno, strerror(errno) );
	exit (EXIT_FAILURE);
}

CameraV4L2::CameraV4L2(const char* _dev_name):
dev_name(_dev_name),
fd(-1),
io(IO_METHOD_MMAP),
buffers(0),
n_buffers(0),
fps(DEFAULT_V4L2_FPS),
imageCounter(0),
buf_size(0)
{
	assert(_dev_name);
	if(!_dev_name)
	{
		printf("[!]][CameraV4L2] Error: empty device name!\n");
		return;
	}

	width = DEFAULT_V4L2_IMAGE_WIDTH;
	height = DEFAULT_V4L2_IMAGE_HEIDHT;
	
	// инициализация
	init();
}

CameraV4L2::CameraV4L2(const char* _dev_name, int _width, int _height, int _fps, io_method _method):
dev_name(_dev_name),
fd(-1),
io(_method),
buffers(0),
n_buffers(0),
fps(_fps),
imageCounter(0),
buf_size(0)
{
	assert(_dev_name);
	if(!_dev_name)
	{
		printf("[!]][CameraV4L2] Error: empty device name!\n");
		return;
	}

	width = _width;
	height = _height;
	
	// инициализация
	init();
}

CameraV4L2::~CameraV4L2() 
{
	this->end();
}

// инициализация 
void CameraV4L2::init()
{
	this->open_device(dev_name.c_str());
	this->init_device();
	this->start_capturing();
	initialised = true;
}

void CameraV4L2::update()
{
	int read = 0;

	// remove previous frame
	remove_frame();

	// get image
	this->mainloop();

	if (!frame)
	{
		printf("[!][CameraV4L2][update] Error: cant get frame!\n");
		return;
	}

	imageCounter++;

#if defined(USE_OPENCV)
	if(show_capture)
	{
		cvShowImage(window_name, frame);
	}
#endif //#if defined(USE_OPENCV)
}

// прекратить видеозахват
void CameraV4L2::end()
{
	if(initialised)
	{
		this->stop_capturing();
		this->uninit_device();
		this->close_device();
		initialised = false;
	}

	remove_frame();
}

//
// методы работы V4L2
//

// открыть файл устройства
void CameraV4L2::open_device(const char* _dev_name)
{
	if(!_dev_name)
		return;

	// get file status
	struct stat st;
	if(-1 == stat(_dev_name, &st))
	{
		fprintf (stderr, "[!][CameraV4L2][open_device] Error: Cannot identify '%s': %d, %s\n",
			_dev_name, errno, strerror (errno));
		exit(EXIT_FAILURE);
	}

	if(!S_ISCHR(st.st_mode)) // character device?
	{ 
		fprintf (stderr, "[!][CameraV4L2][open_device] Error: %s is no device\n", _dev_name);
		exit(EXIT_FAILURE);
	}

	fd = open (_dev_name, O_RDWR /* required */ | O_NONBLOCK, 0);

	if (-1 == fd)
	{
		fprintf (stderr, "[!][CameraV4L2][open_device] Error: cannot open '%s': %d, %s\n",
			_dev_name, errno, strerror (errno));
		exit(EXIT_FAILURE);
	}
}

// инициализация V4L2
void CameraV4L2::init_device()
{
	struct v4l2_capability cap;
	struct v4l2_cropcap cropcap;
	struct v4l2_crop crop;
	struct v4l2_format fmt;
	unsigned int min;
	int res;

	if (-1 == xioctl (fd, VIDIOC_QUERYCAP, &cap))
	{
		if (EINVAL == errno)
		{
			fprintf (stderr, "[!][CameraV4L2][init_device] %s is no V4L2 device\n",
				dev_name.c_str());
			exit (EXIT_FAILURE);
		}
		else
		{
			errno_exit ("VIDIOC_QUERYCAP");
		}
	}

	if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
	{
		fprintf (stderr, "[!][CameraV4L2][init_device] %s is no video capture device\n",
			dev_name.c_str());
		exit (EXIT_FAILURE);
	}

	switch (io) 
	{
	case IO_METHOD_READ:
		if (!(cap.capabilities & V4L2_CAP_READWRITE))
		{
			fprintf (stderr, "[!][CameraV4L2][init_device] %s does not support read i/o\n",
				dev_name.c_str());
			exit (EXIT_FAILURE);
		}

		break;

	case IO_METHOD_MMAP:
	case IO_METHOD_USERPTR:
		if (!(cap.capabilities & V4L2_CAP_STREAMING))
		{
			fprintf (stderr, "[!][CameraV4L2][init_device] %s does not support streaming i/o\n",
				dev_name.c_str());
			exit (EXIT_FAILURE);
		}

		break;
	}


	// Select video input, video standard and tune here.


	CLEAR (cropcap);

	cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (0 == xioctl (fd, VIDIOC_CROPCAP, &cropcap))
	{
		crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		crop.c = cropcap.defrect; // reset to default

		if (-1 == xioctl (fd, VIDIOC_S_CROP, &crop))
		{
			switch (errno) 
			{
			case EINVAL:
				// Cropping not supported.
				break;
			default:
				// Errors ignored.
				break;
			}
		}
	}
	else
	{	
		// Errors ignored.
	}

	// установка параметров видео
	CLEAR (fmt);

	fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width       = width;
	fmt.fmt.pix.height      = height;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_BGR24; 
	//fmt.fmt.pix.pixelformat =  V4L2_PIX_FMT_YUYV;
	fmt.fmt.pix.field       = V4L2_FIELD_ANY; 
	//fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED;

	if (-1 == xioctl (fd, VIDIOC_S_FMT, &fmt))
	{
		errno_exit ("VIDIOC_S_FMT");
	}

	// установка FPS
	printf("[i][CameraV4L2] try to set FPS to %d \n", fps); //$LOG
	struct v4l2_streamparm setfps;
	CLEAR(setfps);
	setfps.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	setfps.parm.capture.timeperframe.numerator=1;
	setfps.parm.capture.timeperframe.denominator=fps;
	setfps.parm.output.timeperframe.numerator=1;
	setfps.parm.output.timeperframe.denominator=fps;

	if(-1 == xioctl(fd, VIDIOC_S_PARM, &setfps))
	{
		//errno_exit("VIDIOC_S_PARM");
		printf("[!][CameraV4L2] Error: set FPS! (VIDIOC_S_PARM)\n"); //$LOG
	}
	
	// get standard
	v4l2_std_id std_id = 0;
	res = xioctl(fd, VIDIOC_G_STD, &std_id);
	printf("[i][CameraV4L2][init_device] Get std_id=%d res=%d\n", (int)std_id, res); //$LOG

#if 1
	// set standard
	struct v4l2_standard st;
	CLEAR(st);
	st.id = V4L2_STD_ALL; // задать стандарт видеосигнала
	res = xioctl(fd, VIDIOC_S_STD, &st.id);
	printf("[i][CameraV4L2][init_device] Set standard=%d res=%d\n", (int)st.id, res); //$LOG
#endif

	struct v4l2_input inp;
	CLEAR(inp);
	inp.index=1;
	inp.type = V4L2_INPUT_TYPE_CAMERA;
	inp.std = (std_id & V4L2_STD_PAL);
	if(-1==xioctl(fd, VIDIOC_ENUMINPUT, &inp))
	{
		errno_exit("VIDIOC_ENUMINPUT");
	}

#if 0
	//controls
	struct v4l2_control control;
	CLEAR(control);
	control.id = V4L2_CID_EXPOSURE_AUTO;
	control.value=0;
	if(-1==xioctl(fd, VIDIOC_S_CTRL, &control))
	{
		errno_exit("VIDIOC_S_CTL");
	}
#endif

	// вывод информации о заданных параметрах
	char fourcc[5] = {0, 0, 0, 0, 0};
	memmove(fourcc, &fmt.fmt.pix.pixelformat, 4);
	printf("[i][CameraV4L2][init_device] capture: frame: %d x %d format: %4s size: %d\n", 
	fmt.fmt.pix.width,
	fmt.fmt.pix.height,
	fourcc,
	fmt.fmt.pix.sizeimage);

#if 1
	//--------------------------------------------------
	// libv4l2cam

	//default values, mins and maxes
	struct v4l2_queryctrl queryctrl;

	CLEAR(queryctrl);
	queryctrl.id = V4L2_CID_BRIGHTNESS;
	if(-1 == xioctl (fd, VIDIOC_QUERYCTRL, &queryctrl))
	{
		if(errno != EINVAL)
		{
			//perror ("VIDIOC_QUERYCTRL");
			//exit(EXIT_FAILURE);
			printf("[i][CameraV4L2][init_device] brightness error\n");
		}
		else
		{
			printf("[i][CameraV4L2][init_device] brightness is not supported\n");
		}
	}
	else if(queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
	{
		printf ("[i][CameraV4L2][init_device] brightness is not supported\n");
	}
	mb=queryctrl.minimum;
	Mb=queryctrl.maximum;
	db=queryctrl.default_value;

	CLEAR(queryctrl);
	queryctrl.id = V4L2_CID_CONTRAST;
	if(-1 == xioctl (fd, VIDIOC_QUERYCTRL, &queryctrl))
	{
		if(errno != EINVAL)
		{
			//perror ("VIDIOC_QUERYCTRL");
			//exit(EXIT_FAILURE);
			printf("[i][CameraV4L2][init_device] contrast error\n");
		}
		else
		{
			printf("[i][CameraV4L2][init_device] contrast is not supported\n");
		}
	}
	else if(queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
	{
		printf ("[i][CameraV4L2][init_device] contrast is not supported\n");
	}
	mc=queryctrl.minimum;
	Mc=queryctrl.maximum;
	dc=queryctrl.default_value;

	CLEAR(queryctrl);
	queryctrl.id = V4L2_CID_SATURATION;
	if(-1 == xioctl (fd, VIDIOC_QUERYCTRL, &queryctrl))
	{
		if(errno != EINVAL)
		{
			//perror ("VIDIOC_QUERYCTRL");
			//exit(EXIT_FAILURE);
			printf("[i][CameraV4L2][init_device] saturation error\n");
		}
		else
		{
			printf("[i][CameraV4L2][init_device] saturation is not supported\n");
		}
	}
	else if(queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
	{
		printf ("[i][CameraV4L2][init_device] saturation is not supported\n");
	}
	ms=queryctrl.minimum;
	Ms=queryctrl.maximum;
	ds=queryctrl.default_value;

	CLEAR(queryctrl);
	queryctrl.id = V4L2_CID_HUE;
	if(-1 == xioctl (fd, VIDIOC_QUERYCTRL, &queryctrl))
	{
		if(errno != EINVAL)
		{
			//perror ("VIDIOC_QUERYCTRL");
			//exit(EXIT_FAILURE);
			printf("[i][CameraV4L2][init_device] hue error\n");
		}
		else
		{
			printf("[i][CameraV4L2][init_device] hue is not supported\n");
		}
	}
	else if(queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
	{
		printf ("[i][CameraV4L2][init_device] hue is not supported\n");
	}
	mh=queryctrl.minimum;
	Mh=queryctrl.maximum;
	dh=queryctrl.default_value;

	CLEAR(queryctrl);
	queryctrl.id = V4L2_CID_HUE_AUTO;
	if(-1 == xioctl (fd, VIDIOC_QUERYCTRL, &queryctrl))
	{
		if(errno != EINVAL)
		{
			//perror ("VIDIOC_QUERYCTRL");
			//exit(EXIT_FAILURE);
			printf("[i][CameraV4L2][init_device] hueauto error\n");
		}
		else
		{
			printf("[i][CameraV4L2][init_device] hueauto is not supported\n");
		}
	} 
	else if(queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
	{
		printf ("[i][CameraV4L2][init_device] hueauto is not supported\n");
	}
	ha=queryctrl.default_value;

	CLEAR(queryctrl);
	queryctrl.id = V4L2_CID_SHARPNESS;
	if(-1 == xioctl (fd, VIDIOC_QUERYCTRL, &queryctrl))
	{
		if(errno != EINVAL)
		{
			//perror ("VIDIOC_QUERYCTRL");
			//exit(EXIT_FAILURE);
			printf("[i][CameraV4L2][init_device] sharpness error\n");
		}
		else
		{
			printf("[i][CameraV4L2][init_device] sharpness is not supported\n");
		}
	}
	else if(queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
	{
		printf ("[i][CameraV4L2][init_device] sharpness is not supported\n");
	}
	msh=queryctrl.minimum;
	Msh=queryctrl.maximum;
	dsh=queryctrl.default_value;
	//--------------------------------------------------
#endif

	// Note VIDIOC_S_FMT may change width and height.

	// Buggy driver paranoia.
	min = fmt.fmt.pix.width * 2;
	if (fmt.fmt.pix.bytesperline < min)
		fmt.fmt.pix.bytesperline = min;
	min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
	if (fmt.fmt.pix.sizeimage < min)
		fmt.fmt.pix.sizeimage = min;

	switch (io)
	{
	case IO_METHOD_READ:
		init_read (fmt.fmt.pix.sizeimage);
		break;

	case IO_METHOD_MMAP:
		init_mmap ();
		break;

	case IO_METHOD_USERPTR:
		init_userp (fmt.fmt.pix.sizeimage);
		break;
	}
	buf_size = fmt.fmt.pix.sizeimage;
}
void CameraV4L2::start_capturing()
{
	unsigned int i;
	enum v4l2_buf_type type;

	switch (io) {
	case IO_METHOD_READ:
		// Nothing to do.
		break;

	case IO_METHOD_MMAP:
		for (i = 0; i < n_buffers; ++i)
		{
			struct v4l2_buffer buf;

			CLEAR (buf);

			buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			buf.memory      = V4L2_MEMORY_MMAP;
			buf.index       = i;

			if (-1 == xioctl (fd, VIDIOC_QBUF, &buf))
				errno_exit ("VIDIOC_QBUF");
		}

		type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

		if (-1 == xioctl (fd, VIDIOC_STREAMON, &type))
			errno_exit ("VIDIOC_STREAMON");

		break;

	case IO_METHOD_USERPTR:
		for (i = 0; i < n_buffers; ++i)
		{
			struct v4l2_buffer buf;

			CLEAR (buf);

			buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			buf.memory      = V4L2_MEMORY_USERPTR;
			buf.index       = i;
			buf.m.userptr	= (unsigned long) buffers[i].start;
			buf.length      = buffers[i].length;

			if (-1 == xioctl (fd, VIDIOC_QBUF, &buf))
				errno_exit ("VIDIOC_QBUF");
		}

		type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

		if (-1 == xioctl (fd, VIDIOC_STREAMON, &type))
			errno_exit ("VIDIOC_STREAMON");

		break;
	}
}
void CameraV4L2::stop_capturing()
{
	enum v4l2_buf_type type;

	switch (io)
	{
	case IO_METHOD_READ:
		// Nothing to do.
		break;

	case IO_METHOD_MMAP:
	case IO_METHOD_USERPTR:
		type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

		if (-1 == xioctl (fd, VIDIOC_STREAMOFF, &type))
			errno_exit ("VIDIOC_STREAMOFF");

		break;
	}
}
void CameraV4L2::uninit_device()
{
	unsigned int i;

	switch (io)
	{
	case IO_METHOD_READ:
		free (buffers[0].start);
		break;

	case IO_METHOD_MMAP:
		for (i = 0; i < n_buffers; ++i)
			if (-1 == munmap (buffers[i].start, buffers[i].length))
				errno_exit ("munmap");
		break;

	case IO_METHOD_USERPTR:
		for (i = 0; i < n_buffers; ++i)
			free (buffers[i].start);
		break;
	}

	free (buffers);
}
void CameraV4L2::close_device()
{
	if (-1 == close (fd))
		errno_exit ("close");

	fd = -1;
}
void CameraV4L2::init_userp(unsigned int buffer_size)
{
	struct v4l2_requestbuffers req;
	unsigned int page_size;
	// get the number of bytes in a page, 
	// where a "page" is the thing used where it says in the description of mmap
	page_size = getpagesize ();
	buffer_size = (buffer_size + page_size - 1) & ~(page_size - 1);

	CLEAR (req);

	req.count               = 4;
	req.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory              = V4L2_MEMORY_USERPTR;

	if (-1 == xioctl (fd, VIDIOC_REQBUFS, &req))
	{
		if (EINVAL == errno)
		{
			fprintf (stderr, "[!][CameraV4L2][init_userp] %s does not support "
				"user pointer i/o\n", dev_name.c_str());
			exit (EXIT_FAILURE);
		}
		else
		{
			errno_exit ("VIDIOC_REQBUFS");
		}
	}

	buffers = (struct buffer *) calloc (4, sizeof (*buffers));

	if (!buffers)
	{
		fprintf (stderr, "[!][CameraV4L2][init_userp] Out of memory\n");
		exit (EXIT_FAILURE);
	}

	for (n_buffers = 0; n_buffers < 4; ++n_buffers)
	{
		buffers[n_buffers].length = buffer_size;
		buffers[n_buffers].start = memalign (/* boundary */ page_size,
			buffer_size);

		if (!buffers[n_buffers].start)
		{
			fprintf (stderr, "[!][CameraV4L2][init_userp] Out of memory\n");
			exit (EXIT_FAILURE);
		}
	}
}
void CameraV4L2::init_mmap()
{
	struct v4l2_requestbuffers req;

	CLEAR (req);

	req.count               = 4;
	req.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory              = V4L2_MEMORY_MMAP;

	if (-1 == xioctl (fd, VIDIOC_REQBUFS, &req))
	{
		if (EINVAL == errno)
		{
			fprintf (stderr, "[!][CameraV4L2][init_mmap] %s does not support memory mapping\n", 
				dev_name.c_str());
			exit (EXIT_FAILURE);
		}
		else
		{
			errno_exit ("VIDIOC_REQBUFS");
		}
	}

	if (req.count < 2)
	{
		fprintf (stderr, "[!][CameraV4L2][init_mmap] Insufficient buffer memory on %s\n",
			dev_name.c_str());
		exit (EXIT_FAILURE);
	}

	buffers = (struct buffer *) calloc (req.count, sizeof (*buffers));

	if (!buffers)
	{
		fprintf (stderr, "[!][CameraV4L2][init_mmap] Out of memory\n");
		exit (EXIT_FAILURE);
	}

	for (n_buffers = 0; n_buffers < req.count; ++n_buffers)
	{
		struct v4l2_buffer buf;

		CLEAR (buf);

		buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory      = V4L2_MEMORY_MMAP;
		buf.index       = n_buffers;

		if (-1 == xioctl (fd, VIDIOC_QUERYBUF, &buf))
			errno_exit ("VIDIOC_QUERYBUF");

		buffers[n_buffers].length = buf.length;
		buffers[n_buffers].start =
			mmap (NULL, // start anywhere
			buf.length,
			PROT_READ | PROT_WRITE, // required
			MAP_SHARED, // recommended
			fd, buf.m.offset);

		if (MAP_FAILED == buffers[n_buffers].start)
			errno_exit ("mmap");
	}
}
void CameraV4L2::init_read(unsigned int buffer_size)
{
	printf("[i][CameraV4L2][init_read]...\n");
	buffers = (struct buffer *) calloc (1, sizeof (*buffers));

	if (!buffers)
	{
		fprintf (stderr, "[!][CameraV4L2][init_read] Out of memory\n");
		exit (EXIT_FAILURE);
	}

	buffers[0].length = buffer_size;
	buffers[0].start = malloc (buffer_size);

	if (!buffers[0].start)
	{
		fprintf (stderr, "[!][CameraV4L2][init_read] Out of memory\n");
		exit (EXIT_FAILURE);
	}
}
void CameraV4L2::mainloop(void)
{
	unsigned int count;

	count = 1;
	
	char key=0;
//	while( key != 27){ 
//	    key = cvWaitKey(33);
	while (count-- > 0) 
	{
		for (;;) {
			fd_set fds;
			struct timeval tv;
			int r;

			FD_ZERO (&fds);
			FD_SET (fd, &fds);

			// Timeout.
			tv.tv_sec = 2;
			tv.tv_usec = 0;

			r = select (fd + 1, &fds, NULL, NULL, &tv);
			if (-1 == r)
			{
				if (EINTR == errno)
					continue;

				errno_exit ("select");
			}

			if (0 == r)
			{
				fprintf (stderr, "[!][CameraV4L2][mainloop] select timeout\n");
				exit (EXIT_FAILURE);
			}

			if (read_frame ())
				break;

			// EAGAIN - continue select loop.
		}
	}
}
int CameraV4L2::read_frame(void)
{
	struct v4l2_buffer buf;
	unsigned int i;
	int res = 0;

	switch (io) 
	{
	case IO_METHOD_READ:
		res = read (fd, buffers[0].start, buffers[0].length);
		if(-1 == res) {
			switch (errno) 
			{
			case EAGAIN:
				return 0;

			case EIO:
				// Could ignore EIO, see spec.

				// fall through

			default:
				errno_exit ("read");
			}
		}
		printf("[i] read %d\n", res); //$LOG
		process_image (buffers[0].start);

		break;

	case IO_METHOD_MMAP:
		CLEAR (buf);

		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;

		if (-1 == xioctl (fd, VIDIOC_DQBUF, &buf))
		{
			switch (errno) 
			{
			case EAGAIN:
				return 0;

			case EIO:
				// Could ignore EIO, see spec.

				// fall through

			default:
				return 0; //errno_exit ("VIDIOC_DQBUF");
			}
		}

		assert (buf.index < n_buffers);

		process_image (buffers[buf.index].start);

		if (-1 == xioctl (fd, VIDIOC_QBUF, &buf))
			return 0; //errno_exit ("VIDIOC_QBUF");

		break;

	case IO_METHOD_USERPTR:
		CLEAR (buf);

		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_USERPTR;

		if (-1 == xioctl (fd, VIDIOC_DQBUF, &buf))
		{
			switch (errno)
			{
			case EAGAIN:
				return 0;

			case EIO:
				// Could ignore EIO, see spec.

				// fall through

			default:
				return 0; //errno_exit ("VIDIOC_DQBUF");
			}
		}

		for (i = 0; i < n_buffers; ++i)
			if (buf.m.userptr == (unsigned long) buffers[i].start
				&& buf.length == buffers[i].length)
				break;

		assert (i < n_buffers);

		process_image ((void *) buf.m.userptr);

		if (-1 == xioctl (fd, VIDIOC_QBUF, &buf))
			return 0; //errno_exit ("VIDIOC_QBUF");

		break;
	}

	return 1;
}
void CameraV4L2::process_image(const void* p)
{
	//fputc ('.', stdout);
	//fflush (stdout);

#if defined(USE_OPENCV)
	frame = this->makePictureFromData((const char*)p, buf_size);

# if VIDEOV4L2_SAVE_IMAGE
	if(frame)
	{
		char file[BUF_SIZE0];
		snprintf(file, BUF_SIZE0, "image%02d.bmp", imageCounter);
		cvSaveImage(file, frame);
	}
# endif //# if VIDEOV4L2_SAVE_IMAGE
#else
	// без OpenCV:
	// копирование полученых данных в буфер
	if(frame)
	{
		delete []frame;
		frame = 0;
	}
	frame_size = 0;

	frame = new char[buf_size];
	if(frame)
	{
		memcpy(frame, (char*)p, buf_size);
		frame_size = buf_size;	
	}
	else
	{
		printf("[!][CameraV4L2][process_image] Error: cant allocate memory!\n");
	}
#endif //#if defined(USE_OPENCV)
}

int CameraV4L2::setBrightness(int v)
{
	if(v<mb || v>Mb)
		return -1;

	struct v4l2_control control;
	control.id = V4L2_CID_BRIGHTNESS;
	control.value = v;

	if(-1 == ioctl (fd, VIDIOC_S_CTRL, &control))
	{
		printf("[!][CameraV4L2] Error setting brightness!\n");
		return -1;
	}

	return 1;
}
int CameraV4L2::setContrast(int v)
{
	if(v<mc || v>Mc)
		return -1;

	struct v4l2_control control;
	control.id = V4L2_CID_CONTRAST;
	control.value = v;

	if(-1 == ioctl (fd, VIDIOC_S_CTRL, &control))
	{
		printf("[!][CameraV4L2] Error setting contrast!\n");
		return -1;
	}

	return 1;
}
int CameraV4L2::setSaturation(int v)
{
	if(v<ms || v>Ms)
		return -1;

	struct v4l2_control control;
	control.id = V4L2_CID_SATURATION;
	control.value = v;

	if(-1 == ioctl (fd, VIDIOC_S_CTRL, &control))
	{
		printf("[!][CameraV4L2] Error setting saturation!\n");
		return -1;
	}

	return 1;
}
int CameraV4L2::setHue(int v)
{
	if(v<mh || v>Mh)
		return -1;

	struct v4l2_control control;
	control.id = V4L2_CID_HUE;
	control.value = v;

	if(-1 == ioctl (fd, VIDIOC_S_CTRL, &control))
	{
		printf("[!][CameraV4L2] Error setting hue!\n");
		return -1;
	}

	return 1;
}
int CameraV4L2::setHueAuto(bool v)
{
	if(v<mh || v>Mh)
		return -1;

	struct v4l2_control control;
	control.id = V4L2_CID_HUE_AUTO;
	control.value = v;

	if(-1 == ioctl (fd, VIDIOC_S_CTRL, &control))
	{
		printf("[!][CameraV4L2] Error setting hue auto!\n");
		return -1;
	}

	return 1;
}
int CameraV4L2::setSharpness(int v)
{
	if(v<mh || v>Mh)
		return -1;

	struct v4l2_control control;
	control.id = V4L2_CID_SHARPNESS;
	control.value = v;

	if(-1 == ioctl (fd, VIDIOC_S_CTRL, &control))
	{
		printf("[!][CameraV4L2] Error setting sharpness!\n");
		return -1;
	}

	return 1;
}

#if defined(USE_OPENCV)
// получить картинку из массива данных YUYV
void CameraV4L2::YUYVdata_to_IplImage(const char* src, int src_size, IplImage *dst)
{
	if(!src || src_size<=0 || !dst)
	{
		return;
	}

	unsigned char *l_=(unsigned char *)dst->imageData;

	int w2 = width/2;

	for(int x=0; x<w2; x++)
	{
		for(int y=0; y<height; y++)
		{
			int y0, y1, u, v; //y0 u y1 v

			int i=(y*w2+x)*4;
			y0=src[i];
			u=src[i+1];
			y1=src[i+2];
			v=src[i+3];

			int r, g, b;
			r =(int)( y0 + (1.370705 * (v-128)) );
			g =(int)( y0 - (0.698001 * (v-128)) - (0.337633 * (u-128)) );
			b =(int)( y0 + (1.732446 * (u-128)) );

			if(r > 255) r = 255;
			if(g > 255) g = 255;
			if(b > 255) b = 255;
			if(r < 0) r = 0;
			if(g < 0) g = 0;
			if(b < 0) b = 0;

			i=(y*dst->width+2*x)*3;
			l_[i] = (unsigned char)(b); //B
			l_[i+1] = (unsigned char)(g); //G
			l_[i+2] = (unsigned char)(r); //R


			r =(int)( y1 + (1.370705 * (v-128)) );
			g =(int)( y1 - (0.698001 * (v-128)) - (0.337633 * (u-128)) );
			b =(int)( y1 + (1.732446 * (u-128)) );

			if(r > 255) r = 255;
			if(g > 255) g = 255;
			if(b > 255) b = 255;
			if(r < 0) r = 0;
			if(g < 0) g = 0;
			if(b < 0) b = 0;

			l_[i+3] = (unsigned char)(b); //B
			l_[i+4] = (unsigned char)(g); //G
			l_[i+5] = (unsigned char)(r); //R

		}
	}
}
#endif //#if defined(USE_OPENCV)

#if defined(USE_OPENCV)
// получить картинку из массива данных
IplImage* CameraV4L2::makePictureFromData(const char* src, int src_size)
{
	if(!src || src_size<=0)
		return 0;

	IplImage* image = 0;

	try
	{
		image = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);

		if(image)
		{
			CvMat image_mat =  cvMat(height, width, CV_8UC3, (char*)src);
			IplImage tmp;
			// преобразуем матрицу в изображение
			cvGetImage(&image_mat, &tmp);
			cvCopy(&tmp, image);
		}
	}
	catch(...)
	{
		printf("[i][CameraV4L2][makePictureFromData] EXCEPTION!\n");
		if(image)
		{
			cvReleaseImage(&image);
			image = 0;
		}
	}

	return image;
}
#endif //#if defined(USE_OPENCV)

#endif //# if defined(LINUX) && defined(USE_V4L2_LIB)
