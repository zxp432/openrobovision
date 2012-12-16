//
// OpenRoboVision
//
// object for get frames from camera via MJPG
//
//
//
// robocraft.ru
//

#include "orv/vision/camera_mjpg.h"

// size of buffer for read data from socket
#define CAMERA_MJPG_READ_BUFFER_SIZE 2048
// default buffers size
// (800*600*3) = 1440000
#define CAMERA_MJPG_VIDEO_BUFFER_SIZE 2880000 // (800*600*3) * 2

// find sequence in buffer
const char* find_data_in_buffer(const char *src, int src_size, const char *data, int data_size)
{
	if(!src || !data || src_size<=0 || data_size<=0)
		return 0;

	const char *pt, *hay;
	int n;

	if(src_size < data_size)
		return 0;
	if(src == data)
		return src;

	if(!memcmp(src, data, data_size))
		return src;

	pt = hay = src;
	n = src_size;

	while((pt =(const char*) memchr(hay, data[0], n)) != 0){
		n -= (int) (pt - hay);
		if(n < data_size){
			break;
		}

		if(!memcmp(pt, data, data_size)){
			return pt;
		}

		if(hay == pt){
			n--;
			hay++;
		}
		else{
			hay = pt;
		}
	}

	return 0;
}

CameraMJPG::CameraMJPG():
port(0),
image_counter(0)
{
	init();
}

CameraMJPG::CameraMJPG(const char* _ip_address, const int _port):
ip_address(_ip_address),
port(_port),
image_counter(0)
{
	init();
}

CameraMJPG::CameraMJPG(const char* _ip_address, const int _port, const char* _command):
ip_address(_ip_address),
port(_port),
image_counter(0)
{
	command = _command;
	init();
}

CameraMJPG::~CameraMJPG()
{
	this->disconnect();
	buf_read.reset();
	buf_video.reset();
}

void CameraMJPG::init()
{
	// allocate memory for buffers
	buf_read.create(CAMERA_MJPG_READ_BUFFER_SIZE);
	buf_video.create(CAMERA_MJPG_VIDEO_BUFFER_SIZE);

	connect();
}

// get next frame
void CameraMJPG::update()
{
	int res = 0;

	// remove previous frame
	remove_frame();

	// get image
	while(!frame){
		if( (res = read_data(buf_read.data, buf_read.real_size))>0 )
			get_image(buf_read.data, res);
		else
			break;
	}

	if (!frame) {
		printf("[!][CameraMJPG][update] Error: cant get frame!\n");
		reconnect(); // переподключение
		return;
	}
	if(show_capture)
	{
		cvShowImage(window_name, frame);
	}
}

void CameraMJPG::end()
{
}

// get frame's width/height
int CameraMJPG::getWidth()
{
	return width;
}

int CameraMJPG::getHeight()
{
	return height;
}

//
// MJPG methods
//

int CameraMJPG::connect()
{
	if(socket.create(Socket::TCP) == SOCKET_ERROR)
	{
		return 1;
	}

	if(socket.connect(ip_address.c_str(), port) == SOCKET_ERROR)
	{
		printf("[!][CameraMJPG] Error: cant connect!\n");
		return 2;
	}

	// send command for server
	send_command(command.c_str(), command.size());

	return 0;
}

int CameraMJPG::disconnect()
{
	socket.close();
	return 0;
}

int CameraMJPG::reconnect()
{
	disconnect();
	return connect();
}

// send command for server
int CameraMJPG::send_command(const char* src, const int src_size)
{
	if(src && src_size>0)
		return socket.write(src, src_size);
	return 0;
}

// set buffers size, depends on frame size
void CameraMJPG::set_buffers_size(const int width, const int height, const int channels_count, const int count)
{
	size_t frame_size = width*height*channels_count;
	buf_video.create(frame_size*count);
}

