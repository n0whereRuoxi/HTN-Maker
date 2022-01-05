#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <set>
#include <tr1/memory>
#include <cstdlib>
#include <algorithm>

#include "exception.hpp"
#include "funcs.hpp"
#include "term.hpp"
#include "term_string.hpp"
#include "term_constant.hpp"
#include "term_variable.hpp"
#include "term_table.hpp"
#include "type_table.hpp"
#include "substitution.hpp"
#include "formula.hpp"
#include "formula_pred.hpp"
#include "formula_equ.hpp"
#include "formula_neg.hpp"
#include "formula_conj.hpp"
#include "operator.hpp"
#include "htn_task_head.hpp"
#include "htn_method.hpp"

/** \file htn_method.hpp
 *  Declaration of HtnMethod class.
 */

/** \file htn_method.cpp
 *  Definition of HtnMethod class.
 */

/** \class HtnMethod
 *  A method in the Ordered Task Decomposition version of Hierarchical Task
 *   Networks, for decomposing its head into a list of subtasks.
 *  A method consists of its head, which is a task, its precondition, which is
 *   a formula in first-order logic, and an ordered list of subtasks, which
 *   are tasks.
 */

/** \var HtnMethod::m_sId
 *  The ID of this method.
 *  This exists because for some applications you want to be able to print the
 *   full decomposition tree and know which method was applied at each branch.
 */

/** \var HtnMethod::m_pHead
 *  A smart pointer to the head of this method.
 */

/** \var HtnMethod::m_pPreconditions
 *  A smart pointer to the precondition of this method.
 */

/** \var HtnMethod::m_vSubtasks
 *  A list of smart pointers to the subtasks of this method.
 */

/** \var HtnMethod::m_TypeTable
 *  A mapping from terms used in the method to their types.
 */

/** \var HtnMethod::m_fQValue
 *  The current Q-value of this method.
 *  This is used by reinforcement learning.
 */

/** \var HtnMethod::m_iQCount
 *  The number of times the Q-value for this method has been updated.
 *  This is used by reinforcement learning.
 */

/**
 *  The one and only TermTable.
 */
extern TermTable g_TermTable;

/**
 *  Retrieve a pointer to a new HtnMethod from a textual description in the 
 *   PDDL language and other stuff.
 *  \param p_sInput INOUT A stream containing a textual description in PDDL of
 *   the method.  This will be advanced beyond it.
 *  \param p_sTypes IN A set of types that may be used.  If non-empty, all 
 *   terms must be of one of these types.  Otherwise, any types are allowed.
 *  \param p_vAllowablePredicates IN A list of predicate symbols with the 
 *   number and types of their parameters that may be used in the precondition.
 *   If non-empty, only these predicates may be used.  Otherwise, any may be.
 *  \return A pointer to a new HtnMethod described in the arguments.  The 
 *   caller is responsible for deallocating this pointer.
 */
HtnMethod * HtnMethod::FromPddl( std::stringstream & p_sInput, 
				 const std::set< std::string, StrLessNoCase > & p_sTypes, 
				 const std::vector< FormulaPred > & p_vAllowablePredicates,
				 long p_iRequirements )
{
  HtnMethod * l_pRet = new HtnMethod();

  EatWhitespace( p_sInput );
  EatString( p_sInput, "(" );
  EatWhitespace( p_sInput );
  EatString( p_sInput, ":method" );
  EatWhitespace( p_sInput );

  std::string l_sTaskName = "( " + ReadString( p_sInput ) + " ";
  EatWhitespace( p_sInput );

  bool l_bHasParameters = false,
    l_bHasPreconditions = false,
    l_bHasSubtasks = false,
    l_bHasId = false,
    l_bHasVars = false;

  while( p_sInput.peek() != ')' )
  {
    std::string l_sFeatureName = ReadString( p_sInput );
    EatWhitespace( p_sInput );
    if( CompareNoCase( l_sFeatureName, ":parameters" ) == 0 )
    {
      if( l_bHasParameters )
	throw Exception( E_NOT_IMPLEMENTED,
			 "A method may not have multiple parameter lists.",
			 __FILE__,
			 __LINE__ );
      l_bHasParameters = true;

      EatString( p_sInput, "(" );
      EatWhitespace( p_sInput );

      while( p_sInput.peek() != ')' )
      {
	std::string l_sName = ReadString( p_sInput );
	l_sTaskName += l_sName + " ";
	if( p_iRequirements & PDDL_REQ_TYPING )
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
	  if( l_pRet->m_TypeTable.find( l_sName ) != l_pRet->m_TypeTable.end() )
	  {
	    if( CompareNoCase( (*l_pRet->m_TypeTable.find( l_sName )).second, l_sTyping ) == 0 )
	      throw Exception( E_NOT_IMPLEMENTED,
			       "Attempt to use a term with multiple types.",
			       __FILE__,
			       __LINE__ );
	  }
	  else
	    l_pRet->m_TypeTable[l_sName] = l_sTyping;
	}
	EatWhitespace( p_sInput );
      }

      EatString( p_sInput, ")" );
      l_sTaskName += ")";
    }
    else if( CompareNoCase( l_sFeatureName, ":vars" ) == 0 )
    {
      if( l_bHasVars )
	throw Exception( E_NOT_IMPLEMENTED,
			 "A method may not have multiple vars blocks.",
			 __FILE__,
			 __LINE__ );
      if( l_bHasPreconditions )
	throw Exception( E_NOT_IMPLEMENTED,
			 "The vars block of a method may not be after the precondition block.",
			 __FILE__,
			 __LINE__ );
      if( l_bHasSubtasks )
	throw Exception( E_NOT_IMPLEMENTED,
			 "The vars block of a method may not be after the subtasks block.",
			 __FILE__,
			 __LINE__ );
      l_bHasVars = true;

      EatString( p_sInput, "(" );
      EatWhitespace( p_sInput );

      while( p_sInput.peek() != ')' )
      {
	std::string l_sName = ReadString( p_sInput );
	if( p_iRequirements & PDDL_REQ_TYPING )
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
	  if( l_pRet->m_TypeTable.find( l_sName ) != l_pRet->m_TypeTable.end() )
	  {
	    if( CompareNoCase( (*l_pRet->m_TypeTable.find( l_sName )).second, l_sTyping ) == 0 )
	      throw Exception( E_NOT_IMPLEMENTED,
			       "Attempt to use a term with multiple types.",
			       __FILE__,
			       __LINE__ );
	  }
	  else
	    l_pRet->m_TypeTable[l_sName] = l_sTyping;
	}
	EatWhitespace( p_sInput );
      }

      EatString( p_sInput, ")" );
    }
    else if( CompareNoCase( l_sFeatureName, ":precondition" ) == 0 )
    {
      if( l_bHasPreconditions )
	throw Exception( E_NOT_IMPLEMENTED,
			 "A method may not have multiple precondition blocks.",
			 __FILE__,
			 __LINE__ );
      l_bHasPreconditions = true;
      l_pRet->m_pPreconditions = FormulaConjP( new FormulaConj( p_sInput, l_pRet->m_TypeTable, p_vAllowablePredicates ) );
    }
    else if( CompareNoCase( l_sFeatureName, ":subtasks" ) == 0 )
    {
      if( l_bHasSubtasks )
	throw Exception( E_NOT_IMPLEMENTED,
			 "A method may not have multiple subtask blocks.",
			 __FILE__,
			 __LINE__ );
      l_bHasSubtasks = true;

      EatString( p_sInput, "(" );
      EatWhitespace( p_sInput );

      while( p_sInput.peek() == '(' )
      {
	l_pRet->m_vSubtasks.push_back( HtnTaskHeadP( new HtnTaskHead( p_sInput, l_pRet->m_TypeTable ) ) );
	EatWhitespace( p_sInput );
      }

      EatString( p_sInput, ")" );

    }
    else if( CompareNoCase( l_sFeatureName, ":id" ) == 0 )
    {
      if( l_bHasId )
	throw Exception( E_NOT_IMPLEMENTED,
			 "A method may not have multiple id blocks.",
			 __FILE__,
			 __LINE__ );
      if( ( p_iRequirements & PDDL_REQ_METHOD_IDS ) == 0 )
	throw Exception( E_PARSE_BAD_STRING,
			"Attempt to use a method ID without declaring the :method-ids PDDL requirement.",
			__FILE__,
			__LINE__ );
      l_bHasId = true;
      std::stringstream l_sIdStream( ReadParenthetical( p_sInput ) );
      EatString( l_sIdStream, "(" );
      EatWhitespace( l_sIdStream );
      l_pRet->m_sId = ReadString( l_sIdStream );
    }
    else if( CompareNoCase( l_sFeatureName, ":q-value" ) == 0 )
    {
      if( ( p_iRequirements & PDDL_REQ_QVALUES ) == 0 )
	throw Exception( E_PARSE_BAD_STRING,
			 "Attempt to use a method Q-value without declaring the :q-values PDDL requirement.",
			 __FILE__,
			 __LINE__ );
      EatString( p_sInput, "(" );
      EatWhitespace( p_sInput );
      l_pRet->m_fQValue = atof( ReadString( p_sInput ).c_str() );
      EatWhitespace( p_sInput );
      EatString( p_sInput, ")" );
      EatWhitespace( p_sInput );
    }
    else if( CompareNoCase( l_sFeatureName, ":q-count" ) == 0 )
    {
      if( ( p_iRequirements & PDDL_REQ_QVALUES ) == 0 )
	throw Exception( E_PARSE_BAD_STRING,
			 "Attempt to use a method Q-value counter without declaring the :q-values PDDL requirement.",
			 __FILE__,
			 __LINE__ );
      EatString( p_sInput, "(" );
      EatWhitespace( p_sInput );
      l_pRet->m_iQCount = atoi( ReadString( p_sInput ).c_str() );
      EatWhitespace( p_sInput );
      EatString( p_sInput, ")" );
      EatWhitespace( p_sInput );
    }
    else
    {
      throw Exception( E_NOT_IMPLEMENTED,
		       "Unknown method feature.",
		       __FILE__,
		       __LINE__ );
    }
    EatWhitespace( p_sInput );
  }

  std::stringstream l_sTaskStream( l_sTaskName );
  l_pRet->m_pHead = HtnTaskHeadP( new HtnTaskHead( l_sTaskStream, l_pRet->m_TypeTable ) );

  return l_pRet;
}

