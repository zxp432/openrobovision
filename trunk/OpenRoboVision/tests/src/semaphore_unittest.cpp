//
// OpenRoboVision
//
// test semaphore object
//
// тестирование объекта семафора
//
//
// robocraft.ru
//

#include "gtest/gtest.h"

#include "orv/system/thread.h"
#include "orv/system/semaphore.h"
#include "orv/system/times.h"

class S: public BaseThread
{
public:
	// Передаем в конструкторе ссылку на флаг и 
	// ссылку на семафор.
	S(volatile int& flag, Semaphore& sem) :
	  _flag(flag), _sem(sem)
	  {}

	  void Execute() {
		  _sem.Wait();
		  _flag = 1;
	  }

private:
	volatile int& _flag;
	Semaphore& _sem;
};

TEST(Semaphore, Generic)
{
	// Начальное значение флага - 0.
	volatile int flag = 0;

	// Создаем объект-семафор
	Semaphore semaphore;
	
	// Создаем параллельный поток выполнения.
	S a(flag, semaphore);
	// Запускаем его.
	a.Start();
	// Ждем для проформы десятую секунды, чтобы дать
	// время параллельному потоку создаться и успеть
	// дойти до строки (1), то есть до захвата блокировки.
	orv::time::sleep(100);

	// Значение флага должно быть все еще 0, так как
	// параллельный поток должен быть блокирован на
	// Wait()
	EXPECT_EQ(0, flag);

	// устанавливаем семафор
	semaphore.Set();

	// Ждем завершения параллельного потока.
	a.Join();
	// Так как параллельный поток завершился, то
	// флаг теперь точно должен быть равен 1.
	EXPECT_EQ(1, flag);
}
