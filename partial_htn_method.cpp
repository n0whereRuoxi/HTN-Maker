#include <vector>
#include <string>
#include <sstream>
#include <set>
#include <tr1/memory>
#include <iostream>

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
#include "formula_equ.hpp"
#include "formula_neg.hpp"
#include "formula_conj.hpp"
#include "operator.hpp"
#include "state.hpp"
#include "htn_task_head.hpp"
#include "htn_task_descr.hpp"
#include "htn_method.hpp"
#include "htn_domain.hpp"
#include "partial_htn_method.hpp"

/** \file partial_htn_method.hpp
 *  Declaration of the PartialHtnMethod class.
 */

/** \file partial_htn_method.cpp
 *  Definition of the PartialHtnMethod class.
 */

/** \class PartialHtnMethod
 *  An HTN method in the midst of being constructed by adding subtasks one at
 *   a time from last to first.
 *  The use of substitutions here is quite complicated, so here is a bit of
 *    documentation:
 *
 *    m_TaskSubs contains a pair linking each variable in the task definition
 *      with a created variable.
 *    m_vOpers->m_pSub contains a pair linking each variable in the operator
 *      definition with a created variable.
 *    m_vMasterSubs contains a pair linking each created variable to a
 *      constant.
 *
 *  Two created variables may refer to the same constant without any special
 *   meaning.  However, when a variable is used in the definition of an
 *   operator and satisfies a precondition of another operator or fulfills an
 *   effect of the task, then it should be linked to the same created variable
 *   as in the operator precondition or task effects.
 *
 *  m_Preconditions are stored in terms of created variables.
 *  m_RemainingAddList are stored in terms of created variables.
 *
 * When creating a new method, it will contain only the created variables.
 */

/** \var PartialHtnMethod::m_pDomain
 *  A smart pointer to the domain from which subtasks of this method may come.
 */

/** \var PartialHtnMethod::m_pTaskDescr
 *  A smart pointer to the task description associated with this method.
 */

/** \var PartialHtnMethod::m_vRemainingAddList
 *  A list of smart pointers to formulas from the add list of the task 
 *   description that have not yet been made true by any subtask.
 *  \todo Is this really necessary, or could they just be part of 
 *   m_vRemainingPrecs?
 */

/** \var PartialHtnMethod::m_vRemainingPrecs
 *  A list of smart pointers to outstanding preconditions of subtasks that 
 *   have not been accomplished by earlier subtasks.
 */

/** \var PartialHtnMethod::m_TaskSubs
 *  A substitution from the variables in the task description to newly created
 *   variables.
 */

/** \var PartialHtnMethod::m_MasterSubs
 *  A substitution from the newly created variables for this method to 
 *   constants.
 */

/** \var PartialHtnMethod::m_vOperators
 *  A list of operators that will be subtasks of the new method.
 *  These must be deallocated with the PartialHtnMethod.
 */

/** \var PartialHtnMethod::m_vOperSubs
 *  A substitution from operator variables to created variables for each 
 *   operator in m_vOperators.
 *  These must be deallocated with the PartialHtnMethod.
 */

/** \var PartialHtnMethod::m_vOperBeforeStates
 *  The state in which each operator in m_vOperators should be executed.  Used
 *   to determine subtask ordering.
 */

/** \var PartialHtnMethod::m_vMethods
 *  A list of methods whose heads will be subtasks of this method.
 *  These must be deallocated with the PartialHtnMethod.
 */

/** \var PartialHtnMethod::m_vMethodSubs
 *  A list of substitutions from method variables to created variables for each
 *   member of m_vMethods.
 *  These must be deallocated with the PartialHtnMethod.
 */

/** \var PartialHtnMethod::m_vMethodEffects
 *  A list of smart pointers to the effects associated with each of the 
 *   methods in m_vMethods, in original method variables.
 */

/** \var PartialHtnMethod::m_vMethodBeforeStates
 *  A list of the states in which each member of m_vMethods starts.
 */

/** \var PartialHtnMethod::m_vMethodAfterStates
 *  A list of the states in which each member of m_vMethods ends.
 */

/** \var PartialHtnMethod::m_iInitStateIndex
 *  The index of the state in some plan where this method begins.
 */

/** \var PartialHtnMethod::m_iFinalStateIndex
 *  The index of the state in some plan where this method ends.
 */

/** \var PartialHtnMethod::m_iCurrentStateIndex
 *  The index of the state we have reached thusfar in the goal regression from
 *   m_iFinalStateIndex to m_iInitStateIndex.
 */

/** \var PartialHtnMethod::m_iTotalCost
 *  The sum of the costs of the new method's subtasks.
 *  This is used for reinforcement learning.
 */

/**
 *  The one and only TermTable.
 */
extern TermTable g_TermTable;

/**
 *  Create a PartialHtnMethod with no subtasks.
 *  \param p_pDomain IN A smart pointer to the domain from which subtasks will
 *   come.
 *  \param p_pTaskDescr IN A smart pointer to a task description for this 
 *   method.
 *  \param p_pTaskSubs IN A substitution from the variables in the task
 *   description to constants from the problem.
 *  \param p_iFinalStateIndex IN The index of the state in the problem where
 *   the effects of the annotated task were satisfied.
 */
PartialHtnMethod::PartialHtnMethod( const std::tr1::shared_ptr< HtnDomain > & p_pDomain,
				    const std::tr1::shared_ptr< HtnTaskDescr > & p_pTaskDescr,
				    const Substitution * p_pTaskSubs,
				    unsigned int p_iFinalStateIndex )
  : m_pDomain( p_pDomain ),
    m_pTaskDescr( p_pTaskDescr )
{
  for( SubMap::const_iterator l_iIter = p_pTaskSubs->Begin(); 
       l_iIter != p_pTaskSubs->End(); 
       l_iIter++ )
  {
    TermVariableP l_pVar;
    if( l_iIter->first->HasTyping() )
      l_pVar = std::tr1::dynamic_pointer_cast< TermVariable >( g_TermTable.Lookup( MakeVarId(), l_iIter->first->GetTyping() ) );
    else
      l_pVar = std::tr1::dynamic_pointer_cast< TermVariable >( g_TermTable.Lookup( MakeVarId() ) );
    m_TaskSubs.AddPair( l_iIter->first, l_pVar );
    m_MasterSubs.AddPair( l_pVar, l_iIter->second );
  }

  FormulaConjP l_pAfterTaskSubs( std::tr1::dynamic_pointer_cast< FormulaConj >( m_pTaskDescr->GetCEffects()->AfterSubstitution( m_TaskSubs, 0 ) ) );

  for( FormulaPVecCI i = l_pAfterTaskSubs->GetBeginConj();
       i != l_pAfterTaskSubs->GetEndConj();
       i++ )
  {
    bool l_bFound = false;
    for( unsigned int j = 0; j < m_vRemainingAddList.size(); j++ )
    {
      if( m_vRemainingAddList[j]->Equal( *( *i ) ) )
	l_bFound = true;
    }
    if( !l_bFound )
      m_vRemainingAddList.push_back( *i );
  }

  m_iFinalStateIndex = p_iFinalStateIndex;
  m_iCurrentStateIndex = p_iFinalStateIndex;
  m_iTotalCost = 0;
}

/**
 *  Construct a PartialHtnMethod as a copy of an existing one.
 *  \param p_Other IN The PartialHtnMethod to copy.
 */
PartialHtnMethod::PartialHtnMethod( const PartialHtnMethod & p_Other )
  : m_pDomain( p_Other.m_pDomain ),
    m_pTaskDescr( p_Other.m_pTaskDescr ),
    m_vRemainingAddList( p_Other.m_vRemainingAddList ),
    m_vRemainingPrecs( p_Other.m_vRemainingPrecs ),
    m_TaskSubs( p_Other.m_TaskSubs ), 
    m_MasterSubs( p_Other.m_MasterSubs ),
    m_vOperBeforeStates( p_Other.m_vOperBeforeStates ),
    m_vMethodBeforeStates( p_Other.m_vMethodBeforeStates ),
    m_vMethodAfterStates( p_Other.m_vMethodAfterStates ),
    m_iTotalCost( p_Other.m_iTotalCost )
{
  for( unsigned int i = 0; i < p_Other.m_vOperators.size(); i++ )
    m_vOperators.push_back( new Operator( *p_Other.m_vOperators[i] ) );

  for( unsigned int i = 0; i < p_Other.m_vOperSubs.size(); i++ )
    m_vOperSubs.push_back( new Substitution( *p_Other.m_vOperSubs[i] ) );

  for( unsigned int i = 0; i < p_Other.m_vMethods.size(); i++ )
    m_vMethods.push_back( new HtnMethod( *p_Other.m_vMethods[i] ) );

  for( unsigned int i = 0; i < p_Other.m_vMethodSubs.size(); i++ )
    m_vMethodSubs.push_back( new Substitution( *p_Other.m_vMethodSubs[i] ) );

  m_iInitStateIndex = p_Other.m_iInitStateIndex;
  m_iFinalStateIndex = p_Other.m_iFinalStateIndex;
  m_iCurrentStateIndex = p_Other.m_iCurrentStateIndex;
}

