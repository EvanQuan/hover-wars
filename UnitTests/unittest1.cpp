#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests
{		
	TEST_CLASS(UnitTestTest)
	{
	public:
		
		TEST_METHOD(TestMethodTrue)
		{
			// TODO: Your test code here
			Assert::IsTrue(true);
		}

		TEST_METHOD(TestMethodAreEqual)
		{
			// TODO: Your test code here
			Assert::AreEqual(1, 1);
		}

	};
}