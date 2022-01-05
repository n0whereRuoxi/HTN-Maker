#include <vector>
#include <string>
#include <cassert>
#include <sstream>
#include <set>
#include <tr1/memory>

#include "exception.hpp"
#include "funcs.hpp"
#include "term.hpp"
#include "term_string.hpp"
#include "term_variable.hpp"
#include "term_constant.hpp"
#include "term_constant_typed.hpp"
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

/** \file strips_solution.hpp
 *  Declaration of the StripsSolution class.
 */

/** \file strips_solution.cpp
 *  Definition of the StripsSolution class.
 */

/** \class StripsSolution
 *  A solution to a classical (STRIPS) planning problem.
 */

/** \var StripsSolution::m_pProblem
 *  A pointer to the problem for which this is a solution.
 */

/** \var StripsSolution::m_vFollowStates
 *  A list of pointers to the States this follow each action in this plan.
 *  These must be deallocated with the StripsSolution.
 */

/** \var StripsSolution::m_vOperatorIndices
 *  A list of the indexes into the domain associated with the solved problem
 *   of the operators in the plan.
 */

/** \var StripsSolution::m_vSubstitutions
 *  A list of Substitutions, one for each operator in the plan, that make them
 *   actions.
 *  These must be deallocated with the StripsSolution.
 */

/**
 *  The one and only TermTable.
 */
extern TermTable g_TermTable;

/**
 *  Construct an empty StripsSolution from the problem it should solve.
 *  \param p_pProblem IN A pointer to the problem that this should solve.
 */
StripsSolution::StripsSolution( const std::tr1::shared_ptr< StripsProblem > & p_pProblem )
  : m_pProblem( p_pProblem )
{
}

/**
 *  Construct a StripsSolution from a pointer to the problem it should solve 
 *   and a string containing a textual representation of the plan.
 *  \param p_pProblem IN A pointer to the problem this should solve.
 *  \param p_sInput IN A string containing a textual representation of a plan.
 */
StripsSolution::StripsSolution( const std::tr1::shared_ptr< StripsProblem > & p_pProblem,
				std::string p_sInput )
  : m_pProblem( p_pProblem )
{
  std::stringstream l_sStream( p_sInput );

  EatWhitespace( l_sStream );
  EatString( l_sStream, "(" );
  EatWhitespace( l_sStream );
  EatString( l_sStream, "defplan" );
  EatWhitespace( l_sStream );
  EatString( l_sStream, m_pProblem->GetCDomain()->GetName() );
  EatWhitespace( l_sStream );
  ReadString( l_sStream );
  //  EatString( l_sStream, m_pProblem->GetName() );
  EatWhitespace( l_sStream );

  while( l_sStream.peek() == '(' )
  {
    EatString( l_sStream, "(" );
    EatWhitespace( l_sStream );
    std::string l_sOpName = ReadString( l_sStream );
    unsigned int l_iOpIndex = m_pProblem->GetCDomain()->GetOperIndexByName( l_sOpName );
    Substitution * l_pNewSubs = new Substitution();
    for( unsigned int i = 0; i < m_pProblem->GetCDomain()->GetCOper( l_iOpIndex )->GetNumParams(); i++ )
    {
      EatWhitespace( l_sStream );
      std::string l_sParam = ReadString( l_sStream );

      l_pNewSubs->AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( m_pProblem->GetCDomain()->GetCOper( l_iOpIndex )->GetCParam( i ) ), ReadTerm( l_sParam, m_pProblem->GetObjectTypes() ) );
    }
    EatWhitespace( l_sStream );
    EatString( l_sStream, ")" );
    EatWhitespace( l_sStream );
    ApplyOperator( l_iOpIndex, l_pNewSubs );
  }

  EatString( l_sStream, ")" );
}

/**
 *  Construct a StripsSolution as a copy of an existing one.
 *  \param p_Other IN The StripsSolution to copy.
 */
