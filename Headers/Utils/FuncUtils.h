#pragma once

/* INCLUDES */
#include <map>

/* CLASS */
/*
Provides non-class specific functions for data manipulation.

@author Evan Quan
*/
template <class K, class V>
class FuncUtils
{
public:
    /*
    @param map<K, V>    m to check if key exists
    @param K            key to check in map
    @param V            default to return if key not present in m
    @return value to key if present, or default if not present
    */
    static V getValueIfNotDefault(map<K, V> m, K key, V default)
    {
        auto it = m.find(key);
        if (it != m.end())
        {
            return m[key];
        }
        else
        {
            return default;
        }
    }
private:
    FuncUtils() {};
    ~FuncUtils() {};
};