/**
 *  Retrieve a pointer to a new HtnMethod from its textual description in the
 *   SHOP formalism.
 *  \param p_sInput INOUT A stream containing a textual description of the 
 *   method in the SHOP formalism.  The stream is advanced beyond it.
 *  \return A pointer to a new HtnMethod based on the provided text.  The 
 *   caller is responsible for deallocating it.
 */
HtnMethod * HtnMethod::FromShop( std::stringstream & p_sInput )
{
  HtnMethod * l_pRet = new HtnMethod();

  EatWhitespace( p_sInput );
  EatString( p_sInput, "(" );
  EatWhitespace( p_sInput );
  EatString( p_sInput, ":method" );
  EatWhitespace( p_sInput );
  l_pRet->m_pHead = HtnTaskHeadP( new HtnTaskHead( p_sInput, l_pRet->m_TypeTable ) );
  EatWhitespace( p_sInput );
  l_pRet->m_pPreconditions = FormulaConjP( new FormulaConj( p_sInput, l_pRet->m_TypeTable, std::vector< FormulaPred >() ) );
  EatWhitespace( p_sInput );
  EatString( p_sInput, "(" );
  EatWhitespace( p_sInput );

  while( p_sInput.peek() == '(' )
  {
    l_pRet->m_vSubtasks.push_back( HtnTaskHeadP( new HtnTaskHead( p_sInput, l_pRet->m_TypeTable ) ) );
    EatWhitespace( p_sInput );
  }

  EatString( p_sInput, ")" );
  EatWhitespace( p_sInput );
  EatString( p_sInput, ")" );

  return l_pRet;
}

/**
 *  Construct a default HtnMethod.  This should only be called by the 
 *   FromShop() and FromPddl() methods.
 */
HtnMethod::HtnMethod()
{
  m_sId = "";
  m_fQValue = 999999;
  m_iQCount = 0;
}

/**
 *  Construct an HtnMethod as a copy of an existing one.
 *  \param p_Other IN The HtnMethod to copy.
 */
HtnMethod::HtnMethod( const HtnMethod & p_Other )
  : m_pHead( p_Other.m_pHead ),
    m_pPreconditions( p_Other.m_pPreconditions ),
    m_fQValue( p_Other.m_fQValue ),
    m_iQCount( p_Other.m_iQCount )
{
  for( unsigned int i = 0; i < p_Other.m_vSubtasks.size(); i++ )
    m_vSubtasks.push_back( p_Other.m_vSubtasks[i] );
  m_sId = p_Other.m_sId;
}

/**
 *  Destruct an HtnMethod.
 */
HtnMethod::~HtnMethod()
{
}

/**
 *  Retrieve a smart pointer to the head of this HtnMethod.
 *  \return A smart pointer to the head of this HtnMethod.
 */
HtnTaskHeadP HtnMethod::GetCHead() const
{
  return m_pHead;
}

/**
 *  Retrieve a smart pointer to the preconditions of this HtnMethod.
 *  \return A smart pointer to the precondition of this HtnMethod.
 */
FormulaConjP HtnMethod::GetCPreconditions() const
{
  return m_pPreconditions;
}

/**
 *  Retrieve the number of subtasks in this HtnMethod.
 *  \return The number of subtasks in this HtnMethod.
 */
unsigned int HtnMethod::GetNumSubtasks() const
{
  return m_vSubtasks.size();
}

/**
 *  Retrieve a smart pointer to one of this HtnMethod's subtasks.
 *  \param p_iIndex IN The 0-based index of the desired subtask.
 *  \return A smart pointer to the requested subtask.
 */
HtnTaskHeadP HtnMethod::GetCSubtask( unsigned int p_iIndex ) const
{
  if( p_iIndex >= m_vSubtasks.size() )
    throw Exception( E_INDEX_OUT_OF_BOUNDS,
		     "Bounds error.",
		     __FILE__,
		     __LINE__ );
  return m_vSubtasks[ p_iIndex ];
}

/**
 *  Retrieve a list of pointers to the variables in this HtnMethod.
 *  \return A list of pointers to the variables in this HtnMethod.
 */
