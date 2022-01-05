#include <vector>
#include <string>
#include <sstream>
#include <cassert>
#include <tr1/memory>

#include "exception.hpp"
#include "funcs.hpp"
#include "term.hpp"
#include "term_string.hpp"
#include "term_constant.hpp"
#include "term_variable.hpp"
#include "term_variable_typed.hpp"
#include "term_constant_typed.hpp"

/** \file term.hpp
 *  Declarations of the Term class and related items.
 */

/** \file term.cpp
 *  Definition of the Term class.
 */

/** \def MAX_SUBS_DEPTH
 *  The maximum number of times a substitution may be recursively applied to
 *   something.
 *  This is necessary because the first application may replace A with B, the
 *   second B with C and so on, but cycles should not be allowed.
 */

/** \enum TermType
 *  The basic type of a Term.
 *  This does not specify an exact class, because both TermVariable and
 *   TermVariableTyped, for example, are of type TT_VARIABLE.
 */

/** \var TT_CONSTANT
 *  A constant term.
 */

/** \var TT_VARIABLE
 *  A variable term, denoted by prepending a '?' to its name.
 */

/** \var TT_FUNCTION
 *  A function term, which is not currently implemented.
 */

/** \class Term
 *  A term in first-order logic.
 *  This is a pure virtual class.
 */

/** Does nothing, but exists for derived classes.
 */
Term::~Term()
{
}


/**
 *  Compare two terms for equality.
 *  \param p_First IN First Term to compare.
 *  \param p_Second IN Second Term to compare.
 *  \return True if the Terms are equal, false otherwise.
 */
bool operator==( const Term & p_First, const Term & p_Second )
{
  return p_First.Equal( p_Second );
}

TermP Term::GetThis() const
{
  return m_pThis.lock();
}
