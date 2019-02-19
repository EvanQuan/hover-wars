#include "Utils/FuncUtils.h"

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