/**
 *  Destruct a PartialHtnMethod.
 */
PartialHtnMethod::~PartialHtnMethod()
{
  for( unsigned int i = 0; i < m_vOperators.size(); i++ )
    delete m_vOperators[i];

  for( unsigned int i = 0; i < m_vOperSubs.size(); i++ )
    delete m_vOperSubs[i];

  for( unsigned int i = 0; i < m_vMethods.size(); i++ )
    delete m_vMethods[i];

  for( unsigned int i = 0; i < m_vMethodSubs.size(); i++ )
    delete m_vMethodSubs[i];
}

/**
 *  Determine whether or not a formula includes any of the oustanding 
 *   preconditions in m_vRemainingPrecs.
 *  \todo Constantly recreating and deleting the grounded versions of this are
 *   ridiculous, and should probably be optimized away.
 *  \param p_pForm IN A smart pointer to the formula that might supply a 
 *   precondition.
 *  \return Whether or not p_pForm supplies an open precondition.
 */
bool PartialHtnMethod::SuppliesPrec( const FormulaConjP & p_pForm ) const
{
  for( unsigned int i = 0; i < m_vRemainingPrecs.size(); i++ )
  {
    FormulaP l_pPreElementNoVars( m_vRemainingPrecs[ i ]->AfterSubstitution( m_MasterSubs, 0 ) );
    bool l_bFound = p_pForm->Implies( l_pPreElementNoVars );
    if( l_bFound )
      return true;
  }
  return false;
}

/**
 *  Determine whether or not a formula includes any of the remaining 
 *   effects in m_vRemainingAddList.
 *  \todo Constantly recreating and deleting the grounded versions of this are
 *   ridiculous, and should probably be optimized away.
 *  \param p_pForm IN A smart pointer to the formula that might supply an 
 *   effect.
 *  \return Whether or not p_pForm supplies an open effect.
 */
bool PartialHtnMethod::SuppliesEffect( const FormulaConjP & p_pForm ) const
{
  for( unsigned int i = 0; i < m_vRemainingAddList.size(); i++ )
  {
    FormulaP l_pAddElementNoVars( m_vRemainingAddList[ i ]->AfterSubstitution( m_MasterSubs, 0 ) );
    bool l_bFound = p_pForm->Implies( l_pAddElementNoVars );
    if( l_bFound )
      return true;
  }
  return false;
}

/**
 *  Determine whether or not a formula provides a precondition that is not also
 *   provided by some other formula.
 *  This is used to guarantee that a subtask is actually useful; that it does
 *   supplies something that it does not also require.
 *  \param p_pForm IN A smart pointer to the formula that should provide new 
 *   preconditions.
 *  \param p_pOld IN A smart pointer to the formula that may have already 
 *   provided those preconditions.
 *  Return Whether or not p_pForm provides preconditions not in p_pOld.
 */
bool PartialHtnMethod::SuppliesNewPrec( const FormulaConjP & p_pForm,
					const FormulaConjP & p_pOld ) const
{
  for( unsigned int i = 0; i < m_vRemainingPrecs.size(); i++ )
  {
    FormulaP l_pPreElementNoVars( m_vRemainingPrecs[ i ]->AfterSubstitution( m_MasterSubs, 0 ) );
    bool l_bFound = p_pForm->Implies( l_pPreElementNoVars ) && !p_pOld->Implies( l_pPreElementNoVars );
    if( l_bFound )
      return true;
  }
  return false;
}

/**
 *  Determine whether or not a formula provides an effect that is not also
 *   provided by some other formula.
 *  This is used to guarantee that a subtask is actually useful; that it does
 *   supplies something that it does not also require.
 *  \param p_pForm IN A smart pointer to the formula that should provide new 
 *   effects.
 *  \param p_pOld IN A smart pointer to the formula that may have already 
 *   provided those effects.
 *  Return Whether or not p_pForm provides effects not in p_pOld.
 */
bool PartialHtnMethod::SuppliesNewEffect( const FormulaConjP & p_pForm,
					  const FormulaConjP & p_pOld ) const
{
  for( unsigned int i = 0; i < m_vRemainingAddList.size(); i++ )
  {
    FormulaP l_pAddElementNoVars( m_vRemainingAddList[ i ]->AfterSubstitution( m_MasterSubs, 0 ) );
    bool l_bFound = p_pForm->Implies( l_pAddElementNoVars ) && !p_pOld->Implies( l_pAddElementNoVars );
    if( l_bFound )
      return true;
  }
  return false;
}

/**
 *  Determine whether or not an operator starting at a specified state 
 *   conflicts with another subtask.
 *  Operators and methods added have associated before and after states, none
 *   of which should overlap.
 *  \param p_iBeforeStateNum IN The state where the operator starts.
 *  \return Whether or not the operator conflicts with anything.
 */
bool PartialHtnMethod::DoesOperatorConflict( unsigned int p_iBeforeStateNum ) const
{
  for( unsigned int i = 0; i < m_vOperBeforeStates.size(); i++ )
  {
    if( p_iBeforeStateNum == m_vOperBeforeStates[i] )
      return true;
  }

  for( unsigned int i = 0; i < m_vMethods.size(); i++ )
  {
    if( p_iBeforeStateNum >= m_vMethodBeforeStates[i] &&
	p_iBeforeStateNum < m_vMethodAfterStates[i] )
      return true;
  }
  return false;
}

/**
 *  Determine whether or not a method starting at a specified state 
 *   conflicts with another subtask.
 *  Operators and methods added have associated before and after states, none
 *   of which should overlap.
 *  \param p_iBeforeStateNum IN The state where the method starts.
 *  \param p_iAfterStateNum IN The state where the method ends.
 *  \return Whether or not the method conflicts with anything.
 */
bool PartialHtnMethod::DoesMethodConflict( unsigned int p_iBeforeStateNum,
					   unsigned int p_iAfterStateNum ) const
{
  for( unsigned int i = 0; i < m_vOperBeforeStates.size(); i++ )
  {
    for( unsigned int j = p_iBeforeStateNum; j < p_iAfterStateNum; j++ )
    {
      if( j == m_vOperBeforeStates[i] )
      {
	return true;
      }
    }
  }

  for( unsigned int i = 0; i < m_vMethods.size(); i++ )
  {
    for( unsigned int j = p_iBeforeStateNum; j < p_iAfterStateNum; j++ )
    {
      if( j >= m_vMethodBeforeStates[i] &&
	  j < m_vMethodAfterStates[i] )
	return true;
    }
  }
  return false;
}

/**
 *  Given a substitution from schema vars to constants, create vars to replace
 *   them.
 *  This also updates the master substitution list to include new created to
 *   constant pairs.
 *  \param p_pRealSub IN A pointer to the substitution from schema vars to 
 *   constants.
 *  \param p_NewSub INOUT An initially empty substitution that will be filled
 *   with created vars to schema vars.
 *  \param p_bPartialGeneralization IN Whether or not variables that refer to
 *   the same constant will be forced to have the same name.
 */
void PartialHtnMethod::CreateNewVars( const Substitution * p_pRealSub,
				      Substitution & p_NewSub,
				      bool p_bPartialGeneralization )
{

  if( p_bPartialGeneralization )
  {
    for( SubMap::const_iterator l_iIter = p_pRealSub->Begin(); 
	 l_iIter != p_pRealSub->End(); 
	 l_iIter++ )
    {
      SubMap::const_iterator l_iOldIndex = m_MasterSubs.FindIndexByTerm( l_iIter->second );
      if( l_iOldIndex == m_MasterSubs.End() )
      {
        TermVariableP l_pVar;
	if( l_iIter->first->HasTyping() )
	  l_pVar = std::tr1::dynamic_pointer_cast< TermVariable >( g_TermTable.Lookup( MakeVarId(), l_iIter->first->GetTyping() ) );
	else
	  l_pVar = std::tr1::dynamic_pointer_cast< TermVariable >( g_TermTable.Lookup( MakeVarId() ) );
	p_NewSub.AddPair( l_iIter->first, l_pVar );
	m_MasterSubs.AddPair( l_pVar, l_iIter->second );
      }
      else
      {
	p_NewSub.AddPair( l_iIter->first, l_iOldIndex->first );
      }
    }
  }
  else
  {
    // Substitute in created variables for those from the definition.
    for( SubMap::const_iterator l_iIter = p_pRealSub->Begin(); 
	 l_iIter != p_pRealSub->End(); 
	 l_iIter++ )
    {
      TermVariableP l_pVar;
      if( l_iIter->first->HasTyping() )
	l_pVar = std::tr1::dynamic_pointer_cast< TermVariable >( g_TermTable.Lookup( MakeVarId(), l_iIter->first->GetTyping() ) );
      else
	l_pVar = std::tr1::dynamic_pointer_cast< TermVariable >( g_TermTable.Lookup( MakeVarId() ) );
      p_NewSub.AddPair( l_iIter->first, l_pVar );
      m_MasterSubs.AddPair( l_pVar, l_iIter->second );
    }
  }
}

