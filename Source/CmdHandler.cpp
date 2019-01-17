#include "CmdHandler.h"
#include "Object_Factory.h"

/**********\
* DEFINES *
\**********/
#define INPUT_SIZE 128

/* Initialize Statics */
const char CmdHandler::cCommands[NUM_CMDS][CMD_SIZE] = { "-help",
														 "b-val",
														 "y-val",
														 "alpha",
														 "beta",
														 "rc-val",
														 "gc-val",
														 "bc-val",
														 "color",
														 "list",
														 "delete",
														 "load",
														 "gooch",
														 "x-toon",
														 "specular",
														 "shine",
														 "r",
														 "threshold",
														 "threshold_min",	
														 "threshold_max"	};

CmdHandler* CmdHandler::m_pInstance = nullptr;

CmdHandler::CmdHandler(GLFWwindow *rWindow) : InputHandler()
{
	// Initializing Base Class
	m_pGPXMngr = GraphicsManager::getInstance(rWindow);
	m_pEntMngr = ENTITY_MANAGER;

	bWireFrame = false;
}

// Get a copy of CmdHandler that doesn't have any initialized
//   Input associated with it.
CmdHandler* CmdHandler::getInstance(GLFWwindow *rWindow)
{
	if (nullptr == m_pInstance)
		m_pInstance = new CmdHandler(rWindow);

	return m_pInstance;
}

// Get a copy of CmdHandler with an associated Input.
CmdHandler* CmdHandler::getInstance(const char* c_Input, int iInputSize, GLFWwindow *rWindow)
{
	if (nullptr == m_pInstance)
		m_pInstance = new CmdHandler(c_Input, iInputSize, rWindow);
	else
		m_pInstance->load_Input(c_Input, iInputSize);

	return m_pInstance;
}

// Destructor
CmdHandler::~CmdHandler()
{
	m_pGPXMngr = nullptr;
	m_pEntMngr = nullptr;
}

// Process initial input, acts as though key word is first word of input.
//// First word can be ignored and function will grab the first word.
//void CmdHandler::process_Input( )
//{
//	char cInput[INPUT_SIZE] = {};
//	char c_FirstWord[CMD_SIZE] = {};
//	bool bEvaluating = true;
//
//	while ( bEvaluating )
//	{
//		cout << "Enter Commands: ";
//		cin.getline( cInput, INPUT_SIZE );
//
//		load_Input( cInput, INPUT_SIZE );
//		get_Next_Word( c_FirstWord, CMD_SIZE );
//
//		// Parse initial command
//		if ( !strcmp( c_FirstWord, cCommands[ SET_B ] ) )
//			bEvaluating = exec_SetB();
//		else if ( !strcmp( c_FirstWord, cCommands[ SET_Y ] ) )
//			bEvaluating = exec_SetY();
//		else if ( !strcmp( c_FirstWord, cCommands[ SET_ALPHA ] ) )
//			bEvaluating = exec_SetAlpha();
//		else if ( !strcmp( c_FirstWord, cCommands[ SET_BETA ] ) )
//			bEvaluating = exec_SetBeta();
//		else if ( !strcmp( c_FirstWord, cCommands[ SET_RC ] ) )
//			bEvaluating = exec_SetRGBVal( GraphicsManager::RED );
//		else if ( !strcmp( c_FirstWord, cCommands[ SET_GC ] ) )
//			bEvaluating = exec_SetRGBVal( GraphicsManager::GREEN );
//		else if ( !strcmp( c_FirstWord, cCommands[ SET_BC ] ) )
//			bEvaluating = exec_SetRGBVal( GraphicsManager::BLUE );
//		else if ( !strcmp( c_FirstWord, cCommands[ HELP ] ) )
//			outputHelpList();
//		else if ( !strcmp( c_FirstWord, cCommands[ SET_COLOR ] ) )
//			bEvaluating = exec_SetColor();
//		else if ( !strcmp( c_FirstWord, cCommands[ LIST ] ) )
//			m_pEntMngr->listEnvironment();
//		else if ( !strcmp( c_FirstWord, cCommands[ DELETE ] ) )
//			bEvaluating = exec_Delete();
//		else if ( !strcmp( c_FirstWord, cCommands[ LOAD ] ) )
//			bEvaluating = exec_Load();
//		else if ( !strcmp( c_FirstWord, cCommands[ TOGGLE_GOOCH ] ) )
//		{
//			m_pGPXMngr->togGooch();
//			bEvaluating = false;
//		}
//		else if ( !strcmp( c_FirstWord, cCommands[ TOGGLE_X_TOON ] ) )
//		{
//			m_pGPXMngr->togToon();
//			bEvaluating = false;
//		}
//		else if ( !strcmp( c_FirstWord, cCommands[ TOGGLE_SPECULAR ] ) )
//		{
//			m_pGPXMngr->togSpec();
//			bEvaluating = false;
//		}
//		else if ( !strcmp( c_FirstWord, cCommands[ SET_SHINE ] ) )
//			bEvaluating = exec_SetShine();
//		else if ( !strcmp( c_FirstWord, cCommands[ SET_R ] ) )
//			bEvaluating = exec_SetR();
//		else if ( !strcmp( c_FirstWord, cCommands[ SET_THRESHOLD ] ) )
//			bEvaluating = exec_SetThreshold();
//		else if ( !strcmp( c_FirstWord, cCommands[ SET_MIN_THRESHOLD ] ) )
//			bEvaluating = exec_SetThresholdMin();
//		else if ( !strcmp( c_FirstWord, cCommands[ SET_MAX_THRESHOLD ] ) )
//			bEvaluating = exec_SetThresholdMax();
//		else
//			cout << "Unknown Command: \"" << c_FirstWord << ".\"" << endl;
//	}
//}

