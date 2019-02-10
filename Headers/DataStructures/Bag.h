#pragma once
#include <map>
#include <iterator>

using namespace std;

/*
Bags contain quantities of objects, which can be added or removed.

Entirely class is defined in header, instead of .cpp file due to template usage.
https://stackoverflow.com/questions/1353973/c-template-linking-error 

Use of map instead of unordered map makees it easier for pairs to be key.
https://www.techiedelight.com/use-std-pair-key-std-unordered_map-cpp/
*/
template <class K>
class Bag
{
public:

/*
@return true if the key is contained
*/
bool contains(K key)
{
	map<K, int>::iterator it = inventory.find(key);
	return it == inventory.end();
}

/*
Add a quantity of keys to this bag.
Negative values will be ignored.

@return the value of the key after insertion
*/
int add(K key, int value)
{
	if (value > 0)
	{
		if (this->contains(key))
		{
			inventory.at(key) += value;
		}
		else
		{
			inventory.insert(make_pair(key, value));
		}
	}
	return this->count(key);
}

/*
Remove the key entirely.

@return the number of elements removed.
*/
int erase(K key)
{
	return inventory.erase(key);
}

/*
Remove a quantity of keys to this bag.
Negative values will be ignored.
If the quantity after removal is 0 or less, the key is removed.

@return the value of the key after removal
*/
int remove(K key, int value)
{
	if (value > 0 && this->contains(key))
	{
		int newValue = this->count(key) - value;

		if (newValue > 0)
		{
			inventory.insert(make_pair(key, newValue));
		}
		else
		{
			this->erase(key);
		}
	}
	return this->count(key);

}

/*
@return the count of key if contained, otherwise 0.
*/
int count(K key)
{
	if (this->contains(key))
	{
		return inventory.at(key);
	}
	else
	{
		return 0;
	}
}

/*
Get the number of all keys contained.

@return the number of keys contained
*/
int size()
{
	return inventory.size();
}

/*
Empty all keys
*/
void Bag<K>::clear()
{
	inventory.clear();
}

private:

	map<K, int> inventory;

};



