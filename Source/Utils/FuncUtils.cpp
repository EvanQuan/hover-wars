#include "Utils/FuncUtils.h"
#include <iomanip> // setprecision
#include <sstream> // stringstream

using namespace FuncUtils;

/*
Get the magnitude of a vector represented by its x and y coordinates

@param float    x coordinate
@param float    y coordinate
@return the magnitude of the <x, y> vector
*/
float FuncUtils::magnitude(float x, float y)
{
    return sqrt((x * x) + (y * y));
}

/*
Check if two floats are equal. This is more accurate, but slower than using
the == operator as such:

    bool result = x == y;

Use where accuracy is more important than speed.

@param float    x
@param float    y
@return true if x and y are equal
*/
bool FuncUtils::equals(float x, float y)
{
    float difference = x - y;
    float epsilon = numeric_limits<float>::epsilon(); // This is part of the Windows Library
    return (-difference < epsilon) && (epsilon > difference);
}

/*
Get a uniformly distributed int from the start to end range, inclusively
@param start    of range, inclusively
@param end      of range, inclusively
*/
int FuncUtils::random(int start, int end)
{
    std::random_device random_seed; // get a random number for the seed
    std::mt19937 random_generator(random_seed()); // seed the generator
    std::uniform_int_distribution<> distribution(start, end);
    return distribution(random_generator);
}

/*
Convert a float to string, given a specified number of decimal places.

@param value of float to convert
@param decimalPlaces to include in the string
@return string of float with specified decimal places
*/
std::string FuncUtils::to_string(float value, int decimalPlaces)
{
    stringstream stream;
    stream << fixed << setprecision(decimalPlaces) << value;
    return stream.str();
}

eHovercraft FuncUtils::playerToHovercraft(ePlayer player)
{
    return (eHovercraft) player;
}

eHovercraft FuncUtils::botToHovercraft(eBot bot)
{
    return (eHovercraft) (bot + HOVERCRAFT_BOT_1);
}

ePlayer FuncUtils::hovercraftToPlayer(eHovercraft hovercraft)
{
    return hovercraft < HOVERCRAFT_BOT_1 ? (ePlayer)hovercraft : PLAYER_INVALID;
}

eBot FuncUtils::hovercraftToBot(eHovercraft hovercraft)
{
    return (HOVERCRAFT_BOT_1 <= hovercraft) && (hovercraft <= HOVERCRAFT_BOT_4) ? (eBot) (hovercraft - HOVERCRAFT_BOT_1) : BOT_INVALID;
}