void CmdHandler::handleKeyBoardInput(int cKey, int iAction, int iMods)
{
	vec3 pMoveVec(0.f, 0.f, 0.f);
	switch (cKey)
	{
	case (GLFW_KEY_W):			// Move Light Forward (along Z)
		pMoveVec.z += LIGHT_MOVE_FACTOR;
		m_pEntMngr->moveLight(pMoveVec);
		break;
	case(GLFW_KEY_S):			// Move Light Backward (along Z)
		pMoveVec.z -= LIGHT_MOVE_FACTOR;
		m_pEntMngr->moveLight(pMoveVec);
		break;
	case(GLFW_KEY_A):			// Move Light Left (along X)
		pMoveVec.x -= LIGHT_MOVE_FACTOR;
		m_pEntMngr->moveLight(pMoveVec);
		break;
	case(GLFW_KEY_D):			// Move Light Right (along X)
		pMoveVec.x += LIGHT_MOVE_FACTOR;
		m_pEntMngr->moveLight(pMoveVec);
		break;
	case(GLFW_KEY_SPACE):		// Move Light Up (along Y)
		pMoveVec.y += LIGHT_MOVE_FACTOR;
		m_pEntMngr->moveLight(pMoveVec);
		break;
	case(GLFW_KEY_X):			// Move Light Down (along Y)
		pMoveVec.y -= LIGHT_MOVE_FACTOR;
		m_pEntMngr->moveLight(pMoveVec);
		break;
	case(GLFW_KEY_F):			// Toggle Wireframe
		if ( GLFW_RELEASE == iAction )
		{
			bWireFrame = !bWireFrame;
			if ( bWireFrame )
				glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
			else
				glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		}
		break;
	case(GLFW_KEY_ENTER):		// Input Handling (TODO)
		break;
	case(GLFW_KEY_UP):
		break;
	case(GLFW_KEY_C):
		if( iAction == GLFW_RELEASE )
			m_pGPXMngr->switchView();
		break;
	case(GLFW_KEY_P):
		if ( iAction == GLFW_RELEASE )
			m_pEntMngr->pause();
		break;
	}
}

/*****************************************************************************\
 * Input Executions     													 *
\*****************************************************************************/

// Executes the b-val command:
//		Checks to ensure correct value input
//		Calls Graphics Manager to set the uniform Variable
bool CmdHandler::exec_SetB()
{
	char c_Num[MAX_INPUT_SIZE] = {};
	char* p_End;
	float fTempVal;
	bool bReturnVal = false;
	int iErr = get_Next_Word(c_Num, MAX_INPUT_SIZE);

	if (ERR_CODE != iErr)
	{
		fTempVal = strtof(c_Num, &p_End);
		if (checkRange(fTempVal, 0.0f, 1.0f))
			m_pGPXMngr->setBVal(fTempVal);
	}
	else
		cout << "Error reading in value: \"" << c_Num << "\".\n";

	return bReturnVal;
}

// Executes the y-val command:
//		Checks to ensure correct value input
//		Calls Graphics Manager to set the uniform Variable
bool CmdHandler::exec_SetY()
{
	char c_Num[MAX_INPUT_SIZE] = {};
	char* p_End;
	float fTempVal;
	bool bReturnVal = false;
	int iErr = get_Next_Word(c_Num, MAX_INPUT_SIZE);

	if (ERR_CODE != iErr)
	{
		fTempVal = strtof(c_Num, &p_End);
		if (checkRange(fTempVal, 0.0f, 1.0f))
			m_pGPXMngr->setYVal(fTempVal);
	}
	else
		cout << "Error reading in value: \"" << c_Num << "\".\n";

	return bReturnVal;
}

