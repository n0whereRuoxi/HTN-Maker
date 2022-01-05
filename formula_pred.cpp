#include <vector>
#include <string>
#include <sstream>
#include <cassert>
#include <tr1/memory>

#include "exception.hpp"
#include "funcs.hpp"
#include "string_table.hpp"
#include "term.hpp"
#include "term_string.hpp"
#include "term_variable.hpp"
#include "term_constant.hpp"
#include "term_table.hpp"
#include "type_table.hpp"
#include "formula.hpp"
#include "formula_pred.hpp"
#include "formula_equ.hpp"
#include "formula_neg.hpp"
#include "formula_conj.hpp"

/** \file formula_pred.hpp
 *  Declaration of the FormulaPred class.
 */

/** \file formula_pred.cpp
 *  Definition of the FormulaPred class.
 */

/** \class FormulaPred
 *  A predicate in predicate logic, which is a type of Formula.
 *  It consists of a predicate symbol, also called the relation, and zero or
 *   more parameters.
 */

/** \var FormulaPred::m_vParams
 *  An ordered list of parameters, which are pointers to Terms.
 *  If using a TermTable, it is instead a list of constant pointers into that
 *   table.
 */

/** \var FormulaPred::m_sRelation
 *  The relation, or predicate symbol of this predicate as a string.
 */

/** \var FormulaPred::m_iRelation
 *  The index of the relation (predicate symbol) of this predicate in a global
 *   StringTable.
 */

/** \class HashPredicate
 *  A functor to hash FormulaPreds.
 *  Will be used if a PredicateTable is ever implemented.
 */

/** \class HashPredicatePointer
 *  A functor to hash pointers to FormulaPreds.
 *  Will be used if a PredicateTable is ever implemented.
 */

/** \class EqualPredicate
 *  A functor to determine whether to FormulaPreds are equal.
 *  Will be used if a PredicateTable is ever implemented.
 */

/** \class EqualPredicatePointer
 *  A functor to determine whether two pointers to FormulaPreds point to equal
 *   ones.
 *  Will be used if a PredicateTable is ever implemented.
 */

/**
 *  The one and only StringTable, defined in string_table.cpp.
 */
extern StringTable g_StrTable;

/**
 *  The one and only TermTable, defined in term_table.cpp.
 */
extern TermTable g_TermTable;

/**
 *  The one and only string hasher, defined in funcs.cpp.
 */
extern HashStr g_StrHasher;

/**
 *  The one and only predicate hasher.
 */
HashPredicate g_PredicateHasher;
/**
 *  The one and only predicate pointer hasher.
 */
HashPredicatePointer g_PredicatePointerHasher;
/**
 *  The one and only predicate equal checker.
 */
EqualPredicate g_PredicateEqualizer;
/**
 *  The one and only predicate pointer equal checker.
 */
EqualPredicatePointer g_PredicatePointerEqualizer;

/**
 *  Construct a FormulaPred from a stream with a string representation, 
 *   requiring that it satisfy some constraints.
 *  \param p_Stream INOUT A stream object containing a string representation
 *   of the predicate.  The stream is advanced beyond it.
 *  \param p_TypeTable IN A table of the allowable Terms in this predicate with
 *   their types.  If this is empty, and Term may be used.
 *  \param p_vAllowablePredicates IN A list of predicate symbols with arities
 *   that should be allowed.  If empty, any may be used.
 */
FormulaPred::FormulaPred( std::stringstream & p_Stream, 
			  const TypeTable & p_TypeTable,
			  const std::vector< FormulaPred > & p_vAllowablePredicates )
{
  ConstructorInternal( p_Stream, p_TypeTable, p_vAllowablePredicates );
}

/**
 *  Construct a copy of an existing predicate.
 *  This should be a copy of every member.
 *  \param p_Other IN The predicate to make a copy of.
 */
FormulaPred::FormulaPred( const FormulaPred & p_Other )
{
  m_iRelation = p_Other.m_iRelation;

  for( unsigned int i = 0; i < p_Other.m_vParams.size(); i++ )
    m_vParams.push_back( p_Other.m_vParams[i] );
}

/**
 *  Construct a FormulaPred from a string representation, requiring that it 
 *   satisfy some constraints.
 *  \param p_sString IN A string representation of the predicate.
 *  \param p_TypeTable IN A table of the allowable Terms in this predicate with
 *   their types.  If this is empty, and Term may be used.
 *  \param p_vAllowablePredicates IN A list of predicate symbols with arities
 *   that should be allowed.  If empty, any may be used.
 */
