//
// OpenRoboVision
//
// example of ORCP usage 
//

#include "orv/robo/orcp.h"
#include "orv/system/console.h"

// serial port
#if defined(WIN32)
# define SERIAL_PORT_NAME "COM4"
#elif defined(LINUX)
# define SERIAL_PORT_NAME "/dev/ttyS0"
#endif
#define SERIAL_PORT_RATE	9600

int main(int argc, char* argv[])
{
	printf("[i] Start... \n");

	ORCP orcp;
#if defined(WIN32)
	orcp.open(SERIAL_PORT_NAME, SERIAL_PORT_RATE);
#elif defined(LINUX)
	orcp.open(SERIAL_PORT_NAME, SERIAL_PORT_RATE);
#endif

	int servo_pin = 7;
	int servo_value = 90;
	bool send = false;

	orcp.analogWrite(servo_pin, servo_value);

	while(1)
	{
	//	if (_kbhit())
	//	{
	//		char key = _getch();
		char key = (char)orv::console::waitKey(1000);
			printf( "[i] Key: %c (%d)\n", key ,key );
			if(key==27) //ESC
				break;
			else if(key == 'w' || key == 'W')
			{
				printf("[i] forward\n");
				servo_value++;
				send = true;
			}
			else if(key == 's' || key == 'S')
			{
				printf("[i] backward\n");
				servo_value--;
				send = true;
			}
			else if(key == 'a' || key == 'A')
			{
				printf("[i] left\n");
			}
			else if(key == 'd' || key == 'D')
			{
				printf("[i] right\n");
			}
	//	}
		if(send)
		{
			orcp.analogWrite(servo_pin, servo_value);
			send = false;
		}
	}


	printf("[i] End.\n");
	return 0;
}