// Executes the alpha command:
//		Checks to ensure correct value input
//		Calls Graphics Manager to set the uniform Variable
bool CmdHandler::exec_SetAlpha()
{
	char c_Num[MAX_INPUT_SIZE] = {};
	char* p_End;
	float fTempVal;
	bool bReturnVal = false;
	int iErr = get_Next_Word(c_Num, MAX_INPUT_SIZE);

	if (ERR_CODE != iErr)
	{
		fTempVal = strtof(c_Num, &p_End);
		if (checkRange(fTempVal, 0.0f, 1.0f))
			m_pGPXMngr->setAlpha(fTempVal);
	}
	else
		cout << "Error reading in value: \"" << c_Num << "\".\n";

	return bReturnVal;
}

// Executes the beta command:
//		Checks to ensure correct value input
//		Calls Graphics Manager to set the uniform Variable
bool CmdHandler::exec_SetBeta()
{
	char c_Num[MAX_INPUT_SIZE] = {};
	char* p_End;
	float fTempVal;
	bool bReturnVal = false;
	int iErr = get_Next_Word(c_Num, MAX_INPUT_SIZE);

	if (ERR_CODE != iErr)
	{
		fTempVal = strtof(c_Num, &p_End);
		if (checkRange(fTempVal, 0.0f, 1.0f))
			m_pGPXMngr->setBeta(fTempVal);
	}
	else
		cout << "Error reading in value: \"" << c_Num << "\".\n";

	return bReturnVal;
}

// Executes the rc-val command:
//		Checks to ensure correct value input
//		Calls Graphics Manager to set the uniform Variable
bool CmdHandler::exec_SetRGBVal(GraphicsManager::eRGB eRGBType )
{
	char c_Num[MAX_INPUT_SIZE] = {};
	char* p_End;
	float fTempVal;
	bool bReturnVal = false;
	int iErr = get_Next_Word(c_Num, MAX_INPUT_SIZE);

	if (ERR_CODE != iErr)
	{
		fTempVal = strtof(c_Num, &p_End);
		if (checkRange(fTempVal, 0.0f, 1.0f))
			m_pGPXMngr->setRGBVal(eRGBType, fTempVal);
	}
	else
		cout << "Error reading in value: \"" << c_Num << "\".\n";

	return bReturnVal;
}

// Executes the color command:
//		Checks to ensure correct value input
//		Calls Graphics Manager to set the uniform Variables
bool CmdHandler::exec_SetColor()
{
	char c_Num[MAX_INPUT_SIZE] = {};
	float fTempVal;
	float fColorRGB[GraphicsManager::RGB_MAX];
	char* p_End;
	int iErr;
	bool bReturnVal = false;

	for (int i = 0; i < GraphicsManager::RGB_MAX; i++)
	{
		iErr = get_Next_Word(c_Num, MAX_INPUT_SIZE);
		if (ERR_CODE != iErr) // Ensure no error occurred.
		{
			// Verify Value
			fTempVal = strtof(c_Num, &p_End);
			bReturnVal |= !checkRange(fTempVal, 0.0f, 1.0f); // True if Valid Range -> set Value
			if (!bReturnVal) 
				fColorRGB[i] = fTempVal;
		}
	}

	if (!bReturnVal)
	{
		m_pGPXMngr->setRGBVal(GraphicsManager::RED, fColorRGB[GraphicsManager::RED]);
		m_pGPXMngr->setRGBVal(GraphicsManager::GREEN, fColorRGB[GraphicsManager::GREEN]);
		m_pGPXMngr->setRGBVal(GraphicsManager::BLUE, fColorRGB[GraphicsManager::BLUE]);
	}

	return bReturnVal;
}
// exec_Load
// Handles Loading functionality of both Models and Textures.  Models can be loaded freely
// into the scene and, by default, load into the center of the scene.  Since Textures are
// bound to objects, an object ID is required to load the texture onto the object.
bool CmdHandler::exec_Load()
{
	char c_Cmd[ MAX_INPUT_SIZE ] = {};
	int iErr = get_Next_Word( c_Cmd, MAX_INPUT_SIZE );
	int iModelErr, iTexErr;
	char c_TexLocation[ MAX_INPUT_SIZE ], c_ModelLocation[ MAX_INPUT_SIZE ];
	string sTexLoc, sModLoc;
	vec3 pPos = vec3( 0.0 );	// By Default, all Models load up in the center of the scene
	bool bReturnVal = false;

	if ( ERR_CODE != iErr )
	{
		if ( 0 == strcmp( c_Cmd, "model" ) )		// Handle input to load a new model
		{
			iModelErr = get_Next_Word( c_ModelLocation, MAX_INPUT_SIZE );
			iTexErr = get_Next_Word( c_TexLocation, MAX_INPUT_SIZE );
			if ( ERR_CODE != iModelErr && ERR_CODE != iTexErr )		// No issues reading the Model and Texture Locations
			{
				sModLoc = string( c_ModelLocation );
				sTexLoc = string( c_TexLocation );
				cout << "Error, unable to load " << sModLoc << " as a model.\n";
			}
			else  // Errors reading model and texture locatiosn
			{
				cout << "Error reading in values for loading a model: \""
					<< c_ModelLocation << " " << c_TexLocation << "\".\n";
				bReturnVal = true;
			}
		}
	}
	else   // Error reading in first parameter
	{
		cout << "Error reading in value: \"" << c_Cmd << "\".\n";
		bReturnVal = true;
	}

	return bReturnVal;
}

