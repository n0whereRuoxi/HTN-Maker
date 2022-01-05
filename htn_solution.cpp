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
#include "htn_method.hpp"
#include "htn_domain.hpp"
#include "htn_problem.hpp"
#include "htn_solution.hpp"

/** \file htn_solution.hpp
 *  Declaration of HtnSolution and DecompPart classes.
 */

/** \file htn_solution.cpp
 *  Definition of HtnSolution and DecompPart classes.
 */

/** \class DecompPart
 *  A node in a hierarchical decomposition tree.
 *  The node has a name, which should be a method or operator name, and 0 or 
 *   more children.  This is not simply a task name, because we added the 
 *   concept of a unique name for each method.
 *  This exists so that it is possible to easily print a full solution 
 *   decomposition for the --show_trace argument to htn-solver2, which exists
 *   for the HTN-learner project with Dr. Qiang Yang of Hong Kong University.
 */

/** \var DecompPart::m_sName
 *  The name of this node.
 *  This is simply the task.
 */

/** \var DecompPart::m_sId
 *  The ID associated with this method.
 *  Should be an empty string for operators.
 */

/** \var DecompPart::m_vChildren
 *  A list of pointers to the children of this node.
 *  These need to be deallocated with the DecompPart.
 */

/**
 *  Construct a childless node from its name.
 *  \param p_sName IN The name of the new node.
 *  \param p_sId IN The ID of the new node.
 */
DecompPart::DecompPart( const std::string & p_sName,
			const std::string & p_sId )
{
  m_sName = p_sName;
  m_sId = p_sId;
}

/**
 *  Construct a node as a copy of an existing one.
 *  Note that if there were a cyclic graph, this would crash and burn.
 *  \param p_Other IN The DecompPart to copy.
 */
DecompPart::DecompPart( const DecompPart & p_Other )
{
  m_sName = p_Other.m_sName;
  m_sId = p_Other.m_sId;
  for( unsigned int i = 0; i < p_Other.m_vChildren.size(); i++ )
  {
    m_vChildren.push_back( new DecompPart( *p_Other.m_vChildren[i] ) );
  }
}

/**
 *  Destruct a node.
 */
DecompPart::~DecompPart()
{
  for( unsigned int i = 0; i < m_vChildren.size(); i++ )
    delete m_vChildren[i];
}

/**
 *  Add a child to this node.
 *  \param p_pNew IN A pointer to the child to be added.  The caller 
 *   relinquishes constrol of this pointer, which will be deallocated with the
 *   DecompPart.
 */
void DecompPart::AddChild( DecompPart * p_pNew )
{
  m_vChildren.push_back( p_pNew );
}

/**
 *  Print this node and its children to a string.
 *  \param p_sIndent IN A string of spaces to make this fit properly in a 
 *   larger tree.
 *  \return A string containing a representation of this tree.
 */
std::string DecompPart::Print( const std::string & p_sIndent ) const
{
  std::stringstream l_sRet;
  l_sRet << p_sIndent <<  m_sName;
  if( m_sId != "" )
    l_sRet << " [" << m_sId << "]";
  l_sRet << "\n";
  if( m_vChildren.size() > 0 )
  {
    l_sRet << p_sIndent << "{\n";
    for( unsigned int i = 0; i < m_vChildren.size(); i++ )
      l_sRet << m_vChildren[i]->Print( p_sIndent + "  " );
    l_sRet << p_sIndent << "}\n";
  }
  return l_sRet.str();
}

/**
 *  Retrieve the list of children of this node.
 *  This is not quite as safe as it should be, because the caller could alter
 *   the contents of the pointers in the vector.
 *  \return The list of children of this node.
 */
const std::vector< DecompPart * > & DecompPart::GetChildren() const
{
  return m_vChildren;
}

const std::string & DecompPart::GetName() const
{
  return m_sName;
}

const std::string & DecompPart::GetId() const
{
  return m_sId;
}

size_t DecompPart::GetMemSizeMin() const
{
  size_t l_iSize = sizeof( DecompPart ) + m_sName.capacity() + m_sId.capacity() + m_vChildren.capacity() * sizeof( DecompPart * );
  return l_iSize;
}

