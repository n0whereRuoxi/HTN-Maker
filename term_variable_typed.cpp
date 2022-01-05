#include <string>
#include <vector>
#include <tr1/memory>

#include "exception.hpp"
#include "funcs.hpp"
#include "string_table.hpp"
#include "term.hpp"
#include "term_string.hpp"
#include "term_variable.hpp"
#include "term_variable_typed.hpp"

/** \file term_variable_typed.hpp
 *  Declaration of the TermVariableTyped class.
 */

/** \file term_variable_typed.cpp
 *  Definition of TermVariableTyped class.
 */

/** \class TermVariableTyped
 *  A typed variable in first-order logic.
 */

/** \var TermVariableTyped::m_sTyping
 *  The string representation of this TermVariableTyped's type.
 */

/** \var TermVariableTyped::m_iTypingStrTableIndex
 *  The index in the global StringTable of this TermVariableTyped's type.
 */

/**
 *  The single global StringTable.
 */
extern StringTable g_StrTable;

/**
 *  Construct a new TermVariableTyped from string representations of the Term
 *   name and type.
 *  \param p_sName IN The name of the Term.
 *  \param p_sTyping IN The type of the Term.
 */
TermVariableTyped::TermVariableTyped( std::string p_sName, 
				      std::string p_sTyping )
  : TermVariable( p_sName )
{
  m_iTypingStrTableIndex = g_StrTable.Lookup( p_sTyping );
}

/** Construct a copy of an existing TermVariableTyped.
 *  \param p_tOther IN The TermVariableTyped to be copied.
 */
/*
TermVariableTyped::TermVariableTyped( const TermVariableTyped & p_tOther )
  : TermVariable( p_tOther )
{
  m_iTypingStrTableIndex = p_tOther.m_iTypingStrTableIndex;
}
*/

/**
 *  Empty destructor for derived classes.
 */
TermVariableTyped::~TermVariableTyped()
{
}

/**
 *  Print this TermVariableTyped to a string representation.
 *  \return A string containing the name of the Term, a hyphen, and the type of
 *   the Term.
 */
std::string TermVariableTyped::ToStr() const
{
  return TermString::ToStr() + " - " + g_StrTable.Lookup( m_iTypingStrTableIndex );
}

/**
 *  Test whether this TermVariableTyped is equal to some other Term.
 *  \param p_Other IN The other Term to be compared against.
 *  \return True if the Terms are equal, otherwise false.
 */
bool TermVariableTyped::Equal( const Term & p_Other ) const
{
  if( !TermVariable::Equal( p_Other ) )
    return false;
  if( !p_Other.HasTyping() )
    return false;
  return m_iTypingStrTableIndex == p_Other.GetTypingStrTableIndex();
}

/**
 *  Retrieve the type of this TermVariableTyped.
 *  \return A string representing the type of this Term.
 */
std::string TermVariableTyped::GetTyping() const
{
  return g_StrTable.Lookup( m_iTypingStrTableIndex );
}