FormulaPred::FormulaPred( std::string p_sString, 
			  const TypeTable & p_TypeTable,
			  const std::vector< FormulaPred > & p_vAllowablePredicates )
{
  std::stringstream l_Stream( p_sString );
  ConstructorInternal( l_Stream, p_TypeTable, p_vAllowablePredicates );
}

/**
 *  Destruct a predicate.
 *  If there is no TermTable, each of the parameters must be deallocated.
 */
FormulaPred::~FormulaPred()
{
}

/**
 *  Construct a default predicate.
 *  This only exists for the convenience of functions that need to create a 
 *   predicate by modifying an existing one.
 */
FormulaPred::FormulaPred()
{
}

/**
 *  Construct a FormulaPred from a stream with a string representation, 
 *   requiring that it satisfy some constraints.
 *  \param p_Stream INOUT A stream object containing a string representation
 *   of the predicate.  The stream is advanced beyond it.
 *  \param p_TypeTable IN A table of the allowable Terms in this predicate with
 *   their types.  If this is empty, and Term may be used.
 *  \param p_vAllowablePredicates IN A list of predicate symbols with arities
 *   that should be allowed.  If empty, any may be used.
 */
void FormulaPred::ConstructorInternal( std::stringstream & p_Stream, 
				       const TypeTable & p_TypeTable,
				       const std::vector< FormulaPred > & p_vAllowablePredicates )
{
  EatWhitespace( p_Stream );
  EatString( p_Stream, "(" );
  EatWhitespace( p_Stream );

  std::string l_sRelation = ReadString( p_Stream );

  m_iRelation = g_StrTable.Lookup( l_sRelation );

  EatWhitespace( p_Stream );
  
  while( p_Stream.peek() != ')' )
  {
    std::string l_sTerm = ReadString( p_Stream );
    m_vParams.push_back( ReadTerm( l_sTerm, p_TypeTable ) );
    EatWhitespace( p_Stream );    
  }

  EatString( p_Stream, ")" );

  if( p_vAllowablePredicates.size() > 0 )
  {
    bool l_bFound = false;
    for( unsigned int i = 0; i < p_vAllowablePredicates.size() && !l_bFound; i++ )
    {
      if( CompareNoCase( p_vAllowablePredicates[i].GetRelation(),
			 l_sRelation ) == 0 )
      {
	l_bFound = true;
	if( p_vAllowablePredicates[i].GetValence() != 
	    m_vParams.size() )
	  throw Exception( E_NOT_IMPLEMENTED,
			   "Predicate " + ToStr() + " does not have the same valence as declared predicate " + p_vAllowablePredicates[i].ToStr() + ".",
			   __FILE__,
			   __LINE__ );
	for( unsigned int j = 0; j < p_vAllowablePredicates[i].GetValence(); j++ )
	{
	  if( p_vAllowablePredicates[i].GetCParam( j )->HasTyping() )
	  {
	    if( !m_vParams[j]->HasTyping() )
	      throw Exception( E_NOT_IMPLEMENTED,
			       "Predicate " + ToStr() + " contains untyped variables, while the declared predicate " + p_vAllowablePredicates[i].ToStr() + " does not.",
			       __FILE__,
			       __LINE__ );
	    if( p_vAllowablePredicates[i].GetCParam( j )->GetTypingStrTableIndex() != m_vParams[j]->GetTypingStrTableIndex() )
	      throw Exception( E_NOT_IMPLEMENTED,
			       "Predicate " + ToStr() + " does not have the same typing as in declared predicate " + p_vAllowablePredicates[i].ToStr() + ".",
			       __FILE__,
			       __LINE__ );
	  }
	  else if( m_vParams[j]->HasTyping() )
	  {
	    throw Exception( E_NOT_IMPLEMENTED,
			     "Predicate " + ToStr() + " contains typed variables, while the declared predicate " + p_vAllowablePredicates[i].ToStr() + " does not.",
			     __FILE__,
			     __LINE__ );
	  }
	}
      }
    }
    if( !l_bFound )
    {
      throw Exception( E_NOT_IMPLEMENTED,
		       "Predicate " + ToStr() + " does not match any declared formula.",
		       __FILE__,
		       __LINE__ );
    }
  }
}

/**
 *  Retrieve the type of this FormulaPred.
 *  \return FT_PRED.
 */
FormulaType FormulaPred::GetType() const
{
  return FT_PRED;
}

/**
 *  Retrieve a string representation of this FormulaPred.
 *  \return A string representation of this FormulaPred.
 */
