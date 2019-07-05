#ifndef __HASHMAPSAMPLE_H__
#define __HASHMAPSAMPLE_H__

#include "Common/RFA_Vector.h"

template <class Key, class Value>
class HashMapSample
{
private:
	class Node {
			friend class HashMapSample;
		private:
			Key	key;
			Value value;
			Node *next;
			unsigned int hashCode;
		public:
			Node( const Key theKey, const Value theValue, unsigned int theHashCode ) : 
			  key(theKey), value(theValue), next(NULL),	hashCode(theHashCode)
			{}
			~Node( )
			{}
	};

	Node **m_MapTable;
	unsigned int m_tablesize, m_size;
	float m_LoadFactor;
public:
	HashMapSample( unsigned int initiailTableSize = 101, const float initialLoadFactor = 0.75f );
	~HashMapSample( void );

	bool contains( const Key& key );
	bool put( const Key& key, const Value& value );
	bool get( const Key& key, Value &value );
	void clear( void );
	void getKeyValues ( rfa::common::RFA_Vector<Key>& aKey, rfa::common::RFA_Vector<Value>& aVal);

	unsigned int getSize( ) const { return m_size; }
	bool isEmpty( ) const { return m_size <= 0; }
	void setHashFunction( unsigned int(*func)(const Key& key) ) { UserHash = func; }
	
private:
	void halveSize( );
	void doubleSize( );
	float getLoadFactor( ) const { return (float) m_size / m_tablesize; }
	unsigned int (*UserHash)( const Key& key );
};

template <class Key, class Value> 
HashMapSample<Key, Value>::HashMapSample( unsigned int initiailTableSize, const float initialLoadFactor) : m_tablesize( initiailTableSize ), m_size( 0 ), m_LoadFactor( initialLoadFactor )
{
	m_MapTable = new Node*[ m_tablesize ];
	for(unsigned int i = 0; i < m_tablesize; i++)
	{
		m_MapTable[ i ] = NULL;
	}
	UserHash = NULL;
}
template <class Key, class Value> 
HashMapSample<Key, Value>::~HashMapSample( void )
{
	clear( );
	delete [] m_MapTable;
}

template <class Key, class Value> 
bool HashMapSample<Key, Value>::contains( const Key& key )
{
	unsigned int hashCode = (*UserHash)(key);
	unsigned int index = hashCode % m_tablesize;

	Node *current = m_MapTable[ index ];

	while( current != NULL )
	{
		if( current->hashCode == hashCode && current->key == key )
		{
			return true;
		}
		current = current->next;
	}
	return false;
}
template <class Key, class Value> 
bool HashMapSample<Key, Value>::put( const Key& key, const Value& value )
{
	unsigned int hashCode = (*UserHash)(key);
	unsigned int index = hashCode % m_tablesize;

	if( contains( key ) )
	{
		return false;
	}
	if( getLoadFactor( ) > m_LoadFactor )
	{
		doubleSize( );
	}

	Node *newNode = new Node( key, value, hashCode );

	if( m_MapTable[ index ] == NULL )
	{
		m_MapTable[ index ] = newNode;
	}
	else 
	{
		newNode->next = m_MapTable[ index ];
		m_MapTable[ index ] = newNode;
	}

	m_size++;
	return true;
}

template <class Key, class Value> 
bool HashMapSample<Key, Value>::get( const Key& key, Value &value )
{
	unsigned int hashCode = (*UserHash)(key);
	unsigned int index = hashCode % m_tablesize;

	Node *current = m_MapTable[ index ];

	while( current != NULL )
	{
		if( current->hashCode == hashCode && current->key == key )
		{
			value = current->value;
			return true;
		}
		current = current->next;
	}
	return false;
}
template <class Key, class Value> 
void HashMapSample<Key, Value>::clear( void )
{
	Node *current = NULL;
	Node *temp = NULL;

	for( unsigned int index = 0; !isEmpty( ) ; index++ )
	{
		current = m_MapTable[ index ];
		while( current != NULL ) 
		{
			temp = current->next;
			delete current;
			m_size--;
			current = temp; 
		}
	}
}

template <class Key, class Value> 
void HashMapSample<Key, Value>::doubleSize( )
{	
	unsigned int newtableSize = 2 * m_tablesize + 1;
	Node **newTable = new Node*[ newtableSize ];

	Node *current = NULL;
	Node *temp = NULL;

	unsigned int newIndex = 0;

	// Initialize the new table
	for(unsigned int i = 0; i < newtableSize; i++)
	{
		newTable[ i ] = NULL;
	}
	// Rehash
	for( unsigned int index = 0; index < m_tablesize; index++ )
	{
		current = m_MapTable[ index ];

		while( current != NULL ) 
		{
			temp = current->next;
			newIndex = current->hashCode % newtableSize;

			if( newTable[ newIndex ] == NULL )
			{
				current->next = NULL;
				newTable[ newIndex ] = current;
			}
			else 
			{
				current->next = newTable[ newIndex ];
				newTable[ newIndex ] = current;
			}

			current = temp; 
		}
	}

	m_tablesize = newtableSize;
	delete [] m_MapTable;
	m_MapTable = newTable;
}
template <class Key, class Value>
void HashMapSample<Key, Value>::getKeyValues ( rfa::common::RFA_Vector<Key>& aKey, rfa::common::RFA_Vector<Value>& aVal)
{
	Node *current = NULL;
	int count = m_size;


	for( unsigned int index = 0; !isEmpty( ) ; index++ )
	{
		current = m_MapTable[ index ];
		while( current != NULL ) 
		{
			aKey.push_back(current->key);
			aVal.push_back(current->value);
			current = current->next;
			count--;

			if(count == 0)
			{
				return;
			}

		}
	}
}

#endif