/**
 *  Process a list of effects, removing any oustanding preconditions or 
 *   remaining effects that can be unified to them.
 *  \param p_pEffects IN A smart pointer to the list of effects in schema vars.
 *  \param p_pRealSub IN A pointer to a substitution from schema vars to 
 *   consts.
 *  \param p_bPartialGeneralization IN Whether or not two variables that refer
 *   to the same constant should be merged.
 *  \param p_Subst INOUT A substitution from schema vars to created vars.  If
 *   unification is performed, this may be changed.
 */
void PartialHtnMethod::ProcessEffects( const FormulaConjP & p_pEffects,
				       const Substitution * p_pRealSub,
				       bool p_bPartialGeneralization,
				       Substitution & p_Subst )
{
  // Process each member of the add list, which may provide one of the 
  //   effects of the task or fulfill a precondition of a later operator or
  //   method.
  for( FormulaPVecCI i = p_pEffects->GetBeginConj();
       i != p_pEffects->GetEndConj();
       i++ )
  {
    // Equality formulas may never be added, and we do not care about
    //  negative effects.
    if( ( *i )->GetType() != FT_PRED )
      continue;

    if( p_bPartialGeneralization )
    {
      FormulaPredP l_pOpAdd( std::tr1::dynamic_pointer_cast< FormulaPred >( ( *i )->AfterSubstitution( p_Subst, 0 ) ) );

      for( unsigned int j = 0; j < m_vRemainingAddList.size(); j++ )
      {
	if( *l_pOpAdd == *m_vRemainingAddList[ j ] )
	{
	  std::vector< FormulaP >::iterator l_Iter = m_vRemainingAddList.begin();
	  for( unsigned int k = 0; k < j; k++ )
	    l_Iter++;
	  m_vRemainingAddList.erase( l_Iter );
	  j--;
	}
      }
      for( unsigned int j = 0; j < m_vRemainingPrecs.size(); j++ )
      {
	if( *l_pOpAdd == *m_vRemainingPrecs[ j ] )
	{
	  std::vector< FormulaP >::iterator l_Iter = m_vRemainingPrecs.begin();
	  for( unsigned int k = 0; k < j; k++ )
	    l_Iter++;
	  m_vRemainingPrecs.erase( l_Iter );
	  j--;
	}
      }
    }
    else
    {
      FormulaPredP l_pOpAdd( std::tr1::dynamic_pointer_cast< FormulaPred >( ( *i )->AfterSubstitution( *p_pRealSub, 0 ) ) );

      // Check each of the task effects to see if this satisfies it.
      for( unsigned int j = 0; j < m_vRemainingAddList.size(); j++ )
      {
	FormulaP l_pAddElementNoVars( m_vRemainingAddList[ j ]->AfterSubstitution( m_MasterSubs, 0 ) );

	// If this does solve a task effect, make the variables the same.
	if( *l_pOpAdd == *l_pAddElementNoVars )
	{
	  DoAddListUnification( *i, j, &p_Subst );
	  j = -1;
	}
      }

      // Check each of the preconditions of later operators and methods to see
      //   if this satisfies it.
      for( unsigned int j = 0; j < m_vRemainingPrecs.size(); j++ )
      {
	FormulaP l_pPreElementNoVars( m_vRemainingPrecs[ j ]->AfterSubstitution( m_MasterSubs, 0 ) );

	// If this does solve a precondition, make the variables the same.
	if( *l_pOpAdd == *l_pPreElementNoVars )
	{
	  DoPrecondUnification( std::tr1::dynamic_pointer_cast< FormulaPred >( *i ), j, &p_Subst );
	  j = -1;
	}
      }
    }
  }
}

/**
 *  Add the preconditions of a new subtask to the list of outstanding ones.
 *  \param p_pPrecs IN A smart pointer to the preconditions of the new 
 *   subtask, in schema variables.
 *  \param p_Subst IN A substitution from schema vars to created vars.
 */
void PartialHtnMethod::ProcessPreconditions( const FormulaConjP & p_pPrecs,
					     const Substitution & p_Subst )
{

  for( FormulaPVecCI i = p_pPrecs->GetBeginConj();
       i != p_pPrecs->GetEndConj();
       i++ )
  {
    FormulaP l_pAfter( ( *i )->AfterSubstitution( p_Subst, 0 ) );
    switch( l_pAfter->GetType() )
    {
    case FT_NEG:
      if( std::tr1::dynamic_pointer_cast< FormulaNeg >( l_pAfter )->GetCNegForm()->GetType() != FT_EQU )
	throw Exception( E_NOT_IMPLEMENTED,
			 "Negative preconditions (other than equalities) are not allowed.",
			 __FILE__,
			 __LINE__ );
      // break intentionally left out.
    case FT_PRED:
    case FT_EQU:
      {
	bool l_bFound = false;
	for( unsigned int j = 0; j < m_vRemainingPrecs.size() && !l_bFound; j++ )
	{
	  if( *m_vRemainingPrecs[ j ] == *l_pAfter )
	  {
	    l_bFound = true;
	  }
	}
	if( !l_bFound )
	{
	  m_vRemainingPrecs.push_back( l_pAfter );
	}
      }
      break;
    case FT_CONJ:
      throw Exception ( E_NOT_IMPLEMENTED,
			"A conjunction was not in simplest form.",
			__FILE__,
			__LINE__ );
      break;
    default:
      throw Exception( E_FORMULA_TYPE_UNKNOWN,
		       "Unknown formula type.",
		       __FILE__,
		       __LINE__ );
    }
  }
}

/**
 *  Add an operator as a subtask of this method.
 *  \param p_pOp IN A pointer to the operator to add.
 *  \param p_pRealSub IN A substitution from variables in the operator to
 *   constants in the problem.
 *  \param p_iBeforeStateNum IN The index of the state in the plan from which
 *   this operator was taken.
 *  \param p_bPartialGeneralization IN Whether or not variables that refer to
 *   different constants should be allowed to remain distinct variables.
 */
void PartialHtnMethod::AddOperator( const Operator * p_pOp, 
				    const Substitution * p_pRealSub,
				    unsigned int p_iBeforeStateNum,
				    bool p_bPartialGeneralization )
{
  if( p_iBeforeStateNum != m_iCurrentStateIndex - 1 )
    throw Exception( E_OPER_OVERLAP,
		     "Operator added to partial method in wrong order.",
		     __FILE__,
		     __LINE__ );

  if( DoesOperatorConflict( p_iBeforeStateNum ) )
    throw Exception( E_OPER_OVERLAP,
		     "Operator added to partial method conflicts with an existing operator or method.",
		     __FILE__,
		     __LINE__ );

  Substitution l_Subst;

  CreateNewVars( p_pRealSub,
		 l_Subst,
		 p_bPartialGeneralization );

  ProcessEffects( p_pOp->GetCEffects(),
		  p_pRealSub,
		  p_bPartialGeneralization,
		  l_Subst );

  ProcessPreconditions( p_pOp->GetCPreconditions(),
			l_Subst );

  m_vOperators.push_back( new Operator( *p_pOp ) );
  m_vOperSubs.push_back( new Substitution( l_Subst ) );
  m_vOperBeforeStates.push_back( p_iBeforeStateNum );

  m_iCurrentStateIndex = p_iBeforeStateNum;
  m_iTotalCost += p_pOp->GetCost();
}

/**
 *  Add the head of a method as a subtask of this method.
 *  \param p_pMethod IN A pointer to the method to add.
 *  \param p_pRealSub IN A substitution from variables in the method to
 *   constants in the problem.
 *  \param p_iBeforeStateNum IN The index of the state in the plan from which
 *   this method began.
 *  \param p_iAfterStateNum IN The index of the state in the plan at which this
 *   method ended.
 *  \param p_pEffects IN A smart pointer to the effects associated with the 
 *   method.
 *  \param p_bPartialGeneralization IN Whether or not variables that refer to
 *   different constants should be allowed to remain distinct variables.
 *  \param p_iCost IN The cost associated with decomposing this method, used 
 *   for calculating a Q-value for the new method.
 */
void PartialHtnMethod::AddMethod( const HtnMethod * p_pMethod,
				  const Substitution * p_pRealSub,
				  unsigned int p_iBeforeStateNum,
				  unsigned int p_iAfterStateNum,
				  const FormulaConjP & p_pEffects,
				  bool p_bPartialGeneralization,
				  int p_iCost )
{
  if( p_iAfterStateNum != m_iCurrentStateIndex )
    throw Exception( E_OPER_OVERLAP,
		     "Method is added to partial out of order.",
		     __FILE__,
		     __LINE__ );

  if( DoesMethodConflict( p_iBeforeStateNum,
			  p_iAfterStateNum ) )
    throw Exception( E_OPER_OVERLAP,
		     "Method added to partial method conflicts with an existing operator or method.",
		     __FILE__,
		     __LINE__ );

  Substitution l_Subst;

  CreateNewVars( p_pRealSub,
		 l_Subst,
		 p_bPartialGeneralization );

  ProcessEffects( p_pEffects,
		  p_pRealSub,
		  p_bPartialGeneralization,
		  l_Subst );

  ProcessPreconditions( p_pMethod->GetCPreconditions(),
			l_Subst );

  m_vMethods.push_back( new HtnMethod( *p_pMethod ) );
  m_vMethodSubs.push_back( new Substitution( l_Subst ) );
  m_vMethodBeforeStates.push_back( p_iBeforeStateNum );
  m_vMethodAfterStates.push_back( p_iAfterStateNum );
  m_vMethodEffects.push_back( p_pEffects );

  m_iCurrentStateIndex = p_iBeforeStateNum;
  m_iTotalCost += p_iCost;
}

