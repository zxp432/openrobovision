//
// OpenRoboVision
//
// object for get frames from camera via MJPG
// (mjpg-streamer)
//
//
// robocraft.ru
//

#ifndef _CAMERA_MJPG_H_
#define _CAMERA_MJPG_H_

#include "orv/system/network.h"
#include "video_capture.h"
#include <string>

// save data to file
#define CAMERA_MJPG_SAVE_JPEG_IMAGE 0

// strings of MIME type
#define CAMERA_MJPG_MIME_CONTENT_TYPE_JPEG "Content-Type: image/jpeg\r\n"
#define CAMERA_MJPG_MIME_CONTENT_TYPE_JPEG_SIZE 26
#define CAMERA_MJPG_MIME_CONTENT_LENGTH "Content-Length: "
#define CAMERA_MJPG_MIME_CONTENT_LENGTH_SIZE 16

// request for mjpg-streamer to get stream
#define CAMERA_MJPG_COMMAND_MJPG_STREAMER_GET_STREAM "GET /?action=stream\r\n\r\n"
#define CAMERA_MJPG_COMMAND_MJPG_STREAMER_GET_STREAM_SIZE 23

// buffer
typedef struct _mjpg_buffer {
	char* 	data;
	size_t 	size;
	size_t 	real_size;
	
	_mjpg_buffer():
	data(NULL), size(0), real_size(0)
	{
	}
	
	// allocate buffer
	int create(size_t _size)
	{
		if(_size==0)
			return 1;
		if(data)
			this->reset();
		data = new char [_size];
		if(!data)
			return 2;
		real_size = _size;
		return 0;
	}
	
	// delete buffer
	void reset()
	{
		if(data)
		{
			delete []data;
			data = NULL;
		}
		size = 0;
		real_size = 0;
	}

	void zero()
	{
		if(data)
		{
			memset(data, 0, real_size);
			size = 0;
		}
	}
} mjpg_buffer;

// find sequence in buffer
const char* find_data_in_buffer(const char *src, int src_size, const char *data, int data_size);

class CameraMJPG : public BaseVideoCapture
{
public:
	CameraMJPG();
	CameraMJPG(const char* ip_address, const int port);
	CameraMJPG(const char* ip_address, const int port, const char* command);
	~CameraMJPG();

	void init();
	void update(); // get next frame
	void end();

	// get frame's width/height
	int getWidth();
	int getHeight();
	
	//
	// MJPG methods
	//
	
	int connect();
	int disconnect();
	int reconnect();
	
	// send command for server
	int send_command(const char* src, const int src_size);
	
	const char* get_ip_address() const { return ip_address.c_str(); }
	void set_ip_address(const char* val) { ip_address = val; }
	int get_port() const { return port; }
	void set_port(int val) { port = val; }
	// set buffers size, depends on frame size
	void set_buffers_size(const int width, const int height, const int channels_count=3, const int count=2);
	
#if defined(USE_OPENCV)
	// разжать картинку(JPEG)
	IplImage* decompress_image(const char* src, int src_size);
#endif //#if defined(USE_OPENCV) 

private:
	// read data from socket
	int read_data(char* dst, int dst_size);
	// get image from buffer
	int get_image(char* buffer, int length);
	// find image in buffer
	int cut_image(char* buffer, int length);

	// socket for talking with server
	Socket socket;

	// address of server
	std::string ip_address;
	int port;

	// command to server
	std::string command;
	
	// frame counter
	unsigned long image_counter;
	
	//
	// buffers
	//
	
	// for read data from socket
	mjpg_buffer buf_read;
	// for store data (for image search)
	mjpg_buffer buf_video;
};

#endif // #ifndef _CAMERA_MJPG_H_
