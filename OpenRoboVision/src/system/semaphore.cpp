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

#include "orv/system/semaphore.h"

//
// http://habrahabr.ru/blogs/programming/122108/
//

// классическая реализация аналога эвентов Windows на POSIX Threads:
// переменная состояния (conditional variable) + mutex. 
// Т. е. заводится флажок (символизирующий эвент), и conditional variable к нему, вместе с мьютексом. 
// Ожидающие потоки в нужный момент входят в состояние ожидания изменения этого флага (синхронизированно через mutex), 
// а инициирующий поток, изменив значение флага, дает команду всем разблокироваться. 

Semaphore::Semaphore()
{
#if defined(WIN32)
	_event = CreateEvent(0, FALSE, FALSE, 0); 
#elif defined(LINUX)
	pthread_mutex_init(&lock, 0);
	pthread_cond_init(&cond, 0);
	counter = 0;
#endif
}

Semaphore::~Semaphore()
{
#if defined(WIN32)
	CloseHandle(_event);
#elif defined(LINUX)
	pthread_cond_destroy( &cond );
	pthread_mutex_destroy( &lock );
#endif
}

// установить
void Semaphore::Set()
{
#if defined(WIN32)
	SetEvent(_event); 
#elif defined(LINUX)
	pthread_mutex_lock( &lock );
	++counter;
	pthread_cond_signal( &cond );
	pthread_mutex_unlock( &lock );
#endif
}

// ждать установки заданное число миллисекунд
bool Semaphore::Wait(DWORD millis)
{
#if defined(WIN32)
	DWORD res = WaitForSingleObject(_event, millis);
	return (res == WAIT_OBJECT_0 ? false : true);
#elif defined(LINUX)
	pthread_mutex_lock( &lock );
	int ret = 0;
	if(millis==INFINITE)
	{
		while (counter == 0)
		{
			// Флаг не установлен.
			// дожидаемся сигнала на переменной состояния,
			// означающего, что значение флага изменилась. 
			// при поступлении сигнала, поток разблокируется, 
			// а цикл снова проверяет флаг и завершается
			ret = pthread_cond_wait(&cond, &lock);
		}
	}
	else
	{
		struct timespec tm;
		clock_gettime(CLOCK_REALTIME, &tm);
		tm.tv_sec += millis / 1000;					// секунды
		tm.tv_nsec += (millis % 1000) * 1000000L;	// наносекунды
		// утсранение переполнения
		// ( если tv_nsec превышает 1000'000'000
		// то функция возвращает ошибку EINVAL )
		if(tm.tv_nsec>1000000000L)
		{
			tm.tv_sec += 1;				
			tm.tv_nsec -= 1000000000L;
		}

		while (counter == 0)
		{
			ret = pthread_cond_timedwait(&cond, &lock, &tm);
			if( ret == ETIMEDOUT)
			{
				pthread_mutex_unlock( &lock );
				return true;
			}
		}
	}
	// здесь, флаг должен быть установлен - нужно его сбросить
	// и разблокировать мьютекс.
	--counter;
	pthread_mutex_unlock( &lock );
	return false;
#endif
}
