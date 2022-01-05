#include <string>
#include <sstream>
#include <cassert>
#include <vector>
#include <set>
#include <tr1/memory>
#include <cstdlib>

#include "exception.hpp"
#include "funcs.hpp"
#include "term.hpp"
#include "term_string.hpp"
#include "term_variable.hpp"
#include "term_constant.hpp"
#include "term_table.hpp"
#include "type_table.hpp"
#include "substitution.hpp"
#include "formula.hpp"
#include "formula_pred.hpp"
#include "formula_neg.hpp"
#include "formula_conj.hpp"
#include "operator.hpp"

/** \file operator.hpp
 *  Declaration of Operator class.
 */

/** \file operator.cpp
 *  Definition of Operator class.
 */

/** \class Operator
 *  A planning operator, as used in PDDL.
 *  It consists of a head (name and parameters), precondition list, and 
 *   effects.  If the precondition list is satisfied in some State, the 
 *   operator is applicable to that State.  Applying it removes its negative
 *   effects from the State and adds its positive effects instead.
 */

/** \var Operator::m_pPreconditions
 *  A smart pointer to the precondition of this Operator.
 *  \todo This should not really be required to be a conjunction.
 */

/** \var Operator::m_pEffects
 *  A smart pointer to the effects of this Operator.
 *  \todo This should not really be required to be a conjunction.
 */

/** \var Operator::m_pHead
 *  A smart pointer to the head of this Operator.
 *  This consists of the operator's name and parameters.
 */

/** \var Operator::m_iCost
 *  The cost of executing this operator.
 *  This is used for reinforcement learning.
 */

/**
 *  The one and only TermTable, defined in term_table.cpp.
 */
extern TermTable g_TermTable;

/**
 *  Get a pointer to a new Operator based on its representation in PDDL.
 *  Technically, this is not strict PDDL because I have extended it to also use
 *   HTN structures and have not implemented some obscure features.
 *  \param p_sInput INOUT A stream containing a textual representation of the
 *   Operator in my version of PDDL.  The stream will be advanced beyond this
 *   representation.
 *  \param p_sTypes IN A list of allowable types.  If empty, all Terms must be
 *   untyped.
 *  \param p_vAllowablePredicates IN A list of allowable predicate symbols 
 *   with their arities and the types of their parameters.  If non-empty, only
 *   those listed may be used in the preconditions and effects.  Otherwise, all
 *   are available.
 *  \return A pointer to a new Operator object based on the textual
 *   description.  The caller is responsible for deallocating it.
 */
