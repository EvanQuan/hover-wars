#pragma once

#include "stdafx.h"

class FuncUtils
{
public:
	template <typename Key, typename Value>
	static Value& get_value_or_default(map<Key, Value>& inMap, const Key& key, Value& default_value);
private:
	FuncUtils();
};