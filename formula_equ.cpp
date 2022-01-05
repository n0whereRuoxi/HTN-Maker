#include <string>
#include <sstream>
#include <tr1/memory>

#include "exception.hpp"
#include "funcs.hpp"
#include "term.hpp"
#include "term_string.hpp"
#include "term_variable.hpp"
#include "term_constant.hpp"
#include "term_table.hpp"
#include "type_table.hpp"
#include "formula.hpp"
#include "formula_conj.hpp"
#include "formula_equ.hpp"

/** \file formula_equ.hpp
 *  Declaration of the FormulaEqu class.
 */

/** \file formula_equ.cpp
 *  Definition of the FormulaEqu class.
 */

/** \class FormulaEqu
 *  An equality formula in first-order logic.
 *  Technically, this is a specific predicate with predicate symbol "=" and
 *   arity 2.  It gets its own class because this is a special predicate that
 *   should never be explicitly stated in a state.  Rather, the planner will 
 *   evaluate it as true when both arguments are the same constant and false
 *   when they are different constants.
 *  These may only be used with the PDDL requirement PDDL_REQ_EQUALITY.
 */

/** \var FormulaEqu::m_pFirstTerm
 *  A pointer to the first argument of the equality.
 *  If no TermTable is used, this must be deallocated.
 */

/** \var FormulaEqu::m_pSecondTerm
 *  A pointer to the second argument of the equality.
 *  If no TermTable is used, this must be deallocated.
 */

/**
 *  The one and only TermTable, defined in term_table.cpp.
 */
extern TermTable g_TermTable;

/**
 *  Construct an equality formula from a stream containing its string
 *   representation and a table of types of possible arguments.
 *  \param p_Stream INOUT A stream containing a string representation of this
 *   equality formula.  The stream is advanced beyond it.
 *  \param p_TypeTable IN A table of allowable terms with their associated 
 *   types.  If empty, and terms will be allowed and they must be untyped.
 */
FormulaEqu::FormulaEqu( std::stringstream & p_Stream,
			const TypeTable & p_TypeTable )
{
  ConstructorInternal( p_Stream, p_TypeTable );
}

/**
 *  Construct an equality formula from a string representation and a table of
 *   types of possible arguments.
 *  \param p_sString IN S string representation of this equality formula.
 *  \param p_TypeTable IN A table of allowable terms with their associated 
 *   types.  If empty, and terms will be allowed and they must be untyped.
 */
FormulaEqu::FormulaEqu( std::string p_sString,
			const TypeTable & p_TypeTable )
{
  std::stringstream l_Stream( p_sString );
  ConstructorInternal( l_Stream, p_TypeTable );
}

/**
 *  Construct an equality formula as a copy of an existing one.
 *  \param p_Other IN The FormulaEqu to copy.
 */
FormulaEqu::FormulaEqu( const FormulaEqu & p_Other )
{
  m_pFirstTerm = p_Other.GetCFirst();
  m_pSecondTerm = p_Other.GetCSecond();
}

/**
 *  Construct a default equality formula.
 *  This exists only as a convenience for FormulaEqu::AfterSubstitution.
 */
FormulaEqu::FormulaEqu()
  : m_pFirstTerm(),
    m_pSecondTerm()
{
}

/**
 *  Destruct an equality formula.
 *  If no TermTable is in use, both terms must be deallocated.
 */
FormulaEqu::~FormulaEqu()
{
}

/**
 *  Construct an equality formula from a stream containing its string
 *   representation and a table of types of possible arguments.
 *  \param p_Stream INOUT A stream containing a string representation of this
 *   equality formula.  The stream is advanced beyond it.
 *  \param p_TypeTable IN A table of allowable terms with their associated 
 *   types.  If empty, and terms will be allowed and they must be untyped.
 */
void FormulaEqu::ConstructorInternal( std::stringstream & p_Stream,
				      const TypeTable & p_TypeTable )
{
  EatWhitespace( p_Stream );
  EatString( p_Stream, "(" );
  EatWhitespace( p_Stream );
  EatString( p_Stream, "=" );
  EatWhitespace( p_Stream );

  std::string l_sFirst = ReadString( p_Stream );
  EatWhitespace( p_Stream );
  std::string l_sSecond = ReadString( p_Stream );
  EatWhitespace( p_Stream );
  EatString( p_Stream, ")" );
  EatWhitespace( p_Stream );

  m_pFirstTerm = ReadTerm( l_sFirst, p_TypeTable );
  m_pSecondTerm = ReadTerm( l_sSecond, p_TypeTable );
}

/**
 *  Retrieve the type of this FormulaEqu.
 *  \return FT_EQU.
 */
FormulaType FormulaEqu::GetType() const
{
  return FT_EQU;
}

/**
 *  Retrieve a string representation of this FormulaEqu.
 *  \return A string representation of this FormulaEqu.
 */
std::string FormulaEqu::ToStr() const
{
  std::string l_sRet;
  l_sRet += "( = ";
  l_sRet += m_pFirstTerm->ToStr();
  l_sRet += " ";
  l_sRet += m_pSecondTerm->ToStr();
  l_sRet += " )";
  return l_sRet;
}

/**
 *  Retrieve a string representation of this FormulaEqu, without typing.
 *  \return A string representation of this FormulaEqu, without typing.
 */
std::string FormulaEqu::ToStrNoTyping() const
{
  std::string l_sRet;
  l_sRet += "( = ";
  l_sRet += m_pFirstTerm->ToStrNoTyping();
  l_sRet += " ";
  l_sRet += m_pSecondTerm->ToStrNoTyping();
  l_sRet += " )";
  return l_sRet;
}

