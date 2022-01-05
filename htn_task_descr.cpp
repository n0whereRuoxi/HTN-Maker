#include <string>
#include <vector>
#include <sstream>
#include <set>
#include <tr1/memory>

#include "exception.hpp"
#include "funcs.hpp"
#include "term.hpp"
#include "term_string.hpp"
#include "term_constant.hpp"
#include "term_variable.hpp"
#include "type_table.hpp"
#include "substitution.hpp"
#include "formula.hpp"
#include "formula_pred.hpp"
#include "formula_neg.hpp"
#include "formula_conj.hpp"
#include "htn_task_head.hpp"
#include "htn_task_descr.hpp"

/** \file htn_task_descr.hpp
 *  Declaration of HtnTaskDescr class.
 */

/** \file htn_task_descr.cpp
 *  Definition of HtnTaskDescr class.
 */

/** \class HtnTaskDescr
 *  A description of what it means to accomplish a task in an HTN.
 *  Specifically, what are the conditions necessary to make it possible to
 *   accomplish the task and what effect will it have?
 */

/** \var HtnTaskDescr::m_pHead
 *  A smart pointer to the head of the task being described.
 */

/** \var HtnTaskDescr::m_pPreconditions
 *  A smart pointer to the conditions under which it is possible to accomplish
 *   the task.
 */

/** \var HtnTaskDescr::m_pEffects
 *  A smart pointer to the effects that will always result from this task 
 *   being accomplished.
 *  Depending on the way the task is accomplished, there may be side effects
 *   as well.
 */

/**
 *  Construct an HtnTaskDescr from its textual representation and stuff.
 *  \param p_sInput INOUT A stream containing a textual representation of the
 *   HtnTaskDescr.  The stream is advanced beyond this description.
 *  \param p_sTypes IN A  list of allowable types.  If non-empty, all terms 
 *   used must have a type in it.  If empty, all terms must be untyped.
 *  \param p_vAllowablePredicates IN A list of allowable predicate symbols with
 *   the number and types of their arguments.  If non-empty, only these 
 *   predicates may be used.  Otherwise, any may be used.
 */
HtnTaskDescr::HtnTaskDescr( std::stringstream & p_sInput, 
			    const std::set< std::string, StrLessNoCase > & p_sTypes,
			    const std::vector< FormulaPred > & p_vAllowablePredicates )
{
  ConstructorInternal( p_sInput, p_sTypes, p_vAllowablePredicates );
}

/**
 *  Construct an HtnTaskDescr from its textual representation and stuff.
 *  \param p_sString IN A textual representation of the HtnTaskDescr.
 *  \param p_sTypes IN A  list of allowable types.  If non-empty, all terms 
 *   used must have a type in it.  If empty, all terms must be untyped.
 *  \param p_vAllowablePredicates IN A list of allowable predicate symbols with
 *   the number and types of their arguments.  If non-empty, only these 
 *   predicates may be used.  Otherwise, any may be used.
 */
HtnTaskDescr::HtnTaskDescr( std::string p_sString, 
			    const std::set< std::string, StrLessNoCase > & p_sTypes,
			    const std::vector< FormulaPred > & p_vAllowablePredicates )
{
  std::stringstream l_sInput( p_sString );
  ConstructorInternal( l_sInput, p_sTypes, p_vAllowablePredicates );
}

/**
 *  Construct an HtnTaskDescr as a copy of an existing one.
 *  \param p_Other IN The HtnTaskDescr to copy.
 */
HtnTaskDescr::HtnTaskDescr( const HtnTaskDescr & p_Other )
{
}

/**
 *  Destruct this HtnTaskDescr.
 */
HtnTaskDescr::~HtnTaskDescr()
{
}

/**
 *  Construct an HtnTaskDescr from its textual representation and stuff.
 *  \param p_sInput INOUT A stream containing a textual representation of the
 *   HtnTaskDescr.  The stream is advanced beyond this description.
 *  \param p_sTypes IN A  list of allowable types.  If non-empty, all terms 
 *   used must have a type in it.  If empty, all terms must be untyped.
 *  \param p_vAllowablePredicates IN A list of allowable predicate symbols with
 *   the number and types of their arguments.  If non-empty, only these 
 *   predicates may be used.  Otherwise, any may be used.
 */