size_t DecompPart::GetMemSizeMax() const
{
  size_t l_iSize = GetMemSizeMin();
  for( unsigned int i = 0; i < m_vChildren.size(); i++ )
    l_iSize += m_vChildren[i]->GetMemSizeMax();
  return l_iSize;
}


/** \class HtnSolution
 *  A solution to an HTN planning problem.
 *  This includes the problem itself, the list of operators (leaves of the
 *   decomposition tree), and now a string representation of the entire tree.
 */

/**
 *  \var HtnSolution::m_pInitState
 *  The initial state of the problem.
 *  This is different from HtnProblem::m_pState, which is the current state as
 *   of any operators that have already been applied.
 *  This must be deallocated with the HtnSolution.
 */

/**
 *  \var HtnSolution::m_vOperIndices
 *  A list of the indices of the operators that solve the problem in the domain
 *   associated with the problem.
 */

/**
 *  \var HtnSolution::m_vOperSubs
 *  A list of the substitutions applied to the operators specified in 
 *   m_vOperIndices to make them actions.
 *  These must be deallocated with the HtnSolution.
 */

/**
 *  \var HtnSolution::m_iNumDecomps
 *  The total number of decompositions that have been performed in this 
 *   solution.
 *  This should be equivalent to the number of non-leaf nodes in the tree.
 */

/**
 *  \var HtnSolution::m_vDecompTree
 *  A list of decomposition trees, one for each top-level task in the initial
 *   task network of the problem.
 *  These must be deallocated with the HtnSolution.
 */

/**
 *  \var HtnSolution::m_vParents
 *  A list of the parents of each task in the list to accomplish.  A top-level
 *   task will have a NULL parent, while any other task will have the method
 *   that decomposed into it as its parent.
 *  This is needed so that the new child can attach itself to the parent.
 *  Note that in a complete solution there are no tasks to accomplish and thus
 *   nothing in this list.
 *  Each entry in this list appears exactly once in m_vDecompTree, and will be
 *   deallocated there.
 */

/**
 *  Construct a default HtnSolution.
 *  This exists only for the benefit of FromShop and FromPddl.
 *  \param p_pDomain IN A pointer to the domain associated with the problem.
 */
HtnSolution::HtnSolution( const std::tr1::shared_ptr< HtnDomain > & p_pDomain )
  : HtnProblem( p_pDomain )
{
  m_iNumDecomps = 0;
  m_pInitState = NULL;
}

/**
 *  Retrieve a pointer to a new HtnSolution from the SHOP representation of 
 *   its problem.
 *  Note that the new "solution" is not a solution at all.  Rather, it is a
 *   blank slate on which a solution may be constructed.
 *  The process of creating an HtnProblem just to read the problem file and
 *   then deleting it seems a bit inefficient, but it works.
 *  \param p_pDomain IN A pointer to the domain associated with this problem.
 *  \param p_sInput INOUT A stream containing a SHOP representation of this.
 *  \return A poitner to a new HtnSolution.  The caller is responsible for
 *   deallocating it.
 */
HtnSolution * HtnSolution::FromShop( const std::tr1::shared_ptr< HtnDomain > & p_pDomain,
				     std::stringstream & p_sInput )
{
  HtnProblem * l_pProb = HtnProblem::FromShop( p_pDomain, p_sInput );
  HtnSolution * l_pRet = HtnSolution::FromProblem( l_pProb );
  delete l_pProb;
  return l_pRet;
}

/**
 *  Retrieve a pointer to a new HtnSolution from the PDDL representation of 
 *   its problem.
 *  Note that the new "solution" is not a solution at all.  Rather, it is a
 *   blank slate on which a solution may be constructed.
 *  The process of creating an HtnProblem just to read the problem file and
 *   then deleting it seems a bit inefficient, but it works.
 *  \param p_pDomain IN A pointer to the domain associated with this problem.
 *  \param p_sInput INOUT A stream containing a PDDL representation of this.
 *  \return A poitner to a new HtnSolution.  The caller is responsible for
 *   deallocating it.
 */
HtnSolution * HtnSolution::FromPddl( const std::tr1::shared_ptr< HtnDomain > & p_pDomain,
				     std::stringstream & p_sInput )
{
  HtnProblem * l_pProb = HtnProblem::FromPddl( p_pDomain, p_sInput );
  HtnSolution * l_pRet = HtnSolution::FromProblem( l_pProb );
  delete l_pProb;
  return l_pRet;
}

