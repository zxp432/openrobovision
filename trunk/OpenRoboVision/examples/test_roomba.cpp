//
// OpenRoboVision
//
// test iRobot Roomba
//

#include "orv/system/types.h"
#include "orv/robo/roomba.h"
#include "orv/system/console.h"

int main(int argc, char* argv[])
{
#if 0
	char buf[1024];
	int res = 0;

	Serial serial;
	serial.open("COM15", 57600);

	printf("[i] ready to read...\n");
	while(1)
	{
		if(res = serial.available())
		{
			if(res = serial.read(buf, res))
			{
				printf("[i] read (%d): \n", res);
				for(int i=0; i<res; i++)
					printf("%02X ", (unsigned char)buf[i]);
				printf("\n");
			}
		}
		orv::time::sleep(20);
	}

	serial.close();
#elif 0
	while(1)
	{
		if (_kbhit())
		{
			char key = _getch();
			printf( "\n[i] Key: %c (%d)\n", key ,key );
		}
	}
#elif 0
	Roomba roomba("COM15");
	roomba.wakeUp();
	roomba.start();
	roomba.enableControl();

	while(1)
	{
		printf("[i] forward\n");
		roomba.drive(200, Roomba::DriveStraight);
		orv::time::sleep(1000);
		printf("[i] backward\n");
		roomba.drive(-200, Roomba::DriveStraight);
		orv::time::sleep(1000);
	}
#elif 1
	Roomba roomba("COM15", 57600);
	roomba.wakeUp();
	roomba.start();
	roomba.enableControl();

	while(1)
	{
		if (_kbhit())
		{
			char key = _getch();
			printf( "[i] Key: %c (%d)\n", key ,key );
			if(key==27) //ESC
				break;
			else if(key == 32) //SPACE
			{
				printf("[i] stop\n");
				roomba.drive(0, Roomba::DriveStraight);
			}
			else if(key == 'w' || key == 'W')
			{
				printf("[i] forward\n");
				roomba.drive(200, Roomba::DriveStraight);
			}
			else if(key == 's' || key == 'S')
			{
				printf("[i] backward\n");
				roomba.drive(-200, Roomba::DriveStraight);
			}
			else if(key == 'a' || key == 'A')
			{
				printf("[i] left\n");
				roomba.drive(200, Roomba::DriveInPlaceCounterClockwise);
			}
			else if(key == 'd' || key == 'D')
			{
				printf("[i] right\n");
				roomba.drive(200, Roomba::DriveInPlaceClockwise);
			}
		}
	}
#endif

	return 0;
}
