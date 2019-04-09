#pragma once
#include <map>
#include <Utils/FuncUtils.h>

using namespace std;

/*
Bags contain quantities of objects, which can be added or removed.

Stuff with compilation and linking with template classes:
https://www.codeproject.com/Articles/48575/How-to-define-a-template-class-in-a-h-file-and-imp
None of these worked for me, so I decided to go with...

Entirely class is defined in header, instead of .cpp file due to template usage.
https://stackoverflow.com/questions/1353973/c-template-linking-error

Use of map instead of unordered map makes it easier for pairs to be key.
https://www.techiedelight.com/use-std-pair-key-std-unordered_map-cpp/

The exercise of reimplementing this from Java to C++ has made me learn just
how different Java's generics are from C++'s templates. Holy hell this was
a pain in the ass for something that looks so simple.

@author Evan Quan
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
    return FuncUtils<K, int>::contains(inventory, key);
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