/**
 *  Replace certain variables in the remaining preconditions and effects.
 *  This is used when the effects of a subtask can be unified with a
 *   remaining effect or precondition.
 *  \param p_ReplaceSub IN The substitution to apply.
 */
void PartialHtnMethod::UpdateAddsAndPrecs( const Substitution & p_ReplaceSub )
{
  std::vector< FormulaP > l_vTempAddList( m_vRemainingAddList );
  m_vRemainingAddList.clear();
  for( unsigned int i = 0; i < l_vTempAddList.size(); i++ )
  {
    FormulaP l_pAfter( l_vTempAddList[i]->AfterSubstitution( p_ReplaceSub, 0 ) );
    bool l_bFound = false;
    for( unsigned int j = 0; j < m_vRemainingAddList.size() && !l_bFound; j++ )
    {
      if( l_pAfter->Equal( *m_vRemainingAddList[j] ) )
	l_bFound = true;
    }
    if( !l_bFound )
      m_vRemainingAddList.push_back( l_pAfter );
  }

  std::vector< FormulaP > l_vTempPrecs( m_vRemainingPrecs );
  m_vRemainingPrecs.clear();
  for( unsigned int i = 0; i < l_vTempPrecs.size(); i++ )
  {
    FormulaP l_pAfter( l_vTempPrecs[i]->AfterSubstitution( p_ReplaceSub, 0 ) );
    bool l_bFound = false;
    for( unsigned int j = 0; j < m_vRemainingPrecs.size() && !l_bFound; j++ )
    {
      if( l_pAfter->Equal( *m_vRemainingPrecs[j] ) )
	l_bFound = true;
    }
    if( !l_bFound )
      m_vRemainingPrecs.push_back( l_pAfter );
  }
}

/**
 *  Replace one created variable with a new one in various substitutions.
 *  This is used when an effect of a subtask can unify to an outstanding
 *   effect or precondition.
 *  \param p_pOldVar IN A pointer to the created variable to be replaced.
 *  \param p_pNewVar In A pointer to the new created variable.
 *  \param p_pSub INOUT A pointer to a substitution that will eventually be
 *   from the variables in an operator or method schema to created variables.
 *  \param p_ReplaceSub INOUT A substitution that will eventually contain all
 *   pairs of old-new variables and can be applied to outstanding preconditions
 *   and effects.
 */
void PartialHtnMethod::DoAddListTermReplacement( const TermVariableP & p_pOldVar,
						 const TermVariableP & p_pNewVar,
						 Substitution * p_pSub,
						 Substitution & p_ReplaceSub )
{
  switch( p_pOldVar->GetType() )
  {
  case TT_CONSTANT:
    break;
  case TT_VARIABLE:
    {
      TermVariableP l_pOldTerm = std::tr1::dynamic_pointer_cast< TermVariable >( p_pSub->FindIndexByVar( p_pOldVar )->second );
      TermVariableP l_pNewTerm = p_pNewVar;
      if( !( *l_pOldTerm == *l_pNewTerm ) )
      {
	p_pSub->ReplaceTerm( l_pOldTerm, l_pNewTerm );
	m_MasterSubs.ReplaceTerm( l_pOldTerm, l_pNewTerm );
	m_TaskSubs.ReplaceTerm( l_pOldTerm, l_pNewTerm );
	p_ReplaceSub.AddPair( l_pOldTerm, l_pNewTerm );
	for( unsigned int j = 0; j < m_vOperSubs.size(); j++ )
	  m_vOperSubs[j]->ReplaceTerm( l_pOldTerm, l_pNewTerm );
	for( unsigned int j = 0; j < m_vMethodSubs.size(); j++ )
	  m_vMethodSubs[j]->ReplaceTerm( l_pOldTerm, l_pNewTerm );
      }
    }
    break;
  default:
    throw Exception( E_TERM_TYPE_UNKNOWN,
		     "Unknown term type.",
		     __FILE__,
		     __LINE__ );
  }
}

/**
 *  Unify the effect of a subtask with a remaining effect, removing it.
 *  \param p_pNewEffects IN A smart pointer to the effect of the subtask.
 *  \param p_iTaskAddListIndex IN An index of the matching entry in the
 *   m_vRemainingAddList.
 *  \param p_pSub INOUT A substitution for the subtask, which will be altered.
 */
void PartialHtnMethod::DoAddListUnification( const FormulaP & p_pNewEffects,
					     unsigned int p_iTaskAddListIndex,
					     Substitution * p_pSub )
{
  Substitution l_ReplaceSub;

  if( p_pNewEffects->GetType() != m_vRemainingAddList[ p_iTaskAddListIndex ]->GetType() )
    throw Exception( E_NOT_IMPLEMENTED,
		     "Formulas are not of the same type.",
		     __FILE__,
		     __LINE__ );

  switch( p_pNewEffects->GetType() )
  {
  case FT_PRED:
    {
      for( unsigned int i = 0; i < std::tr1::dynamic_pointer_cast< FormulaPred >( p_pNewEffects )->GetValence(); i++ )
      {
	TermVariableP l_pOldVar = std::tr1::dynamic_pointer_cast< TermVariable >( std::tr1::dynamic_pointer_cast< FormulaPred >( p_pNewEffects )->GetCParam( i ) );
	TermVariableP l_pNewVar = std::tr1::dynamic_pointer_cast< TermVariable >( std::tr1::dynamic_pointer_cast< FormulaPred >( m_vRemainingAddList[ p_iTaskAddListIndex ] )->GetCParam( i ) );

	DoAddListTermReplacement( l_pOldVar,
				  l_pNewVar,
				  p_pSub,
				  l_ReplaceSub );
      }
    }
    break;
  case FT_CONJ:
    throw Exception( E_NOT_IMPLEMENTED,
		     "A conjunct was not in simplest form.",
		     __FILE__,
		     __LINE__ );
  case FT_NEG:
    {
      if( std::tr1::dynamic_pointer_cast< FormulaNeg >( p_pNewEffects )->GetCNegForm()->GetType() != FT_EQU ||
	  std::tr1::dynamic_pointer_cast< FormulaNeg >( m_vRemainingAddList[ p_iTaskAddListIndex ] )->GetCNegForm()->GetType() != FT_EQU )
	throw Exception( E_NOT_IMPLEMENTED,
			 "Only equality formulas may be negated in preconditions.",
			 __FILE__,
			 __LINE__ );

      TermVariableP l_pOldVar = std::tr1::dynamic_pointer_cast< TermVariable >( std::tr1::dynamic_pointer_cast< FormulaEqu >( std::tr1::dynamic_pointer_cast< FormulaNeg >( p_pNewEffects )->GetCNegForm() )->GetCFirst() );
      TermVariableP l_pNewVar = std::tr1::dynamic_pointer_cast< TermVariable >( std::tr1::dynamic_pointer_cast< FormulaEqu >( std::tr1::dynamic_pointer_cast< FormulaNeg >( m_vRemainingAddList[ p_iTaskAddListIndex ] )->GetCNegForm() )->GetCFirst() );

      DoAddListTermReplacement( l_pOldVar,
				l_pNewVar,
				p_pSub,
				l_ReplaceSub );

      l_pOldVar = std::tr1::dynamic_pointer_cast< TermVariable >( std::tr1::dynamic_pointer_cast< FormulaEqu >( std::tr1::dynamic_pointer_cast< FormulaNeg >( p_pNewEffects )->GetCNegForm() )->GetCSecond() );
      l_pNewVar = std::tr1::dynamic_pointer_cast< TermVariable >( std::tr1::dynamic_pointer_cast< FormulaEqu >( std::tr1::dynamic_pointer_cast< FormulaNeg >( m_vRemainingAddList[ p_iTaskAddListIndex ] )->GetCNegForm() )->GetCSecond() );

      DoAddListTermReplacement( l_pOldVar,
				l_pNewVar,
				p_pSub,
				l_ReplaceSub );
    }
    break;
  case FT_EQU:
    {
      TermVariableP l_pOldVar = std::tr1::dynamic_pointer_cast< TermVariable >( std::tr1::dynamic_pointer_cast< FormulaEqu >( p_pNewEffects )->GetCFirst() );
      TermVariableP l_pNewVar = std::tr1::dynamic_pointer_cast< TermVariable >( std::tr1::dynamic_pointer_cast< FormulaEqu >( m_vRemainingAddList[ p_iTaskAddListIndex ] )->GetCFirst() );

      DoAddListTermReplacement( l_pOldVar,
				l_pNewVar,
				p_pSub,
				l_ReplaceSub );

      l_pOldVar = std::tr1::dynamic_pointer_cast< TermVariable >( std::tr1::dynamic_pointer_cast< FormulaEqu >( p_pNewEffects )->GetCSecond() );
      l_pNewVar = std::tr1::dynamic_pointer_cast< TermVariable >( std::tr1::dynamic_pointer_cast< FormulaEqu >( m_vRemainingAddList[ p_iTaskAddListIndex ] )->GetCSecond() );

      DoAddListTermReplacement( l_pOldVar,
				l_pNewVar,
				p_pSub,
				l_ReplaceSub );
    }
    break;
  default:
    throw Exception( E_NOT_IMPLEMENTED,
		     "Formula is of unknown type.",
		     __FILE__,
		     __LINE__ );
  }

  std::vector< FormulaP >::iterator l_Iter = m_vRemainingAddList.begin();
  for( unsigned int i = 0; i < p_iTaskAddListIndex; i++ )
  {
    l_Iter++;
  }
  m_vRemainingAddList.erase( l_Iter );
  UpdateAddsAndPrecs( l_ReplaceSub );
}

