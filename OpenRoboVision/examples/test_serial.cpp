//
// OpenRoboVision
//
// пример работы с последовательным портом
//
// чтобы считать своё же сообщение, посланное в последовательный порт,
// у него нужно соединить между собой пины 2 и 3
//

#include "orv/system/serial.h"

#include <string.h> //memset

// название и скорость последовательного порта
#if defined(WIN32)
# define SERIAL_PORT_NAME "COM4"
#elif defined(LINUX)
# define SERIAL_PORT_NAME "/dev/ttyS0"
#endif
#define SERIAL_PORT_RATE	9600

#ifndef BUF_SIZE1
# define BUF_SIZE1 256
#endif

int main(int argc, char* argv[])
{
	printf("[i] Start... \n");

	Serial serial;
#if defined(WIN32)
	serial.open(SERIAL_PORT_NAME, SERIAL_PORT_RATE, true);
#elif defined(LINUX)
	serial.open(SERIAL_PORT_NAME, SERIAL_PORT_RATE);
#endif

	if(!serial.connected())
	{
		printf("[!] Cant open port!\n");
		return -1;
	}

	char c = 'y';
	int res = 12;

	char buf[BUF_SIZE1];
	memset(buf, 0, BUF_SIZE1);

	while(true)
	{

		serial.write( &c, 1 );
#if 0
		if(res = serial.available()){
			if( res = serial.Read(buf, res) ){
				printf("%d %s\n", res, buf);
			}
		}
#else
		if(serial.waitInput(1000)==0)
			printf("[i] timeout!\n");
		else
		{
			if(res = serial.available())
			{
				res = serial.read(buf, res);
				printf("%d %s\n", res, buf);
			}
		}
#endif
	}
	serial.close();

	printf("[i] End.\n");
	return 0;
}
