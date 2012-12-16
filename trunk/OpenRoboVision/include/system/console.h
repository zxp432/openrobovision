//
// OpenRoboVision
//
// console functions
//
// robocraft.ru
//

#ifndef _ORV_CONSOLE_H_
#define _ORV_CONSOLE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#if defined(WIN32)
# include <windows.h>
#elif defined(LINUX)
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>
#include <stropts.h>
#include <sys/ioctl.h>
#endif //#if defined(WIN32)

#if defined(WIN32)
# include <conio.h>
#elif defined(LINUX)
// Linux (POSIX) implementation of _kbhit().
int _kbhit();
// Linux equivalent of getch() (from conio)
int _getch();
#endif //#if defined(WIN32)


namespace orv
{
	namespace console
	{
		// wait key
		int waitKey(int msec=0);

	}; //namespace console

}; //namespace orv

#endif //#ifndef _ORV_CONSOLE_H_

