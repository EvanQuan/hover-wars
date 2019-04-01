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
float FuncUtils::getMagnitude(float x, float y)
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

// Seed only once
std::random_device random_seed; // get a random number for the seed
std::mt19937 random_generator(random_seed()); // seed the generator

/*
    Get a uniformly distributed int from the start to end range, inclusively
    @param start    of range, inclusively
    @param end      of range, inclusively
*/
int FuncUtils::random(int start, int end)
{
    std::uniform_int_distribution<> distribution(start, end);
    return distribution(random_generator);
}

/*
    Get a uniformly distributed int from 0 to range, exclusively
    @param range    of ints to choose from
*/
int FuncUtils::random(int range)
{
    std::uniform_int_distribution<> distribution(0, range - 1);
    return distribution(random_generator);
}

/*
Convert a float to string, given a specified number of decimal places.

@param value of float to convert
@param decimalPlaces to include in the string
@return string of float with specified decimal places
*/
std::string FuncUtils::toString(float value, int decimalPlaces)
{
    stringstream stream;
    stream << fixed << setprecision(decimalPlaces) << value;
    return stream.str();
}

#define M_PI 3.1415926535
/*
    Get the roll (x-axis rotation) from a specified quaterion

    @param q        to get roll
    @return roll of q
*/
double FuncUtils::getRoll(glm::quat q)
{
    double sinr_cosp = +2.0 * (q.w * q.x + q.y * q.z);
    double cosr_cosp = +1.0 - 2.0 * (q.x * q.x + q.y * q.y);
    return atan2(sinr_cosp, cosr_cosp);
}

/*
    Get the pitch (y-axis rotation) from a specified quaterion

    @param q        to get pitch
    @return pitch of q
*/
double FuncUtils::getPitch(glm::quat q)
{
    double sinp = +2.0 * (q.w * q.y - q.z * q.x);
    if (fabs(sinp) >= 1) {
        return copysign(M_PI / 2, sinp); // use 90 degrees if out of range
    } else {
        return asin(sinp);
    }
}

/*
    Get the yaw (z-axiz rotation) from a specified quaterion

    @param q        to get yaw
    @return yaw of q
*/
double FuncUtils::getYaw(glm::quat q)
{
    double siny_cosp = +2.0 * (q.w * q.z + q.x * q.y);
    double cosy_cosp = +1.0 - 2.0 * (q.y * q.y + q.z * q.z);
    return atan2(siny_cosp, cosy_cosp);
}

/*
    Add increase to value, and keep it within min and max modulo bounds.

    @param value
    @param increase
    @param min
    @param max
*/
int FuncUtils::addModulo(int value, int increase, int min, int max)
{
    return FuncUtils::max(min, ((value + increase)  % (max + 1)));
}

/*
    Subtract decrease from value, and keep it within min and max modulo
    bounds. More expensive than FuncUtils::addModulo()

    @param value
    @param decrease
    @param min
    @param max
*/
int FuncUtils::subtractModulo(int value, int decrease, int min, int max)
{
    max++;
    return FuncUtils::max(min, (((value - decrease) % max) + max) % max);
}

#define SECONDS_PER_MINUTE 60
/*
    Format time in seconds to form:

        minutes:seconds

    @param seconds  to format
    @return formatted time
*/
std::string FuncUtils::timeToString(int timeInSeconds)
{
    int total = (int) timeInSeconds;
    int seconds = total % SECONDS_PER_MINUTE;
    int minutes = (total / SECONDS_PER_MINUTE) % SECONDS_PER_MINUTE;
    return std::to_string(minutes) + ":" + (seconds < 10 ? "0" : "") + std::to_string(seconds);
}

double FuncUtils::log(int value, int base)
{
    return std::log(value) / std::log(base);
}

