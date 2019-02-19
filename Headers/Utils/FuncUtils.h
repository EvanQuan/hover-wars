#pragma once

/* INCLUDES */
#include <map>
#include <unordered_map>

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
    float magnitude(float x, float y);

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
}
