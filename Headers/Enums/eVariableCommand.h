#pragma once

/*
Represents a variable command. Their actions require extra x and y parameters
to complete the commands.
*/
enum VariableCommand
{
	COMMAND_MOVE,
	COMMAND_TURN,
	COMMAND_INVALID_VARIABLE,
};
