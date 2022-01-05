#include <string>
#include <cassert>
#include <vector>
#include <tr1/memory>

#include "exception.hpp"
#include "funcs.hpp"
#include "term.hpp"
#include "term_string.hpp"
#include "term_table.hpp"
#include "term_variable.hpp"
#include "term_constant.hpp"
#include "substitution.hpp"

/** \file term_variable.hpp
 *  Declaration of TermVariable class.
 */

/** \file term_variable.cpp
 *  Definition of TermVariable class.
 */

/** \class TermVariable
 *  A variable in first-order logic.
 */

extern TermTable g_TermTable;

/**
 *  Construct a new TermVariable for the string representation of the variable.
 *  \param p_sName IN The string representation of the variable.
 */
TermVariable::TermVariable( std::string p_sName )
  : TermString( p_sName )
{
}

/**
 *  Construct a new TermVariable as a copy of an existing one.
 *  \param p_tOther IN The TermVariable to copy.
 */
/*
TermVariable::TermVariable( const TermVariable & p_tOther )
  : TermString( p_tOther )
{
}
*/

/**
 *  Empty destructor for derived classes.
 */
TermVariable::~TermVariable()
{
}

/**
 *  Return the TermType of this Term (TT_VARIABLE).
 *  \return TT_VARIABLE.
 */
TermType TermVariable::GetType() const
{
  return TT_VARIABLE;
}

/**
 *  Retrive a new Term that results from a Substitution on this.
 *  Throws E_SUBS_RECURSE if p_iDepth > MAX_SUBS_DEPTH.
 *  Throws E_TERM_TYPE_UNKNOWN if the type of a Term in the Substitution is 
 *   unknown.
 *  \param p_sSubs IN The variable/term Substitution to apply.
 *  \param p_iDepth IN The depth of this Substitution attempt.
 *  \return A pointer to a Term with variables from the Substitution 
 *   replaced by their associated Term.  It should point into the global
 *   TermTable, which will take care of deallocating it.
 */
TermP TermVariable::AfterSubstitution( const Substitution & p_sSubs,
				       unsigned int p_iDepth ) const
{
  if( p_iDepth > MAX_SUBS_DEPTH )
    throw Exception( E_SUBS_RECURSE,
		     "Infinitely recursive substitution suspected.",
		     __FILE__,
		     __LINE__ );

  SubMap::const_iterator l_iIter = p_sSubs.FindIndexByVar( std::tr1::dynamic_pointer_cast< TermVariable >( GetThis() ) );

  if( l_iIter != p_sSubs.End() )
  {
    switch( l_iIter->second->GetType() )
    {
    case TT_CONSTANT:
      return l_iIter->second;
    case TT_VARIABLE:
      return l_iIter->second->AfterSubstitution( p_sSubs, p_iDepth + 1 );
    default:
      throw Exception( E_TERM_TYPE_UNKNOWN,
		       "Substitution term should be a variable or constant.",
		       __FILE__,
		       __LINE__ );
    }
  }
  else
    return GetThis();
}

/**
 *  Test whether another Term is equal to this one.
 *  \param p_Other IN The Term to be compared to.
 *  \return Whether or not the Term is exactly equal to this one.
 */
bool TermVariable::Equal( const Term & p_Other ) const
{
  return ( this == &p_Other );
}