/**
 *  Unify an effect of a subtask with an oustanding precondition, which is 
 *   removed.
 *  \param p_pNewEffects IN A smart pointer to the subtask effect.
 *  \param p_iPrecondIndex IN The index of the matching entry in 
 *   m_vRemainingPrecs.
 *  \param p_pSub INOUT A substitution for the subtask, which will be altered.
 */
void PartialHtnMethod::DoPrecondUnification( const FormulaPredP & p_pNewEffects,
					     unsigned int p_iPrecondIndex,
					     Substitution * p_pSub )
{
  Substitution l_ReplaceSub;

  if( p_pNewEffects->GetType() != m_vRemainingPrecs[ p_iPrecondIndex ]->GetType() )
    throw Exception( E_NOT_IMPLEMENTED,
		     "Formulas are not of the same type.",
		     __FILE__,
		     __LINE__ );

  switch( p_pNewEffects->GetType() )
  {
  case FT_PRED:
    {
      for( unsigned int i = 0; i < std::tr1::dynamic_pointer_cast< FormulaPred >( p_pNewEffects )->GetValence(); i++ )
      {
	TermVariableP l_pOldVar = std::tr1::dynamic_pointer_cast< TermVariable >( std::tr1::dynamic_pointer_cast< FormulaPred >( p_pNewEffects )->GetCParam( i ) );
	TermVariableP l_pNewVar = std::tr1::dynamic_pointer_cast< TermVariable >( std::tr1::dynamic_pointer_cast< FormulaPred >( m_vRemainingPrecs[ p_iPrecondIndex ] )->GetCParam( i ) );

	DoAddListTermReplacement( l_pOldVar,
				  l_pNewVar,
				  p_pSub,
				  l_ReplaceSub );
      }
    }
    break;
  case FT_CONJ:
    throw Exception( E_NOT_IMPLEMENTED,
		     "A conjunct was not in simplest form.",
		     __FILE__,
		     __LINE__ );
  case FT_NEG:
    {
      if( std::tr1::dynamic_pointer_cast< FormulaNeg >( p_pNewEffects )->GetCNegForm()->GetType() != FT_EQU ||
	  std::tr1::dynamic_pointer_cast< FormulaNeg >( m_vRemainingPrecs[ p_iPrecondIndex ] )->GetCNegForm()->GetType() != FT_EQU )
	throw Exception( E_NOT_IMPLEMENTED,
			 "Only equality formulas may be negated in preconditions.",
			 __FILE__,
			 __LINE__ );

      TermVariableP l_pOldVar = std::tr1::dynamic_pointer_cast< TermVariable >( std::tr1::dynamic_pointer_cast< FormulaEqu >( std::tr1::dynamic_pointer_cast< FormulaNeg >( p_pNewEffects )->GetCNegForm() )->GetCFirst() );
      TermVariableP l_pNewVar = std::tr1::dynamic_pointer_cast< TermVariable >( std::tr1::dynamic_pointer_cast< FormulaEqu >( std::tr1::dynamic_pointer_cast< FormulaNeg >( m_vRemainingPrecs[ p_iPrecondIndex ] )->GetCNegForm() )->GetCFirst() );

      DoAddListTermReplacement( l_pOldVar,
				l_pNewVar,
				p_pSub,
				l_ReplaceSub );

      l_pOldVar = std::tr1::dynamic_pointer_cast< TermVariable >( std::tr1::dynamic_pointer_cast< FormulaEqu >( std::tr1::dynamic_pointer_cast< FormulaNeg >( p_pNewEffects )->GetCNegForm() )->GetCSecond() );
      l_pNewVar = std::tr1::dynamic_pointer_cast< TermVariable >( std::tr1::dynamic_pointer_cast< FormulaEqu >( std::tr1::dynamic_pointer_cast< FormulaNeg >( m_vRemainingPrecs[ p_iPrecondIndex ] )->GetCNegForm() )->GetCSecond() );

      DoAddListTermReplacement( l_pOldVar,
				l_pNewVar,
				p_pSub,
				l_ReplaceSub );
    }
    break;
  case FT_EQU:
    {
      TermVariableP l_pOldVar = std::tr1::dynamic_pointer_cast< TermVariable >( std::tr1::dynamic_pointer_cast< FormulaEqu >( p_pNewEffects )->GetCFirst() );
      TermVariableP l_pNewVar = std::tr1::dynamic_pointer_cast< TermVariable >( std::tr1::dynamic_pointer_cast< FormulaEqu >( m_vRemainingPrecs[ p_iPrecondIndex ] )->GetCFirst() );

      DoAddListTermReplacement( l_pOldVar,
				l_pNewVar,
				p_pSub,
				l_ReplaceSub );

      l_pOldVar = std::tr1::dynamic_pointer_cast< TermVariable >( std::tr1::dynamic_pointer_cast< FormulaEqu >( p_pNewEffects )->GetCSecond() );
      l_pNewVar = std::tr1::dynamic_pointer_cast< TermVariable >( std::tr1::dynamic_pointer_cast< FormulaEqu >( m_vRemainingPrecs[ p_iPrecondIndex ] )->GetCSecond() );

      DoAddListTermReplacement( l_pOldVar,
				l_pNewVar,
				p_pSub,
				l_ReplaceSub );
    }
    break;
  default:
    throw Exception( E_NOT_IMPLEMENTED,
		     "Formula is of unknown type.",
		     __FILE__,
		     __LINE__ );
  }

  std::vector< FormulaP >::iterator l_Iter = m_vRemainingPrecs.begin();
  for( unsigned int i = 0; i < p_iPrecondIndex; i++ )
  {
    l_Iter++;
  }
  m_vRemainingPrecs.erase( l_Iter );
  UpdateAddsAndPrecs( l_ReplaceSub );
}

/**
 *  Retrieve a smart pointer to the task description associated with this 
 *   method.
 *  \return A smart pointer to the task description associated with this 
 *   method.
 */
std::tr1::shared_ptr< HtnTaskDescr > PartialHtnMethod::GetCTaskDescr() const
{
  return m_pTaskDescr;
}

/**
 *  Retrieve a pointer to the substitution from task variables to created ones.
 *  \return A pointer to the substitution from task variables to created ones.
 */
const Substitution * PartialHtnMethod::GetCTaskSubs() const
{
  return &m_TaskSubs;
}

/**
 *  Retrieve a pointer to the substitution from created vars to consts.
 *  \return A pointer to the substitution from created vars to consts.
 */
const Substitution * PartialHtnMethod::GetCMasterSubs() const
{
  return &m_MasterSubs;
}

/**
 *  Determine whether or not a list of formulas contains a particular inequality.
 *  \param p_vSearch IN The list of smart pointers to formulas in which to 
 *   search.
 *  \param p_pVar1 IN A pointer to the first unequal variable.
 *  \param p_pVar2 IN A pointer to the second unequal variable.
 *  \return Whether or not p_vSearch contains p_pVar1 != p_pVar2.
 */
bool ContainsInequality( const std::vector< FormulaP > & p_vSearch,
			 const TermVariableP & p_pVar1,
			 const TermVariableP & p_pVar2 )
{
  for( unsigned int i = 0; i < p_vSearch.size(); i++ )
  {
    if( p_vSearch[i]->GetType() == FT_NEG && 
	std::tr1::dynamic_pointer_cast< FormulaNeg >( p_vSearch[i] )->GetCNegForm()->GetType() == FT_EQU )
    {
      if( std::tr1::dynamic_pointer_cast< FormulaEqu >( std::tr1::dynamic_pointer_cast< FormulaNeg >( p_vSearch[i] )->GetCNegForm() )->GetCFirst() ==
	  p_pVar1 &&
	  std::tr1::dynamic_pointer_cast< FormulaEqu >( std::tr1::dynamic_pointer_cast< FormulaNeg >( p_vSearch[i] )->GetCNegForm() )->GetCSecond() ==
	  p_pVar2 )
	return true;
      if( std::tr1::dynamic_pointer_cast< FormulaEqu >( std::tr1::dynamic_pointer_cast< FormulaNeg >( p_vSearch[i] )->GetCNegForm() )->GetCFirst() ==
	  p_pVar2 &&
	  std::tr1::dynamic_pointer_cast< FormulaEqu >( std::tr1::dynamic_pointer_cast< FormulaNeg >( p_vSearch[i] )->GetCNegForm() )->GetCSecond() ==
	  p_pVar1 )
	return true;
    }
  }
  return false;
}