#if defined(USE_OPENCV)
// разжать картинку(JPEG)
IplImage* CameraMJPG::decompress_image(const char* src, int src_size)
{
	if(!src || src_size<=0)
		return NULL;

	IplImage* image = NULL;

	try
	{
		CvMat jpeg_mat = cvMat(src_size, 1, CV_8UC1, (void*)src);
		image = cvDecodeImage(&jpeg_mat);
	}
	catch(const std::exception &e)
	{
		printf("[!][CameraMJPG][decompress_image] Exception: %s\n", e.what());
		image = NULL;
	}
	catch(...)
	{
		printf("[!][CameraMJPG][decompress_image] Exception in cvDecodeImage()!\n");
		image = NULL;
		throw;
	}

	return image;
}
#endif //#if defined(USE_OPENCV) 

// read data from socket
int CameraMJPG::read_data(char* dst, int dst_size)
{
	if(!dst || dst_size<=0)
		return 0;

	return socket.read(dst, dst_size);
}

// get image from buffer
int CameraMJPG::get_image(char* buffer, int length)
{
	if(!buffer || length<=0)
		return 1;

	if(buf_video.data){
		if(buf_video.real_size - buf_video.size > length){
			memcpy(buf_video.data+buf_video.size, buffer, length );
			buf_video.size += length;
		}
		else{
			printf("[!][CameraMJPG][get_image] Error: need more buffer!\n");
			buf_video.zero();
			return 2;
		}

		return cut_image(buf_video.data, buf_video.size);
	}

	return 0;
}

// find image in buffer
int CameraMJPG::cut_image(char* buffer, int length)
{
	if(!buffer || length<=0)
		return 1;

	// find MIME header
	if( find_data_in_buffer(buffer, length, 
		CAMERA_MJPG_MIME_CONTENT_TYPE_JPEG, CAMERA_MJPG_MIME_CONTENT_TYPE_JPEG_SIZE) != 0 )
	{ 

		//printf("[i][CameraMJPG][cut_image] Image header!\n");

		// get message with image size
		const char* pdest1 = find_data_in_buffer(buffer, length, 
			CAMERA_MJPG_MIME_CONTENT_LENGTH, CAMERA_MJPG_MIME_CONTENT_LENGTH_SIZE); 

		pdest1 += CAMERA_MJPG_MIME_CONTENT_LENGTH_SIZE;

		const char* pdest2 = strstr(pdest1, "\r\n\r\n");
		int str_len = pdest2 - pdest1;

		if (str_len > 128){
			printf("[!][CameraMJPG][cut_image] Error: image size error!\n");
			str_len = 128;
		}

		char str_size[128];
		memcpy(str_size, pdest1, str_len);
		str_size[str_len] = 0;

		int size = atoi(str_size);
		//	printf("[i][CameraMJPG][cut_image] image size: %d \n", size);

		if(size > buf_video.real_size)
			printf("[!][CameraMJPG][cut_image] Warning: too big image: %d !\n", size);

		// get data length
		int recv_len = length - (pdest2 - buffer + 4);

		// pointer to image data
		const char* pimage = pdest2 + 4;

		// not all image in buffer
		if (recv_len < size){
			return 2;
		}

		//
		// got image!
		//

		image_counter++;

#if CAMERA_MJPG_SAVE_JPEG_IMAGE
		//
		// save JPEG into file
		//
		char file[128];
		snprintf(file, 128, "image%02d.jpg", imageCounter);
		writeBufferToFile(file, pimage, size);
#endif //#if VIDEOSERVER_SAVE_JPEG_IMAGE

		remove_frame();

#if defined(USE_OPENCV)
		//
		// unpack JPEG-image
		//
		frame = decompress_image(pimage, size);
#else
		frame = new char[size];
		if(frame){
			memcpy(frame, pimage, size);
			frame_size = size;	
		}
		else{
			printf("[!][cut_image][cut_image] Error: cant allocate memory!\n");
		}
#endif // #if defined(USE_OPENCV)

#if 0
		// reset buffer
		buf_video.zero();
#else
		int i, j;
		for(j=0, i=(pimage-buffer)+size; i<length; i++, j++){
			buf_video.data[j] = buf_video.data[i];
		}
		buf_video.size = j;
#endif

		return 0;
	}

	return 3;
}