Operator * Operator::FromPddl( std::stringstream & p_sInput, 
			       const std::set< std::string, StrLessNoCase > & p_sTypes,
			       const std::vector< FormulaPred > & p_vAllowablePredicates )
{
  Operator * l_pRet = new Operator();

  TypeTable l_TypeTable;

  EatWhitespace( p_sInput );
  EatString( p_sInput, "(" );

  EatWhitespace( p_sInput );
  EatString( p_sInput, ":action" );
  EatWhitespace( p_sInput );

  std::string l_sName = ReadString( p_sInput );
  EatWhitespace( p_sInput );

  bool l_bHasParameters = false,
    l_bHasPreconditions = false,
    l_bHasEffects = false;

  std::vector< std::string > l_vParameters;

  while( p_sInput.peek() != ')' )
  {
    std::string l_sFeatureName = ReadString( p_sInput );
    EatWhitespace( p_sInput );
    if( CompareNoCase( l_sFeatureName, ":parameters" ) == 0 )
    {
      if( l_bHasParameters )
	throw Exception( E_NOT_IMPLEMENTED,
			 "An action may not have multiple parameter lists.",
			 __FILE__,
			 __LINE__ );
      l_bHasParameters = true;

      EatString( p_sInput, "(" );
      EatWhitespace( p_sInput );
    
      while( p_sInput.peek() != ')' )
      {
	std::string l_sName = ReadString( p_sInput );
	  EatWhitespace( p_sInput );
	if( p_sTypes.size() > 0 ) // Typing is used
	{
	  EatString( p_sInput, "-" );
	  EatWhitespace( p_sInput );
	  std::string l_sTyping = ReadString( p_sInput );
	  EatWhitespace( p_sInput );
	  if( p_sTypes.find( l_sTyping ) == p_sTypes.end() )
	    throw Exception( E_NOT_IMPLEMENTED,
			     "Attempt to use an unregistered type.",
			     __FILE__,
			     __LINE__ );
	  if( l_TypeTable.find( l_sName ) != l_TypeTable.end() )
	  {
	    if( CompareNoCase( (*l_TypeTable.find( l_sName )).second, l_sTyping ) == 0 )
	    {
	      throw Exception( E_NOT_IMPLEMENTED,
			       "Attempt to use a term with multiple types.",
			       __FILE__,
			       __LINE__ );
	    }
	  }
	  else
	  {
	    l_TypeTable[l_sName] = l_sTyping;
	  }
	}
	l_vParameters.push_back( l_sName );
      }

      EatString( p_sInput, ")" );

    }
    else if( CompareNoCase( l_sFeatureName, ":precondition" ) == 0 )
    {
      if( l_bHasPreconditions )
	throw Exception( E_NOT_IMPLEMENTED,
			 "An action may not have multiple precondition blocks.",
			 __FILE__,
			 __LINE__ );
      l_bHasPreconditions = true;
      l_pRet->m_pPreconditions = FormulaConjP( new FormulaConj( p_sInput, l_TypeTable, p_vAllowablePredicates ) );
    }
    else if( CompareNoCase( l_sFeatureName, ":effect" ) == 0 )
    {
      if( l_bHasEffects )
	throw Exception( E_NOT_IMPLEMENTED,
			 "An action may not have multiple effect blocks.",
			 __FILE__,
			 __LINE__ );
      l_bHasEffects = true;
      l_pRet->m_pEffects = FormulaConjP( new FormulaConj( p_sInput, l_TypeTable, p_vAllowablePredicates ) );
    }
    else if( CompareNoCase( l_sFeatureName, ":id" ) == 0 )
    {
      throw Exception( E_NOT_IMPLEMENTED,
		       "An action may not have an id block.",
		       __FILE__,
		       __LINE__ );
    }
    else if( CompareNoCase( l_sFeatureName, ":cost" ) == 0 )
    {
      EatString( p_sInput, "(" );
      EatWhitespace( p_sInput );
      l_pRet->m_iCost = atoi( ReadString( p_sInput ).c_str() );
      EatWhitespace( p_sInput );
      EatString( p_sInput, ")" );
      EatWhitespace( p_sInput );
    }
    else
    {
      std::string l_sMessage;
      l_sMessage += "Unknown action feature ";
      l_sMessage += l_sFeatureName;
      l_sMessage += " in action ";
      l_sMessage += l_sName;
      l_sMessage += ".";
      throw Exception( E_NOT_IMPLEMENTED,
		       l_sMessage,
		       __FILE__,
		       __LINE__ );
    }
    EatWhitespace( p_sInput );
  }

  if( !l_bHasParameters )
    throw Exception( E_NOT_IMPLEMENTED,
		     "Each action must have a parameters block.",
		     __FILE__,
		     __LINE__ );
  if( !l_bHasPreconditions )
    throw Exception( E_NOT_IMPLEMENTED,
		     "Each action must have a preconditions block.",
		     __FILE__,
		     __LINE__ );
  if( !l_bHasEffects )
    throw Exception( E_NOT_IMPLEMENTED,
		     "Each action must have an effects block.",
		     __FILE__,
		     __LINE__ );

  EatString( p_sInput, ")" );

  std::string l_sHead = "( " + l_sName;
  for( unsigned int i = 0; i < l_vParameters.size(); i++ )
    l_sHead += " " + l_vParameters[i];
  l_sHead += " )";
  l_pRet->m_pHead = FormulaPredP( new FormulaPred( l_sHead, l_TypeTable, std::vector< FormulaPred >() ) );

  return l_pRet;
}

/**
 *  Get a pointer to a new Operator based on its representation in SHOP.
 *  \param p_sInput INOUT A stream containing a textual representation of the
 *   Operator in SHOP syntax.  The stream will be advanced beyond this
 *   representation.
 *  \return A pointer to a new Operator object based on the textual
 *   description.  The caller is responsible for deallocating it.
 */
Operator * Operator::FromShop( std::stringstream & p_sInput )
{
  Operator * l_pRet = new Operator();

  EatString( p_sInput, "(" );
  EatWhitespace( p_sInput );
  EatString( p_sInput, ":operator" );
  EatWhitespace( p_sInput );

  l_pRet->m_pHead = FormulaPredP( new FormulaPred( p_sInput, TypeTable(), std::vector< FormulaPred >() ) );
  EatWhitespace( p_sInput );

  l_pRet->m_pPreconditions = FormulaConjP( new FormulaConj( p_sInput, TypeTable(), std::vector< FormulaPred >() ) );

  EatWhitespace( p_sInput );

  std::string l_sEffects = "( and ";

  EatString( p_sInput, "(" );
  EatWhitespace( p_sInput );

  while( p_sInput.peek() == '(' )
  {
    l_sEffects += "( not ";
    l_sEffects += FormulaPred( p_sInput, TypeTable(), std::vector< FormulaPred >() ).ToStr();
    l_sEffects += " ) ";
    EatWhitespace( p_sInput );
  }

  EatString( p_sInput, ")" );
  EatWhitespace( p_sInput );
  EatString( p_sInput, "(" );
  EatWhitespace( p_sInput );

  while( p_sInput.peek() == '(' )
  {
    l_sEffects += FormulaPred( p_sInput, TypeTable(), std::vector< FormulaPred >() ).ToStr();
    l_sEffects += " ";
    EatWhitespace( p_sInput );
  }

  EatString( p_sInput, ")" );
  EatWhitespace( p_sInput );
  EatString( p_sInput, ")" );

  l_sEffects += ")";

  l_pRet->m_pEffects = FormulaConjP( new FormulaConj( l_sEffects, TypeTable(), std::vector< FormulaPred >() ) );

  return l_pRet;
}