/**
 *  Retrieve a pointer to the first equal Term.
 *  \return A pointer to the first equal Term.  It has the same lifetime as 
 *   this FormulaEqu, unless you are using a TermTable.
 */
TermP FormulaEqu::GetCFirst() const
{
  return m_pFirstTerm;
}

/**
 *  Retrieve a pointer to the second equal Term.
 *  \return A pointer to the second equal Term.  It has the same lifetime as
 *   this FormulaEqu, unless you are using a TermTable.
 */
TermP FormulaEqu::GetCSecond() const
{
  return m_pSecondTerm;
}

/**
 *  Retrieve a smart pointer to a new FormulaEqu that results from applying a 
 *   Substitution to this.
 *  \param p_sSubs IN The Substitution to apply.
 *  \param p_iDepth IN The current depth of this Substitution attempt.
 *  \return A smart pointer to a new FormulaEqu that results from applying a 
 *   Substitution to this.
 */
FormulaP FormulaEqu::AfterSubstitution( const Substitution & p_sSubs,
					unsigned int p_iDepth ) const
{
  FormulaEqu * l_pAfter = new FormulaEqu();

  l_pAfter->m_pFirstTerm = m_pFirstTerm->AfterSubstitution( p_sSubs, p_iDepth + 1 );
  l_pAfter->m_pSecondTerm = m_pSecondTerm->AfterSubstitution( p_sSubs, p_iDepth + 1 );
  return FormulaP( l_pAfter );
}

/**
 *  Determine whether or not this FormulaEqu is ground (contains no variables).
 *  \return Whether or not this FormulaEqu is ground (contains no variables).
 */
bool FormulaEqu::IsGround() const
{
  return ( m_pFirstTerm->GetType() != TT_VARIABLE &&
	   m_pSecondTerm->GetType() != TT_VARIABLE );
}

/**
 *  Determine whether or not this FormulaEqu imples some other Formula.
 *  A FormulaEqu may only imply an identical FormulaEqu or a conjunction of 
 *   copies of itself.
 *  \param p_pOther IN A smart pointer to the Formula that might be implied.
 *  \return Whether or not this FormulaEqu implies p_pOther.
 */
bool FormulaEqu::Implies( const FormulaP & p_pOther ) const
{
  switch( p_pOther->GetType() )
  {
  case FT_PRED:
    return false;
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
    return Equal( *p_pOther );
  default:
    throw Exception( E_FORMULA_TYPE_UNKNOWN,
		     "Unknown formula type used.",
		     __FILE__,
		     __LINE__ );
  }
}

/**
 *  Retrieve a list of all TermVariables used in this FormulaEqu.
 *  \return A vector of pointers to all of the TermVariables used in this 
 *   FormulaEqu.  These pointers have the same lifetime as this FormulaEqu,
 *   unless a TermTable is in use.
 */
std::vector< TermVariableP > FormulaEqu::GetVariables() const
{
  std::vector< TermVariableP > l_vRet;
  if( m_pFirstTerm->GetType() == TT_VARIABLE )
    l_vRet.push_back( std::tr1::dynamic_pointer_cast< TermVariable >( m_pFirstTerm ) );
  if( m_pSecondTerm->GetType() == TT_VARIABLE && !m_pSecondTerm->Equal( *m_pFirstTerm ) )
    l_vRet.push_back( std::tr1::dynamic_pointer_cast< TermVariable >( m_pSecondTerm ) );
  return l_vRet;
}

/**
 *  Retrieve a list of all TermConstants used in this FormulaEqu.
 *  \return A vector of pointers to all of the TermConstants used in this 
 *   FormulaEqu.  These pointers have the same lifetime as this FormulaEqu,
 *   unless a TermTable is in use.
 */
std::vector< TermConstantP > FormulaEqu::GetConstants() const
{
  std::vector< TermConstantP > l_vRet;
  if( m_pFirstTerm->GetType() == TT_CONSTANT )
    l_vRet.push_back( std::tr1::dynamic_pointer_cast< TermConstant >( m_pFirstTerm ) );
  if( m_pSecondTerm->GetType() == TT_CONSTANT && !m_pSecondTerm->Equal( *m_pFirstTerm ) )
    l_vRet.push_back( std::tr1::dynamic_pointer_cast< TermConstant >( m_pSecondTerm ) );
  return l_vRet;
}

/**
 *  Determine whether or not this FormulaEqu is equivalent to another Formula.
 *  \param p_Other IN The Formula to compare against.
 *  \return Whether or not this FormulaEqu is equivalent to another Formula.
 */
bool FormulaEqu::Equal( const Formula & p_Other ) const
{
  if( p_Other.GetType() != FT_EQU )
    return false;

  const FormulaEqu & l_Other = dynamic_cast<const FormulaEqu &>(p_Other);

  if( m_pFirstTerm->Equal( *l_Other.m_pFirstTerm ) &&
      m_pSecondTerm->Equal( *l_Other.m_pSecondTerm ) )
    return true;
  if( m_pFirstTerm->Equal( *l_Other.m_pSecondTerm ) &&
      m_pSecondTerm->Equal( *l_Other.m_pFirstTerm ) )
    return true;

  return false;
}

size_t FormulaEqu::GetMemSizeMin() const
{
  return sizeof( FormulaEqu );
}

size_t FormulaEqu::GetMemSizeMax() const
{
  return sizeof( FormulaEqu ) + m_pFirstTerm->GetMemSizeMax() + m_pSecondTerm->GetMemSizeMax();
}
