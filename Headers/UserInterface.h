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
	UserInterface(GLFWwindow* window);
	static UserInterface* m_pInstance;

	void setScore(int joystickID, int score);

	/// Holds all state information relevant to a character as loaded using FreeType
	struct Character {
		GLuint TextureID;   // ID handle of the glyph texture
		glm::ivec2 Size;    // Size of glyph
		glm::ivec2 Bearing; // Offset from baseline to left/top of glyph
		GLuint Advance;     // Horizontal offset to advance to next glyph
	};

	std::map<GLchar, Character> Characters;

	// Window reference
	GLFWwindow* m_pWindow;
};