std::vector< TermVariableP > HtnMethod::GetVariables() const
{
  std::vector< TermVariableP > l_vRet = m_pPreconditions->GetVariables();

  std::vector< TermVariableP > l_vTemp = m_pHead->GetVariables();

  for( unsigned int j = 0; j < l_vTemp.size(); j++ )
  {
    bool l_bFound = false;
    for( unsigned int k = 0; k < l_vRet.size() && !l_bFound; k++ )
    {
      if( *l_vTemp[j] == *l_vRet[k] )
	l_bFound = true;
    }
    if( !l_bFound )
      l_vRet.push_back( l_vTemp[j] );
  }

  for( unsigned int i = 0; i < m_vSubtasks.size(); i++ )
  {
    l_vTemp = m_vSubtasks[i]->GetVariables();
    for( unsigned int j = 0; j < l_vTemp.size(); j++ )
    {
      bool l_bFound = false;
      for( unsigned int k = 0; k < l_vRet.size() && !l_bFound; k++ )
      {
	if( *l_vTemp[j] == *l_vRet[k] )
	  l_bFound = true;
      }
      if( !l_bFound )
	l_vRet.push_back( l_vTemp[j] );
    } 
  }

  return l_vRet;
}

std::set< TermVariableP > HtnMethod::GetRelVars() const
{
  std::set< TermVariableP > l_vRet;

  std::vector< TermVariableP > l_vTemp = m_pHead->GetVariables();

  for( unsigned int j = 0; j < l_vTemp.size(); j++ )
  {
    l_vRet.insert( l_vTemp[j] );
  }

  for( unsigned int i = 0; i < m_vSubtasks.size(); i++ )
  {
    l_vTemp = m_vSubtasks[i]->GetVariables();
    for( unsigned int j = 0; j < l_vTemp.size(); j++ )
    {
      l_vRet.insert( l_vTemp[j] );
    } 
  }

  return l_vRet;
}

/**
 *  Retrieve a list of pointers to the relevant variables in this HtnMethod.
 *  That is, these are the variables that appear in either the head of the
 *   task or one of its subtasks.  Variables that only appear in the 
 *   precondition are not considered relevant.
 *  \return A list of pointers to the variables in this HtnMethod.
 */
std::tr1::shared_ptr< const std::vector< TermVariableP > > HtnMethod::GetRelevantVariables() const
{
  if( m_pVariables )
    return m_pVariables;

  std::vector< TermVariableP > l_vRet = m_pHead->GetVariables();

  for( unsigned int i = 0; i < m_vSubtasks.size(); i++ )
  {
    std::vector< TermVariableP > l_vTemp = m_vSubtasks[i]->GetVariables();
    for( unsigned int j = 0; j < l_vTemp.size(); j++ )
    {
      bool l_bFound = false;
      for( unsigned int k = 0; k < l_vRet.size() && !l_bFound; k++ )
      {
	if( *l_vTemp[j] == *l_vRet[k] )
	  l_bFound = true;
      }
      if( !l_bFound )
	l_vRet.push_back( l_vTemp[j] );
    } 
  }

  m_pVariables = std::tr1::shared_ptr< std::vector< TermVariableP > >( new std::vector< TermVariableP >( l_vRet ) );
  return m_pVariables;
}

/**
 *  A class that exists simply to maintain some bookkeeping information about
 *   a Formula.
 *  This is used so that a list of formulas can be sorted based on the number
 *   of variables constraining them without continually regenerating those
 *   numbers as the members in the list are updated.
 */
class SortableFormulaNode
{
public:
  /**
   *  Construct a Formula node with all old variables.
   *  \param l_pForm IN The Formula to encapsulate.
   */
  SortableFormulaNode( const FormulaP & l_pForm )
    : m_pForm( l_pForm ),
      m_vOldVars( l_pForm->GetVariables() )
  {
    if( m_pForm->GetType() == FT_PRED )
      m_iNumVars = std::tr1::dynamic_pointer_cast< FormulaPred >( m_pForm )->GetValence();
    else
      m_iNumVars = 2;
  }

  /**
   *  Get a Node with the substitution applied and counts updated.
   *  \param IN l_Subst The susbtitution to apply.
   *  \return A node based on this after the substitution.
   */
  std::tr1::shared_ptr< SortableFormulaNode > AfterSubstitution( const Substitution & l_Subst ) const
  {
    std::tr1::shared_ptr< SortableFormulaNode > l_pNewNode( new SortableFormulaNode );
    l_pNewNode->m_iNumVars = m_iNumVars;

    for( std::vector< TermVariableP >::const_iterator i = m_vOldVars.begin();
	 i != m_vOldVars.end(); i++ )
    {
      if( l_Subst.FindIndexByVar( *i ) == l_Subst.End() )
	l_pNewNode->m_vOldVars.push_back( *i );
    }

    if( m_vOldVars.size() > l_pNewNode->m_vOldVars.size() )
      l_pNewNode->m_pForm = m_pForm->AfterSubstitution( l_Subst, 0 );
    else
      l_pNewNode->m_pForm = m_pForm;
    return l_pNewNode;
  }

  /**
   *  Retrieve the number of variables that have not been substituted.
   *  \return The number of variables that have not been substituted.
   */
  unsigned int GetNumOldVars() const
  {
    return m_vOldVars.size();
  }

  /**
   *  Retrieve the valence of the formula.
   *  \return The valence of the formula.
   */
  unsigned int GetNumVars() const
  {
    return m_iNumVars;
  }

  /**
   *  Retrieve the encapsulated formula.
   *  \return The encapsulated formula.
   */
  const FormulaP GetFormula() const
  {
    return m_pForm;
  }

private:
  /**
   *  Construct an empty node.
   */
  SortableFormulaNode()
  {
  }

  /**
   *  The encapsulated formula.
   */
  FormulaP m_pForm;

  /**
   *  A list of variables that have not yet been substituted.
   */
  std::vector< TermVariableP > m_vOldVars;

  /**
   *  The valence of the formula (2 for equalities).
   */
  unsigned int m_iNumVars;
};

/**
 *  A functor for determining which of two formulas is closest to being 
 *   specified completely in new variables.
 */
struct SortableNodeClosest
{
  /**
   *  Determine whether or not one formula is closer than another to being
   *   specified completely in new variables.
   *  \param p_pForm1 IN The first encapsulated formula.
   *  \param p_pForm2 IN The second encapsulated formula.
   *  \return Whether or not p_pForm1 is closer than p_pForm2.
   */
  bool operator()( const std::tr1::shared_ptr< SortableFormulaNode > & p_pForm1,
		   const std::tr1::shared_ptr< SortableFormulaNode > & p_pForm2 ) const
  {
    FormulaType l_Type1 = p_pForm1->GetFormula()->GetType();
    FormulaType l_Type2 = p_pForm2->GetFormula()->GetType();
    if( l_Type1 == FT_EQU && l_Type2 == FT_PRED )
      return true;
    if( l_Type1 == FT_EQU && l_Type2 == FT_NEG )
      return true;
    if( l_Type1 == FT_PRED && l_Type2 == FT_EQU )
      return false;
    if( l_Type1 == FT_PRED && l_Type2 == FT_NEG )
      return true;
    if( l_Type1 == FT_NEG && l_Type2 == FT_EQU )
      return false;
    if( l_Type1 == FT_NEG && l_Type2 == FT_PRED )
      return false;

    if( p_pForm1->GetNumOldVars() < p_pForm2->GetNumOldVars() )
      return true;
    else if( p_pForm1->GetNumOldVars() == p_pForm2->GetNumOldVars() )
      return p_pForm1->GetNumVars() > p_pForm2->GetNumVars();
    else
      return false;

  }
} SortableNodeClosestSorter;

bool CanSubsume( const FormulaPVec & p_vMyPrecs,
		 const FormulaPVec & p_vHisPrecs,
		 const Substitution & p_Sub );

