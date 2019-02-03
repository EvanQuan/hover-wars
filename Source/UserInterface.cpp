#include "UserInterface.h"


// Singleton instance
UserInterface* UserInterface::m_pInstance = nullptr;

UserInterface::UserInterface(GLFWwindow* window)
{
	m_pWindow = window;
	displayCount = 0;
	initializeUserInterface();

}

UserInterface* UserInterface::getInstance(GLFWwindow* window)
{
	if (nullptr == m_pInstance)
	{
		m_pInstance = new UserInterface(window);
	}
	return m_pInstance;
}

UserInterface::~UserInterface()
{
	// Let go of Window
	m_pWindow = nullptr;
}

/*
Set the number of UIs to display
*/
void UserInterface::setDisplayCount(int count)
{
	if (DISPLAY_COUNT_MIN < count)
	{
		count = DISPLAY_COUNT_MIN;
	}
	else if (DISPLAY_COUNT_MAX > count)
	{
		count = DISPLAY_COUNT_MAX;
	}
}

/*
Initialize all aspects of the UI. This should be done at the start of every game.
*/
void UserInterface::initializeUserInterface()
{
	initializeScores();
	initializeCooldowns();
}

/*
Initialize the scores of all players to 0
*/
void UserInterface::initializeScores()
{
	for (int i = DISPLAY_COUNT_MIN; i < DISPLAY_COUNT_MAX; i++)
	{
		setScore(i, 0);
	}
}

/*
Initialize the cool down counts for all abilities
*/
void UserInterface::initializeCooldowns()
{
	for (int i = DISPLAY_COUNT_MIN; i < DISPLAY_COUNT_MAX; i++)
	{
		// TODO
	}
}

void UserInterface::setScore(int joystickID, int score)
{
	scores.insert(pair<int, int>(joystickID, score));
	// TODO
	// make this render
}


// https://learnopengl.com/code_viewer.php?code=in-practice/text_rendering
// void UserInterface::renderText(Shader &shader, string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
// {
	// Activate corresponding render state	
	// TODO shader stuff  here
	// shader.Use();
	// glUniform3f(glGetUniformLocation(shader.Program, "textColor"), color.x, color.y, color.z);
	// glActiveTexture(GL_TEXTURE0);
	// glBindVertexArray(VAO);
	
	// Iterate through all characters
//	std::string::const_iterator c;
//	for (c = text.begin(); c != text.end(); c++) 
//	{
//		Character ch = characters[*c];
//		
//		GLfloat xpos = x + ch.bearing.x * scale;
//		GLfloat ypos = y - (ch.size.y - ch.bearing.y) * scale;
//		
//		GLfloat w = ch.size.x * scale;
//		GLfloat h = ch.size.y * scale;
//		// Update VBO for each character
//		GLfloat vertices[6][4] = {
//			{ xpos,     ypos + h,   0.0, 0.0 },            
//			{ xpos,     ypos,       0.0, 1.0 },
//			{ xpos + w, ypos,       1.0, 1.0 },
//			
//			{ xpos,     ypos + h,   0.0, 0.0 },
//			{ xpos + w, ypos,       1.0, 1.0 },
//			{ xpos + w, ypos + h,   1.0, 0.0 }           
//		};
//		// Render glyph texture over quad
//		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
//		// Update content of VBO memory
//		glBindBuffer(GL_ARRAY_BUFFER, VBO);
//		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData
//		
//		glBindBuffer(GL_ARRAY_BUFFER, 0);
//		// Render quad
//		glDrawArrays(GL_TRIANGLES, 0, 6);
//		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
//		x += (ch.advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
//	}
//	glBindVertexArray(0);
//	glBindTexture(GL_TEXTURE_2D, 0);
// }
