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
#include "formula_pred.hpp"
#include "formula_equ.hpp"
#include "formula_neg.hpp"
#include "formula_conj.hpp"

/** \file formula.hpp
 *  Declaration of the Formula class and FormulaType enum.
 */

/** \file formula.cpp
 *  Definition of the Formula class.
 */

/** \enum FormulaType
 *  The category of a Formula in first-order logic.
 */

/** \var FT_PRED
 *  A Formula that takes the form of a predicate.
 */

/** \var FT_NEG
 *  A Formula that takes the form of a negation of another Formula.
 */

/** \var FT_CONJ
 *  A Formula that takes the form of a conjunction of other Formulas.
 */

/** \var FT_EQU
 *  A Formula that takes the form of an equality statement between two Terms.
 */

/** \class Formula
 *  A Formula in first-order logic.
 */

/**
 *  Empty destructor for derived classes.
 */
Formula::~Formula()
{
}

/**
 *  Determine whether or not this Formula may be a member of a State.
 *  \todo If the above description is correct, this should not include 
 *   negations or equality.  So what does it really do?
 *  \todo Rather than this logic, it should be a virtual function.
 *  \return Whether or not this Formula may be a member of a State.
 */
bool Formula::IsValidAtom() const
{
  switch( GetType() )
  {
  case FT_PRED:
    return true;
  case FT_CONJ:
    return false;
  case FT_NEG:
    if( dynamic_cast< const FormulaNeg * >( this )->GetCNegForm()->GetType() == FT_PRED ||
	dynamic_cast< const FormulaNeg * >( this )->GetCNegForm()->GetType() == FT_EQU )
      return true;
    else
      return false;
  case FT_EQU:
    return true;
  default:
    throw Exception( E_FORMULA_TYPE_UNKNOWN,
		     "Unknown Formula Type",
		     __FILE__,
		     __LINE__ );
  }
}

/**
 *  Create a new Formula from its textual representation.
 *  \param p_Stream INOUT A stringstream containing a representation of the 
 *   Formula.  The get pointer will be advanced beyond it.
 *  \param p_TypeTable IN A TypeTable from Terms to their types.  If this is
 *   non-empty, only Terms that appear in it will be allowed, and they will be
 *   instantiated with their associated types.  If it is empty, any Terms will
 *   be allowed, and they will be untyped.
 *  \param p_vAllowablePredicates IN A list of predicate symbols that may be 
 *   used in this Formula.  If non-empty only those predicates may be used,
 *   otherwise, any may be.
 *  \return A smart pointer to a Formula with that representation.
 */
FormulaP NewFormula( std::stringstream & p_Stream, 
		     const TypeTable & p_TypeTable,
		     const std::vector< FormulaPred > & p_vAllowablePredicates ) throw( MissingStringException )
{
  std::streampos l_Pos = p_Stream.tellg();

  EatString( p_Stream, "(" );
  EatWhitespace( p_Stream );
  std::string l_sWord = ReadString( p_Stream );
  p_Stream.seekg( l_Pos );

  if( !CompareNoCase( l_sWord, "not" ) )
    return FormulaP( new FormulaNeg( p_Stream, p_TypeTable, p_vAllowablePredicates ) );
  else if( !CompareNoCase( l_sWord, "and" ) )
    return FormulaP( new FormulaConj( p_Stream, p_TypeTable, p_vAllowablePredicates ) );
  else if( !CompareNoCase( l_sWord, "=" ) )
    return FormulaP( new FormulaEqu( p_Stream, p_TypeTable ) );
  else if( l_sWord.length() == 0 )
    throw MissingStringException( "Expected non-zero length predicate.",
				  __FILE__,
				  __LINE__,
				  "any predicate symbol" );
  else
    return FormulaP( new FormulaPred( p_Stream, p_TypeTable, p_vAllowablePredicates ) );
}

/**
 *  Determine whether or not two Formulas are equal.
 *  \param p_First IN The first Formula to compare.
 *  \param p_Second IN The second Formula to compare.
 *  \return Whether or not p_First and p_Second are equal.
 */
bool operator==( const Formula & p_First, const Formula & p_Second )
{
  return p_First.Equal( p_Second );
}

