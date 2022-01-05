#include <string>
#include <vector>
#include <tr1/memory>

#include "exception.hpp"
#include "funcs.hpp"
#include "term.hpp"
#include "term_string.hpp"
#include "term_table.hpp"
#include "term_constant.hpp"
#include "term_variable.hpp"
#include "substitution.hpp"

/** \file term_constant.hpp
 *  Declaration of TermConstant class.
 */

/** \file term_constant.cpp
 *  Definition of TermConstant class.
 */

/** \class TermConstant
 *  A constant in first order logic, represented as a string literal that
 *   does not begin with '?'.
 */

extern TermTable g_TermTable;

/**
 *  Construct a new TermConstant from the string that represents it.
 *  \param p_sName IN The string that represents this TermConstant.
 */
TermConstant::TermConstant( std::string p_sName )
  : TermString( p_sName )
{
}

/**
 *  Construct a new TermConstant as an exact copy of another.
 *  \param p_tOther IN The TermConstant to copy.
 */
/*
TermConstant::TermConstant( const TermConstant & p_tOther )
  : TermString( p_tOther )
{
}
*/

/**
 *  Empty destructor for derived classes.
 */
TermConstant::~TermConstant()
{
}

/**
 *  Retrieve the TermType of this Term (TT_CONSTANT).
 *  \return TT_CONSTANT.
 */
TermType TermConstant::GetType() const
{
  return TT_CONSTANT;
}

/**
 *  Return a pointer to this TermConstant, because a Substitution will not
 *   affect it.
 *  \param p_sSubs IN The Substitution to apply.
 *  \param p_iDepth IN The depth of this Substitution attempt.
 *  \return A pointer to this.  It should be in the global TermTable, which
 *   will be responsible for deallocating it.
 */
TermP TermConstant::AfterSubstitution( const Substitution & p_sSubs,
				       unsigned int p_iDepth ) const
{
  return GetThis();
}

/**
 *  Determine whether or not this is equal to another Term.
 *  First makes a pointer comparison for some efficiency.
 *  Then requires that the other also be a TermConstant and that either both
 *   have the same type or neither has a type.
 *  Then requires that the underlying strings are identical.
 *  \param p_Other IN The Term to compare against.
 *  \return True if the Terms are equal, otherwise false.
 */
bool TermConstant::Equal( const Term & p_Other ) const
{
  return ( this == &p_Other );
}