/**
 *  Retrieve a pointer to a vector of inequalities that are not in the current
 *   preconditions but need to be.
 *  \param p_vPrecs IN A vector of smart pointers to preconditions.
 *  \param p_vParams IN A vector of parameters.
 *  \param p_vVars IN A vector of variables.
 *  \return A pointer to a list of string representations of inequalities.  The
 *   caller is responsible for deallocating it.
 */
std::vector< std::string > * CreateInequalities( const std::vector< FormulaP > & p_vPrecs,
						 const std::vector< TermVariableP > & p_vParams,
						 const std::vector< TermVariableP > & p_vVars )
{
  std::vector< std::string > * l_pInequalities = new std::vector< std::string >;
  for( unsigned int i = 0; i < p_vParams.size(); i++ )
  {
    for( unsigned int j = i + 1; j < p_vParams.size(); j++ )
    {
      if( ( !p_vParams[i]->HasTyping() && !p_vParams[j]->HasTyping() ) ||
	  CompareNoCase( p_vParams[i]->GetTyping(), p_vParams[j]->GetTyping() ) == 0 )
      {
	if( !ContainsInequality( p_vPrecs, p_vParams[i], p_vParams[j] ) )
	  l_pInequalities->push_back( "( not ( = " + p_vParams[i]->ToStrNoTyping() + " " + p_vParams[j]->ToStrNoTyping() + " ) )\n" );
      }
    }
    for( unsigned int j = 0; j < p_vVars.size(); j++ )
    {
      if( ( !p_vParams[i]->HasTyping() && !p_vVars[j]->HasTyping() ) ||
	  CompareNoCase( p_vParams[i]->GetTyping(), p_vVars[j]->GetTyping() ) == 0 )
      {
	if( !ContainsInequality( p_vPrecs, p_vParams[i], p_vVars[j] ) )
	  l_pInequalities->push_back( "( not ( = " + p_vParams[i]->ToStrNoTyping() + " " + p_vVars[j]->ToStrNoTyping() + " ) )\n" );
      }
    }
  }

  for( unsigned int i = 0; i < p_vVars.size(); i++ )
  {
    for( unsigned int j = i + 1; j < p_vVars.size(); j++ )
    {
      if( ( !p_vVars[i]->HasTyping() && !p_vVars[j]->HasTyping() ) ||
	  CompareNoCase( p_vVars[i]->GetTyping(), p_vVars[j]->GetTyping() ) == 0 )
      {
	if( !ContainsInequality( p_vPrecs, p_vVars[i], p_vVars[j] ) )
	  l_pInequalities->push_back( "( not ( = " + p_vVars[i]->ToStrNoTyping() + " " + p_vVars[j]->ToStrNoTyping() + " ) )\n" );
      }
    }
  }

  return l_pInequalities;
}

/**
 *  Create a new complete method from this partial method.
 *  \param p_bSafe IN Whether or not to add verification tasks.
 *  \param p_bPartialGeneralization IN Whether or not to force all vars to be
 *   different.
 *  \param p_bQValues IN Whether or not to calculate an initial Q-value for 
 *    the method.
 *  \return A new method from these subtasks and preconditions.
 */
HtnMethod * PartialHtnMethod::CreateMethod( bool p_bSafe,
					    bool p_bPartialGeneralization,
					    bool p_bQValues ) const
{
  std::vector< TermVariableP > l_vParams;
  std::vector< TermVariableP > l_vVars;

  // Create a list of the parameters of this new method
  for( unsigned int i = 0; i < m_pTaskDescr->GetCHead()->GetValence(); i++ )
  {
    l_vParams.push_back( std::tr1::dynamic_pointer_cast< TermVariable >( m_TaskSubs.FindIndexByVar( std::tr1::dynamic_pointer_cast< TermVariable >( m_pTaskDescr->GetCHead()->GetCParam( i ) ) )->second ) );
  }


  // Create a list of preconditions from the task preconditions, outstanding
  //  preconditions, and remaining effects.
  std::vector< FormulaP > l_vPrecs;

  FormulaConjP l_pTaskPrecsAfterSubs( std::tr1::dynamic_pointer_cast< FormulaConj >( m_pTaskDescr->GetCPreconditions()->AfterSubstitution( m_TaskSubs, 0 ) ) );
  for( FormulaPVecCI i = l_pTaskPrecsAfterSubs->GetBeginConj();
       i != l_pTaskPrecsAfterSubs->GetEndConj();
       i++ )
  {
    bool l_bFound = false;
    for( unsigned int j = 0; j < l_vPrecs.size() && !l_bFound; j++ )
    {
      if( l_vPrecs[j]->Implies( *i ) )
	l_bFound = true;
    }
    if( !l_bFound )
      l_vPrecs.push_back( *i );
  }

  for( unsigned int i = 0; i < m_vRemainingPrecs.size(); i++ )
  {
    bool l_bFound = false;
    for( unsigned int j = 0; j < l_vPrecs.size() && !l_bFound; j++ )
    {
      if( l_vPrecs[j]->Implies( m_vRemainingPrecs[i] ) )
	l_bFound = true;
    }
    if( !l_bFound )
      l_vPrecs.push_back( m_vRemainingPrecs[i] );
  }

  for( unsigned int i = 0; i < m_vRemainingAddList.size(); i++ )
  {
    bool l_bFound = false;
    for( unsigned int j = 0; j < l_vPrecs.size() && !l_bFound; j++ )
    {
      if( l_vPrecs[j]->Implies( m_vRemainingAddList[i] ) )
	l_bFound = true;
    }
    if( !l_bFound )
      l_vPrecs.push_back( m_vRemainingAddList[i] );
  }

  // Fill the list of free variables that are used in the preconditions.
  for( unsigned int i = 0; i < l_vPrecs.size(); i++ )
  {
    std::vector< TermVariableP > l_vPrecVars = l_vPrecs[i]->GetVariables();
    for( unsigned int j = 0; j < l_vPrecVars.size(); j++ )
    {
      bool l_bFound = false;
      for( unsigned int k = 0; k < l_vParams.size() && !l_bFound; k++ )
      {
	if( l_vParams[k]->Equal( *l_vPrecVars[j] ) )
	  l_bFound = true;
      }
      for( unsigned int k = 0; k < l_vVars.size() && !l_bFound; k++ )
      {
	if( l_vVars[k]->Equal( *l_vPrecVars[j] ) )
	  l_bFound = true;
      }
      if( !l_bFound )
      {
	l_vVars.push_back( l_vPrecVars[j] );
      }
    }
    std::vector< TermConstantP > l_vPrecConsts = l_vPrecs[i]->GetConstants();
    if( l_vPrecConsts.size() > 0 )
      throw Exception( E_NOT_IMPLEMENTED,
		       "The preconditions of a method should not contain constants.",
		       __FILE__,
		       __LINE__ );
  }

  // Create the subtasks of this new method
  std::vector< std::string > l_vSubtasks;

  for( unsigned int i = m_iInitStateIndex; 
       i < m_iFinalStateIndex; 
       /* intentionally blank */ )
  {
    std::string l_sSubtask = "";
    bool l_bGotOne = false;
    for( unsigned int j = 0; j < m_vOperators.size() && !l_bGotOne; j++ )
    {
      if( m_vOperBeforeStates[j] == i )
      {
	l_sSubtask += "( " + m_vOperators[j]->GetName();

	for( unsigned int k = 0; k < m_vOperators[j]->GetNumParams(); k++ )
	{
	  l_sSubtask += " " + m_vOperators[j]->GetCParam( k )->AfterSubstitution( *m_vOperSubs[j], 0 )->ToStrNoTyping();
	  bool l_bAlreadyKnown = false;
	  TermP l_pSearchTerm = m_vOperSubs[j]->FindIndexByVar( std::tr1::dynamic_pointer_cast< TermVariable >( m_vOperators[j]->GetCParam( k ) ) )->second;
	  for( unsigned int l = 0; l < l_vParams.size() && !l_bAlreadyKnown; l++ )
	    if( l_vParams[l]->Equal( *l_pSearchTerm ) )
	      l_bAlreadyKnown = true;
	  for( unsigned int l = 0; l < l_vVars.size() && !l_bAlreadyKnown; l++ )
	    if( l_vVars[l]->Equal( *l_pSearchTerm ) )
	      l_bAlreadyKnown = true;
	  if( !l_bAlreadyKnown )
	  {
	    l_vVars.push_back( std::tr1::dynamic_pointer_cast< TermVariable >( l_pSearchTerm ) );
	  }
	}
	l_sSubtask += " )";
	l_bGotOne = true;
	i++;
      }
    }
    for( unsigned int j = 0; j < m_vMethods.size() && !l_bGotOne; j++ )
    {
      if( m_vMethodBeforeStates[j] == i )
      {
	l_sSubtask += "( " + m_vMethods[j]->GetCHead()->GetName();

	for( unsigned int k = 0; k < m_vMethods[j]->GetCHead()->GetNumParams(); k++ )
	{
	  l_sSubtask += " " + m_vMethods[j]->GetCHead()->GetCParam( k )->AfterSubstitution( *m_vMethodSubs[j], 0 )->ToStrNoTyping();

	  bool l_bAlreadyKnown = false;
	  TermP l_pSearchTerm = m_vMethodSubs[j]->FindIndexByVar( std::tr1::dynamic_pointer_cast< TermVariable >( m_vMethods[j]->GetCHead()->GetCParam( k ) ) )->second;
	  for( unsigned int l = 0; l < l_vParams.size() && !l_bAlreadyKnown; l++ )
	    if( l_vParams[l]->Equal( *l_pSearchTerm ) )
	      l_bAlreadyKnown = true;
	  for( unsigned int l = 0; l < l_vVars.size() && !l_bAlreadyKnown; l++ )
	    if( l_vVars[l]->Equal( *l_pSearchTerm ) )
	      l_bAlreadyKnown = true;
	  if( !l_bAlreadyKnown )
	  {
	    l_vVars.push_back( std::tr1::dynamic_pointer_cast< TermVariable >( l_pSearchTerm ) );
	  }

	}
	l_sSubtask += " )";
	l_bGotOne = true;
	i = m_vMethodAfterStates[j];
      }
    }
    if( !l_bGotOne )
      i++;
    l_vSubtasks.push_back( l_sSubtask );
  }

  // Create verification tasks
  if( p_bSafe )
  {
    std::string l_sVerifier = "";
    l_sVerifier += "( ";
    l_sVerifier += m_pTaskDescr->GetCHead()->GetName();
    l_sVerifier += "-verify";
    FormulaPredP l_pHeadAfterSubs( std::tr1::dynamic_pointer_cast< FormulaPred >( m_pTaskDescr->GetCHead()->AfterSubstitution( m_TaskSubs, 0 ) ) );
    for( unsigned int i = 0; i < l_pHeadAfterSubs->GetValence(); i++ )
      l_sVerifier += " " + l_pHeadAfterSubs->GetCParam( i )->ToStrNoTyping();
    l_sVerifier += " )";
    l_vSubtasks.push_back( l_sVerifier );
  }

  std::vector< std::string > * l_pInequalities = NULL;
  // Create different predicates for each pair of variables of the same type.
  if( p_bPartialGeneralization )
  {
    l_pInequalities = CreateInequalities( l_vPrecs,
					  l_vParams,
					  l_vVars );
  }

  // Actually write the method description into a string, then build one from 
  //  it.
  std::string l_sMethod = "( :method ";
  l_sMethod = "  ( :method ";
  l_sMethod += m_pTaskDescr->GetCHead()->GetName() + "\n";
  l_sMethod += "    :parameters\n";
  l_sMethod += "    (\n";
  for( unsigned int i = 0; i < l_vParams.size(); i++ )
    l_sMethod += "      " + l_vParams[i]->ToStr() + "\n";
  l_sMethod += "    )\n";
  if( l_vVars.size() > 0 )
  {
    l_sMethod += "    :vars\n";
    l_sMethod += "    (\n";
    for( unsigned int i = 0; i < l_vVars.size(); i++ )
      l_sMethod += "      " + l_vVars[i]->ToStr() + "\n";
    l_sMethod += "    )\n";
  }
  l_sMethod += "    :precondition\n";
  l_sMethod += "    ( and\n";
  for( unsigned int i = 0; i < l_vPrecs.size(); i++ )
    l_sMethod += "      " + l_vPrecs[i]->ToStrNoTyping() + "\n";
  if( l_pInequalities )
    for( unsigned int i = 0; i < l_pInequalities->size(); i++ )
      l_sMethod += l_pInequalities->at( i );
  l_sMethod += "    )\n";
  l_sMethod += "    :subtasks\n";
  l_sMethod += "    (\n";
  for( unsigned int i = 0; i < l_vSubtasks.size(); i++ )
    l_sMethod += "      " + l_vSubtasks[i] + "\n";
  l_sMethod += "    )\n";

  // Calculate an initial Q-value
  if( p_bQValues )
  {
    std::stringstream l_sTemp;
    l_sTemp << "    :q-value ( " << m_iTotalCost << " )\n";
    l_sTemp << "    :q-count ( 1 )\n";
    l_sMethod += l_sTemp.str();
  }

  l_sMethod += "  )\n";

  if( l_pInequalities )
    delete l_pInequalities;

  std::stringstream l_sMethodStream( l_sMethod );
  return HtnMethod::FromPddl( l_sMethodStream, m_pDomain->GetAllowableTypes(), m_pDomain->GetAllowablePredicates(), m_pDomain->GetRequirements() );
}