/**
 *  Retrieve a pointer to a new HtnSolution from the HtnProblem that it
 *   solves.
 *  Note that the new "solution" is not a solution at all.  Rather, it is a
 *   blank slate on which a solution may be constructed.
 *  \param p_pProblem IN A pointer to the problem to be solved.
 */
HtnSolution * HtnSolution::FromProblem( const HtnProblem * p_pProblem )
{
  HtnSolution * l_pRet = new HtnSolution( p_pProblem->GetDomain() );
  l_pRet->m_pState = new State( *p_pProblem->GetCState() );
  l_pRet->m_pInitState = new State( *l_pRet->m_pState );

  for( unsigned int i = 0; i < p_pProblem->GetNumOutstandingTasks(); i++ )
  {
    l_pRet->m_vOutstandingTasks.push_back( p_pProblem->GetTask( i ) );
    l_pRet->m_vParents.push_back( NULL );
  }

  l_pRet->m_sProbName = p_pProblem->GetName();
  l_pRet->m_iRequirements = p_pProblem->GetRequirements();
  return l_pRet;
}

/**
 *  Given a list of DecompParts, a specific DecompPart from that list, and
 *   another list of equal length, return the entry that is in the same 
 *   position in the second list as the given one is in the first.
 *  This is actually even more complicated than it sounds, because each entry
 *   in the list actually has child entries that need to be checked as well.
 *  \todo This could probably be eliminated if the DecompParts were stored in
 *   reference-counted pointers.  Or perhaps not, because they cannot be const
 *   and will have different children added to them potentially.
 *  \param p_pToFind The search term, which must exist somewhere in p_vFindIn.
 *  \param p_vFindIn The list in which to search for p_pToFind.
 *  \param p_vEquivIn The other list from which to extract.
 *  \return A pointer to the proper position in p_pEquivIn.
 */
DecompPart * FindEquiv( const DecompPart * p_pToFind,
			const std::vector< DecompPart * > & p_vFindIn,
			const std::vector< DecompPart * > & p_vEquivIn )
{
  for( unsigned int i = 0; i < p_vFindIn.size(); i++ )
  {
    if( p_vFindIn[i] == p_pToFind )
      return p_vEquivIn[i];
    DecompPart * l_pChildFind = FindEquiv( p_pToFind,
					   p_vFindIn[i]->GetChildren(),
					   p_vEquivIn[i]->GetChildren() );
    if( l_pChildFind != NULL )
      return l_pChildFind;
  }
  return NULL;
}

/**
 *  Construct an HtnSolution as a copy of an existing one.
 *  \param p_Other IN The HtnSolution to copy.
 */
HtnSolution::HtnSolution( const HtnSolution & p_Other )
  : HtnProblem( p_Other ),
    m_pInitState( new State( *p_Other.m_pInitState ) )
{
  m_iNumDecomps = p_Other.m_iNumDecomps;
  for( unsigned int i = 0; i < p_Other.m_vOperIndices.size(); i++ )
    m_vOperIndices.push_back( p_Other.m_vOperIndices[i] );
  for( unsigned int i = 0; i < p_Other.m_vOperSubs.size(); i++ )
    m_vOperSubs.push_back( new Substitution( *p_Other.m_vOperSubs[i] ) );
  for( unsigned int i = 0; i < p_Other.m_vDecompTree.size(); i++ )
    m_vDecompTree.push_back( new DecompPart( *p_Other.m_vDecompTree[i] ) );
  for( unsigned int i = 0; i < p_Other.m_vParents.size(); i++ )
  {
    if( p_Other.m_vParents[i] == NULL )
    {
      m_vParents.push_back( NULL );
    }
    else
    {
      DecompPart * l_pFound = FindEquiv( p_Other.m_vParents[i],
					 p_Other.m_vDecompTree,
					 m_vDecompTree );
      if( l_pFound != NULL )
	m_vParents.push_back( l_pFound );
      else
	throw Exception( E_NOT_IMPLEMENTED,
			 "A node's parent did not exist.",
			 __FILE__,
			 __LINE__ );
    }
  }
}

/**
 *  Destruct an HtnSolution.
 */