bool NewCanSubsume( const std::vector< std::tr1::shared_ptr< SortableFormulaNode > > & p_vMyPrecs,
		    const FormulaPVec & p_vHisPrecs,
		    const Substitution & p_Sub );

/**
 *  Determine whether or not "my" formulas can subsume "his".
 *  This is a helper function for NewCanSubsume().
 *  This tries all ways that the first member of p_vMyPrecs, which must be a
 *   predicate, can be unified with a member of p_vHisPrecs, then recurses.
 *  \param p_vMyPrecs IN A list of "my" formulas.
 *  \param p_vHisPrecs IN A list of "his" formulas.
 *  \return Whether or not "my" formulas can subsume "his".
 */
bool NewCanSubsumePredicate( const std::vector< std::tr1::shared_ptr< SortableFormulaNode > > & p_vMyPrecs,
			     const FormulaPVec & p_vHisPrecs )
{
  FormulaPredP l_pCurPrec = std::tr1::dynamic_pointer_cast< FormulaPred >( p_vMyPrecs[0]->GetFormula() );

  for( unsigned int i = 0; i < p_vHisPrecs.size(); i++ )
  {
    if( p_vHisPrecs[i]->GetType() != FT_PRED )
      continue;
    FormulaPredP l_pCurHis = std::tr1::dynamic_pointer_cast< FormulaPred >( p_vHisPrecs[i] );
    if( l_pCurPrec->GetRelationIndex() != l_pCurHis->GetRelationIndex() )
      continue;
    if( l_pCurPrec->GetValence() != l_pCurHis->GetValence() )
      continue;
    Substitution l_Sub;
    bool l_bBad = false;
    for( unsigned int j = 0; j < l_pCurPrec->GetValence() && !l_bBad; j++ )
    {
      if( CompareNoCase( l_pCurPrec->GetCParam( j )->ToStr().substr( 0, 10 ), "?temp_new_" ) == 0 )
      {
	if( !( *l_pCurPrec->GetCParam( j ) == *l_pCurHis->GetCParam( j ) ) )
	  l_bBad = true;;
      }
      else
      {
	SubMap::const_iterator l_iConjSubsIndex = l_Sub.FindIndexByVar( std::tr1::dynamic_pointer_cast< TermVariable >( l_pCurPrec->GetCParam( j ) ) );
	if( l_iConjSubsIndex != l_Sub.End() )
	{
	  if( !( *l_pCurHis->GetCParam( j ) == *l_iConjSubsIndex->second ) )
	    l_bBad = true;
	}
	else
	{
	  l_Sub.AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( l_pCurPrec->GetCParam( j ) ), l_pCurHis->GetCParam( j ) );
	}
      }
    }
    if( !l_bBad )
    {
      if( NewCanSubsume( p_vMyPrecs, p_vHisPrecs, l_Sub ) )
	return true;
    }
  }
  return false;
}

/**
 *  Determine whether or not "my" formulas can subsume "his".
 *  This is a helper function for NewCanSubsume().
 *  This tries all ways that the first member of p_vMyPrecs, which must be an
 *   equality, can be unified with a member of p_vHisPrecs, then recurses.
 *  \param p_vMyPrecs IN A list of "my" formulas.
 *  \param p_vHisPrecs IN A list of "his" formulas.
 *  \return Whether or not "my" formulas can subsume "his".
 */
bool NewCanSubsumeEquality( const std::vector< std::tr1::shared_ptr< SortableFormulaNode > > & p_vMyPrecs,
			    const FormulaPVec & p_vHisPrecs )
{
  FormulaEquP l_pCurPrec = std::tr1::dynamic_pointer_cast< FormulaEqu >( p_vMyPrecs[0]->GetFormula() );
  for( unsigned int i = 0; i < p_vHisPrecs.size(); i++ )
  {
    if( p_vHisPrecs[i]->GetType() != FT_EQU )
      continue;
    FormulaEquP l_pCurHis = std::tr1::dynamic_pointer_cast< FormulaEqu >( p_vHisPrecs[i] );
    Substitution l_Sub;

    if( CompareNoCase( l_pCurPrec->GetCFirst()->ToStr().substr( 0, 10 ), "?temp_new_" ) == 0 )
    {
      if( !( *l_pCurPrec->GetCFirst() == *l_pCurHis->GetCFirst() ) )
	continue;
    }
    else
    {
      l_Sub.AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( l_pCurPrec->GetCFirst() ), l_pCurHis->GetCFirst() );
    }

    if( CompareNoCase( l_pCurPrec->GetCSecond()->ToStr().substr( 0, 10 ), "?temp_new_" ) == 0 )
    {
      if( !( *l_pCurPrec->GetCSecond() == *l_pCurHis->GetCSecond() ) )
	continue;
    }
    else
    {
      if( *l_pCurPrec->GetCFirst() == *l_pCurPrec->GetCSecond() )
      {
	if( !( *l_pCurHis->GetCFirst() == *l_pCurHis->GetCSecond() ) )
	  continue;
      }
      else
      {
	l_Sub.AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( l_pCurPrec->GetCSecond() ), l_pCurHis->GetCSecond() );
      }
    }

    if( NewCanSubsume( p_vMyPrecs, p_vHisPrecs, l_Sub ) )
      return true;

  }
  return false;
}

/**
 *  Determine whether or not "my" formulas can subsume "his".
 *  This is a helper function for NewCanSubsume().
 *  This tries all ways that the first member of p_vMyPrecs, which must be a
 *   negation, can be unified with a member of p_vHisPrecs, then recurses.
 *  \param p_vMyPrecs IN A list of "my" formulas.
 *  \param p_vHisPrecs IN A list of "his" formulas.
 *  \return Whether or not "my" formulas can subsume "his".
 */
bool NewCanSubsumeNegation( const std::vector< std::tr1::shared_ptr< SortableFormulaNode > > & p_vMyPrecs,
			    const FormulaPVec & p_vHisPrecs )
{
  FormulaNegP l_pCurPrec = std::tr1::dynamic_pointer_cast< FormulaNeg >( p_vMyPrecs[0]->GetFormula() );
  if( l_pCurPrec->GetCNegForm()->GetType() != FT_EQU )
    throw Exception( E_NOT_IMPLEMENTED,
		     "Only equalities may be negated here.",
		     __FILE__,
		     __LINE__ );
  FormulaEquP l_pMyEqu = std::tr1::dynamic_pointer_cast< FormulaEqu >( l_pCurPrec->GetCNegForm() );

  for( unsigned int i = 0; i < p_vHisPrecs.size(); i++ )
  {
    if( p_vHisPrecs[i]->GetType() != FT_NEG )
      continue;
    FormulaNegP l_pCurHis = std::tr1::dynamic_pointer_cast< FormulaNeg >( p_vHisPrecs[i] );
    if( l_pCurHis->GetCNegForm()->GetType() != FT_EQU )
      throw Exception( E_NOT_IMPLEMENTED,
		       "Only equalities may be negated here.",
		       __FILE__,
		       __LINE__ );
    FormulaEquP l_pHisEqu = std::tr1::dynamic_pointer_cast< FormulaEqu >( l_pCurHis->GetCNegForm() );

    Substitution l_Sub;

    if( CompareNoCase( l_pMyEqu->GetCFirst()->ToStr().substr( 0, 10 ), "?temp_new_" ) == 0 )
    {
      if( !( *l_pMyEqu->GetCFirst() == *l_pHisEqu->GetCFirst() ) )
	continue;
    }
    else
    {
      l_Sub.AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( l_pMyEqu->GetCFirst() ), l_pHisEqu->GetCFirst() );
    }

    if( CompareNoCase( l_pMyEqu->GetCSecond()->ToStr().substr( 0, 10 ), "?temp_new_" ) == 0 )
    {
      if( !( *l_pMyEqu->GetCSecond() == *l_pHisEqu->GetCSecond() ) )
	continue;
    }
    else
    {
      if( *l_pMyEqu->GetCFirst() == *l_pMyEqu->GetCSecond() )
      {
	if( !( *l_pHisEqu->GetCFirst() == *l_pHisEqu->GetCSecond() ) )
	  continue;
      }
      else
      {
	l_Sub.AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( l_pMyEqu->GetCSecond() ), l_pHisEqu->GetCSecond() );
      }
    }

    if( NewCanSubsume( p_vMyPrecs, p_vHisPrecs, l_Sub ) )
      return true;

  }

  return false;
}

