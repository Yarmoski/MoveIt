#ifndef EXPANDABLEHASHMAP
#define EXPANDABLEHASHMAP

#include <vector>
#include <list>

//custom implementation of an open hash map

template <typename KeyType, typename ValueType>
class ExpandableHashMap
{
public:
	ExpandableHashMap(double maximumLoadFactor = 0.5);
	~ExpandableHashMap();
	void reset();
	int size() const;
	void associate(const KeyType& key, const ValueType& value);

	// for a map that can't be modified, return a pointer to const ValueType
	const ValueType* find(const KeyType& key) const;

	// for a modifiable map, return a pointer to modifiable ValueType
	ValueType* find(const KeyType& key)
	{
		return const_cast<ValueType*>(const_cast<const ExpandableHashMap*>(this)->find(key));
	}

	// C++11 syntax for preventing copying and assignment
	ExpandableHashMap(const ExpandableHashMap&) = delete;
	ExpandableHashMap& operator=(const ExpandableHashMap&) = delete;

private:
	class Node
	{
	public:
		Node(KeyType m_key, ValueType m_value)
			:m_key(m_key), m_value(m_value)
		{
		}
		KeyType m_key;
		ValueType m_value;
	};
	double m_loadFactor;
	int m_associations;
	std::vector<std::list<Node>> m_map;
	unsigned int getBucketNumber(const KeyType& key) const
	{
		unsigned int hasher(const KeyType & k);  // prototype          
		unsigned int h = hasher(key);
		return h % m_map.size();
	}
};

template<typename KeyType, typename ValueType>
ExpandableHashMap<KeyType, ValueType>::ExpandableHashMap(double maximumLoadFactor)
	:m_map(8), m_associations(0)
{
	//if the load factor is negative, set it to the default 0.5
	if (maximumLoadFactor <= 0)
		m_loadFactor = 0.5;
	//otherwise, set it to the inputted one
	else
		m_loadFactor = maximumLoadFactor;
}

template<typename KeyType, typename ValueType>
ExpandableHashMap<KeyType, ValueType>::~ExpandableHashMap()
{
}

template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::reset()
{
	//clear the vector
	m_map.clear();
	//set vector size to 8
	m_map.resize(8);
	//reset associations
	m_associations = 0;
}

template<typename KeyType, typename ValueType>
int ExpandableHashMap<KeyType, ValueType>::size() const
{
	return m_associations;
}

template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::associate(const KeyType& key, const ValueType& value)
{
	//set curr to the list at the bucket of interest
	auto curr = m_map[getBucketNumber(key)];

	//if the key already exists at the bucket, update its value and return
	ValueType* pointer = find(key);
	if (pointer != nullptr)
	{
		(*pointer) = value;
		return;
	}

	//add the association
	m_map[getBucketNumber(key)].push_back(Node(key, value));
	m_associations++;

	

	//if this association exceeds the loadfactor
	if (m_associations > (m_loadFactor * m_map.size()))
	{
		//create a new map that has twice the number of buckets as the old map
		std::vector<std::list<Node>> newMap(m_map.size() * 2);
		

		//Rehash all items from the current hash map into the new, larger hash map 

		//set internal map to the new map
		newMap.swap(m_map);

		//loop through the old map
		for (int i = 0; i < newMap.size(); i++)
		{

			//set it to the first item of the list
			auto it = newMap[i].begin();

			//loop through list
			for (;it != newMap[i].end(); it++)
			{
				//add each node's key and value to the appropriate node in the new map
				m_map[getBucketNumber((*it).m_key)].push_back(Node((*it).m_key, (*it).m_value));
			}
		}

	}

}

template<typename KeyType, typename ValueType>
const ValueType* ExpandableHashMap<KeyType, ValueType>::find(const KeyType& key) const
{
	
	if (m_associations == 0)
		return nullptr;

	//hash key to find appropriate bucket
	//list at bucket number
	auto it = m_map[getBucketNumber(key)].begin();
	for (; it != m_map[getBucketNumber(key)].end(); it++)
	{
		if ((*it).m_key == key)
		{
			return &((*it).m_value);
		}
	}
	
	//if no association found, return nullptr
	return nullptr; 
}

#endif