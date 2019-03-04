#include "stdafx.h"
#include "CppUnitTest.h"
#include "..\TestClass.cpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Test
{		
	TEST_CLASS(TestClassTest)
	{

	public:
		
		::TestClass t;
		int x;
		TEST_METHOD_INITIALIZE(InitWithDefaultConstructor)
		{
			t = ::TestClass();
			x = 0;
		}


		TEST_METHOD(Add1Plus1Equals2_AreEqual)
		{
			x = t.add(1, 1);
			Assert::AreEqual(2, x);
		}

		TEST_METHOD(Add1Plus3Equals3_NotEquals)
		{
			x = t.add(1, 1);
			Assert::AreNotEqual(3, t.add(1, 1));
		}

		TEST_METHOD(TestMethodAreEqual)
		{
			Assert::AreEqual(1, 1);
		}

	};
}