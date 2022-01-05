#include <string>
#include <vector>
#include <tr1/memory>

#include "exception.hpp"
#include "funcs.hpp"
#include "term.hpp"
#include "term_string.hpp"
#include "term_variable.hpp"
#include "term_constant.hpp"
#include "term_variable_typed.hpp"
#include "term_constant_typed.hpp"
#include "term_table.hpp"

/** \file term_table.hpp
 *  Declaration of the TermTable class.
 */

/** \file term_table.cpp
 *  Definition of the TermTable class and the single global object of it, 
 *   g_TermTable.
 */

/** \class TermTable
 *  A hash table that maps strings into the Terms that they represent.
 *  The primary purpose of this is to speed-up the process of determining
 *   whether or not two Terms are equal.  If both are stored only in a single
 *   TermTable, an O(1) pointer comparison will determine if they are equal.
 *  As a nice side effect, storing each Term only once provides a small amount
 *   of space savings.  If this were used without the StringTable, that
 *   savings would be even more significant.
 *  There should only be a single instance of this for each program, otherwise
 *   the comparisons mentioned above will not work.
 */

/** \var TermTable::m_mTermLookup
 *  A hash table from strings to Terms.
 *  This is the heart of TermTable.
 */

/**
 *  The one and only instance of TermTable that should ever exist.
 */
TermTable g_TermTable;

/**
 *  Destructor.
 *  Removes all of the elements in the hash table, because they are dynamically
 *   allocated.
 */
TermTable::~TermTable()
{
}

/**
 *  Lookup a pointer to the Term associated with a given string, it's name, and
 *   creates one if it does not exist.
 *  This should be an O(1) operation.
 *  When creating a new Term, the string is first uppercased to be consistent
 *   everywhere in the program.  Note that this is not necessary for 
 *   comparison.
 *  \param l_sKey IN The name of a Term to find or create a pointer to.
 *  \return A const pointer to a Term related to the input string.  It is
 *   guaranteed to be the same as any other pointer returned for that string.
 *   The pointer will be invalid when the TermTable goes out of scope, but this
 *   should only happen when global objects are cleaned up.
 */
TermP TermTable::Lookup( std::string l_sKey )
{
  std::tr1::unordered_map< std::string, TermP, HashStr, StrEquNoCase >::iterator l_Iter = m_mTermLookup.find( l_sKey );
  if( l_Iter == m_mTermLookup.end() )
  {
    std::string l_sUpper;
    //    for( int i = 0; i < l_sKey.size(); i++ )
    //      l_sUpper += toupper( l_sKey[i] );
    l_sUpper = l_sKey;

    TermP l_pNew( NewTerm( l_sUpper ) );
    l_pNew->m_pThis = l_pNew;

    m_mTermLookup[l_sUpper] = l_pNew;
    return l_pNew;
  }
  else
    return (*l_Iter).second;
}

/**
 *  Lookup a pointer to the Term associated with a given string, it's name, and
 *   a second string, it's type, creating one if it does not exist.
 *  The type is not part of the key.  Rather, there may only be one Term with
 *   a given name, and any other calls will need to be of the same type.
 *  \param l_sKey IN The name of a Term to find or create a pointer to.
 *  \param l_sTyping IN The type associated wih the Term to find or create.
 *  \return A const pointer to a Term related to the input string.  It is
 *   guaranteed to be the same as any other pointer returned for that string.
 *   The pointer will be invalid when the TermTable goes out of scope, but this
 *   should only happen when global objects are cleaned up.
 */
TermP TermTable::Lookup( std::string l_sKey, 
			 std::string l_sTyping )
{
  std::tr1::unordered_map< std::string, TermP, HashStr, StrEquNoCase >::iterator l_Iter = m_mTermLookup.find( l_sKey );
  if( l_Iter == m_mTermLookup.end() )
  {
    std::string l_sUpper;
    //    for( int i = 0; i < l_sKey.size(); i++ )
    //      l_sUpper += toupper( l_sKey[i] );
    l_sUpper = l_sKey;

    TermP l_pNew( NewTerm( l_sUpper, l_sTyping ) );
    l_pNew->m_pThis = l_pNew;

    m_mTermLookup[l_sUpper] = l_pNew;
    return l_pNew;
  }
  else
  {
    if( CompareNoCase( (*l_Iter).second->GetTyping(), l_sTyping ) != 0 )
      throw Exception( E_NOT_IMPLEMENTED,
		       "Term " + l_sKey + " was already declared with type " + (*l_Iter).second->GetTyping() + ", but is now being used with type " + l_sTyping + ".",
		       __FILE__,
		       __LINE__ );
    return (*l_Iter).second;
  }
}

/**
 *  Create a new Term from a string representation.
 *  The caller is responsible for deallocating it.
 *  Leading whitespace is stripped from the input.
 *  \param p_Stream INOUT A stringstream containing a term name.
 *  \return A pointer to a new Term with that name.
 */
Term * TermTable::NewTerm( std::stringstream & p_Stream )
{
  EatWhitespace( p_Stream );
  std::string l_sTemp = ReadString( p_Stream );
  return NewTerm( l_sTemp );

}

/**
 *  Create a new Term from a string representation.
 *  The caller is responsible for deallocating it.
 *  The caller is responsible for stripping whitespace from the beginning and
 *   end of the string before calling.
 *  \param p_sString IN A string containing a term name.
 *  \return A pointer to a new Term with that name.
 */
Term * TermTable::NewTerm( std::string p_sString ) throw ( MissingStringException )
{
  if( p_sString.length() > 0 )
  {
    if( p_sString[0] == '?' )
      return new TermVariable( p_sString );
    else
      return new TermConstant( p_sString );
  }
  else
    throw MissingStringException( "Expecting a non-zero length term.",
				  __FILE__,
				  __LINE__,
				  "any term" );
}

/**
 *  Create a new Term from a string representation of its name and type.
 *  This refers to the semantic type of the Term, not its TermType.
 *  The caller is responsible for deallocating it.
 *  The caller is responsible for stripping whitespace from the beginning and
 *   end of both strings.
 *  \param p_sName IN A string containing a term name.
 *  \param p_sTyping IN A string containing a type name.
 *  \return A pointer to a new Term with that name and type.
 */
Term * TermTable::NewTerm( std::string p_sName, std::string p_sTyping ) throw ( MissingStringException )
{
  if( p_sName.length() > 0 )
  {
    if( p_sName[0] == '?' )
      return new TermVariableTyped( p_sName, p_sTyping );
    else
      return new TermConstantTyped( p_sName, p_sTyping );
  }
  else
    throw MissingStringException( "Expecting a non-zero length term.",
				  __FILE__,
				  __LINE__,
				  "any term" );
}

size_t TermTable::GetMemSizeMin() const
{
  size_t l_iSize = sizeof( TermTable ) + m_mTermLookup.bucket_count() * ( sizeof( std::string ) + sizeof( TermP ) );
  for( std::tr1::unordered_map< std::string, TermP, HashStr, StrEquNoCase >::const_iterator i = m_mTermLookup.begin(); i != m_mTermLookup.end(); i++ )
    l_iSize += (*i).first.capacity() + (*i).second->GetMemSizeMax();
  return l_iSize;
}

size_t TermTable::GetMemSizeMax() const
{
  return GetMemSizeMin();
}
