//
// тесты
//

#include <stdio.h>

#include "definitions.h"

#include "gtest/gtest.h"

int main(int argc, char* argv[])
{
	printf("[i] Start...\n");

	testing::InitGoogleTest(&argc, argv);
	// ѕринудительно печатаем врем€ работы тестов.
	testing::GTEST_FLAG(print_time) = true;
	return RUN_ALL_TESTS();
}
