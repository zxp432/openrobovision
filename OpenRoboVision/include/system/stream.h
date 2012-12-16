//
// OpenRoboVision
//
// base class for stream datas
//
//
// robocraft.ru
//

#ifndef _ORV_STREAM_H_
#define _ORV_STREAM_H_

class Stream
{
public:
	Stream(){}
	~Stream(){}

	// pure virtual methods
	virtual int open(void) = 0;
	virtual int close(void) = 0;

	virtual int read(void *ptr, int count) = 0;
	virtual int write(const void *ptr, int len) = 0;
};

#endif // #ifndef _ORV_STREAM_H_
