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

    This is a standard method of map in C++20. I can feel the disapointment as
    you look over this James, since this is such a trivially unnecessary
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

    This is a standard method of map in C++20. I can feel the disapointment as
    you look over this James, since this is such a trivially unnecessary
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
    Retrieve the value from the specified key in a given map. If the key/value
    pair does not exist, return the default value instead.

    @param map<K, V>    m to check if key exists
    @param K            key to check in map
    @param V            defaultValue to return if key not present in m
    @return value to key if present, or default if not present
    */
    template <class K, class V>
    V getValueIfNotDefault(map<K, V> m, K key, V defaultValue)
    {
        return contains(m, key) ? m[key] : defaultValue;
    }

    /*
    Retrieve the value from the specified key in a given unordered_map. If the
    key/value pair does not exist, return the default value instead.

    @param unordered_map<K, V>  m to check if key exists
    @param K                    key to check in map
    @param V                    defaultValue to return if key not present in m
    @return value to key if present, or default if not present
    */
    template <class K, class V>
    V getValueIfNotDefault(unordered_map<K, V> m, K key, V defaultValue)
    {
        return contains(m, key) ? m[key] : defaultValue;
    }

    /*
    Get the magnitude of a vector represented by its x and y coordinates

    @param float    x coordinate
    @param float    y coordinate
    @return the magnitude of the <x, y> vector
    */
    float getMagnitude(float x, float y);

    /*
    Check if two floats are equal. This is more accurate, but slower than using
    the == operator as such:

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
    Convert a float to string, given a specified number of decimal places.

    @param value of float to convert
    @param decimalPlaces to include in the string
    @return string of float with specified decimal places
    */
    std::string to_string(float value, int decimalPlaces);

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

    int moduloIncrease(int value, int increase, int min, int max);
    int moduloDecrease(int value, int decrease, int min, int max);

    int max(int num1, int num2);
    int min(int num1, int num2);

    eHovercraft playerToHovercraft(ePlayer player);
    eHovercraft botToHovercraft(eBot bot);
    ePlayer hovercraftToPlayer(eHovercraft hovercraft);
    eBot hovercraftToBot(eHovercraft hovercraft);
}
