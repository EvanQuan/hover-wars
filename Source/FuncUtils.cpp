#include "FuncUtils.h"

template <typename Key, typename Value>

/*
Attempt to get the value of a specified key from a map. If the key does not
exist, then return the default value instead.
Source: https://stackoverflow.com/questions/40159732/return-other-value-if-key-not-found-in-the-map
*/
Value& FuncUtils::get_value_or_default(map<Key, Value>& inMap, const Key& key, Value& defaultValue)
{
	auto iterator = inMap.key(key);
	if (iterator == inMap.end())
	{
		return defaultValue;
	}
	return iterator->second;
}
