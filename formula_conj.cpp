#include <vector>
#include <string>
#include <sstream>
#include <cassert>
#include <tr1/memory>

#include "exception.hpp"
#include "funcs.hpp"
#include "term.hpp"
#include "term_string.hpp"
#include "term_variable.hpp"
#include "term_constant.hpp"
#include "type_table.hpp"
#include "formula.hpp"
#include "formula_conj.hpp"

/** \file formula_conj.hpp
 *  Declaration of the FormulaConj class.
 */

/** \file formula_conj.cpp
 *  Definition of the FormulaConj class.
 */

/** \class FormulaConj
 *  A conjunction of formulas in first-order logic.
 *  For the sake of efficiency, we only allow formulas in a particular form, in
 *   which the only children of a FormulaConj are of type FormulaPred, 
 *   FormulaEqu, or a FormulaNeg of one of them.
 */

/** \var FormulaConj::m_vConjuncts
 *  A list of smart pointers to the conjuncts of this Formula.
 */

/**
 *  Construct a FormulaConj from a stream containing its string representation.
 *  \param p_Stream INOUT A stream containing a string representation of this
 *   FormulaConj.  It will be advanced beyond this.
 *  \param p_TypeTable IN A table of allowable Terms and their types.  If not
 *   empty, only Terms in the table may be used, and they will be given their
 *   associated types.  Otherwise, any may be used untyped.
 *  \param p_vAllowablePredicates IN A list of allowable predicate symbols with
 *   their arities and types of parameters.  If empty, any will be allowed.
 */
FormulaConj::FormulaConj( std::stringstream & p_Stream, 
			  const TypeTable & p_TypeTable,
			  const std::vector< FormulaPred > & p_vAllowablePredicates )
{
  ConstructorInternal( p_Stream, p_TypeTable, p_vAllowablePredicates );
}

/**
 *  Construct a copy of an existing FormulaConj.
 *  \param p_Other IN The FormulaConj to copy.
 */
FormulaConj::FormulaConj( const FormulaConj & p_Other )
  : m_vConjuncts( p_Other.m_vConjuncts )
{
}

/**
 *  Construct a FormulaConj from its string representation.
 *  \param p_sString IN A string representation of this FormulaConj.
 *  \param p_TypeTable IN A table of allowable Terms and their types.  If not
 *   empty, only Terms in the table may be used, and they will be given their
 *   associated types.  Otherwise, any may be used untyped.
 *  \param p_vAllowablePredicates IN A list of allowable predicate symbols with
 *   their arities and types of parameters.  If empty, any will be allowed.
 */
FormulaConj::FormulaConj( std::string p_sString, 
			  const TypeTable & p_TypeTable,
			  const std::vector< FormulaPred > & p_vAllowablePredicates )
{
  std::stringstream l_Stream( p_sString );
  ConstructorInternal( l_Stream, p_TypeTable, p_vAllowablePredicates );
}

/**
 *  Construct a FormulaConj from a list of smart pointers to conjuncts.
 *  \param p_Vec IN A list of smart pointers to Formulas.
 */
FormulaConj::FormulaConj( const FormulaPVec & p_Vec )
  : m_vConjuncts( p_Vec )
{
}

/**
 *  Destruct a FormulaConj.
 */
FormulaConj::~FormulaConj()
{
}

/**
 *  Construct a default FormulaConj.
 *  This exists only for the convenience of FormulaConj::AfterSubstitution.
 */
FormulaConj::FormulaConj()
{
}

/**
 *  Construct a FormulaConj from a stream containing its string representation.
 *  \param p_Stream INOUT A stream containing a string representation of this
 *   FormulaConj.  It will be advanced beyond this.
 *  \param p_TypeTable IN A table of allowable Terms and their types.  If not
 *   empty, only Terms in the table may be used, and they will be given their
 *   associated types.  Otherwise, any may be used untyped.
 *  \param p_vAllowablePredicates IN A list of allowable predicate symbols with
 *   their arities and types of parameters.  If empty, any will be allowed.
 */
void FormulaConj::ConstructorInternal( std::stringstream & p_Stream, 
				       const TypeTable & p_TypeTable,
				       const std::vector< FormulaPred > & p_vAllowablePredicates )
{
  EatWhitespace( p_Stream );
  EatString( p_Stream, "(" );
  EatWhitespace( p_Stream );

  // Special case for where a list is an implicit conjunction.
  if( p_Stream.peek() != '(' )
  {
    if( p_Stream.peek() == ')' )
    {
      EatString( p_Stream, ")" );
      return;
    }
    EatString( p_Stream, "and" );
  }

  EatWhitespace( p_Stream );

  while( p_Stream.peek() != ')' )
  {
    m_vConjuncts.push_back( FormulaP( NewFormula( p_Stream, p_TypeTable, p_vAllowablePredicates ) ) );
    EatWhitespace( p_Stream );
  }

  EatString( p_Stream, ")" );  
}