/**
 *  Determine whether or not one list of preconditions can subsume another.
 *  Specifically, is there some substitution that is an extension of p_Sub and
 *   that can be applied to the members of p_vMyPrecs such that each of those
 *   members will be implied by the members of p_vHisPrecs.
 *  The members of p_vHisPrecs should use only variables of the form ?temp_new 
 *   while the members of p_vMyPrecs initially use only ?temp_old variables but
 *   through recursive calls replace them with ?temp_new variables.
 *  This works by applying the substitution to the members of p_vMyPrecs, 
 *   verifying that any in only ?temp_new variables match a member of 
 *   p_vHisPrecs, and sorts the remaining ones in order from closest to a
 *   member of p_vHisPrecs to furthest.  Then it recursively calls itself
 *   through a helper that generates all possible unifications of the first
 *   member of the new p_vMyPrecs to a member of p_vHisPrecs.
 *  See State::GetInstantiations() for a very similar algorithm.
 *  \param p_vMyPrecs IN A vector of "my" formulas.
 *  \param p_vHisPrecs IN A vector of "his" formulas.
 *  \param p_Sub IN A substitution that can be applied to p_vMyPrecs to move it
 *   closer to p_vHisPrecs.
 *  \return Whether or not my formulas can subsume his.
 */
bool NewCanSubsume( const std::vector< std::tr1::shared_ptr< SortableFormulaNode > > & p_vMyPrecs,
		    const FormulaPVec & p_vHisPrecs,
		    const Substitution & p_Sub )
{
  std::vector< std::tr1::shared_ptr< SortableFormulaNode > > l_vNewMyPrecs;
  for( unsigned int i = 0; i < p_vMyPrecs.size(); i++ )
  {
    std::tr1::shared_ptr< SortableFormulaNode > l_pNewNode = p_vMyPrecs[i]->AfterSubstitution( p_Sub );
    if( l_pNewNode->GetNumOldVars() == 0 )
    {
      bool l_bFound = false;
      for( unsigned int j = 0; j < p_vHisPrecs.size() && !l_bFound; j++ )
      {
	if( l_pNewNode->GetFormula()->Equal( *p_vHisPrecs[j] ) )
	  l_bFound = true;
      }
      if( !l_bFound )
	return false;
    }
    else
    {
      l_vNewMyPrecs.push_back( l_pNewNode );
    }
  }

  if( l_vNewMyPrecs.size() == 0 )
    return true;

  std::sort< std::vector< std::tr1::shared_ptr< SortableFormulaNode > >::iterator, SortableNodeClosest >( l_vNewMyPrecs.begin(), l_vNewMyPrecs.end(), SortableNodeClosestSorter );

  switch( l_vNewMyPrecs[0]->GetFormula()->GetType() )
  {
  case FT_PRED:
    return NewCanSubsumePredicate( l_vNewMyPrecs, p_vHisPrecs );
  case FT_EQU:
    return NewCanSubsumeEquality( l_vNewMyPrecs, p_vHisPrecs );
  case FT_NEG:
    return NewCanSubsumeNegation( l_vNewMyPrecs, p_vHisPrecs );
  case FT_CONJ:
    throw Exception( E_NOT_IMPLEMENTED,
		     "A conjunction contained another conjunction.",
		     __FILE__,
		     __LINE__ );
  default:
    throw Exception( E_NOT_IMPLEMENTED,
		     "A conjunction contained an unknown formuxla type.",
		     __FILE__,
		     __LINE__ );
  }
}

/**
 *  Determine whether or not one list of preconditions can subsume another.
 *  This just encapsulates "my" formulas and calls NewCanSubsume().
 *  \param p_vMyPrecs IN A vector of "my" formulas.
 *  \param p_vHisPrecs IN A vector of "his" formulas.
 *  \param p_Sub IN A substitution that can be applied to p_vMyPrecs to move it
 *   closer to p_vHisPrecs.
 *  \return Whether or not my formulas can subsume his.
 */
bool CanSubsume( const FormulaPVec & p_vMyPrecs,
		 const FormulaPVec & p_vHisPrecs,
		 const Substitution & p_Sub )
{
  std::vector< std::tr1::shared_ptr< SortableFormulaNode > > l_vMyPrecs;
  for( unsigned int i = 0; i < p_vMyPrecs.size(); i++ )
    l_vMyPrecs.push_back( std::tr1::shared_ptr< SortableFormulaNode >( new SortableFormulaNode( p_vMyPrecs[i] ) ) );

  return NewCanSubsume( l_vMyPrecs, p_vHisPrecs, p_Sub );
}

/**
 *  Determine whether or not this HtnMethod subsumes another.
 *  Subsumption means that there exists a Substitution that may be applied to
 *   this such that both have the same head and subtasks, and the 
 *   precondition of the other implies the precondition of this.
 *  In other words, the preconditions of this are more general than the other.
 *  Finding a Substitution that makes this true, or proving that none such
 *   exists, is non-trivial.
 *  \param p_pOther IN A pointer to the other HtnMethod that this might
 *   subsume.
 *  \return Whether or not this subsumes the other.
 *  \todo Break this function up, possibly simplify or optimize it.
 */