StripsSolution::StripsSolution( const StripsSolution & p_Other )
  : m_pProblem( p_Other.m_pProblem )
{
  for( unsigned int i = 0; i < p_Other.m_vOperatorIndices.size(); i++ )
    m_vOperatorIndices.push_back( p_Other.m_vOperatorIndices[i] );
  for( unsigned int i = 0; i < p_Other.m_vFollowStates.size(); i++ )
    m_vFollowStates.push_back( new State( *p_Other.m_vFollowStates[i] ) );
  for( unsigned int i = 0; i < p_Other.m_vSubstitutions.size(); i++ )
    m_vSubstitutions.push_back( new Substitution( *p_Other.m_vSubstitutions[i] ) );
}

/**
 *  Destruct a StripsSolution.
 */
StripsSolution::~StripsSolution()
{
  for( unsigned int i = 0; i < m_vFollowStates.size(); i++ )
    delete m_vFollowStates[i];
  for( unsigned int i = 0; i < m_vSubstitutions.size(); i++ )
    delete m_vSubstitutions[i];
}

/**
 *  Retrieve a pointer to the problem that this solves.
 *  \return A pointer to the problem that this solves.
 */
std::tr1::shared_ptr< StripsProblem > StripsSolution::GetCProblem() const
{
  return m_pProblem;
}

/**
 *  Add a new action to this plan.
 *  \param p_iOperIndexInDomain IN The 0-based index of the operator to add in
 *   the associated domain.
 *  \param p_pNewSub IN A pointer to a Substitution that makes the selected
 *   operator ground and applicable.  The caller relinquishes control of this
 *   pointer.
 *  \return Whether or not this addition created a cycle in the plan.
 */
bool StripsSolution::ApplyOperator( unsigned int p_iOperIndexInDomain, 
				    Substitution * p_pNewSub )
{
  if( p_iOperIndexInDomain >= m_pProblem->GetCDomain()->GetNumOpers() )
    throw Exception( E_INDEX_OUT_OF_BOUNDS,
		     "Bounds error.",
		     __FILE__,
		     __LINE__ );
  m_vOperatorIndices.push_back( p_iOperIndexInDomain );
  m_vSubstitutions.push_back( p_pNewSub );
  State * l_pNewState = NULL;
  if( m_vFollowStates.size() == 0 )
    l_pNewState = m_pProblem->GetCInitState()->NextState( m_pProblem->GetCDomain()->GetCOper( p_iOperIndexInDomain ), p_pNewSub );
  else
    l_pNewState = m_vFollowStates.back()->NextState( m_pProblem->GetCDomain()->GetCOper( p_iOperIndexInDomain ), p_pNewSub );

  bool l_bRet = ContainsState( *l_pNewState );
  m_vFollowStates.push_back( l_pNewState );
  return l_bRet;
}

/**
 *  Retrieve the current length of this plan.
 *  \return The current length of this plan.
 */
unsigned int StripsSolution::GetPlanLength() const
{
  return m_vFollowStates.size();
}

/**
 *  Retrieve a pointer to a requested State in this solution.
 *  \param p_iIndex IN The 0-based index of the requested State.
 *  \return A pointer to the requested State.  This pointer has the same 
 *   lifetime as this StripsSolution.
 */
const State * StripsSolution::GetCState( unsigned int p_iIndex ) const
{
  if( p_iIndex == 0 )
    return m_pProblem->GetCInitState();
  else
    return GetCFollowState( p_iIndex - 1 );
}

/**
 *  Retrieve a pointer to the State following a certain action.
 *  \param p_iIndex IN The 0-based index of the action that precedes the 
 *   desired State.
 *  \return A pointer to the requested State.  This pointer has the same
 *   lifetime as this StripsSolution.
 */
const State * StripsSolution::GetCFollowState( unsigned int p_iIndex ) const
{
  if( p_iIndex < m_vFollowStates.size() )
    return m_vFollowStates[ p_iIndex ];
  else
  {
    throw Exception( E_INDEX_OUT_OF_BOUNDS,
		     "Bounds error.",
		     __FILE__,
		     __LINE__ );
  }
}