/**
 *  Retrieve the type of this FormulaConj.
 *  \return FT_CONJ.
 */
FormulaType FormulaConj::GetType() const
{
  return FT_CONJ;
}

/**
 *  Retrieve a string representation of this FormulaConj.
 *  \return A string representation of this FormulaConj.
 */
std::string FormulaConj::ToStr() const
{
  std::string l_sRet;

  if( m_vConjuncts.size() == 0 )
    return "( )";

  l_sRet += "( and ";

  for( FormulaPVecCI i = m_vConjuncts.begin();
       i != m_vConjuncts.end();
       i++ )
  {
    l_sRet += (*i)->ToStr();
    l_sRet += " ";
  }

  l_sRet += ")";
  return l_sRet;
}

/**
 *  Retrieve a string representation of this FormulaConj without typing.
 *  \return A string representation of this FormulaConj without typing.
 */
std::string FormulaConj::ToStrNoTyping() const
{
  std::string l_sRet;

  if( m_vConjuncts.size() == 0 )
    return "( )";

  l_sRet += "( and ";

  for( FormulaPVecCI i = m_vConjuncts.begin();
       i != m_vConjuncts.end();
       i++ )
  {
    l_sRet += (*i)->ToStrNoTyping();
    l_sRet += " ";
  }

  l_sRet += ")";
  return l_sRet;
}

/**
 *  Retrieve a smart pointer to a new FormulaConj that results from applying a
 *   Substitution to this one.
 *  \param p_sSubs IN The Substitution to apply.
 *  \param p_iDepth IN The current depth of this Substitution attempt.
 *  \return A smart pointer to a new FormulaConj that results from applying 
 *   the given Substitution to this.
 */
FormulaP FormulaConj::AfterSubstitution( const Substitution & p_sSubs,
					 unsigned int p_iDepth ) const
{
  FormulaConj * l_pAfter = new FormulaConj();

  for( FormulaPVecCI i = m_vConjuncts.begin();
       i != m_vConjuncts.end();
       i++ )
  {
    l_pAfter->m_vConjuncts.push_back( (*i)->AfterSubstitution( p_sSubs, p_iDepth + 1 ) );
  }

  return FormulaP( l_pAfter );
}

/**
 *  Determine whether or not this FormulaConj is ground (contains no 
 *   variables).
 *  \return Whether or not this FormulaConj is ground (contains no variables).
 */
bool FormulaConj::IsGround() const
{
  for( FormulaPVecCI i = m_vConjuncts.begin();
       i != m_vConjuncts.end();
       i++ )
  {
    if( !(*i)->IsGround() )
      return false;
  }
  return true;
}

/**
 *  Determine whether or not this FormulaConj implies some other Formula.
 *  An atomic formula is implied if one of the conjuncts implies it.  Another
 *   conjunction is implied if each of its conjuncts is implied.
 *  \param p_pOther IN A smart pointer to the Formula that might be implied.
 *  \return Whether or not this implies the other Formula.
 */
bool FormulaConj::Implies( const FormulaP & p_pOther ) const
{
  switch( p_pOther->GetType() )
  {
  case FT_PRED:
  case FT_NEG:
  case FT_EQU:
    for( FormulaPVecCI i = m_vConjuncts.begin();
	 i != m_vConjuncts.end();
	 i++ )
    {
      if( (*i)->Implies( p_pOther ) )
	return true;
    }
    return false;
  case FT_CONJ:
  {
    FormulaConjP l_pOther = std::tr1::dynamic_pointer_cast< FormulaConj >( p_pOther );
    for( FormulaPVecCI i = l_pOther->GetBeginConj();
	 i != l_pOther->GetEndConj();
	 i++ )
    {
      if( !Implies( *i ) )
	return false;
    }
    return true;
  }
  default:
    throw Exception( E_FORMULA_TYPE_UNKNOWN,
		     "Unknown formula type used.",
		     __FILE__,
		     __LINE__ );
  }
}

/**
 *  Retrieve a list of pointers to all of the TermVariables used in this 
 *   FormulaConj.
 *  \return A list of pointers to all of the TermVariables used in this
 *   FormulaConj.  They have the same lifetime as this FormulaConj unless a
 *   TermTable is in use.
 */
std::vector< TermVariableP > FormulaConj::GetVariables() const
{
  std::vector< TermVariableP > l_vRet;

  for( FormulaPVecCI i = m_vConjuncts.begin();
       i != m_vConjuncts.end();
       i++ )
  {
    std::vector< TermVariableP > l_vTemp = ( *i )->GetVariables();
    for( unsigned int j = 0; j < l_vTemp.size(); j++ )
    {
      bool l_bFound = false;
      for( unsigned int k = 0; k < l_vRet.size() && !l_bFound; k++ )
      {
	if( *l_vRet[k] == *l_vTemp[j] )
	  l_bFound = true;
      }
      if( !l_bFound )
	l_vRet.push_back( l_vTemp[j] );
    }
  }

  return l_vRet;
}