/**
 *  Retrieve a smart pointer to the grounded effects associated with this 
 *   method.
 *  \return A smart pointer to the grounded effects associated with this 
 *   method.  
 */
FormulaConjP PartialHtnMethod::GetGroundEffects() const
{
  FormulaP l_pAfter( m_pTaskDescr->GetCEffects()->AfterSubstitution( m_TaskSubs, 0 ) );
  FormulaP l_pRet( l_pAfter->AfterSubstitution( m_MasterSubs, 0 ) );
  return std::tr1::dynamic_pointer_cast< FormulaConj >( l_pRet );
}

/**
 *  Retrieve a smart pointer to the grounded preconditions of the task 
 *   associated with this method.
 *  \return A smart pointer to the grounded preconditions of the task 
 *   associated with this method.
 */
FormulaConjP PartialHtnMethod::GetGroundPreconditions() const
{
  FormulaP l_pAfter( m_pTaskDescr->GetCPreconditions()->AfterSubstitution( m_TaskSubs, 0 ) );
  FormulaP l_pRet( l_pAfter->AfterSubstitution( m_MasterSubs, 0 ) );
  return std::tr1::dynamic_pointer_cast< FormulaConj >( l_pRet );
}

/**
 *  Retrieve the index of the state in the plan where this method begins.
 *  \return The index of the state in the plan where this method begins.
 */
unsigned int PartialHtnMethod::GetInitStateNum() const
{
  return m_iInitStateIndex;
}

/**
 *  Retrieve the index of the state in the plan where this method ends.
 *  \return The index of the state in the plan where this method ends.
 */
unsigned int PartialHtnMethod::GetFinalStateNum() const
{
  return m_iFinalStateIndex;
}

/**
 *  Retrieve the index of the state in the plan currently reached.
 *  \return The index of the state in the plan currently reached.
 */
unsigned int PartialHtnMethod::GetCurrentStateNum() const
{
  return m_iCurrentStateIndex;
}

/**
 *  Set the index of the state in the plan where this method begins.
 *  \param p_iInitStateNum IN The index of the state in the plan where this
 *   method begins.
 */
void PartialHtnMethod::MarkMethodStart( unsigned int p_iInitStateNum )
{
  m_iInitStateIndex = p_iInitStateNum;
}

/**
 *  Skip the current step in the plan as useless.
 */
void PartialHtnMethod::Advance()
{
  m_iCurrentStateIndex--;
}

/**
 *  Replace the current task variables with unique ones.
 *  This may only be called before subtasks are added.
 *  I do not recall why this is necessary, but presume that it was.
 */
void PartialHtnMethod::GetNewTaskVars()
{
  if( !m_vMethods.empty() || !m_vOperators.empty() )
    throw Exception( E_NOT_IMPLEMENTED,
		     "Vars cannot be replaced once you start.",
		     __FILE__,
		     __LINE__ );

  Substitution l_ReplaceSubs;
  std::vector< TermVariableP > l_vOldVars;

  for( SubMap::const_iterator l_iIter = m_TaskSubs.Begin(); 
       l_iIter != m_TaskSubs.End(); 
       l_iIter++ )
  {
    l_vOldVars.push_back( std::tr1::dynamic_pointer_cast< TermVariable >( l_iIter->second ) );
  }

  for( unsigned int i = 0; i < l_vOldVars.size(); i++ )
  {
    TermVariableP l_pVar;
    if( l_vOldVars[i]->HasTyping() )
      l_pVar = std::tr1::dynamic_pointer_cast< TermVariable >( g_TermTable.Lookup( MakeVarId(), l_vOldVars[i]->GetTyping() ) );
    else
      l_pVar = std::tr1::dynamic_pointer_cast< TermVariable >( g_TermTable.Lookup( MakeVarId() ) );
    m_TaskSubs.ReplaceTerm( l_vOldVars[i], l_pVar );
    m_MasterSubs.ReplaceTerm( l_vOldVars[i], l_pVar );
    l_ReplaceSubs.AddPair( l_vOldVars[i], l_pVar );
  }

  std::vector< FormulaP > l_vAddList;

  while( !m_vRemainingAddList.empty() )
  {
    l_vAddList.push_back( m_vRemainingAddList.back() );
    m_vRemainingAddList.pop_back();
  }
  while( !l_vAddList.empty() )
  {
    m_vRemainingAddList.push_back( l_vAddList.back()->AfterSubstitution( l_ReplaceSubs, 0 ) );
    l_vAddList.pop_back();
  }
}