/**
 *  Retrieve a pointer to one of the Operators in this plan.
 *  \param p_iIndex IN The 0-based index of the desired Operator.
 *  \return A pointer to the requested Operator.  This pointer has the same
 *   lifetime as this StripsSolution.
 */
const Operator * StripsSolution::GetCOperator( unsigned int p_iIndex ) const
{
  if( p_iIndex < m_vOperatorIndices.size() )
    return m_pProblem->GetCDomain()->GetCOper( m_vOperatorIndices[ p_iIndex ] );
  else
  {
    throw Exception( E_INDEX_OUT_OF_BOUNDS,
		     "Bounds error.",
		     __FILE__,
		     __LINE__ );
  }
}

/**
 *  Retrieve a pointer to the Substitution associated with one of the Operators
 *   in this plan.
 *  \param p_iIndex IN The 0-based index of the desired Substitution.
 *  \return A pointer to the requested Substitution.  This pointer has the 
 *   same lifetime as this StripsSolution.
 */
const Substitution * StripsSolution::GetCSubstitution( unsigned int p_iIndex ) const
{
  if( p_iIndex < m_vSubstitutions.size() )
    return m_vSubstitutions[ p_iIndex ];
  else
  {
    throw Exception( E_INDEX_OUT_OF_BOUNDS,
		     "Bounds error.",
		     __FILE__,
		     __LINE__ );
  }
}

/**
 *  Determine whether or not a given State appears in this solution.
 *  \param p_State IN The State to be searched for.
 *  \return Whether or not the given State appears in this solution.
 */
bool StripsSolution::ContainsState( const State & p_State ) const
{
  for( unsigned int i = 0; i < m_vFollowStates.size(); i++ )
  {
    if( p_State == *m_vFollowStates[i] )
      return true;
  }
  return  ( p_State == *m_pProblem->GetCInitState() );
}

/**
 *  Determine whether this plan is in fact a complete and correct Solution to
 *   its problem.
 *  The Operators must already be applicable, so the only check required is 
 *   that the last State satisfies the goals of the problem.
 *  \return Whether this plan is in fact a complete and correct Solution to
 *   its problem.
 */
bool StripsSolution::IsComplete() const
{
  const State * l_pFinalState = 0;
  if( m_vFollowStates.size() == 0 )
    l_pFinalState = m_pProblem->GetCInitState();
  else
    l_pFinalState = m_vFollowStates.back();
  return l_pFinalState->IsConsistent( m_pProblem->GetCGoals() );
}

size_t StripsSolution::GetMemSizeMin() const
{
  size_t l_iSize = sizeof( StripsSolution ) + (m_vFollowStates.capacity() - m_vFollowStates.size()) * sizeof( State * ) + m_vOperatorIndices.capacity() * sizeof( unsigned int ) + (m_vSubstitutions.capacity() - m_vSubstitutions.size()) * sizeof( Substitution * );
  for( unsigned int i = 0; i < m_vFollowStates.size(); i++ )
    l_iSize += m_vFollowStates[i]->GetMemSizeMin();
  for( unsigned int i = 0; i < m_vSubstitutions.size(); i++ )
    l_iSize += m_vSubstitutions[i]->GetMemSizeMin();
  return l_iSize;
}

size_t StripsSolution::GetMemSizeMax() const
{
  size_t l_iSize = sizeof( StripsSolution ) + (m_vFollowStates.capacity() - m_vFollowStates.size()) * sizeof( State * ) + m_vOperatorIndices.capacity() * sizeof( unsigned int ) + (m_vSubstitutions.capacity() - m_vSubstitutions.size()) * sizeof( Substitution * );
  for( unsigned int i = 0; i < m_vFollowStates.size(); i++ )
    l_iSize += m_vFollowStates[i]->GetMemSizeMax();
  for( unsigned int i = 0; i < m_vSubstitutions.size(); i++ )
    l_iSize += m_vSubstitutions[i]->GetMemSizeMax();
  return l_iSize;
}
