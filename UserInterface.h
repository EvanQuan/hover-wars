#pragma once
#include "stdafx.h"
/*
An interface to all user interface features.

- Display text, images
*/
class UserInterface
{
public:
	static UserInterface* getInstance(GLFWwindow* window);
	UserInterface();
	~UserInterface();
};

