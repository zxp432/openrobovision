//
// OpenRoboVision
//
// test threads object
//
// тестирование объекта потоков
//
//
// robocraft.ru
//

//
// http://easy-coding.blogspot.com/2009/01/threadcpp.html
//

#include "gtest/gtest.h"

#include "orv/system/thread.h"
#include "orv/system/times.h"

class SimpleThread : public BaseThread
{
public:
	SimpleThread():__done(false) { }

	bool done() const { return __done; }

protected:
	void Execute() {
		__done = true;
	}

private:
	bool __done;
};

// Декларируем тест с именем RunningInParallel в группе тестов ThreadTest.
TEST(Thread, Generic)
{
	// Создаем объект нашего класса
	SimpleThread thread;
	// Внимание! Макрос EXPECT_FALSE смотрит, какое значение у его аргумента.
	// Если это ложь, то все нормально, и выполнение теста идет дальше. Если же нет,
	// то печатается сообщение об ошибке, хотя тест продолжает работу.
	// В нашем случае тут должно быть false по смыслу.
	EXPECT_FALSE(thread.done());
	// Запускаем поток на выполнение
	thread.Start();
	// Ждем завершение потока
	thread.Join();
	// Макрос EXPECT_TRUE смотрит, какое значение у его аргумента.
	// Если это истина, то все нормально, и выполнение теста идет дальше. Если же нет,
	// то печатается сообщение об ошибке, хотя тест продолжает работу.
	// Тут мы уже ждем не false, а true, потому что поток должен был изменить значение
	// этого флага.
	EXPECT_TRUE(thread.done());
}


// "Нескончаемый поток"
class EternalThread: public BaseThread
{
protected:

	void Execute() {
		// Данный поток будет работать вечно, пока его не убьют извне.
		while (true) {
			orv::time::sleep(1);
		}
	}
};

TEST(Thread, Kill)
{
	// Создаем "вечный" поток
	EternalThread thread;
	// Запускаем его
	thread.Start();
	// Убиваем его
	thread.Kill();
	// Если функция Kill() не работает, ты мы никогда не дождемся окончания потока
	// и программа тут повиснет.
	thread.Join();
}