/**
 *  Retrieve a list of pointers to all of the TermConstants used in this 
 *   FormulaConj.
 *  \return A list of pointers to all of the TermConstants used in this
 *   FormulaConj.  They have the same lifetime as this FormulaConj unless a
 *   TermTable is in use.
 */
std::vector< TermConstantP > FormulaConj::GetConstants() const
{
  std::vector< TermConstantP > l_vRet;

  for( FormulaPVecCI i = m_vConjuncts.begin();
       i != m_vConjuncts.end();
       i++ )
  {
    std::vector< TermConstantP > l_vTemp = ( *i )->GetConstants();
    for( unsigned int j = 0; j < l_vTemp.size(); j++ )
    {
      bool l_bFound = false;
      for( unsigned int k = 0; k < l_vRet.size() && !l_bFound; k++ )
      {
	if( *l_vRet[k] == *l_vTemp[j] )
	  l_bFound = true;
      }
      if( !l_bFound )
	l_vRet.push_back( l_vTemp[j] );
    }
  }

  return l_vRet;
}

/**
 *  Determine whether or not this FormulaConj is equal to another Formula.
 *  \param p_Other IN The Formula to compare against.
 *  \return Whether or not this is equivalent to the other Formula.  Two 
 *   conjunctions are equivalent if neither contains a conjunct that the other
 *   does not.
 */
bool FormulaConj::Equal( const Formula & p_Other ) const
{
  if( p_Other.GetType() != FT_CONJ )
    return false;

  const FormulaConj & l_Other = dynamic_cast<const FormulaConj &>(p_Other);

  if( l_Other.m_vConjuncts.size() != m_vConjuncts.size() )
    return false;

  for( FormulaPVecCI i = m_vConjuncts.begin();
       i != m_vConjuncts.end();
       i++ )
  {
    bool l_bFound = false;
    for( FormulaPVecCI j = l_Other.m_vConjuncts.begin();
	 j != l_Other.m_vConjuncts.end();
	 j++ )
    {
      if( ( *i )->Equal( *( *j ) ) )
      {
	l_bFound = true;
      }
    }
    if( !l_bFound )
      return false;
  }

  return true;
}

/**
 *  Remove one of the conjuncts from this FormulaConj.
 *  \param p_iIndex IN The 0-based index of the conjunct to remove.
 */
/*
void FormulaConj::RemoveConj( unsigned int p_iIndex )
{
  if( p_iIndex >= m_vConjuncts.size() )
    throw Exception( E_INDEX_OUT_OF_BOUNDS,
		     "Bounds Error",
		     __FILE__,
		     __LINE__ );
  std::vector< FormulaP >::iterator l_Iter = m_vConjuncts.begin();
  for( unsigned int i = 0; i < p_iIndex; i++ )
    l_Iter++;
  m_vConjuncts.erase( l_Iter );
}
*/

/**
 *  Add a copy of a Formula as a new conjunct.
 *  \param p_pNew IN A pointer to the Formula that should be copied and added.
 */
/*
void FormulaConj::AddConjCopy( const FormulaP & p_pNew )
{
  for( unsigned int i = 0; i < m_vConjuncts.size(); i++ )
  {
    if( p_pNew->Equal( *m_vConjuncts[i] ) )
      return;
  }

  m_vConjuncts.push_back( p_pNew );
}
*/

/**
 *  Get an iterator to the first conjunct.
 *  \return An iterator to the first conjunct.
 */
FormulaPVecCI FormulaConj::GetBeginConj() const
{
  return m_vConjuncts.begin();
}

/**
 *  Get an iterator to the position beyond the last conjunct.
 *  \return An iterator to the position beyond the last conjunct.
 */
FormulaPVecCI FormulaConj::GetEndConj() const
{
  return m_vConjuncts.end();
}

unsigned int FormulaConj::GetNumConjs() const
{
  return m_vConjuncts.size();
}

size_t FormulaConj::GetMemSizeMin() const
{
  return sizeof( FormulaConj ) + m_vConjuncts.capacity() * sizeof( FormulaP );
}

size_t FormulaConj::GetMemSizeMax() const
{
  size_t l_iSize = sizeof( FormulaConj ) + m_vConjuncts.capacity() * sizeof( FormulaP );
  for( unsigned int i = 0; i < m_vConjuncts.size(); i++ )
  {
    l_iSize += m_vConjuncts[i]->GetMemSizeMax();
  }
  return l_iSize;
}