HtnSolution::~HtnSolution()
{
  for( unsigned int i = 0; i < m_vOperSubs.size(); i++ )
    delete m_vOperSubs[i];
  for( unsigned int i = 0; i < m_vDecompTree.size(); i++ )
    delete m_vDecompTree[i];
  delete m_pInitState;
}

/**
 *  Apply an operator to the current task and state.
 *  That is, the head of the operator modulo the substition must be the same 
 *   as the top task on the stack.  Furthermore, the preconditions of the 
 *   operator modulo the substitition must be satisfied in the current state.
 *  Applying this operator alters the current state and adds to the list.
 *  If this is a top-level task (that is, the last entry in m_vParents is NULL)
 *   then put a new entry in m_vDecompTree because it is a root.  Otherwise,
 *   add this as a child of what is currently the last m_vParent.  Either way,
 *   pop the last m_vParent.
 *  \param p_iOperIndexInDomain IN The index in the associated domain of the
 *   operator to apply.
 *  \param p_pNewSub IN A pointer to a substitution that grounds the operator.
 *  \todo This doesn't actually check the parameters!
 */
void HtnSolution::ApplyOperator( unsigned int p_iOperIndexInDomain,
				 const Substitution * p_pNewSub )
{
  const Operator * l_pOper = m_pDomain->GetCOperator( p_iOperIndexInDomain );

  if( CompareNoCase( l_pOper->GetName(), m_vOutstandingTasks.back()->GetName() ) != 0 ) 
  {
    throw Exception( E_OPER_NOT_APPLICABLE,
		     "Operator head does not match task.",
		     __FILE__,
		     __LINE__ );
  }

  FormulaP l_pPrecs( l_pOper->GetCPreconditions()->AfterSubstitution( *p_pNewSub, 0 ) );
  if( !m_pState->IsConsistent( l_pPrecs ) )
  {
    throw Exception( E_OPER_NOT_APPLICABLE,
		     "Operator preconditions are not satisfied.",
		     __FILE__,
		     __LINE__ );
  }

  State * l_pOldState = m_pState;
  m_pState = l_pOldState->NextState( m_pDomain->GetCOperator( p_iOperIndexInDomain ), p_pNewSub );
  delete l_pOldState;
  m_vOperIndices.push_back( p_iOperIndexInDomain );
  m_vOperSubs.push_back( new Substitution( *p_pNewSub ) );

  DecompPart * l_pNewPart = new DecompPart( m_vOutstandingTasks.back()->ToStr(), "" );
  if( m_vParents.back() == NULL )
    m_vDecompTree.push_back( l_pNewPart );
  else
    m_vParents.back()->AddChild( l_pNewPart );
  m_vOutstandingTasks.pop_back();
  m_vParents.pop_back();
}

/**
 *  Apply a method to the current task and state.
 *  That is, the head of the method modulo the substition must be the same 
 *   as the top task on the stack.  Furthermore, the preconditions of the 
 *   method modulo the substitition must be satisfied in the current state.
 *  Applying this method pops the last task to be accomplished and adds its
 *   subtasks to this list in reverse order.
 *  If this is a top-level task (that is, the last entry in m_vParents is NULL)
 *   then put a new entry in m_vDecompTree because it is a root.  Otherwise,
 *   add this as a child of what is currently the last m_vParent.  Either way,
 *   pop the last m_vParent and put new entires in m_vParent of this for each
 *   subtask.
 *  \param p_iMethodIndexInDomain IN The index in the associated domain of the
 *   method to apply.
 *  \param p_pNewSub IN A pointer to a substitution that grounds the method.
 *  \todo This doesn't actually check the parameters!
 */
