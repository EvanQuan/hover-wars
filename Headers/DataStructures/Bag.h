#pragma once
#include <stdafx.h>
// #include <unordered_map>

/*
Bags contain quantities of objects, which can be added or removed.
*/
template <class K>
class Bag
{
public:
	Bag();
	~Bag();

	bool contains(K key);
	int insert(K key, int value);
	int erase(K key);
	int remove(K key, int value);
	int count(K key);
	int size();
	void clear();

private:

	unordered_map<K, int> inventory;

};



