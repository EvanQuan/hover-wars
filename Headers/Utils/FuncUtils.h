#pragma once

/* INCLUDES */
#define GLM_ENABLE_EXPERIMENTAL
#include <map>
#include <unordered_map>
#include <random>
#include "Enums/ePlayer.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/ext.hpp>

/*
    Provides non-class specific functions for data manipulation.

    @author Evan Quan
*/
using namespace std;
namespace FuncUtils
{
    /*
        Check if a map contains a specified key.

        This is a standard method of map in C++20. I can feel the disapointment
        as you look over this James, since this is such a trivially unnecessary
        function.

        @param map<K, V>    m to check if the key exists
        @param K            key to check in map
        @return true if the specified map contains the specified key.
    */
    template <class K, class V>
    bool contains(map<K, V> m, K key)
    {
        return m.find(key) != m.end();
    }

    /*
        Check if a vector contains a specified key in O(n) time.
        @param vector<K>    v to check if the key exists
        @param K            key to check in vector
        @return true if the specified vector contains the specified key.
    */
    template <class K>
    bool contains(vector<K> v, K key)
    {
        return std::find(v.begin(), v.end(), key) != v.end();
    }
    /*
        Swaps two variables of the same type.
    */
    template <class K>
    void swap(K LHS, K RHS)
    {
        K Temp = LHS;
        LHS = RHS;
        RHS = Temp;
    }

    /*
        Check if an unordered map contains a specified key.

        This is a standard method of map in C++20. I can feel the disapointment
        as you look over this James, since this is such a trivially unnecessary
        function.

        @param unordered_map<K, V>  m to check if the key exists
        @param K                    key to check in map
        @return true if the specified unordered_map contains the specified key.
    */
    template <class K, class V>
    bool contains(unordered_map<K, V> m, K key)
    {
        return m.find(key) != m.end();
    }

    /*
        Retrieve the value from the specified key in a given map. If the
        key/value pair does not exist, return the default value instead.

        @param map<K, V>    m to check if key exists
        @param K            key to check in map
        @param V            defaultValue to return if key not present in m
        @return value to key if present, or default if not present
    */
    template <class K, class V>
    V getOrDefault(map<K, V> m, K key, V defaultValue)
    {
        return contains(m, key) ? m[key] : defaultValue;
    }

    /*
        Retrieve the value from the specified key in a given unordered_map. If
        the key/value pair does not exist, return the default value instead.

        @param unordered_map<K, V>  m to check if key exists
        @param K                    key to check in map
        @param V                    defaultValue to return if key not present
                                    in m
        @return value to key if present, or default if not present
    */
    template <class K, class V>
    V getOrDefault(unordered_map<K, V> m, K key, V defaultValue)
    {
        return contains(m, key) ? m[key] : defaultValue;
    }

    template <class K>
    K max(K value1, K value2)
    {
        return value1 > value2 ? value1 : value2;
    }

    template <class K>
    K min(K value1, K value2)
    {
        return value1 < value2 ? value1 : value2;
    }

    /*
        Bound a value between a min and max value of the same type

        @param value    to bound
        @param min      that value can be
        @param max      that value can be
        @return new value bounded between min and max
    */
    template <class K>
    K bound(K value, K min, K max)
    {
        return FuncUtils::min(max, FuncUtils::max(min, value));
    }

    /*
        Get the magnitude of a vector represented by its x and y coordinates

        @param float    x coordinate
        @param float    y coordinate
        @return the magnitude of the <x, y> vector
    */
    float getMagnitude(float x, float y);

    /*
        Check if two floats are equal. This is more accurate, but slower than
        using the == operator as such:

            bool result = x == y;

        Use where accuracy is more important than speed.

        @param float    x
        @param float    y
        @return true if x and y are equal
    */
    bool equals(float x, float y);

    /*
        Get a uniformly distributed int from the start to end range, inclusively
        @param start    of range, inclusively
        @param end      of range, inclusively
    */
    int random(int start, int end);

    /*
        Get a uniformly distributed int from 0 to range, exclusively
        @param range    of ints to choose from
    */
    int random(int range);

    /*
        Convert a float to string, given a specified number of decimal places.

        @param value of float to convert
        @param decimalPlaces to include in the string
        @return string of float with specified decimal places
    */
    std::string toString(float value, int decimalPlaces);


    /*
        Get the roll (x-axis rotation) from a specified quaterion

        @param q        to get roll
        @return roll of q
    */
    double getRoll(glm::quat q);
    /*
        Get the pitch (y-axis rotation) from a specified quaterion

        @param q        to get pitch
        @return pitch of q
    */
    double getPitch(glm::quat q);
    /*
        Get the yaw (z-axis rotation) from a specified quaterion

        @param q        to get yaw
        @return yaw of q
    */
    double getYaw(glm::quat q);

    /*
        Add increase to value, and keep it within min and max modulo bounds.

        @param value
        @param increase
        @param min
        @param max
    */
    int addModulo(int value, int increase, int min, int max);

    /*
        Subtract decrease from value, and keep it within min and max modulo
        bounds. More expensive than FuncUtils::addModulo()

        @param value
        @param decrease
        @param min
        @param max
    */
    int subtractModulo(int value, int decrease, int min, int max);

    /*
        Format time in seconds to form:

            minutes:seconds

        @param seconds  to format
        @return formatted time
    */
    std::string timeToString(int seconds);

    double log(int value, int base);

    /*
        Sleep this thread for the specified amount of time, in seconds

        @param timeInSeconds    to sleep
    */
    void sleep(float timeInSeconds);
}
