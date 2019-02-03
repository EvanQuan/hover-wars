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
	~UserInterface();

	void setPlayerCount(int count);
	void addScore(int joystickID, int score);
	void subtractScore(int joystickID, int score);
	void resetScore(int joystickID);

private:
	UserInterface();
	static UserInterface* m_pInstance;

	void setScore(int joystickID, int score);
};