std::string FormulaPred::ToStr() const
{
  std::string l_sRet;
  l_sRet += "( ";

  l_sRet += g_StrTable.Lookup( m_iRelation );
  
  for( unsigned int i = 0; i < m_vParams.size(); i++ )
  {
    l_sRet += " ";
    l_sRet += m_vParams[i]->ToStr();
  }

  l_sRet += " )";
  return l_sRet;
}

/**
 *  Retrieve a string representation of this FormulaPred without typing.
 *  \return A string representation of this FormulaPred without typing.
 */
std::string FormulaPred::ToStrNoTyping() const
{
  std::string l_sRet;
  l_sRet += "( ";

  l_sRet += g_StrTable.Lookup( m_iRelation );
  
  for( unsigned int i = 0; i < m_vParams.size(); i++ )
  {
    l_sRet += " ";
    l_sRet += m_vParams[i]->ToStrNoTyping();
  }

  l_sRet += " )";
  return l_sRet;
}

/**
 *  Retrieve the relation (predicate symbol) of this FormulaPred.
 *  \return The relation (predicate symbol) of this FormulaPred.
 */
std::string FormulaPred::GetRelation() const
{
  return g_StrTable.Lookup( m_iRelation );
}

/**
 *  Retrieve a pointer to one of the paramters of this predicate.
 *  \param p_iIndex IN The 0-based index of the paramter.
 *  \return A pointer to the parameter.  This will only be valid as long as
 *   the predicate is, unless a TermTable is in use.
 */
TermP FormulaPred::GetCParam( unsigned int p_iIndex ) const
{
  if( p_iIndex < m_vParams.size() )
    return m_vParams[ p_iIndex ];

  throw Exception( E_INDEX_OUT_OF_BOUNDS,
		   "Bounds error.",
		   __FILE__,
		   __LINE__ );
}

/**
 *  Retrieve a new smart pointer to a new predicate that results from applying
 *   a Substitution to this.
 *  \param p_sSubs IN The Substitution to apply.
 *  \param p_iDepth IN The current depth of this Substitution.
 *  \return A smart pointer to a new predicate that results from applying the 
 *   given Substitution to this.
 */
FormulaP FormulaPred::AfterSubstitution( const Substitution & p_sSubs,
					 unsigned int p_iDepth ) const
{
  FormulaPred * l_pAfter = new FormulaPred();

  l_pAfter->m_iRelation = m_iRelation;

  for( unsigned int i = 0; i < m_vParams.size(); i++ )
  {
    l_pAfter->m_vParams.push_back( m_vParams[i]->AfterSubstitution( p_sSubs, p_iDepth + 1 ) );
  }

  return FormulaP( l_pAfter );
}

/**
 *  Determine whether or not this predicate is ground (contains no variables).
 *  \return Whether or not this predicate is ground.
 */
bool FormulaPred::IsGround() const
{
  for( unsigned int i = 0; i < m_vParams.size(); i++ )
  {
    if( m_vParams[i]->GetType() == TT_VARIABLE )
      return false;
  }
  return true;
}

/**
 *  Determine whether or not this predicate implies some other Formula.
 *  The only things that a predicate may imply is a copy of itself or a 
 *   conjunction of copies of itself.
 *  \param p_pOther IN A smart pointer to the Formula that might be implied.
 *  \return Whether or not this predicate implies the other Formula.
 */
bool FormulaPred::Implies( const FormulaP & p_pOther ) const
{
  switch( p_pOther->GetType() )
  {
  case FT_PRED:
    return Equal( *p_pOther );
  case FT_NEG:
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
  case FT_EQU:
    return false;
  default:
    throw Exception( E_FORMULA_TYPE_UNKNOWN,
		     "Unknown formula type used.",
		     __FILE__,
		     __LINE__ );
  }
}

/**
 *  Retrieve a list of pointers to all variables used in this predicate.
 *  \return A list of pointers to all variables used in this predicate.  Those
 *   pointers will only be valid as long as this predicate exists, unless a
 *   TermTable is in use.
 */
std::vector< TermVariableP > FormulaPred::GetVariables() const
{
  std::vector< TermVariableP > l_vRet;

  for( unsigned int i = 0; i < GetValence(); i++ )
  {
    if( m_vParams[i]->GetType() == TT_VARIABLE )
    {
      bool l_bFound = false;
      for( unsigned int j = 0; j < l_vRet.size() && !l_bFound; j++ )
      {
	if( *l_vRet[j] == *m_vParams[i] )
	  l_bFound = true;
      }
      if( !l_bFound )
	l_vRet.push_back( std::tr1::dynamic_pointer_cast< TermVariable >( m_vParams[i] ) );
    }
  }

  return l_vRet;
}

