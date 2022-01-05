#include <vector>
#include <string>
#include <cassert>
#include <iostream>
#include <tr1/memory>
#include <cstdlib>

#include "exception.hpp"
#include "funcs.hpp"
#include "string_table.hpp"
#include "term.hpp"
#include "term_string.hpp"
#include "term_variable.hpp"
#include "term_constant.hpp"
#include "substitution.hpp"

/** \file term_string.hpp
 *  Declaration of the TermString class.
 */

/** \file term_string.cpp
 *  Definition of the TermString class.
 */

/** \class TermString
 *  A literal logical Term, represented as a string with no whitespace.
 */

/** \var TermString::m_sValue
 *  The string representation of this Term.
 */

/**
 *  Construct a new TermString from the string that it represents.
 *  This removes any leading or trailing whitespace.
 *  If this Term is an automatic variable, make sure the next automatic ID is
 *   higher than it.
 *  \param p_sName IN The string representation of this Term.
 */
TermString::TermString( std::string p_sName )
{
  while( p_sName[0] == ' ' || p_sName[0] == '\n' ||
	 p_sName[0] == '\r' || p_sName[0] == '\t' )
    p_sName.erase( p_sName.begin() );
  while( p_sName[p_sName.size() - 1] == ' ' ||
	 p_sName[p_sName.size() - 1] == '\n' ||
	 p_sName[p_sName.size() - 1] == '\r' ||
	 p_sName[p_sName.size() - 1] == '\t' )
    p_sName.erase( p_sName.size() - 1, 1 );

  m_sValue = p_sName;

  if( p_sName.size() > 6 && 
      p_sName[0] == '?' &&
      ( p_sName[1] == 'a' || p_sName[1] == 'A' ) &&
      ( p_sName[2] == 'u' || p_sName[2] == 'U' ) &&
      ( p_sName[3] == 't' || p_sName[3] == 'T' ) &&
      ( p_sName[4] == 'o' || p_sName[4] == 'O' ) &&
      p_sName[5] == '_' )
  {
    std::string l_sNum = p_sName.substr( 6 );
    IncreaseNextVarId( atoi( l_sNum.c_str() ) );
  }
}

/**
 *  Construct a copy of an existing TermString.
 *  \param p_tOther IN The TermString to copy.
 */
/*
TermString::TermString( const TermString & p_tOther )
{
  m_iStrTableIndex = p_tOther.m_iStrTableIndex;
}
*/

/**
 *  Empty destructor for derived classes.
 */
TermString::~TermString()
{
}

/**
 *  Print this TermString to a string.
 *  Typed derived classes will need to override this.
 *  \return The string from which this TermString was constructed.
 */
std::string TermString::ToStr() const
{
  return m_sValue;
}

/**
 *  Print this TermString to a string without typing.
 *  \return The string from which this TermString was constructed.
 */
std::string TermString::ToStrNoTyping() const
{
  return m_sValue;
}

size_t TermString::GetMemSizeMin() const
{
  return sizeof( TermString );
}

size_t TermString::GetMemSizeMax() const
{
  return sizeof( TermString );
}
