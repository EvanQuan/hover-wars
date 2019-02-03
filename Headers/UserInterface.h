#pragma once
#include "stdafx.h"
#include "Shader.h"

#define DISPLAY_COUNT_MIN 0
#define DISPLAY_COUNT_MAX 4
/*
An interface to all user interface features.

- Display text, images
*/
class UserInterface
{
public:
	static UserInterface* getInstance(GLFWwindow* window);

	~UserInterface();

	void setDisplayCount(int count);
	void addScore(int joystickID, int score);
	void subtractScore(int joystickID, int score);
	void resetScore(int joystickID);

private:
	UserInterface(GLFWwindow* window);
	static UserInterface* m_pInstance;

	void setScore(int joystickID, int score);
	void renderText(Shader &shader, string text, GLfloat x, GLfloat y, GLfloat scale, vec3 color);
	void initializeScores();
	void initializeUserInterface();
	void initializeDisplayCount();
	void initializeCooldowns();

	/// Holds all state information relevant to a character as loaded using FreeType
	struct Character {
		GLuint TextureID;   // ID handle of the glyph texture
		glm::ivec2 Size;    // Size of glyph
		glm::ivec2 Bearing; // Offset from baseline to left/top of glyph
		GLuint Advance;     // Horizontal offset to advance to next glyph
	};

	std::map<GLchar, Character> characters;

	std::map<int, int> scores;

	int displayCount;

	// Window reference
	GLFWwindow* m_pWindow;
};