bool HtnMethod::Subsumes( const HtnMethod * p_pOther ) const
{
  // Check the simple things
  // *start*

  if( CompareNoCase( GetCHead()->GetName(), p_pOther->GetCHead()->GetName() ) != 0 )
    return false;
  if( GetCHead()->GetNumParams() != p_pOther->GetCHead()->GetNumParams() )
    return false;
  if( GetNumSubtasks() != p_pOther->GetNumSubtasks() )
    return false;

  for( unsigned int i = 0; i < GetNumSubtasks(); i++ )
  {
    if( CompareNoCase( GetCSubtask( i )->GetName(), 
		       p_pOther->GetCSubtask( i )->GetName() ) != 0 )
      return false;
  }

  // *end*

  // Create a substitution from all variables in this to new temporary
  //  variables (l_MySubs), the same for all variables in the other method
  //  (l_OtherSubs), and one from this's new variables to the other's new
  //  variables (l_pMasterSubs).
  // This level of indirection is necessary in case this and the other method
  //  might use some of the same variable names.
  // *start*

  Substitution l_MySubs;
  Substitution l_OtherSubs;
  Substitution l_MasterSubs;

  std::vector< TermVariableP > l_vVars = GetVariables();

  for( unsigned int i = 0; i < l_vVars.size(); i++ )
  {
    if( l_vVars[i]->HasTyping() )
    {
      l_MySubs.AddPair( l_vVars[i],
			g_TermTable.Lookup( MakeTempOldId(), l_vVars[i]->GetTyping() ) );
    }
    else
    {
      l_MySubs.AddPair( l_vVars[i],
			g_TermTable.Lookup( MakeTempOldId() ) );
    }
  }

  l_vVars = p_pOther->GetVariables();

  for( unsigned int i = 0; i < l_vVars.size(); i++ )
  {
    if( l_vVars[i]->HasTyping() )
    {
      l_OtherSubs.AddPair( l_vVars[i],
			   g_TermTable.Lookup( MakeTempNewId(), l_vVars[i]->GetTyping() ) );
    }
    else
    {
      l_OtherSubs.AddPair( l_vVars[i],
			   g_TermTable.Lookup( MakeTempNewId() ) );
    }
  } 

  // *end*

  // Populate l_pMasterSubs with those things that we know must match, and exit
  //  if they do not.
  // *start*

  for( unsigned int i = 0; i < GetCHead()->GetNumParams(); i++ )
  {
    TermP l_pMyParam = GetCHead()->GetCParam( i );
    TermP l_pOtherParam = p_pOther->GetCHead()->GetCParam( i );
    l_pMyParam = l_MySubs.FindIndexByVar( std::tr1::dynamic_pointer_cast< TermVariable >( l_pMyParam ) )->second;
    l_pOtherParam = l_OtherSubs.FindIndexByVar( std::tr1::dynamic_pointer_cast< TermVariable >( l_pOtherParam ) )->second;

    SubMap::const_iterator l_iSubsIndex = l_MasterSubs.FindIndexByVar( std::tr1::dynamic_pointer_cast< TermVariable >( l_pMyParam ) );
    if( l_iSubsIndex == l_MasterSubs.End() )
    {
      l_MasterSubs.AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( l_pMyParam ),
			    l_pOtherParam );
    }
    else
    {
      if( !( *l_iSubsIndex->second == *l_pOtherParam ) )
      {
	return false;
      }
    }
  }

  for( unsigned int i = 0; i < m_vSubtasks.size(); i++ )
  {
    for( unsigned int j = 0; j < m_vSubtasks[i]->GetNumParams(); j++ )
    {
      TermP l_pMyParam = m_vSubtasks[i]->GetCParam( j );
      TermP l_pOtherParam = p_pOther->m_vSubtasks[i]->GetCParam( j );
      l_pMyParam = l_MySubs.FindIndexByVar( std::tr1::dynamic_pointer_cast< TermVariable >( l_pMyParam ) )->second;
      l_pOtherParam = l_OtherSubs.FindIndexByVar( std::tr1::dynamic_pointer_cast< TermVariable >( l_pOtherParam ) )->second;

      SubMap::const_iterator l_iSubsIndex = l_MasterSubs.FindIndexByVar( std::tr1::dynamic_pointer_cast< TermVariable >( l_pMyParam ) );
      if( l_iSubsIndex == l_MasterSubs.End() )
      {
	l_MasterSubs.AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( l_pMyParam ),
			      l_pOtherParam );
      }
      else
      {
	if( !( *l_iSubsIndex->second == *l_pOtherParam ) )
	{
	  return false;
	}
      }
    }
  }

  // *end*

  // Prepare for the real search, which is similar to that used in 
  //  State::GetInstantiations().

  FormulaConjP l_pMyPrecs( std::tr1::dynamic_pointer_cast< FormulaConj >( GetCPreconditions()->AfterSubstitution( l_MySubs, 0 ) ) );
  FormulaConjP l_pOtherPrecs( std::tr1::dynamic_pointer_cast< FormulaConj >( p_pOther->GetCPreconditions()->AfterSubstitution( l_OtherSubs, 0 ) ) );

  FormulaPVec l_vMine;
  FormulaPVec l_vHis;
  for( FormulaPVecCI i = l_pMyPrecs->GetBeginConj();
       i != l_pMyPrecs->GetEndConj();
       i++ )
    l_vMine.push_back( *i );
  for( FormulaPVecCI i = l_pOtherPrecs->GetBeginConj();
       i != l_pOtherPrecs->GetEndConj();
       i++ )
    l_vHis.push_back( *i );

  return CanSubsume( l_vMine, l_vHis, l_MasterSubs );
}

/**
 *  Return a pointer to a substitution that unifies p_pNewEqu with OldEqu 
 *   and is an extension of p_pOldSubs, or NULL if none exists.
 *  \param p_pOldSubs IN A pointer to a partial substitution to extend.
 *  \param p_pNewEqu IN A smart pointer to an equality to which the 
 *   substitution may be applied to yield p_pOldEqu.
 *  \param p_pOldEqu IN A smart pointer to an equality to unify p_pNewEqu to.
 *  \return A pointer to a substitution that fulfills the above properties, or
 *   NULL if none exists.  The caller is responsible for deallocating this
 *   pointer.
 */
Substitution * UnifyEqualities( const Substitution * p_pOldSubs,
				FormulaEquP p_pNewEqu,
				FormulaEquP p_pOldEqu )
{
  if( p_pNewEqu->GetCFirst()->HasTyping() &&
      p_pOldEqu->GetCFirst()->HasTyping() &&
      CompareNoCase( p_pNewEqu->GetCFirst()->GetTyping(), 
		     p_pOldEqu->GetCFirst()->GetTyping() ) != 0 )
    return NULL;

  Substitution * l_pNewSubs = new Substitution( *p_pOldSubs );
  bool l_bAddedOne = false;
  bool l_bStillGood = true;
  if( l_pNewSubs->FindIndexByTerm( std::tr1::dynamic_pointer_cast< TermVariable >( p_pNewEqu->GetCFirst() ) ) == l_pNewSubs->End() )
  {
    l_pNewSubs->AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( p_pNewEqu->GetCFirst() ), p_pOldEqu->GetCFirst() );
    l_bAddedOne = true;
  }
  else if( !( *p_pNewEqu->GetCFirst() == *p_pOldEqu->GetCFirst() ) )
  {
    l_bStillGood = false;
  }
  if( l_pNewSubs->FindIndexByTerm( std::tr1::dynamic_pointer_cast< TermVariable >( p_pNewEqu->GetCSecond() ) ) == l_pNewSubs->End() )
  {
    l_pNewSubs->AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( p_pNewEqu->GetCSecond() ), p_pOldEqu->GetCSecond() );
    l_bAddedOne = true;
  }
  else if( !( *p_pNewEqu->GetCSecond() == *p_pOldEqu->GetCSecond() ) )
  {
    l_bStillGood = false;
  }
  if( l_bAddedOne && l_bStillGood )
    return l_pNewSubs;
  else
    delete l_pNewSubs;
  return NULL;
}

/**
 *  Return a pointer to a substitution that unifies p_pNewPred with OldPred 
 *   and is an extension of p_pOldSubs, or NULL if none exists.
 *  \param p_pOldSubs IN A pointer to a partial substitution to extend.
 *  \param p_pNewPred IN A smart pointer to a predicate to which the 
 *   substitution may be applied to yield p_pOldPred.
 *  \param p_pOldPred IN A smart pointer to a predicate to unify p_pNewPred to.
 *  \return A pointer to a substitution that fulfills the above properties, or
 *   NULL if none exists.  The caller is responsible for deallocating this
 *   pointer.
 */
