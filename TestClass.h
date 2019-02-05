#pragma once
#include "stdafx.h"

class TestClass
{
public:
	TestClass();
	~TestClass();

	int add(int x, int y);

	void setValue(int value);
	int getValue();

private:
	int value;
};