/**
 *  Retrieve a list of pointers to all constants used in this predicate.
 *  \return A list of pointers to all constants used in this predicate.  Those
 *   pointers will only be valid as long as this predicate exists, unless a
 *   TermTable is in use.
 */
std::vector< TermConstantP > FormulaPred::GetConstants() const
{
  std::vector< TermConstantP > l_vRet;

  for( unsigned int i = 0; i < GetValence(); i++ )
  {
    if( m_vParams[i]->GetType() == TT_CONSTANT )
    {
      bool l_bFound = false;
      for( unsigned int j = 0; j < l_vRet.size() && !l_bFound; j++ )
      {
	if( *l_vRet[j] == *m_vParams[i] )
	  l_bFound = true;
      }
      if( !l_bFound )
	l_vRet.push_back( std::tr1::dynamic_pointer_cast< TermConstant >( m_vParams[i] ) );
    }
  }

  return l_vRet;
}

/**
 *  Determine whether this predicate is equal to some other Formula.
 *  \param p_Other IN The other Formula to compare against.
 *  \return Whether or not this is equal to the other.
 */
bool FormulaPred::Equal( const Formula & p_Other ) const
{
  //  if( p_Other.GetType() != FT_PRED )
  if( typeid( p_Other ) == typeid( FormulaNeg ) ||
      typeid( p_Other ) == typeid( FormulaConj ) ||
      typeid( p_Other ) == typeid( FormulaEqu ) )
    return false;

  const FormulaPred & l_Other = dynamic_cast<const FormulaPred &>(p_Other);

  if( GetRelationIndex() != l_Other.GetRelationIndex() )
    return false;

  if( GetValence() != l_Other.GetValence() )
    return false;

  for( unsigned int i = 0; i < GetValence(); i++ )
  {
    if( !m_vParams[i]->Equal( *l_Other.m_vParams[i] ) )
      return false;
  }

  return true;
}

/**
 *  Retrieve the number of arguments to this predicate.
 *  \return The number of arguments to this predicate.
 */
unsigned int FormulaPred::GetValence() const
{
  return m_vParams.size();
}

/**
 *  Hash a FormulaPred.
 *  \param x IN The FormulaPred to hash.
 *  \return A hash value for x.
 */
size_t HashPredicate::operator() ( const FormulaPred & x ) const
{
  size_t l_iHash = 0;

  l_iHash += x.GetRelationIndex();

  for( unsigned int i = 0; i < x.GetValence(); i++ )
  {
    l_iHash = l_iHash << 1;

    l_iHash += (size_t)x.GetCParam( i ).operator->(); // The term is always at this address
  }

  return l_iHash;
}

/**
 *  Hash a FormulaPred from a pointer to it.
 *  \param x IN A pointer to the FormulaPred to hash.
 *  \return A hash value for the FormulaPred pointed to.
 */
size_t HashPredicatePointer::operator() ( const FormulaPred * x ) const
{
  return g_PredicateHasher( *x );
}

/**
 *  Determine whether or not two FormulaPreds are equal.
 *  \param x IN The first FormulaPred.
 *  \param y IN The second FormulaPred.
 *  \return Whether or not x and y are equal.
 */
bool EqualPredicate::operator() ( const FormulaPred & x, const FormulaPred & y ) const
{
  return x.Equal( y );
}

/**
 *  Determine whether or not the FormulaPreds pointed to by two pointers are 
 *   equal.
 *  \param x IN A pointer to the first FormulaPred.
 *  \param y IN A pointer to the second FormulaPred.
 *  \return Whether or not *x and *y are equal.
 */
bool EqualPredicatePointer::operator() ( const FormulaPred * x, const FormulaPred * y ) const
{
  return x->Equal( *y );
}

/**
 *  Determine whether or not the relations (predicate symbols) from two 
 *   predicates are the same.
 *  \param p_First IN The first FormulaPred to compare.
 *  \param p_Second IN THe second FormulaPred to compare.
 *  \return Whether or not the relations are equal.
 */
bool PredRelationsAreEqual( const FormulaPred & p_First,
			    const FormulaPred & p_Second )
{
  return p_First.GetRelationIndex() == p_Second.GetRelationIndex();
}

size_t FormulaPred::GetMemSizeMin() const
{
  return sizeof( FormulaPred ) + m_vParams.capacity() * sizeof( TermP );
}

size_t FormulaPred::GetMemSizeMax() const
{
  size_t l_iSize = sizeof( FormulaPred ) + m_vParams.capacity() * sizeof( TermP );
  for( unsigned int i = 0; i < m_vParams.size(); i++ )
  {
    l_iSize += m_vParams[i]->GetMemSizeMax();
  }
  return l_iSize;
}
