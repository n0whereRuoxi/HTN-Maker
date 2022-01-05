#include <vector>
#include <string>
#include <cassert>
#include <set>
#include <tr1/memory>

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
#include "strips_domain.hpp"
#include "strips_problem.hpp"
#include "strips_solution.hpp"
#include "htn_task_head.hpp"
#include "htn_task_descr.hpp"
#include "htn_method.hpp"
#include "annotated_plan.hpp"

/** \file annotated_plan.hpp
 *  Declaration of the AnnotatedPlan class.
 */

/** \file annotated_plan.cpp
 *  Definition of the AnnotatedPlan class.
 */

/** \class AnnotatedPlan
 *  A plan that has been annotated with methods that subsume different chunks 
 *   of the plan.
 *  This is used by HTN-MAKER to find task-subtask relationships.
 */

/** \var AnnotatedPlan::m_vMethodSubs
 *  A list of substitutions that make the methods in m_vMethods ground.
 *  These need to be deallocated with the AnnotatedPlan.
 */

/** \var AnnotatedPlan::m_vMethods
 *  A list of methods that cover part of this plan.
 *  These need to be deallocated with the AnnotatedPlan.
 */

/** \var AnnotatedPlan::m_vBeforeStates
 *  A list of the states in which each of the methods in m_vMethods begins.
 */

/** \var AnnotatedPlan::m_vAfterStates
 *  A list of the states in which each of the methods in m_vMethods ends.
 */

/** \var AnnotatedPlan::m_vTaskDescrs
 *  A list of smart pointers to the annotated tasks associated with each of 
 *   the methods in m_vMethods.
 */

/** \var AnnotatedPlan::m_vMethodEffects
 *  A list of smart pointers to the effects associated with each of the 
 *   methods in m_vMethods.
 */

/** \var AnnotatedPlan::m_vMethodCosts
 *  A list of the costs of the method instantiations in m_vMethods.
 */

/**
 *  The one and only TermTable.
 */
extern TermTable g_TermTable;

/**
 *  Construct a plan to be annotated from a problem and string representation
 *   of the plan.
 *  \param p_pProblem IN A pointer to the problem this solves.
 *  \param p_sInput IN A string containing a representation of the plan.
 */
AnnotatedPlan::AnnotatedPlan( const std::tr1::shared_ptr< StripsProblem > & p_pProblem,
			      std::string p_sInput )
  : StripsSolution( p_pProblem, p_sInput )
{
}

/**
 *  Construct an AnnotatedPlan as a copy of another.
 *  \param p_Other IN The AnnotatedPlan to copy.
 */
AnnotatedPlan::AnnotatedPlan( const AnnotatedPlan & p_Other )
  : StripsSolution( p_Other )
{
  for( unsigned int i = 0; i < p_Other.m_vMethods.size(); i++ )
  {
    m_vMethodSubs.push_back( new Substitution( *p_Other.m_vMethodSubs[i] ) );
    m_vMethods.push_back( new HtnMethod( *p_Other.m_vMethods[i] ) );
    m_vBeforeStates.push_back( p_Other.m_vBeforeStates[i] );
    m_vAfterStates.push_back( p_Other.m_vAfterStates[i] );
    m_vTaskDescrs.push_back( p_Other.m_vTaskDescrs[i] );
    m_vMethodEffects.push_back( p_Other.m_vMethodEffects[i] );
    m_vMethodCosts.push_back( p_Other.m_vMethodCosts[i] );
  }
}

/**
 *  Destruct this AnnotatedPlan.
 */
AnnotatedPlan::~AnnotatedPlan()
{
  for( unsigned int i = 0; i < m_vMethods.size(); i++ )
  {
    delete m_vMethodSubs[i];
    delete m_vMethods[i];
  }
}

/**
 *  Retrieve the number of methods added as annotations to this plan.
 *  \return The number of methods added as annotations to this plan.
 */
unsigned int AnnotatedPlan::GetNumMethods() const
{
  return m_vMethods.size();
}

/**
 *  Retrieve one of the methods added as annotations to his plan.
 *  \param p_iIndex IN The 0-based index of the desired method.
 *  A pointer to the requested method.
 */
const HtnMethod * AnnotatedPlan::GetCMethod( unsigned int p_iIndex ) const
{
  if( p_iIndex < m_vMethods.size() )
    return m_vMethods[ p_iIndex ];
  else
    throw Exception( E_INDEX_OUT_OF_BOUNDS,
		     "Bounds error.",
		     __FILE__,
		     __LINE__ );
}

/**
 *  Retrieve a pointer to the substitution associated with one of the methods.
 *  \param p_iIndex IN The 0-based index of the desired substitution.
 *  \return A pointer to the substitution associated with one of the methods.
 */
const Substitution * AnnotatedPlan::GetCMethodSub( unsigned int p_iIndex ) const
{
  if( p_iIndex < m_vMethodSubs.size() )
    return m_vMethodSubs[ p_iIndex ];
  else
    throw Exception( E_INDEX_OUT_OF_BOUNDS,
		     "Bounds error.",
		     __FILE__,
		     __LINE__ );
}

/**
 *  Retrieve the index of the before state of one of the methods.
 *  \param p_iIndex IN The 0-based index of the desired before state.
 *  \return The index of the requested before state.
 */