/**
 *  Construct a default Operator.
 *  This exists only for convenience.
 */
Operator::Operator()
{
  m_iCost = 0;
}

/**
 *  Construct an Operator as a copy of an existing one.
 *  \param p_Other IN The Operator to make a copy of.
 */
Operator::Operator( const Operator & p_Other )
  : m_pPreconditions( p_Other.m_pPreconditions ),
    m_pEffects( p_Other.m_pEffects ),
    m_pHead( p_Other.m_pHead ),
    m_iCost( p_Other.m_iCost )
{
}

/**
 *  Destruct an Operator.
 */
Operator::~Operator()
{
}

/**
 *  Retrieve a smart pointer to the effects of this Operator.
 *  \return A smart pointer to the effects of this Operator.
 */
FormulaConjP Operator::GetCEffects() const
{
  return m_pEffects;
}

/**
 *  Retrieve a smart pointer to the preconditions of this Operator.
 *  \return A smart pointer to the preconditions of this Operator.
 */
FormulaConjP Operator::GetCPreconditions() const
{
  return m_pPreconditions;
}

/**
 *  Retrieve the number of parameters in the head of this Operator.
 *  \return The number of parameters in the head of this Operator.
 */
unsigned int Operator::GetNumParams() const
{
  return m_pHead->GetValence();
}

/**
 *  Retrieve a pointer to a desired parameter in the head of this Operator.
 *  \param p_iIndex IN The 0-based index of the desired parameter.
 *  \return A pointer to a desired parameter in the head of this Operator.  It
 *   has the same lifetime as this Operator, unless a TermTable is used.
 */
TermP Operator::GetCParam( unsigned int p_iIndex ) const
{
  return m_pHead->GetCParam( p_iIndex );
}

/**
 *  Retrieve the name of this Operator.
 *  \return The name of this Operator.
 */
std::string Operator::GetName() const
{
  return m_pHead->GetRelation();
}

/**
 *  Retrieve a string representation of this Operator.
 *  \param p_bIsHtn IN Whether this should be SHOP syntax (true) or PDDL 
 *   syntax (false).
 *  \param p_iIndent IN The number of spaces to indent each line.
 *  \return A string representation of this Operator.
 *  \todo Replace this with a ToPddl and a ToHtn.
 */
