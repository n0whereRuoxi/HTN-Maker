#include <string>
#include <vector>
#include <tr1/memory>

#include "exception.hpp"
#include "funcs.hpp"
#include "string_table.hpp"
#include "term.hpp"
#include "term_string.hpp"
#include "term_constant.hpp"
#include "term_constant_typed.hpp"

/** \file term_constant_typed.hpp
 *  Declaration of TermConstantTyped class.
 */

/** \file term_constant_typed.cpp
 *  Definition of TermConstantTyped class.
 */

/** \class TermConstantTyped
 *  A typed constant in first-order logic.
 */

/** \var TermConstantTyped::m_sTyping
 *  The string representation of this TermConstantTyped's type.
 */

/** \var TermConstantTyped::m_iTypingStrTableIndex
 *  The index in the global StringTable of this TermConstantTyped's type.
 */

/**
 *  The single global StringTable.
 */
extern StringTable g_StrTable;

/**
 *  Construct a new TermConstantTyped from string representations of the Term
 *   name and type.
 *  \param p_sName IN The name of the Term.
 *  \param p_sTyping IN The type of the Term.
 */
TermConstantTyped::TermConstantTyped( std::string p_sName, 
				      std::string p_sTyping )
  : TermConstant( p_sName )
{
  m_iTypingStrTableIndex = g_StrTable.Lookup( p_sTyping );
}

/**
 *  Construct a copy of an existing TermConstantTyped.
 *  \param p_tOther IN The TermConstantTyped to be copied.
 */
/*
TermConstantTyped::TermConstantTyped( const TermConstantTyped & p_tOther )
  : TermConstant( p_tOther )
{
  m_iTypingStrTableIndex = p_tOther.m_iTypingStrTableIndex;
}
*/

/**
 *  Empty destructor for derived classes.
 */
TermConstantTyped::~TermConstantTyped()
{
}

/**
 *  Print this TermConstantTyped to a string representation.
 *  \return A string containing the name of the Term, a hyphen, and the type of
 *   the Term.
 */
std::string TermConstantTyped::ToStr() const
{
  return TermString::ToStr() + " - " + g_StrTable.Lookup( m_iTypingStrTableIndex );
}

/**
 *  Test whether this TermConstantTyped is equal to some other Term.
 *  \param p_Other IN The other Term to be compared against.
 *  \return True if the Terms are equal, otherwise false.
 */
bool TermConstantTyped::Equal( const Term & p_Other ) const
{
  if( !TermConstant::Equal( p_Other ) )
    return false;
  if( !p_Other.HasTyping() )
    return false;
  return m_iTypingStrTableIndex == p_Other.GetTypingStrTableIndex();
}

/**
 *  Retrieve the type of this TermConstantTyped.
 *  \return A string representing the type of this Term.
 */
std::string TermConstantTyped::GetTyping() const
{
  return g_StrTable.Lookup( m_iTypingStrTableIndex );
}