void HtnSolution::ApplyMethod( unsigned int p_iMethodIndexInDomain, 
			       const Substitution * p_pNewSub )
{
  m_iNumDecomps++;
  const HtnMethod * l_pMethod = m_pDomain->GetCMethod( p_iMethodIndexInDomain );
  HtnTaskHeadP l_pHead( std::tr1::dynamic_pointer_cast< HtnTaskHead >( l_pMethod->GetCHead()->AfterSubstitution( *p_pNewSub, 0 ) ) );
  if( !( *l_pHead == *m_vOutstandingTasks.back() ) )
  {
    std::stringstream l_sMessage;
    l_sMessage << "Attempt to apply method index " << p_iMethodIndexInDomain << " to task " << m_vOutstandingTasks.back()->ToStr() << "\n\n";
    l_sMessage << l_pMethod->ToPddl( m_pDomain->GetRequirements() ) << "\n\n";
    l_sMessage << p_pNewSub->ToStr() << "\n\n";
    l_sMessage << "The head of the method, with the above substitution applied, does not match the task.\n";
    throw Exception( E_METHOD_NOT_APPLICABLE,
		     l_sMessage.str(),
		     __FILE__,
		     __LINE__ );
  }

  FormulaP l_pPrecs( l_pMethod->GetCPreconditions()->AfterSubstitution( *p_pNewSub, 0 ) );
  if( !m_pState->IsConsistent( l_pPrecs ) )
  {
    throw Exception( E_METHOD_NOT_APPLICABLE,
		     "Method preconditions are not satisfied.",
		     __FILE__,
		     __LINE__ );
  }

  DecompPart * l_pNewPart = new DecompPart( l_pHead->ToStr(), l_pMethod->GetId() );
  if( m_vParents.back() == NULL )
    m_vDecompTree.push_back( l_pNewPart );
  else
    m_vParents.back()->AddChild( l_pNewPart );

  m_vOutstandingTasks.pop_back();
  m_vParents.pop_back();

  std::vector< HtnTaskHeadP > l_vTasks;

  for( unsigned int i = 0; i < l_pMethod->GetNumSubtasks(); i++ )
  {
    HtnTaskHeadP l_pTask( std::tr1::dynamic_pointer_cast< HtnTaskHead >( l_pMethod->GetCSubtask( i )->AfterSubstitution( *p_pNewSub, 0 ) ) );

    l_vTasks.push_back( l_pTask );
  }

  while( !l_vTasks.empty() )
  {
    m_vOutstandingTasks.push_back( l_vTasks.back() );
    m_vParents.push_back( l_pNewPart );
    l_vTasks.pop_back();
  }
}

/**
 *  Retrieve the length of the plan thus far.
 *  \return The number of leaves in the current decomposition tree.
 */
unsigned int HtnSolution::GetPlanLength() const
{
  return m_vOperIndices.size();
}

/**
 *  Retrieve a pointer to one of the operators in this plan.
 *  \param p_iIndex IN The 0-based index of the desired operator.
 *  \return A pointer to the desired operator.
 */
const Operator * HtnSolution::GetCOperator( unsigned int p_iIndex ) const
{
  if( p_iIndex < m_vOperIndices.size() )
    return m_pDomain->GetCOperator( m_vOperIndices[ p_iIndex ] );
  else
  {
    throw Exception( E_INDEX_OUT_OF_BOUNDS,
		     "Bounds error.",
		     __FILE__,
		     __LINE__ );
  }
}

/**
 *  Retrieve a pointer to one of the substitutions for the actions in this 
 *   plan.
 *  \param p_iIndex IN The 0-based index of the desired substitution.
 *  \return A pointer to the desired substitution.
 */
const Substitution * HtnSolution::GetCSubstitution( unsigned int p_iIndex ) const
{
  if( p_iIndex < m_vOperSubs.size() )
    return m_vOperSubs[ p_iIndex ];
  else
  {
    throw Exception( E_INDEX_OUT_OF_BOUNDS,
		     "Bounds error.",
		     __FILE__,
		     __LINE__ );
  }
}

/**
 *  Determine whether or not this is a complete solution.
 *  \return Whether or not this is a complete solution.
 */
bool HtnSolution::IsComplete() const
{
  return m_vOutstandingTasks.empty();
}

/**
 *  Retrieve a string represenation of this solution.
 *  \param p_bIncludeTrace IN Whether or not to print the full decomposition
 *   tree with the plan.
 *  \return A string representation of this solution.
 */
