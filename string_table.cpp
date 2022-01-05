#include <string>
#include <vector>
#include <tr1/unordered_map>

#include "exception.hpp"
#include "funcs.hpp"
#include "string_table.hpp"

/** \file string_table.hpp
 *  Declaration of the StringTable class.
 */

/** \file string_table.cpp
 *  Definition of the StringTable class and g_StrTable.
 */

/** \class StringTable
 *  A hash table of strings to integers.
 *  Using this, string comparisons can be O(1).
 */

/** \var StringTable::m_mIntLookup
 *  A hashmap from strings to integers.
 *  This is so looking up based on a string is amortized O(1).
 *  The integers are the indexes of the strings in StringTable::m_vStrLookup.
 */

/** \var StringTable::m_vStrLookup
 *  A vector of strings.
 *  This is so looking up based on an integer is O(1).
 *  Indexes in this vector match the integers in StringTable::m_mIntLookup.
 */

/**
 *  The single global StringTable that should be used in a running instance of
 *   any executable built with this.
 */
StringTable g_StrTable;

/**
 *  Lookup a string in the hash table, returning an auto-incremented ID.
 *  Inserts the string into the table if it does not currently exist there.
 *  \param l_sKey IN The string to lookup and possibly add.
 *  \return The ID associated with the input string in the table.
 */
unsigned int StringTable::Lookup( std::string l_sKey )
{
  std::tr1::unordered_map< std::string, unsigned int, HashStr, StrEquNoCase >::iterator l_Iter = m_mIntLookup.find( l_sKey );
  if( l_Iter == m_mIntLookup.end() )
  {
    std::string l_sUpper;
    for( unsigned int i = 0; i < l_sKey.size(); i++ )
      l_sUpper += toupper( l_sKey[i] );
    m_vStrLookup.push_back( l_sUpper );
    unsigned int l_iNewInt = m_vStrLookup.size() - 1;
    m_mIntLookup[l_sUpper] = l_iNewInt;
    return l_iNewInt;
  }
  else
    return (*l_Iter).second;
}

/**
 *  Lookup an integer in the vector, returning the associated string.
 *  Throws E_INDEX_OUT_OF_BOUNDS if the integer is not in the table.
 *  \param l_iKey IN The integer to lookup.
 *  \return The string associated with the input integer.
 */
std::string StringTable::Lookup( unsigned int l_iKey )
{
  if( l_iKey < m_vStrLookup.size() )
    return m_vStrLookup[ l_iKey ];
  else
    throw Exception( E_INDEX_OUT_OF_BOUNDS,
		     "Bounds error.",
		     __FILE__,
		     __LINE__ );
}

size_t StringTable::GetMemSizeMin() const
{
  size_t l_iSize = sizeof( StringTable ) + m_vStrLookup.capacity() * sizeof( std::string ) + m_mIntLookup.size() * (sizeof( std::string ) + sizeof( unsigned int ));
  for( unsigned int i = 0; i < m_vStrLookup.size(); i++ )
    l_iSize += 2 * m_vStrLookup[i].capacity();
  return l_iSize;
}

size_t StringTable::GetMemSizeMax() const
{
  return GetMemSizeMin();
}