std::string Operator::ToStr( bool p_bIsHtn, int p_iIndent ) const
{
  std::string l_sRet;

  std::string l_sIndent = "";
  for( int i = 0; i < p_iIndent; i++ ) 
    l_sIndent += " ";

  if( p_bIsHtn )
  {
    l_sRet += l_sIndent;
    l_sRet += "( :operator ";
    l_sRet += m_pHead->ToStr();
    l_sRet += "\n";

    l_sRet += l_sIndent;
    l_sRet += "  (\n";
    for( FormulaPVecCI i = m_pPreconditions->GetBeginConj();
	 i != m_pPreconditions->GetEndConj();
	 i++ )
    {
      l_sRet += l_sIndent;
      l_sRet += "    ";
      l_sRet += ( *i )->ToStr();
      l_sRet += "\n";
    }
    l_sRet += l_sIndent;
    l_sRet += "  )\n";

    std::string l_sAddList = "";
    std::string l_sDelList = "";
    l_sAddList += l_sIndent;
    l_sDelList += l_sIndent;
    l_sAddList += "  (\n";
    l_sDelList += "  (\n";
    for( FormulaPVecCI i = m_pEffects->GetBeginConj();
	 i != m_pEffects->GetEndConj();
	 i++ )
    {
      switch( ( *i )->GetType() )
      {
      case FT_PRED:
	l_sAddList += l_sIndent;
	l_sAddList += "    ";
	l_sAddList += ( *i )->ToStr();
	l_sAddList += "\n";
	break;
      case FT_NEG:
	l_sDelList += l_sIndent;
	l_sDelList += "    ";
	l_sDelList += std::tr1::dynamic_pointer_cast< FormulaNeg >( ( *i ) )->GetCNegForm()->ToStr();
	l_sDelList += "\n";
	break;
      case FT_CONJ:
	throw Exception( E_STATE_NOT_ATOM,
			 "For now, all operator effects must be a predicate or negated predicate.",
			 __FILE__,
			 __LINE__ );
      default:
	throw Exception( E_FORMULA_TYPE_UNKNOWN,
			 "Unknown formula type.",
			 __FILE__,
			 __LINE__ );
      }
    }
    l_sAddList += l_sIndent;
    l_sDelList += l_sIndent;
    l_sAddList += "  )\n";
    l_sDelList += "  )\n";

    l_sRet += l_sDelList;
    l_sRet += l_sAddList;
    l_sRet += l_sIndent;
    l_sRet += ")\n";
  }
  else
  {
    l_sRet += l_sIndent + "( :action ";
    l_sRet += m_pHead->GetRelation();
    l_sRet += "\n";
    l_sRet += l_sIndent + "  :parameters\n";
    l_sRet += l_sIndent + "  (\n";
  
    for( unsigned int i = 0; i < m_pHead->GetValence(); i++ )
    {
      l_sRet += l_sIndent + "    ";
      l_sRet += m_pHead->GetCParam( i )->ToStr();
      l_sRet += "\n";
    }

    l_sRet += l_sIndent + "  )\n";

    l_sRet += l_sIndent + "  :precondition\n";
    l_sRet += l_sIndent + "  ";
    l_sRet += m_pPreconditions->ToStrNoTyping();
    l_sRet += "\n";
    l_sRet += l_sIndent + "  :effect\n";
    l_sRet += l_sIndent + "  ";
    l_sRet += m_pEffects->ToStrNoTyping();
    l_sRet += "\n";

    if( m_iCost != 0 )
    {
      std::stringstream l_sTemp;
      l_sTemp << l_sIndent << "  :cost ( " << m_iCost << " )\n";
      l_sRet += l_sTemp.str();
    }

    l_sRet += l_sIndent + ")";
  }



  return l_sRet;
}

/**
 *  Determine whether or not this is equivalent to another Operator, meaning 
 *   that one can be converted to the other by a Substitution.
 *  \param p_pOther IN The Operator to which this might be equivalent.
 *  \return Whether or not this is equivalent to p_pOther.
 */
bool Operator::Equivalent( const Operator * p_pOther ) const
{
  if( CompareNoCase( m_pHead->GetRelation(),
		    p_pOther->m_pHead->GetRelation() ) != 0 )
    return false;

  Substitution l_Subst;

  // This will probably fail if the operators use the same variable names
  // but in different places.  Hopefully, that won't ever happen.
  for( unsigned int i = 0; i < m_pHead->GetValence(); i++ )
  {
    if( m_pHead->GetCParam( i )->Equal( *p_pOther->m_pHead->GetCParam( i ) ) )
      continue;
    if( l_Subst.FindIndexByVar( std::tr1::dynamic_pointer_cast< TermVariable >( m_pHead->GetCParam( i ) ) ) == l_Subst.End() )
      l_Subst.AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( m_pHead->GetCParam( i ) ), p_pOther->m_pHead->GetCParam( i ) );
    else if( !l_Subst.FindIndexByVar( std::tr1::dynamic_pointer_cast< TermVariable >( m_pHead->GetCParam( i ) ) )->second->Equal( *p_pOther->m_pHead->GetCParam( i ) ) )
      return false;
  }

  // This will fail if we do not require all variables in precs and effects
  // to be bound in the operator parameters.
  FormulaP l_pPrecs( m_pPreconditions->AfterSubstitution( l_Subst, 0 ) );
  if( !l_pPrecs->Equal( *p_pOther->m_pPreconditions ) )
  {
    return false;
  }
  FormulaP l_pEffects( m_pEffects->AfterSubstitution( l_Subst, 0 ) );
  if( !l_pEffects->Equal( *p_pOther->m_pEffects ) )
  {
    return false;
  }

  return true;
}

/**
 *  Retrieve a smart pointer to the head of this Operator.
 *  \return A smart pointer to the head of this Operator.
 */
FormulaPredP Operator::GetCHead() const
{
  return m_pHead;
}

/**
 *  Retrieve the cost of this Operator.
 *  If not specified in the domain file, it will be 0.
 *  \return The cost of this Operator.
 */
int Operator::GetCost() const
{
  return m_iCost;
}

size_t Operator::GetMemSizeMin() const
{
  return sizeof( Operator );
}

size_t Operator::GetMemSizeMax() const
{
  return sizeof( Operator ) + m_pPreconditions->GetMemSizeMax() + m_pEffects->GetMemSizeMax() + m_pHead->GetMemSizeMax();
}