std::string HtnSolution::Print( bool p_bIncludeTrace ) const
{
  std::string l_sRet = "";

  State * l_pIntermediateState = NULL;

  if( p_bIncludeTrace )
  {
    l_pIntermediateState = new State( *m_pInitState );
    l_sRet += "\nState:\n";
    l_sRet += l_pIntermediateState->ToStr();
    l_sRet += "\n";
  }

  for( unsigned int j = 0; j < GetPlanLength(); j++ )
  {
    l_sRet += "\t( "; 
    l_sRet += GetCOperator( j )->GetName();
    l_sRet += " ";

    for( unsigned int k = 0; k < GetCOperator( j )->GetNumParams(); k++ )
    {
      l_sRet += GetCOperator( j )->GetCParam( k )->AfterSubstitution( *GetCSubstitution( j ), 0 )->ToStr();
      l_sRet += " ";
    }

    l_sRet +=  ")\n";

    if( p_bIncludeTrace )
    {
      l_sRet += "\nState:\n";
      State * l_pTempState = l_pIntermediateState->NextState( GetCOperator( j ), GetCSubstitution( j ) );
      delete l_pIntermediateState;
      l_pIntermediateState = l_pTempState;
      l_sRet += l_pIntermediateState->ToStr();
      l_sRet += "\n";
    }
  }

  if( p_bIncludeTrace )
  {
    l_sRet += "\n\nTrace\n";
    for( unsigned int i = 0; i < m_vDecompTree.size(); i++ )
      l_sRet += m_vDecompTree[i]->Print( "" );
  }
  return l_sRet;
}

/**
 *  Retrieve a representation of this partial solution.
 *  \todo This should probably be renamed.
 *  \return A representation of this partial solution.
 */
std::string HtnSolution::ToStr() const
{
  std::string l_sRet = "";

  l_sRet += "  Task List:\n";
  for( unsigned int i = m_vOutstandingTasks.size() - 1; i >= 0; i-- )
  {
    l_sRet += "    ";
    l_sRet += m_vOutstandingTasks[i]->ToStr();
    l_sRet += "\n";
  }

  l_sRet += "  Current Plan:\n";
  for( unsigned int j = 0; j < GetPlanLength(); j++ )
  {
    l_sRet += "    ( "; 
    l_sRet += GetCOperator( j )->GetName();
    l_sRet += " ";

    for( unsigned int k = 0; k < GetCOperator( j )->GetNumParams(); k++ )
    {
      l_sRet += GetCOperator( j )->GetCParam( k )->AfterSubstitution( *GetCSubstitution( j ), 0 )->ToStr();
      l_sRet += " ";
    }

    l_sRet +=  ")\n";
  }
  return l_sRet;
}

/**
 *  Retrieve the number of decompositions in this solution.
 *  \return The number of decompositions in this solution.
 */
int HtnSolution::GetNumDecomps() const
{
  return m_iNumDecomps;
}

size_t HtnSolution::GetMemSizeMin() const
{
  size_t l_iSize = HtnProblem::GetMemSizeMin() + sizeof( HtnSolution ) - sizeof( HtnProblem ) + m_pInitState->GetMemSizeMin() + m_vOperIndices.capacity() * sizeof( unsigned int ) + m_vOperSubs.capacity() * sizeof( Substitution * ) + m_vDecompTree.capacity() * sizeof( DecompPart * ) + m_vParents.capacity() * sizeof( DecompPart * );
  for( unsigned int i = 0; i < m_vOperSubs.size(); i++ )
    l_iSize += m_vOperSubs[i]->GetMemSizeMin();
  for( unsigned int i = 0; i < m_vDecompTree.size(); i++ )
    l_iSize += m_vDecompTree[i]->GetMemSizeMin();
  return l_iSize;
}

size_t HtnSolution::GetMemSizeMax() const
{
  size_t l_iSize = HtnProblem::GetMemSizeMin() + sizeof( HtnSolution ) - sizeof( HtnProblem ) + m_pInitState->GetMemSizeMin() + m_vOperIndices.capacity() * sizeof( unsigned int ) + m_vOperSubs.capacity() * sizeof( Substitution * ) + m_vDecompTree.capacity() * sizeof( DecompPart * ) + m_vParents.capacity() * sizeof( DecompPart * );
  for( unsigned int i = 0; i < m_vOperSubs.size(); i++ )
    l_iSize += m_vOperSubs[i]->GetMemSizeMax();
  for( unsigned int i = 0; i < m_vDecompTree.size(); i++ )
    l_iSize += m_vDecompTree[i]->GetMemSizeMax();
  return l_iSize;
}

const std::vector< DecompPart * > & HtnSolution::GetDecompTree() const
{
  return m_vDecompTree;
}