Substitution * UnifyPredicates( const Substitution * p_pOldSubs,
				FormulaPredP p_pNewPred,
				FormulaPredP p_pOldPred )
{
  if( CompareNoCase( p_pNewPred->GetRelation(), p_pOldPred->GetRelation() ) == 0 )
  {
    Substitution * l_pNewSubs = new Substitution( *p_pOldSubs );
    bool l_bAddedOne = false;
    bool l_bStillGood = true;
    for( unsigned int k = 0; k < p_pNewPred->GetValence() && l_bStillGood; k++ )
    {
      if( l_pNewSubs->FindIndexByTerm( std::tr1::dynamic_pointer_cast< TermVariable >( p_pNewPred->GetCParam( k ) ) ) == l_pNewSubs->End() )
      {
	l_pNewSubs->AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( p_pNewPred->GetCParam( k ) ), p_pOldPred->GetCParam( k ) );
	l_bAddedOne = true;
      }
      else if( !( *p_pNewPred->GetCParam( k ) == *p_pOldPred->GetCParam( k ) ) )
      {
	l_bStillGood = false;
      }
    }
    if( l_bAddedOne && l_bStillGood )
      return l_pNewSubs;
    else
      delete l_pNewSubs;
  }
  return NULL;
}

/**
 *  Retrieve a string containing a textual description of the HtnMethod in the
 *   SHOP formalism.
 *  \return A string containing a textual description of the HtnMethod in the
 *   SHOP formalism.
 *  \todo Change this method name to ToShop.
 */
std::string HtnMethod::ToStr() const
{
  std::string l_sRet;

  l_sRet += "    ( :method ";
  l_sRet += m_pHead->ToStr();

  l_sRet += "\n";

  l_sRet += "      (\n";
  for( FormulaPVecCI i = m_pPreconditions->GetBeginConj();
       i != m_pPreconditions->GetEndConj();
       i++ )
  {
    l_sRet += "        ";
    l_sRet += ( *i )->ToStr();
    l_sRet += "\n";
  }
  l_sRet += "      )\n";

  l_sRet += "      (\n";
  for( unsigned int i = 0; i < m_vSubtasks.size(); i++ )
  {
    l_sRet += "        ";
    l_sRet += m_vSubtasks[i]->ToStr();
    l_sRet += "\n";
  }
  l_sRet += "      )\n";
  l_sRet += "    )\n";

  return l_sRet;
}

/**
 *  Retrieve a string containing a textual description of the HtnMethod in the
 *   PDDL formalism.
 *  \return A string containing a textual description of the HtnMethod in the
 *   PDDL formalism.
 *  \todo Break out the GetVariables part into a separate function.
 */
std::string HtnMethod::ToPddl( long p_iRequirements ) const
{
  std::vector< TermVariableP > l_vVars;
  std::vector< TermVariableP > l_vPrecVars = m_pPreconditions->GetVariables();
  for( unsigned int i = 0; i < l_vPrecVars.size(); i++ )
  {
    bool l_bFound = false;
    for( unsigned int j = 0; j < l_vVars.size() && !l_bFound; j++ )
    {
      if( l_vVars[j]->Equal( *l_vPrecVars[i] ) )
	l_bFound = true;
    }
    for( unsigned int j = 0; j < m_pHead->GetNumParams() && !l_bFound; j++ )
    {
      if( m_pHead->GetCParam( j )->Equal( *l_vPrecVars[i] ) )
	l_bFound = true;
    }
    if( !l_bFound )
      l_vVars.push_back( l_vPrecVars[i] );
  }
  for( unsigned int i = 0; i < m_vSubtasks.size(); i++ )
  {
    for( unsigned int j = 0; j < m_vSubtasks[i]->GetNumParams(); j++ )
    {
      bool l_bFound = false;
      for( unsigned int k = 0; k < l_vVars.size() && !l_bFound; k++ )
      {
	if( l_vVars[k]->Equal( *m_vSubtasks[i]->GetCParam( j ) ) )
	  l_bFound = true;
      }
      for( unsigned int k = 0; k < m_pHead->GetNumParams() && !l_bFound; k++ )
      {
	if( m_pHead->GetCParam( k )->Equal( *m_vSubtasks[i]->GetCParam( j ) ) )
	  l_bFound = true;
      }
      if( !l_bFound )
	l_vVars.push_back( std::tr1::dynamic_pointer_cast< TermVariable >( m_vSubtasks[i]->GetCParam( j ) ) );
    }
  }

  std::string l_sRet = "";

  l_sRet += "  ( :method " + m_pHead->GetName() + "\n";
  l_sRet += "    :parameters\n";
  l_sRet += "    (\n";
  for( unsigned int i = 0; i < m_pHead->GetNumParams(); i++ )
  {
    l_sRet += "      ";
    l_sRet += m_pHead->GetCParam( i )->ToStr();
    l_sRet += "\n";
  }
  l_sRet += "    )\n";
  if( l_vVars.size() > 0 )
  {
    l_sRet += "    :vars\n";
    l_sRet += "    (\n";
    for( unsigned int i = 0; i < l_vVars.size(); i++ )
    {
      l_sRet += "      " + l_vVars[i]->ToStr() + "\n";
    }
    l_sRet += "    )\n";
  }
  l_sRet += "    :precondition\n";
  l_sRet += "    " + m_pPreconditions->ToStrNoTyping() + "\n";
  l_sRet += "    :subtasks\n";
  l_sRet += "    ( ";
  for( unsigned int i = 0; i < m_vSubtasks.size(); i++ )
  {
    if( i != 0 )
      l_sRet += "      ";
    l_sRet += m_vSubtasks[i]->ToStrNoTyping();
    if( i != m_vSubtasks.size() - 1 )
      l_sRet += "\n";
  }
  l_sRet += " )\n";
  if( p_iRequirements & PDDL_REQ_METHOD_IDS )
    l_sRet += "    :id ( " + m_sId + " )\n";
  if( p_iRequirements & PDDL_REQ_QVALUES )
  {
    std::stringstream l_sTemp;
    l_sTemp << "    :q-value ( " << m_fQValue << " )\n";
    l_sTemp << "    :q-count ( " << m_iQCount << " )\n";
    l_sRet += l_sTemp.str();
  }
  l_sRet += "  )\n";

  return l_sRet;
}

/**
 *  Determine whether a vector containing pointers to terms contains a 
 *   particular term instance.
 *  \param p_Vec IN A vector of pointers to terms in which to search.
 *  \param p_pTerm IN A pointer to a term for which to search.
 *  \return Whether the vector contains the search term.
 */
bool VecContainsTerm( const std::vector< TermP > & p_Vec,
		      TermP p_pTerm )
{
  for( unsigned int i = 0; i < p_Vec.size(); i++ )
  {
    if( *p_Vec[i] == *p_pTerm )
      return true;
  }
  return false;
}

/**
 *  Determine whether or not each of the subtasks of this method contains at
 *   least one parameter that also appears in the head or preconditions of the
 *   method.
 *  This is deprecated, and only exists because we once thought this would be
 *   a useful metric.
 *  \return Whether or not each subtask is linked to the head.
 */
