#include "TestClass.h"


TestClass::TestClass() : value(0)
{
	// std::cout << "TestClass initialized" << std::endl;
}


TestClass::~TestClass()
{
	// std::cout << "TestClass deleted" << std::endl;
}

int TestClass::add(int x, int y)
{
	return x + y;
}

int TestClass::getValue()
{
	return this->value;
}

void TestClass::setValue(int value)
{
	this->value = value;
}
