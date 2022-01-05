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
#include "formula_conj.hpp"
#include "formula_neg.hpp"

/** \file formula_neg.hpp
 *  Declaration of FormulaNeg class.
 */

/** \file formula_neg.cpp
 *  Definition of FormulaNeg class.
 */

/** \class FormulaNeg
 *  A negation of a Formula in first-order logic.
 *  For the sake of efficiency, we require that all formulas be in a certain
 *   form, in which the only formulas that may be negated are FormulaPreds and
 *   FormulaEqus.
 */

/** \var FormulaNeg::m_pNegForm
 *  A smart pointer to the Formula that this is a negation of.
 */

/**
 *  Construct a FormulaNeg from a stream containing its string representation,
 *   a table of allowable Terms with their types, and a list of allowable
 *   predicate symbols.
 *  \param p_Stream INOUT A stream containing a string representation of the
 *   FormulaNeg.  It will be advanced beyond this.
 *  \param p_TypeTable IN A table of allowable Terms and their types.  If 
 *   non-empty, only Terms that appear in the table will be allowed in this and
 *   sub-Formulas, and they will be given the type associated with them.  If it
 *   is empty, any Terms may be used and are given no types.
 *  \param p_vAllowablePredicates IN A list of predicate symbols and arities 
 *   that may be used in this and subformulas.  If empty, any may be used.
 */
FormulaNeg::FormulaNeg( std::stringstream & p_Stream, 
			const TypeTable & p_TypeTable,
			const std::vector< FormulaPred > & p_vAllowablePredicates )
{
  ConstructorInternal( p_Stream, p_TypeTable, p_vAllowablePredicates );
}

/**
 *  Construct a FormulaNeg as a copy of an existing one.
 *  \param p_Other IN The FormulaNeg to make a copy of.
 */
FormulaNeg::FormulaNeg( const FormulaNeg & p_Other )
  : m_pNegForm( p_Other.m_pNegForm )
{
}

/**
 *  Construct a FormulaNeg from its string representation, a table of 
 *   allowable Terms with their types, and a list of allowable predicate 
 *   symbols.
 *  \param p_sString IN A  string representation of the FormulaNeg.
 *  \param p_TypeTable IN A table of allowable Terms and their types.  If 
 *   non-empty, only Terms that appear in the table will be allowed in this and
 *   sub-Formulas, and they will be given the type associated with them.  If it
 *   is empty, any Terms may be used and are given no types.
 *  \param p_vAllowablePredicates IN A list of predicate symbols and arities 
 *   that may be used in this and subformulas.  If empty, any may be used.
 */
FormulaNeg::FormulaNeg( std::string p_sString,
			const TypeTable & p_TypeTable,
			const std::vector< FormulaPred > & p_vAllowablePredicates )
{
  std::stringstream l_Stream( p_sString );
  ConstructorInternal( l_Stream, p_TypeTable, p_vAllowablePredicates );
}

/**
 *  Destruct a FormulaNeg.
 */
FormulaNeg::~FormulaNeg()
{
}

/**
 *  Construct a default FormulaNeg.
 *  This exists only for the convenience of AfterSubstitution.
 */
FormulaNeg::FormulaNeg()
{
}

/**
 *  Construct a FormulaNeg from a stream containing its string representation,
 *   a table of allowable Terms with their types, and a list of allowable
 *   predicate symbols.
 *  \param p_Stream INOUT A stream containing a string representation of the
 *   FormulaNeg.  It will be advanced beyond this.
 *  \param p_TypeTable IN A table of allowable Terms and their types.  If 
 *   non-empty, only Terms that appear in the table will be allowed in this and
 *   sub-Formulas, and they will be given the type associated with them.  If it
 *   is empty, any Terms may be used and are given no types.
 *  \param p_vAllowablePredicates IN A list of predicate symbols and arities 
 *   that may be used in this and subformulas.  If empty, any may be used.
 */
void FormulaNeg::ConstructorInternal( std::stringstream & p_Stream, 
				      const TypeTable & p_TypeTable,
				      const std::vector< FormulaPred > & p_vAllowablePredicates )
{
  EatWhitespace( p_Stream );
  EatString( p_Stream, "(" );
  EatWhitespace( p_Stream );
  EatString( p_Stream, "not" );
  EatWhitespace( p_Stream );

  m_pNegForm = FormulaP( NewFormula( p_Stream, p_TypeTable, p_vAllowablePredicates ) );

  EatWhitespace( p_Stream );
  EatString( p_Stream, ")" );

  if( m_pNegForm->GetType() != FT_PRED && m_pNegForm->GetType() != FT_EQU )
    throw Exception( E_NEG_NOT_PRED,
		     "For now, only predicates and equalities may be negated.",
		     __FILE__,
		     __LINE__ );
}

