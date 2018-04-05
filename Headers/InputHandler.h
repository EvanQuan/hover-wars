#pragma once
#include <iostream>

using namespace std;

#define MAX_INPUT_SIZE 256

// Input Handler Class
// Handles input by storing a line of text and returning a word at a time.
class InputHandler
{
public:
	~InputHandler();

	// Public Enum for Errors.
	enum INPUT_ERRORS
	{
		INPUT_HEALTHY = 0,
		INPUT_TRUNCATED,
		INPUT_EMPTY
	};

	// Loads a new input into the Handler
	void load_Input(const char* c_Input, int iInputSize);

	// Returns the next word up to the next space or new line character.
	int get_Next_Word(char* c_ReturnWord, int iReturnSize);

	// Error checking: Check the current status of the Input Handler.
	INPUT_ERRORS check_Status();

protected:
	// Singleton, accessable only from child classes.
	InputHandler();
	InputHandler(const char* c_Input, int iInputSize);  // Overloaded Constructor for loading input on initialization.
	InputHandler( const InputHandler* pNewInptHndlr );

	char m_cBuffer[MAX_INPUT_SIZE + 1];
	int m_iInputSize, m_iCurrIndex;
	INPUT_ERRORS m_eErrors;	// "Healthy" means that there's input left on the buffer and no errors have occurred.

	bool p_EOB() {	// Check to see if reached End of Buffer (EOB)
		bool bReturn = !(m_iCurrIndex < m_iInputSize) ||
				'\0' == m_cBuffer[m_iCurrIndex];
		if (bReturn)
			m_eErrors = INPUT_EMPTY;
		return bReturn;
	}
};

