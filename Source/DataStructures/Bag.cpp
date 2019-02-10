//#include "DataStructures/Bag.h"
//#include "Enums/ePlayer.h"
//#include "Enums/ePowerup.h"
//
//template <class K>
//Bag<K>::Bag()
//{
//}
//
//template <class K>
//Bag<K>::~Bag()
//{
//}
//
///*
//@return true if the key is contained
//*/
//template<class K>
//bool Bag<K>::contains(K key)
//{
	//unordered_map<K, int>::iterator it = inventory.find(key);
	//return it == inventory.end();
//}
//
///*
//Add a quantity of keys to this bag.
//Negative values will be ignored.
//
//@return the value of the key after insertion
//*/
//template <class K>
//int Bag<K>::add(K key, int value)
//{
	//if (value > 0)
	//{
		//if (this->contains(key))
		//{
			//inventory.at(key) += value;
		//}
		//else
		//{
			//inventory.insert(make_pair(key, value));
		//}
	//}
	//return this->count(key);
//}
//
///*
//Remove the key entirely.
//
//@return the number of elements removed.
//*/
//template<class K>
//int Bag<K>::erase(K key)
//{
	//return inventory.erase(key);
//}
//
///*
//Remove a quantity of keys to this bag.
//Negative values will be ignored.
//If the quantity after removal is 0 or less, the key is removed.
//
//@return the value of the key after removal
//*/
//template<class K>
//int Bag<K>::remove(K key, int value)
//{
	//if (value > 0 && this->contains(key))
	//{
		//int newValue = this->count(key) - value;
//
		//if (newValue > 0)
		//{
			//inventory.insert(make_pair(key, newValue));
		//}
		//else
		//{
			//this->erase(key);
		//}
	//}
	//return this->count(key);
//
//}
//
///*
//@return the count of key if contained, otherwise 0.
//*/
//template<class K>
//int Bag<K>::count(K key)
//{
	//if (this->contains(key))
	//{
		//return inventory.at(key);
	//}
	//else
	//{
		//return 0;
	//}
//}
//
///*
//Get the number of all keys contained.
//
//@return the number of keys contained
//*/
//template<class K>
//int Bag<K>::size()
//{
	//return inventory.size();
//}
//
///*
//Empty all keys
//*/
//template<class K>
//void Bag<K>::clear()
//{
	//inventory.clear();
//}
//
//// No need to call this. It's to void link errors.
//void temporaryFunction()
//{
	//Bag<ePlayer> tempBag;
	//// Bag<pair<ePlayer, ePlayer>> tempBag2;
//}