/*
FormulaPLess g_FormulaPLess;

bool FormulaPLess::operator()( const FormulaP & p_pForm1,
			       const FormulaP & p_pForm2 ) const
{
  switch( p_pForm1->GetType() )
  {
  case FT_PRED:
    {
      switch( p_pForm2->GetType() )
      {
      case FT_PRED:
	{
	  FormulaPredP l_pFirst = std::tr1::dynamic_pointer_cast< FormulaPred >( p_pForm1 );
	  FormulaPredP l_pSecond = std::tr1::dynamic_pointer_cast< FormulaPred >( p_pForm2 );
	  int l_iResult = l_pSecond->GetRelationIndex() - l_pFirst->GetRelationIndex();
	  if( l_iResult < 0 )
	    return true;
	  if( l_iResult > 0 )
	    return false;
	  l_iResult = l_pSecond->GetValence() - l_pFirst->GetValence();
	  if( l_iResult < 0 )
	    return true;
	  if( l_iResult > 0 )
	    return false;
	  for( unsigned int i = 0; i < l_pFirst->GetValence(); i++ )
	  {
	    if( l_pFirst->GetCParam( i ) < l_pSecond->GetCParam( i ) )
	      return true;
	    if( l_pSecond->GetCParam( i ) < l_pFirst->GetCParam( i ) )
	      return false;
	  }
	  return false;
	}
      case FT_EQU:
	return true;
      case FT_NEG:
	return true;
      case FT_CONJ:
	return true;
      default:
	throw Exception( E_FORMULA_TYPE_UNKNOWN,
			 "Unknown formula type.",
			 __FILE__,
			 __LINE__ );
      }
    }
  case FT_EQU:
    {
      switch( p_pForm2->GetType() )
      {
      case FT_PRED:
	return false;
      case FT_EQU:
	{
	  FormulaEquP l_pFirst = std::tr1::dynamic_pointer_cast< FormulaEqu >( p_pForm1 );
	  FormulaEquP l_pSecond = std::tr1::dynamic_pointer_cast< FormulaEqu >( p_pForm2 );
	  if( l_pFirst->GetCFirst() < l_pSecond->GetCFirst() )
	    return true;
	  if( l_pSecond->GetCFirst() < l_pFirst->GetCFirst() )
	    return false;
	  if( l_pFirst->GetCSecond() < l_pSecond->GetCSecond() )
	    return true;
	  if( l_pSecond->GetCSecond() < l_pFirst->GetCSecond() )
	    return false;
	  return false;
	}
      case FT_NEG:
	return true;
      case FT_CONJ:
	return true;
      default:
	throw Exception( E_FORMULA_TYPE_UNKNOWN,
			 "Unknown formula type.",
			 __FILE__,
			 __LINE__ );
      }
    }
  case FT_NEG:
    {
      switch( p_pForm2->GetType() )
      {
      case FT_PRED:
	return false;
      case FT_EQU:
	return false;
      case FT_NEG:
	{
	  FormulaNegP l_pFirst = std::tr1::dynamic_pointer_cast< FormulaNeg >( p_pForm1 );
	  FormulaNegP l_pSecond = std::tr1::dynamic_pointer_cast< FormulaNeg >( p_pForm2 );
	  return g_FormulaPLess( l_pFirst->GetCNegForm(), l_pSecond->GetCNegForm() );
	}
      case FT_CONJ:
	return true;
      default:
	throw Exception( E_FORMULA_TYPE_UNKNOWN,
			 "Unknown formula type.",
			 __FILE__,
			 __LINE__ );
      }
    }
  case FT_CONJ:
    {
      switch( p_pForm2->GetType() )
      {
      case FT_PRED:
	return false;
      case FT_EQU:
	return false;
      case FT_NEG:
	return false;
      case FT_CONJ:
	{
	  FormulaConjP l_pFirst = std::tr1::dynamic_pointer_cast< FormulaConj >( p_pForm1 );
	  FormulaConjP l_pSecond = std::tr1::dynamic_pointer_cast< FormulaConj >( p_pForm2 );

	  FormulaPSetCI i = l_pFirst->GetBeginConj();
	  FormulaPSetCI j = l_pSecond->GetBeginConj();
	  for( ;
	       i != l_pFirst->GetEndConj() && j != l_pSecond->GetEndConj();
	     )
	  {
	    if( g_FormulaPLess( *i, *j ) )
	      return true;
	    if( g_FormulaPLess( *j, *i ) )
	      return false;
	    i++;
	    j++;
	  }
	  return false;
	}
      default:
	throw Exception( E_FORMULA_TYPE_UNKNOWN,
			 "Unknown formula type.",
			 __FILE__,
			 __LINE__ );
      }
    }
  default:
    throw Exception( E_FORMULA_TYPE_UNKNOWN,
		     "Unknown formula type.",
		     __FILE__,
		     __LINE__ );
  }
  throw Exception( E_NOT_IMPLEMENTED,
		   "It should not be possible to get here.",
		   __FILE__,
		   __LINE__ );
}
*/