unsigned int AnnotatedPlan::GetMethodBeforeState( unsigned int p_iIndex ) const
{
  if( p_iIndex < m_vBeforeStates.size() )
    return m_vBeforeStates[ p_iIndex ];
  else
    throw Exception( E_INDEX_OUT_OF_BOUNDS,
		     "Bounds error.",
		     __FILE__,
		     __LINE__ );
}

/**
 *  Retrieve the index of the after state of one of the methods.
 *  \param p_iIndex IN The 0-based index of the desired after state.
 *  \return The index of the requested after state.
 */
unsigned int AnnotatedPlan::GetMethodAfterState( unsigned int p_iIndex ) const
{
  if( p_iIndex < m_vAfterStates.size() )
    return m_vAfterStates[ p_iIndex ];
  else
    throw Exception( E_INDEX_OUT_OF_BOUNDS,
		     "Bounds error.",
		     __FILE__,
		     __LINE__ );
}

/**
 *  Retrieve a smart pointer to the task description associated with a method.
 *  \param p_iIndex IN The 0-based index of the desired task description.
 *  \return A smart pointer to the requested task description.
 */
std::tr1::shared_ptr< HtnTaskDescr > AnnotatedPlan::GetCTaskDescr( unsigned int p_iIndex ) const
{
  if( p_iIndex < m_vTaskDescrs.size() )
    return m_vTaskDescrs[ p_iIndex ];
  else
    throw Exception( E_INDEX_OUT_OF_BOUNDS,
		     "Bounds error.",
		     __FILE__,
		     __LINE__ );
}

/**
 *  Retrieve a smart pointer to the effects associated with one of the methods.
 *  \param p_iIndex IN The 0-based index of the desired effects.
 *  \return A smart pointer to the requested effects.
 */
FormulaConjP AnnotatedPlan::GetCMethodEffects( unsigned int p_iIndex ) const
{
  if( p_iIndex < m_vMethodEffects.size() )
    return m_vMethodEffects[ p_iIndex ];
  else
    throw Exception( E_INDEX_OUT_OF_BOUNDS,
		     "Bounds error.",
		     __FILE__,
		     __LINE__ );
}

/**
 *  Retrieve the cost of a method instantiation.
 *  \param p_iIndex IN The 0-based index of the desired cost.
 *  \return The cost of the requested method instantiation.
 */
double AnnotatedPlan::GetMethodCost( unsigned int p_iIndex ) const
{
  if( p_iIndex < m_vMethodCosts.size() )
    return m_vMethodCosts[ p_iIndex ];
  else
    throw Exception( E_INDEX_OUT_OF_BOUNDS,
		     "Bounds error.",
		     __FILE__,
		     __LINE__ );
}

/**
 *  Add a new method as an annotation to this plan.
 *  For some reason, everything is stored with new unique variables.  I do not
 *   recall why, but it must have been important.
 *  \param p_pMethod IN A pointer to the method to add.
 *  \param p_pSub In A pointer to a substitution that makes the method ground.
 *  \param p_iBeforeStateNum IN The index of the state where the method begins.
 *  \param p_iAfterStateNum IN The index of the state where the method ends.
 *  \param p_pTaskDescr IN A smart pointer to the task description of the 
 *   method.
 *  \param p_pMethodEffects IN A smart pointer to the effects associated with 
 *   the method.
 *  \param p_fMethodCost IN The cost of this method, for RL.
 */
void AnnotatedPlan::AddMethodInst( const HtnMethod * p_pMethod,
				   const Substitution * p_pSub,
				   unsigned int p_iBeforeStateNum,
				   unsigned int p_iAfterStateNum,
				   std::tr1::shared_ptr< HtnTaskDescr > p_pTaskDescr,
				   FormulaConjP p_pMethodEffects,
				   double p_fMethodCost )
{
  Substitution l_ChangeOfVars;
  Substitution * l_pNewSub = new Substitution;
  for( SubMap::const_iterator l_iIter = p_pSub->Begin(); 
       l_iIter != p_pSub->End(); 
       l_iIter++ )
  {
    TermVariableP l_pOld = l_iIter->first;
    TermP l_pConst = l_iIter->second;
    TermVariableP l_pNewTerm;
    if( l_pOld->HasTyping() )
      l_pNewTerm = std::tr1::dynamic_pointer_cast< TermVariable >( g_TermTable.Lookup( MakeOldId(), l_pOld->GetTyping() ) );
    else
      l_pNewTerm = std::tr1::dynamic_pointer_cast< TermVariable >( g_TermTable.Lookup( MakeOldId() ) );
    l_ChangeOfVars.AddPair( l_pOld, l_pNewTerm );
    l_pNewSub->AddPair( l_pNewTerm, l_pConst );
  }

  m_vMethods.push_back( p_pMethod->AfterSubstitution( l_ChangeOfVars, 0 ) );
  m_vMethodSubs.push_back( l_pNewSub );
  m_vBeforeStates.push_back( p_iBeforeStateNum );
  m_vAfterStates.push_back( p_iAfterStateNum );
  m_vTaskDescrs.push_back( p_pTaskDescr->AfterSubstitution( l_ChangeOfVars, 0 ) );
  m_vMethodEffects.push_back( std::tr1::dynamic_pointer_cast< FormulaConj >( p_pMethodEffects->AfterSubstitution( l_ChangeOfVars, 0 ) ) );
  m_vMethodCosts.push_back( p_fMethodCost );
}

