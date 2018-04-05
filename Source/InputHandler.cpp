#include "stdafx.h"
#include "InputHandler.h"


InputHandler::InputHandler()
{
	m_eErrors		= INPUT_EMPTY;
	m_cBuffer[0]	= 0;
	m_iCurrIndex	= 0;
	m_iInputSize	= 0;
}

// Overloaded Constructor for loading input on initialization.
InputHandler::InputHandler(const char* c_Input, int iInputSize)
{
	this->load_Input(c_Input, iInputSize);
}

InputHandler::~InputHandler()
{

}

// Loads a new input into the Handler
void InputHandler::load_Input(const char* c_Input, int iInputSize)
{
	// Verify the size of the input to ensure it doesn't go out of bounds.
	if (iInputSize > MAX_INPUT_SIZE)
	{
		m_iInputSize = MAX_INPUT_SIZE;
		m_eErrors = INPUT_TRUNCATED;
	}
	else
	{
		m_iInputSize = iInputSize;
		m_eErrors = INPUT_HEALTHY;
	}

	// Copy Input into Handler Buffer
	for (int i = 0; i < m_iInputSize; ++i)
	{
		m_cBuffer[i] = c_Input[i];
	}

	m_iCurrIndex = 0;
}

// Returns the next word up to the next space or new line character.
int InputHandler::get_Next_Word(char* c_ReturnWord, int iReturnSize)
{
	int iReturnVal = -1;		// Return an Error if nothing happened.
	int iIndex = 0;

	// Process if there's stuff in the buffer to grab.
	if (INPUT_HEALTHY == m_eErrors)
	{
		iReturnVal = 0;

		// Skip leading whitespace
		while ((m_iCurrIndex < m_iInputSize) && (' ' == m_cBuffer[m_iCurrIndex]))
			m_iCurrIndex++;

		// Copy next word
		while ((iIndex < (iReturnSize - 1)) && (m_cBuffer[m_iCurrIndex] != ' ') && !p_EOB())
		{
			c_ReturnWord[iIndex] = m_cBuffer[m_iCurrIndex];
			iIndex++;
			m_iCurrIndex++;
		}

		// Set nullptr terminator
		c_ReturnWord[iIndex] = '\0';

		// Return the number of Characters Copied.
		iReturnVal = iIndex;
	}
	
	return iReturnVal;
}

InputHandler::INPUT_ERRORS InputHandler::check_Status()
{
	return m_eErrors;
}