/**
 *  Retrieve a smart pointer to the effects actually accomplished by this 
 *   method.
 *  I believe this always uses the task effects associated with any 
 *   nonprimitive subtasks, but am not completely positive.
 *  \return A smart pointer to the effects actually accomplished by this 
 *   method.  The caller is responsible for deallocating it.
 */
FormulaConjP PartialHtnMethod::GetActualEffects() const
{
  std::vector< FormulaP > l_vPosEffects;
  std::vector< FormulaP > l_vNegEffects;

  for( unsigned int i = m_iFinalStateIndex; 
       i > m_iInitStateIndex; 
       /*intentionally blank*/ )
  {
    // Get the effects of the subtask that ends at this state.
    FormulaConjP l_pEffects;
    for( unsigned int j = 0; j < m_vMethodAfterStates.size() && !l_pEffects.get(); j++ )
    {
      if( m_vMethodAfterStates[j] == i )
      {
	l_pEffects = FormulaConjP( std::tr1::dynamic_pointer_cast< FormulaConj >( m_vMethodEffects[j]->AfterSubstitution( *m_vMethodSubs[j], 0 ) ) );
	i = m_vMethodBeforeStates[j];
      }
    }
    for( unsigned int j = 0; j < m_vOperators.size() && !l_pEffects.get(); j++ )
    {
      if( m_vOperBeforeStates[j] + 1 == i )
      {
	l_pEffects = FormulaConjP( std::tr1::dynamic_pointer_cast< FormulaConj >( m_vOperators[j]->GetCEffects()->AfterSubstitution( *m_vOperSubs[j], 0 ) ) );
	i--;
      }
    }

    // Process each effect
    if( l_pEffects.get() )
    {
      for( FormulaPVecCI k = l_pEffects->GetBeginConj();
	   k != l_pEffects->GetEndConj();
	   k++ )
      {
	bool l_bFound = false;
	switch( ( *k )->GetType() )
	{
	case FT_PRED:
	  // Add as a positive effect unless it is already a positive or
	  //  negative effect.
	  for( unsigned int l = 0; l < l_vNegEffects.size() && !l_bFound; l++ )
	  {
	    if( *( *k ) == *std::tr1::dynamic_pointer_cast< FormulaNeg >( l_vNegEffects[l] )->GetCNegForm() )
	      l_bFound = true;
	  }
	  if( l_bFound )
	    break;
	  for( unsigned int l = 0; l < l_vPosEffects.size() && !l_bFound; l++ )
	  {
	    if( *( *k ) == *l_vPosEffects[l] )
	      l_bFound = true;
	  }
	  if( !l_bFound )
	    l_vPosEffects.push_back( *k );
	  break;
	case FT_NEG:
	  if( std::tr1::dynamic_pointer_cast< FormulaNeg >( *k )->GetCNegForm()->GetType() != FT_PRED )
	    throw Exception( E_NOT_IMPLEMENTED,
			     "The only type of negative effect allowed is a predicate.",
			     __FILE__,
			     __LINE__ );
	  // Add as a negative effect unless it is already a positive or
	  //  negative effect.
	  for( unsigned int l = 0; l < l_vPosEffects.size() && !l_bFound; l++ )
	  {
	    if( *std::tr1::dynamic_pointer_cast< FormulaNeg >( *k )->GetCNegForm() == *l_vPosEffects[l] )
	      l_bFound = true;
	  }
	  if( l_bFound )
	    break;
	  for( unsigned int l = 0; l < l_vNegEffects.size() && !l_bFound; l++ )
	  {
	    if( *( *k ) == *l_vNegEffects[l] )
	      l_bFound = true;
	  }
	  if( !l_bFound )
	    l_vNegEffects.push_back( *k );
	  break;
	case FT_EQU:
	  throw Exception( E_NOT_IMPLEMENTED,
			   "An equality should never be an effect.",
			   __FILE__,
			   __LINE__ );
	case FT_CONJ:
	  throw Exception( E_NOT_IMPLEMENTED,
			   "A conjunction was not in simplest form.",
			   __FILE__,
			   __LINE__ );
	default:
	  throw Exception( E_NOT_IMPLEMENTED,
			   "An effect must be a predicate or negation of a predicate.",
			   __FILE__,
			   __LINE__ );
	}
      }
    }
    else
      i--;
  }

  FormulaPVec l_sEffectsSet;
  for( FormulaPVecCI i = l_vPosEffects.begin();
       i != l_vPosEffects.end();
       i++ )
    l_sEffectsSet.push_back( *i );
  for( FormulaPVecCI i = l_vNegEffects.begin();
       i != l_vNegEffects.end();
       i++ )
    l_sEffectsSet.push_back( *i );

  return FormulaConjP( new FormulaConj( l_sEffectsSet ) );
}

/**
 *  Determine whether all members of the remaining add list are satisfied in a 
 *   state.
 *  \param p_pState IN A pointer to the state where they might be satisfied.
 *  \return Whether or not all members of the remainingadd list are satisfied
 *   in the given state.
 */
bool PartialHtnMethod::RemainingAddListSatisfied( const State * p_pState ) const
{
  for( unsigned int i = 0; i < m_vRemainingAddList.size(); i++ )
  {
    FormulaP l_pAddGround( m_vRemainingAddList[i]->AfterSubstitution( m_MasterSubs, 0 ) );
    if( !p_pState->IsConsistent( l_pAddGround ) )
    {
      return false;
    }
  }
  return true;
}

size_t PartialHtnMethod::GetMemSizeMin() const
{
  size_t l_iSize = sizeof( PartialHtnMethod ) + m_vRemainingAddList.capacity() * sizeof( FormulaP ) + m_vRemainingPrecs.capacity() * sizeof( FormulaP ) + m_vOperators.capacity() * sizeof( Operator * ) + m_vOperSubs.capacity() * sizeof( Substitution * ) + m_vOperBeforeStates.capacity() * sizeof( unsigned int ) + m_vMethods.capacity() * sizeof( HtnMethod * ) + m_vMethodSubs.capacity() * sizeof( Substitution * ) + m_vMethodEffects.capacity() * sizeof( FormulaConjP ) + m_vMethodBeforeStates.capacity() * sizeof( unsigned int ) + m_vMethodAfterStates.capacity() * sizeof( unsigned int );
  l_iSize += m_TaskSubs.GetMemSizeMin() - sizeof( Substitution );
  l_iSize += m_MasterSubs.GetMemSizeMin() - sizeof( Substitution );

  for( unsigned int i = 0; i < m_vOperators.size(); i++ )
    l_iSize += m_vOperators[i]->GetMemSizeMin();
  for( unsigned int i = 0; i < m_vOperSubs.size(); i++ )
    l_iSize += m_vOperSubs[i]->GetMemSizeMin();
  for( unsigned int i = 0; i < m_vMethods.size(); i++ )
    l_iSize += m_vMethods[i]->GetMemSizeMin();
  for( unsigned int i = 0; i < m_vMethodSubs.size(); i++ )
    l_iSize += m_vMethodSubs[i]->GetMemSizeMin();
  return l_iSize;
}

size_t PartialHtnMethod::GetMemSizeMax() const
{
  size_t l_iSize = sizeof( PartialHtnMethod ) + m_vRemainingAddList.capacity() * sizeof( FormulaP ) + m_vRemainingPrecs.capacity() * sizeof( FormulaP ) + m_vOperators.capacity() * sizeof( Operator * ) + m_vOperSubs.capacity() * sizeof( Substitution * ) + m_vOperBeforeStates.capacity() * sizeof( unsigned int ) + m_vMethods.capacity() * sizeof( HtnMethod * ) + m_vMethodSubs.capacity() * sizeof( Substitution * ) + m_vMethodEffects.capacity() * sizeof( FormulaConjP ) + m_vMethodBeforeStates.capacity() * sizeof( unsigned int ) + m_vMethodAfterStates.capacity() * sizeof( unsigned int );
  l_iSize += m_TaskSubs.GetMemSizeMax() - sizeof( Substitution );
  l_iSize += m_MasterSubs.GetMemSizeMax() - sizeof( Substitution );

  for( unsigned int i = 0; i < m_vOperators.size(); i++ )
    l_iSize += m_vOperators[i]->GetMemSizeMax();
  for( unsigned int i = 0; i < m_vOperSubs.size(); i++ )
    l_iSize += m_vOperSubs[i]->GetMemSizeMax();
  for( unsigned int i = 0; i < m_vMethods.size(); i++ )
    l_iSize += m_vMethods[i]->GetMemSizeMax();
  for( unsigned int i = 0; i < m_vMethodSubs.size(); i++ )
    l_iSize += m_vMethodSubs[i]->GetMemSizeMax();
  for( unsigned int i = 0; i < m_vRemainingAddList.size(); i++ )
    l_iSize += m_vRemainingAddList[i]->GetMemSizeMax();
  for( unsigned int i = 0; i < m_vRemainingPrecs.size(); i++ )
    l_iSize += m_vRemainingPrecs[i]->GetMemSizeMax();
  for( unsigned int i = 0; i < m_vMethodEffects.size(); i++ )
    l_iSize += m_vMethodEffects[i]->GetMemSizeMax();
  return l_iSize;
}