/**
 *  Retrieve the type of this FormulaNeg.
 *  \return FT_NEG.
 */
FormulaType FormulaNeg::GetType() const
{
  return FT_NEG;
}

/**
 *  Retrieve a string representation of this FormulaNeg.
 *  \return A string representation of this FormulaNeg.
 */
std::string FormulaNeg::ToStr() const
{
  std::string l_sRet;
  l_sRet += "( not ";
  l_sRet += m_pNegForm->ToStr();
  l_sRet += " )";
  return l_sRet;
}

/**
 *  Retrieve a string representation of this FormulaNeg, without typing.
 *  \return A string representation of this FormulaNeg, without typing.
 */
std::string FormulaNeg::ToStrNoTyping() const
{
  std::string l_sRet;
  l_sRet += "( not ";
  l_sRet += m_pNegForm->ToStrNoTyping();
  l_sRet += " )";
  return l_sRet;
}

/**
 *  Retrieve a smart pointer to the Formula that this is a negation of.
 *  \return A smart pointer to the Formula that this is a negation of.
 */
FormulaP FormulaNeg::GetCNegForm() const
{
  return m_pNegForm;
}

/**
 *  Retrieve a smart pointer to a new Formula that results from applying a 
 *   Substitution to this FormulaNeg.
 *  \param p_sSubs IN The Substitution to apply.
 *  \param p_iDepth IN The current depth of this Substitution attempt.
 *  \return A smart pointer to a new Formula that results from applying the 
 *   Substitution to this.
 */
FormulaP FormulaNeg::AfterSubstitution( const Substitution & p_sSubs,
					unsigned int p_iDepth ) const
{
  FormulaNeg * l_pAfter = new FormulaNeg();
  l_pAfter->m_pNegForm = m_pNegForm->AfterSubstitution( p_sSubs, p_iDepth + 1 );
  return FormulaP( l_pAfter );
}

/**
 *  Determine whether or not this FormulaNeg is ground (contains no variables).
 *  \return Whether or not this FormulaNeg is ground (contains no variables).
 */
bool FormulaNeg::IsGround() const
{
  return m_pNegForm->IsGround();
}

/**
 *  Determine whether or not this FormulaNeg imples some other Formula.
 *  The only things a FormulaNeg may imply are a copy of itself or a 
 *   conjunction of copies of itself.
 *  \param p_pOther IN A smart pointer to the Formula that might be implied.
 *  \return Whether or not this implies p_pOther.
 */
bool FormulaNeg::Implies( const FormulaP & p_pOther ) const
{
  switch( p_pOther->GetType() )
  {
  case FT_PRED:
    return false;
  case FT_EQU:
    return false;
  case FT_NEG:
    return Equal( *p_pOther );
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
		     "Unknown formula type.",
		     __FILE__,
		     __LINE__ );
  }
}

/**
 *  Retrieve a list of TermVariables found in this FormulaNeg.
 *  \return A list of pointers to TermVariables found in this FormulaNeg.  
 *   These have the same lifetime as this FormulaNeg, unless a TermTable is in
 *   use.
 */
std::vector< TermVariableP > FormulaNeg::GetVariables() const
{
  return m_pNegForm->GetVariables();
}

/**
 *  Retrieve a list of TermConstants found in this FormulaNeg.
 *  \return A list of pointers to TermConstants found in this FormulaNeg.  
 *   These have the same lifetime as this FormulaNeg, unless a TermTable is in
 *   use.
 */
std::vector< TermConstantP > FormulaNeg::GetConstants() const
{
  return m_pNegForm->GetConstants();
}

/**
 *  Determine whether or not this FormulaNeg is equal to some other Formula.
 *  \param p_Other IN The Formula to compare against.
 *  \return Whether or not the two Formulas are equivalent.
 */
bool FormulaNeg::Equal( const Formula & p_Other ) const
{
  if( p_Other.GetType() != FT_NEG )
    return false;

  const FormulaNeg & l_Other = dynamic_cast<const FormulaNeg &>(p_Other);

  return GetCNegForm()->Equal( *l_Other.m_pNegForm );
}

size_t FormulaNeg::GetMemSizeMin() const
{
  return sizeof( FormulaNeg );
}

size_t FormulaNeg::GetMemSizeMax() const
{
  return sizeof( FormulaNeg ) + m_pNegForm->GetMemSizeMax();
}
