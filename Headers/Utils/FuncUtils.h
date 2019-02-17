#pragma once

/* INCLUDES */
#include <map>

/* CLASS */
/*
Provides non-class specific functions for data manipulation.

@author Evan Quan
*/
using namespace std;
template <class K, class V>
class FuncUtils
{
public:
    /*
    Retrieve the value from the specified key in a given map. If the key/value
    pair does not exist, return the default value instead.

    @param map<K, V>    m to check if key exists
    @param K            key to check in map
    @param V            defaultValue to return if key not present in m
    @return value to key if present, or default if not present
    */
    static V getValueIfNotDefault(map<K, V> m, K key, V defaultValue)
    {
        return contains(m, key) ? m[key] : defaultValue;
    }

    /*
    Check if a map contains a specified key.

    This is a standard method of map in C++20. I can feel the disapointment as
    you look over this James, since this is such a trivially unnecessary
    function.

    @param map<K, V>    m to check if the key exists
    @param K            key to check in map
    @return true if the specified map contains the specified key.
    */
    static bool contains(map<K, V> m, K key)
    {
        return m.find(key) != m.end();
    }

    /*
    Get the magnitude of a vector represented by its x and y coordinates

    @param float    x coordinate
    @param float    y coordinate
    @return the magnitude of the <x, y> vector
    */
    static float magnitude(float x, float y)
    {
        return sqrt((x * x) + (y * y));
    }

private:
    /*
    Not instantiable.
    */
    FuncUtils() {};
    ~FuncUtils() {};
};