void HtnTaskDescr::ConstructorInternal( std::stringstream & p_sInput, 
					const std::set< std::string, StrLessNoCase > & p_sTypes,
					const std::vector< FormulaPred > & p_vAllowablePredicates )
{
  TypeTable l_TypeTable;

  EatWhitespace( p_sInput );
  EatString( p_sInput, "(" );

  EatWhitespace( p_sInput );
  EatString( p_sInput, ":task" );
  EatWhitespace( p_sInput );

  std::string l_sHead = "( ";
  l_sHead += ReadString( p_sInput );

  EatWhitespace( p_sInput );
  EatString( p_sInput, ":parameters" );
  EatWhitespace( p_sInput );

  EatString( p_sInput, "(" );
  EatWhitespace( p_sInput );

  while( p_sInput.peek() != ')' )
  {
    std::string l_sName = ReadString( p_sInput );
    l_sHead += " " + l_sName;
    if( p_sTypes.size() > 0 )
    {
      EatWhitespace( p_sInput );
      EatString( p_sInput, "-" );
      EatWhitespace( p_sInput );
      std::string l_sTyping = ReadString( p_sInput );
      if( p_sTypes.find( l_sTyping ) == p_sTypes.end() )
	throw Exception( E_NOT_IMPLEMENTED,
			 "Attempt to use an unregistered type.",
			 __FILE__,
			 __LINE__ );
      if( l_TypeTable.find( l_sName ) != l_TypeTable.end() )
      {
	if( CompareNoCase( (*l_TypeTable.find( l_sName )).second, l_sTyping ) == 0 )
	  throw Exception( E_NOT_IMPLEMENTED,
			   "Attempt to use a term with multiple types.",
			   __FILE__,
			   __LINE__ );
      }
      else
	l_TypeTable[l_sName] = l_sTyping;
    }
    EatWhitespace( p_sInput );
  }

  l_sHead += " )";

  m_pHead = HtnTaskHeadP( new HtnTaskHead( l_sHead, l_TypeTable ) );

  EatString( p_sInput, ")" );

  EatWhitespace( p_sInput );
  EatString( p_sInput, ":precondition" );
  EatWhitespace( p_sInput );

  m_pPreconditions = FormulaConjP( new FormulaConj( p_sInput, l_TypeTable, p_vAllowablePredicates ) );

  EatWhitespace( p_sInput );
  EatString( p_sInput, ":effect" );
  EatWhitespace( p_sInput );

  m_pEffects = FormulaConjP( new FormulaConj( p_sInput, l_TypeTable, p_vAllowablePredicates ) );

  EatWhitespace( p_sInput );
  EatString( p_sInput, ")" );
}

/**
 *  Retrieve a smart pointer to the head of this HtnTaskDescr.
 *  \return A smart pointer to the head of this HtnTaskDescr.
 */
HtnTaskHeadP HtnTaskDescr::GetCHead() const
{
  return m_pHead;
}

/**
 *  Retrieve a smart pointer to the preconditions of this HtnTaskDescr.
 *  \return A smart pointer to the preconditions of this HtnTaskDescr.
 */
FormulaConjP HtnTaskDescr::GetCPreconditions() const
{
  return m_pPreconditions;
}

/**
 *  Retrieve a smart pointer to the effects of this HtnTaskDescr.
 *  \return A smart pointer to the effects of this HtnTaskDescr.
 */
FormulaConjP HtnTaskDescr::GetCEffects() const
{
  return m_pEffects;
}

/**
 *  Retrieve a smart pointer to a new HtnTaskDescr that results from applying a
 *   Substitution to this one.
 *  \param p_Sub IN The Substitution to apply.
 *  \param p_iRecurseLevel IN The depth of this Substitution attempt.
 *  \return A smart pointer to a new HtnTaskDescr that results from applying 
 *   the given Substitution to this one.
 */
std::tr1::shared_ptr< HtnTaskDescr > HtnTaskDescr::AfterSubstitution( const Substitution & p_Sub,
								      unsigned int p_iRecurseLevel) const
{
  HtnTaskDescr * l_pRet = new HtnTaskDescr( *this );
  l_pRet->m_pHead = std::tr1::dynamic_pointer_cast< HtnTaskHead >( m_pHead->AfterSubstitution( p_Sub, p_iRecurseLevel ) );
  l_pRet->m_pPreconditions = std::tr1::dynamic_pointer_cast< FormulaConj >( m_pPreconditions->AfterSubstitution( p_Sub, p_iRecurseLevel ) );
  l_pRet->m_pEffects = std::tr1::dynamic_pointer_cast< FormulaConj >( m_pEffects->AfterSubstitution( p_Sub, p_iRecurseLevel ) );
  return std::tr1::shared_ptr< HtnTaskDescr >( l_pRet );
}

size_t HtnTaskDescr::GetMemSizeMin() const
{
  size_t l_iSize = sizeof( HtnTaskDescr );
  return l_iSize;
}

size_t HtnTaskDescr::GetMemSizeMax() const
{
  size_t l_iSize = sizeof( HtnTaskDescr ) + m_pHead->GetMemSizeMax() + m_pPreconditions->GetMemSizeMax() + m_pEffects->GetMemSizeMax();
  return l_iSize;
}
