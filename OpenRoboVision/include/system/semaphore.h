//
// OpenRoboVision
//
// Semaphore - object for threads synchronization
//
// Semaphore (семафор) - объект для синхронизации потоков
//
//
// robocraft.ru
//

#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_

#if defined(WIN32)
# include <windows.h>
# include <process.h>
#elif defined(LINUX)
# include <pthread.h>
# include <time.h>  // clock_gettime() // нужно линковать библиотеку librt
# include <errno.h> // ETIMEDOUT

typedef unsigned long       DWORD;

# ifndef INFINITE
#  define INFINITE            0xFFFFFFFF  // Infinite timeout
# endif
#endif //#ifdef WIN32

class Semaphore
{
public:
	Semaphore();
	~Semaphore();
	
	// установить
	void Set();

	// ждать установки заданное число миллисекунд
	bool Wait(DWORD millis=INFINITE);

private:
#if defined(WIN32)
	HANDLE _event;
#elif defined(LINUX)
	pthread_mutex_t lock;
	pthread_cond_t cond;
	int counter;
#endif

	// Копирование и присваивание не реализовано.  Предотвратим их использование,
    // объявив соответствующие методы закрытыми.
    Semaphore( const Semaphore & );
    Semaphore & operator=( const Semaphore & );
};

#endif //#ifndef _SEMAPHORE_H_