// Set shininess coefficient of the specular highlights and toon-shading.
bool CmdHandler::exec_SetShine()
{
	char c_Num[ MAX_INPUT_SIZE ] = {};
	char* p_End;
	float fTempVal;
	bool bReturnVal = false;
	int iErr = get_Next_Word( c_Num, MAX_INPUT_SIZE );

	if ( ERR_CODE != iErr )
	{
		fTempVal = strtof( c_Num, &p_End );
		if ( checkRange( fTempVal, 1.0f, FLT_MAX ) )
			m_pGPXMngr->setShine( fTempVal );
	}
	else
	{
		cout << "Error reading in value: \"" << c_Num << "\".\n";
		bReturnVal = true;
	}

	return bReturnVal;
}

// Sets the R-value which changes the magnitude of the x-toon effect.
bool CmdHandler::exec_SetR()
{
	char c_Num[ MAX_INPUT_SIZE ] = {};
	char* p_End;
	float fTempVal;
	bool bReturnVal = false;
	int iErr = get_Next_Word( c_Num, MAX_INPUT_SIZE );

	if ( ERR_CODE != iErr )
	{
		fTempVal = strtof( c_Num, &p_End );
		if ( checkRange( fTempVal, 0.0f, FLT_MAX ) )
			m_pGPXMngr->setR( fTempVal );
	}
	else
	{
		cout << "Error reading in value: \"" << c_Num << "\".\n";
		bReturnVal = true;
	}

	return bReturnVal;
}

// Executes command to set the Edge Thresholds of the Edge Buffers.
bool CmdHandler::exec_SetThreshold()
{
	bool bReturnVal = false;

	// Evaluate the Min and Max in order
	bReturnVal |= exec_SetThresholdMin();
	bReturnVal |= exec_SetThresholdMax();

	return bReturnVal;
}

// Executes command to set the Edge Thresholds of the Edge Buffers.
bool CmdHandler::exec_SetThresholdMin()
{
	char c_Num[ MAX_INPUT_SIZE ] = {};
	char* pEnd;
	float fTempVal;
	int iErr = get_Next_Word( c_Num, MAX_INPUT_SIZE );
	bool bReturnVal = false;

	if ( ERR_CODE != iErr )
	{
		fTempVal = strtof( c_Num, &pEnd );
		if ( checkRange( fTempVal, 0.0f, 360.0f ) )
			m_pEntMngr->setMinThreshold( fTempVal );
	}
	else
	{
		cout << "Error reading in value: \"" << c_Num << "\".\n";
		bReturnVal = true;
	}

	return bReturnVal;
}

// Executes command to set the Edge Thresholds of the Edge Buffers.
bool CmdHandler::exec_SetThresholdMax()
{
	char c_Num[ MAX_INPUT_SIZE ] = {};
	char* pEnd;
	float fTempVal;
	int iErr = get_Next_Word( c_Num, MAX_INPUT_SIZE );
	bool bReturnVal = false;

	if ( ERR_CODE != iErr )
	{
		fTempVal = strtof( c_Num, &pEnd );
		if ( checkRange( fTempVal, 0.0f, 360.0f ) )
			m_pEntMngr->setMaxThreshold( fTempVal );
	}
	else
	{
		cout << "Error reading in value: \"" << c_Num << "\".\n";
		bReturnVal = true;
	}

	return bReturnVal;
}

// Checks the range of a floating point value between a given Min and Max.
// Returns true if value is in range
//         false otherwise
bool CmdHandler::checkRange(float fVal, float fMIN, float fMAX)
{
	bool bNotInRange = (fVal < fMIN || fVal > fMAX);

	if (bNotInRange)
		cout << "Error : Invalid range for RGB Value: " << fVal <<
			".  Please choose value between " << fMIN << " and " 
			<< fMAX << "." << endl;

	return !bNotInRange;
}