bool HtnMethod::SubtasksArePartiallyLinked() const
{
  for( unsigned int i = 0; i < m_vSubtasks.size(); i++ )
  {
    bool l_bFound = false;
    std::vector< TermP > l_vToCheck;
    std::vector< TermP > l_vDone;

    for( unsigned int j = 0; j < m_vSubtasks[i]->GetNumParams(); j++ )
    {
      l_vToCheck.push_back(  m_vSubtasks[i]->GetCParam( j ) );
    }

    while( !l_vToCheck.empty() && !l_bFound )
    {
      TermP l_pCurTerm( l_vToCheck.front() );
      l_vToCheck.erase( l_vToCheck.begin() );
      l_vDone.push_back( l_pCurTerm );

      for( unsigned int j = 0; j < m_pHead->GetNumParams() && !l_bFound; j++ )
      {
	if( *l_pCurTerm == *m_pHead->GetCParam( j ) )
	  l_bFound = true;
      }

      for( FormulaPVecCI j = m_pPreconditions->GetBeginConj();
	   j != m_pPreconditions->GetEndConj() && !l_bFound;
	   j++ )
      {
	if( ( *j )->GetType() != FT_PRED )
	  throw Exception( E_FORMULA_TYPE_UNKNOWN,
			   "This should be a predicate.",
			   __FILE__,
			   __LINE__ );

	FormulaPredP l_pCurPred( std::tr1::dynamic_pointer_cast< FormulaPred >( *j ) );

	if( CompareNoCase( l_pCurPred->GetRelation(), "different" ) == 0 )
	  continue;

	bool l_bPrecContains = false;
	for( unsigned int k = 0; k < l_pCurPred->GetValence() && !l_bPrecContains; k++ )
	{
	  if( *l_pCurTerm == *l_pCurPred->GetCParam( k ) )
	    l_bPrecContains = true;
	}

	for( unsigned int k = 0; k < l_pCurPred->GetValence() && l_bPrecContains; k++ )
	{
	  if( !VecContainsTerm( l_vToCheck, l_pCurPred->GetCParam( k ) ) &&
	      !VecContainsTerm( l_vDone, l_pCurPred->GetCParam( k ) ) )
	    l_vToCheck.push_back( l_pCurPred->GetCParam( k ) );
	}
      }
    }

    if( !l_bFound )
      return false;
  }

  return true;
}

/**
 *  Retrieve a pointer to a new HtnMethod that results from applying a
 *   Substitution to this.
 *  \param p_sSub IN The Substitution to apply.
 *  \param p_iDepth IN The depth of this Substitution attempt.
 *  \return A new HtnMethod that results from applying a Substitution to this.
 *   The caller is responsible for deallocating it.
 */
HtnMethod * HtnMethod::AfterSubstitution( const Substitution & p_sSub,
					  unsigned int p_iDepth ) const
{
  HtnMethod * l_pNew = new HtnMethod( *this );

  l_pNew->m_pHead = std::tr1::dynamic_pointer_cast< HtnTaskHead >( m_pHead->AfterSubstitution( p_sSub, p_iDepth ) );
  l_pNew->m_pPreconditions = std::tr1::dynamic_pointer_cast< FormulaConj >( m_pPreconditions->AfterSubstitution( p_sSub, p_iDepth ) );

  for( unsigned int i = 0; i < m_vSubtasks.size(); i++ )
  {
    l_pNew->m_vSubtasks[i] = std::tr1::dynamic_pointer_cast< HtnTaskHead >( m_vSubtasks[i]->AfterSubstitution( p_sSub, p_iDepth ) );
  }
  return l_pNew;
}

/**
 *  For each subtask that is a non-deterministic operator, add a subtask 
 *   directly after it to guarantee that the desired outcome occurred.
 *  The determinized versions of non-deterministic operators have -xx appended
 *   to their names, where xx is a two digit number.
 *  The checking task is the name of the operator, without the -xx suffix, with
 *   a check- prefix.
 *  Additionally, if this is a checking method itself, it should call itself as
 *   a last subtask.
 */
void HtnMethod::AddNdCheckers()
{
  for( unsigned int i = 0; i < m_vSubtasks.size(); i++ )
  {
    std::string l_sName = m_vSubtasks[i]->GetName();
    int l_iNameLen = l_sName.length();
    if( l_sName[0] == '!' &&
	l_iNameLen > 3 &&
	l_sName[l_iNameLen - 3] == '-' &&
	isdigit( l_sName[l_iNameLen - 2] ) &&
	isdigit( l_sName[l_iNameLen - 1] ) )
    {
      std::string l_sRealName = l_sName.substr( 1, l_iNameLen - 4 );
      std::string l_sChecker = "check-" + l_sRealName;

      std::string l_sCheckHead = "( " + l_sChecker;
      for( unsigned int j = 0; j < m_vSubtasks[i]->GetNumParams(); j++ )
	l_sCheckHead += " " + m_vSubtasks[i]->GetCParam( j )->ToStrNoTyping();
      l_sCheckHead += " )\n";

      HtnTaskHeadP l_pNewTask( new HtnTaskHead( l_sCheckHead,
						m_TypeTable ) );

      if( i == m_vSubtasks.size() - 1 ||
	  !l_pNewTask->Equal( *m_vSubtasks[i+1] ) )
      {
	std::vector< HtnTaskHeadP >::iterator l_Iter = m_vSubtasks.begin();
	for( unsigned int j = 0; j <= i; j++ )
	  l_Iter++;
	m_vSubtasks.insert( l_Iter, l_pNewTask );
      }
    }
  }

  if( GetCHead()->GetName().length() > 6 && CompareNoCase( GetCHead()->GetName().substr( 0, 6 ), "check-" ) == 0 && !GetCHead()->Equal( *m_vSubtasks.back() ) )
  {
    HtnTaskHeadP l_pNewTask( new HtnTaskHead( *GetCHead() ) );
    m_vSubtasks.push_back( l_pNewTask );
  }
}

/**
 *  Retrieve the Q-value of this method.
 *  Currently, this is only used by HTN-Maker for calculating the Q-values of 
 *   other methods.
 *  \return The Q-value of this method.
 */
double HtnMethod::GetQValue() const
{
  return m_fQValue;
}

/**
 *  Retrieve the number of times the Q-value of this method has been updated.
 *  This is needed in order to update the Q-value.
 *  \return The number of times the Q-value of this method has been updated.
 */
int HtnMethod::GetQCount() const
{
  return m_iQCount;
}

/**
 *  Update the Q-value for this method with a new instance.
 *  This is adding the cost of this new instance to a running average.
 *  \param p_fNewCost IN The cost in this new instance.
 */
void HtnMethod::UpdateQValue( double p_fNewCost )
{
  if( m_iQCount == 0 )
    m_fQValue = 0.0;
  m_iQCount++;
  m_fQValue = m_fQValue + ((double)1 / m_iQCount)*( p_fNewCost - m_fQValue );
}

size_t HtnMethod::GetMemSizeMin() const
{
  size_t l_iSize = sizeof( HtnMethod ) + m_sId.capacity() + m_vSubtasks.capacity() * sizeof( HtnTaskHeadP );
  if( m_pVariables )
    l_iSize += m_pVariables->capacity() * sizeof( TermVariableP );
  return l_iSize;
}

size_t HtnMethod::GetMemSizeMax() const
{
  size_t l_iSize = sizeof( HtnMethod ) + m_sId.capacity() + (m_vSubtasks.capacity() - m_vSubtasks.size()) * sizeof( HtnTaskHeadP );
  if( m_pVariables)
    l_iSize += m_pVariables->capacity() * sizeof( TermVariableP );
  for( unsigned int i = 0; i < m_vSubtasks.size(); i++ )
    l_iSize += m_vSubtasks[i]->GetMemSizeMax();
  return l_iSize;
}

const std::string & HtnMethod::GetId() const
{
  return m_sId;
}

void HtnMethod::SetId( const std::string & p_sId )
{
  m_sId = p_sId;
}
