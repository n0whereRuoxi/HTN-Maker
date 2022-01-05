#include <vector>
#include <string>
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

/** \file htn_problem.hpp
 *  Declaration of the HtnProblem class.
 */

/** \file htn_problem.cpp
 *  Definition of the HtnProblem class.
 */

/** \class HtnProblem
 *  A Hierarchical Task Network planning problem.
 *  That is, an initial state and an initial ordered list of tasks to 
 *   accomplish.
 */

/** \var HtnProblem::m_pDomain
 *  A reference-counted pointer to the domain associated with this problem.
 */

/** \var HtnProblem::m_pState
 *  A pointer to the initial state.
 *  This must be deallocated with the HtnProblem.
 */

/** \var HtnProblem::m_vOutstandingTasks
 *  A list of smart pointers to the tasks in the initial task network.
 *  Note that these are stored in reverse order.  That is, the first task to be
 *   accomplished is the last one in the list.
 */

/** \var HtnProblem::m_sProbName
 *  The name of this problem.
 */

/** \var HtnProblem::m_iRequirements
 *  The PDDL requirements of this problem.
 *  See the constants in funcs.hpp.
 */

/**
 *  Construct a default HtnProblem.
 *  This exists only for HtnProblem::FromShop() and HtnProblem::FromPddl().
 *  \param p_pDomain IN A smart pointer to the domain associated with this 
 *   problem.
 */
HtnProblem::HtnProblem( const std::tr1::shared_ptr< HtnDomain > & p_pDomain )
  : m_pDomain( p_pDomain )
{
  m_pState = NULL;
}

/**
 *  Construct an HtnProblem as a copy of an existing one.
 *  \param p_Other IN The problem to copy.
 */
HtnProblem::HtnProblem( const HtnProblem & p_Other )
  : m_pDomain( p_Other.m_pDomain )
{
  m_pState = new State( *p_Other.m_pState );
  for( unsigned int i = 0; i < p_Other.m_vOutstandingTasks.size(); i++ )
    m_vOutstandingTasks.push_back( p_Other.m_vOutstandingTasks[i] );
  m_sProbName = p_Other.m_sProbName;
  m_iRequirements = p_Other.m_iRequirements;
}

/**
 *  Destruct an HtnProblem.
 */
HtnProblem::~HtnProblem()
{
  delete m_pState;
}

/**
 *  Retrieve a pointer to a new HtnProblem from its SHOP representation.
 *  \param p_pDomain IN A smart pointer to the associated domain.
 *  \param p_sInput INOUT A stream containing a textual representation of the
 *   problem in the SHOP formalism.  The stream will be advanced beyond this.
 *  \return A pointer to a new HtnProblem based on the text.  The caller is
 *   responsible for deallocating this pointer.
 */
HtnProblem * HtnProblem::FromShop( const std::tr1::shared_ptr< HtnDomain > & p_pDomain,
				   std::stringstream & p_sInput )
{
  HtnProblem * l_pRet = new HtnProblem( p_pDomain );

  EatWhitespace( p_sInput );
  EatString( p_sInput, "(" );
  EatWhitespace( p_sInput );
  EatString( p_sInput, "defproblem" );
  EatWhitespace( p_sInput );
  l_pRet->m_sProbName = ReadString( p_sInput );
  EatWhitespace( p_sInput );
  std::string l_sDomainName = ReadString( p_sInput );
  EatWhitespace( p_sInput );

  if( CompareNoCase( l_sDomainName, p_pDomain->GetDomainName() ) != 0 )
    throw Exception( E_DOMAIN_MATCH,
		     "The problem file does not match the domain file.",
		     __FILE__,
		     __LINE__ );

  l_pRet->m_pState = new State( p_sInput, 0, p_pDomain->GetConstantTypes(), p_pDomain->GetAllowablePredicates() );

  EatWhitespace( p_sInput );
  EatString( p_sInput, "(" );
  EatWhitespace( p_sInput );

  std::vector< HtnTaskHeadP > l_vTasks;

  while( p_sInput.peek() == '(' )
  {
    l_vTasks.push_back( HtnTaskHeadP( new HtnTaskHead( p_sInput, p_pDomain->GetConstantTypes() ) ) );
    EatWhitespace( p_sInput );
  }

  EatString( p_sInput, ")" );
  EatWhitespace( p_sInput );
  EatString( p_sInput, ")" );

  while( !l_vTasks.empty() )
  {
    l_pRet->m_vOutstandingTasks.push_back( l_vTasks.back() );
    l_vTasks.pop_back();
  }

  l_pRet->m_iRequirements = PDDL_REQ_STRIPS | PDDL_REQ_HTN;

  return l_pRet;
}

/**
 *  Retrieve a pointer to a new HtnProblem based on its PDDL representation.
 *  \param p_pDomain IN A smart pointer to the associated domain.
 *  \param p_sInput INOUT A stream containing a textual representation of this
 *   problem in the PDDL language.  The stream is advanced beyond this.
 *  \return A pointer to a new HtnProblem based on the text.  The caller is
 *   responsible for deallocating it.
 */
HtnProblem * HtnProblem::FromPddl( const std::tr1::shared_ptr< HtnDomain > & p_pDomain,
				   std::stringstream & p_sInput )
{
  HtnProblem * l_pRet = new HtnProblem( p_pDomain );
  l_pRet->m_iRequirements = PDDL_REQ_STRIPS | PDDL_REQ_HTN;

  TypeTable l_ObjectTypes;

  EatWhitespace( p_sInput );
  EatString( p_sInput, "(" );
  EatWhitespace( p_sInput );
  EatString( p_sInput, "define" );
  EatWhitespace( p_sInput );
  EatString( p_sInput, "(" );
  EatWhitespace( p_sInput );
  EatString( p_sInput, "htn-problem" );
  EatWhitespace( p_sInput );

  l_pRet->m_sProbName = ReadString( p_sInput );

  EatWhitespace( p_sInput );
  EatString( p_sInput, ")" );
  EatWhitespace( p_sInput );
  EatString( p_sInput, "(" );
  EatWhitespace( p_sInput );
  EatString( p_sInput, ":domain" );
  EatWhitespace( p_sInput );

  std::string l_sDomainName = ReadString( p_sInput );
  EatWhitespace( p_sInput );

  if( CompareNoCase( l_sDomainName, p_pDomain->GetDomainName() ) != 0 )
    throw Exception( E_DOMAIN_MATCH,
		     "The problem file does not match the domain file.",
		     __FILE__,
		     __LINE__ );

  EatString( p_sInput, ")" );
  EatWhitespace( p_sInput );

  bool l_bHasRequirements = false,
    l_bHasObjects = false,
    l_bHasInit = false,
    l_bHasGoal = false;

  while( p_sInput.peek() == '(' )
  {
    std::string l_sFeature = ReadParenthetical( p_sInput );
    EatWhitespace( p_sInput );

    std::stringstream l_sFeatureStream( l_sFeature );
    EatString( l_sFeatureStream, "(" );
    EatWhitespace( l_sFeatureStream );
    std::string l_sFeatureName = ReadString( l_sFeatureStream );
    EatWhitespace( l_sFeatureStream );

    if( CompareNoCase( l_sFeatureName, ":requirements" ) == 0 )
    {
      if( l_bHasRequirements )
	throw Exception( E_NOT_IMPLEMENTED,
			 "A PDDL problem may not have multiple requirements blocks.",
			 __FILE__,
			 __LINE__ );
      if( l_bHasObjects )
	throw Exception( E_NOT_IMPLEMENTED,
			 "The PDDL rqeuirements block must come before the objects block.",
			 __FILE__,
			 __LINE__ );
      if( l_bHasInit )
	throw Exception( E_NOT_IMPLEMENTED,
			 "The PDDL rqeuirements block must come before the init block.",
			 __FILE__,
			 __LINE__ );
      if( l_bHasGoal )
	throw Exception( E_NOT_IMPLEMENTED,
			 "The PDDL requirements block must come before the goal block.",
			 __FILE__,
			 __LINE__ );
      l_bHasRequirements = true;
      l_pRet->m_iRequirements = ParseRequirements( l_sFeature, true );
    }
    else if( CompareNoCase( l_sFeatureName, ":objects" ) == 0 )
    {
      if( l_bHasObjects )
	throw Exception( E_NOT_IMPLEMENTED,
			 "A PDDL problem may not have multiple objects blocks.",
			 __FILE__,
			 __LINE__ );
      if( l_bHasInit )
	throw Exception( E_NOT_IMPLEMENTED,
			 "The PDDL objects block must come before the init block.",
			 __FILE__,
			 __LINE__ );
      if( l_bHasGoal )
	throw Exception( E_NOT_IMPLEMENTED,
			 "The PDDL objects block must come before the goal block.",
			 __FILE__,
			 __LINE__ );
      l_bHasObjects = true;

      while( l_sFeatureStream.peek() != ')' )
      {
	std::string l_sNewObj = ReadString( l_sFeatureStream );
	EatWhitespace( l_sFeatureStream );
	std::string l_sTyping = "";
	if( p_pDomain->GetRequirements() & PDDL_REQ_TYPING )
	{
	  EatString( l_sFeatureStream, "-" );
	  EatWhitespace( l_sFeatureStream );
	  l_sTyping = ReadString( l_sFeatureStream );
	  EatWhitespace( l_sFeatureStream );
	}

	if( l_ObjectTypes.find( l_sNewObj ) == l_ObjectTypes.end() )
	{
	  if( p_pDomain->GetConstantTypes().size() > 0 )
	  {
	    if( p_pDomain->GetConstantTypes().find( l_sNewObj ) == 
		p_pDomain->GetConstantTypes().end() )
	      throw Exception( E_NOT_IMPLEMENTED,
			       "Object " + l_sNewObj + " is not a declared constant in the domain file.",
			       __FILE__,
			       __LINE__ );
	    else if( CompareNoCase( (*(p_pDomain->GetConstantTypes().find( l_sNewObj ))).second, l_sTyping ) != 0 )
	      throw Exception( E_NOT_IMPLEMENTED,
			       "Object " + l_sNewObj + " is not of the same type as constant " + l_sNewObj + " in the domain file.",
			       __FILE__,
			       __LINE__ );
	  }
	  l_ObjectTypes[l_sNewObj] = l_sTyping;
	}
	else
	  throw Exception( E_NOT_IMPLEMENTED,
			   "Object " + l_sNewObj + " has been declared twice.",
			   __FILE__,
			   __LINE__ );
      }
    }
    else if( CompareNoCase( l_sFeatureName, ":init" ) == 0 )
    {
      if( l_bHasInit )
	throw Exception( E_NOT_IMPLEMENTED,
			 "A PDDL problem may not have multiple init blocks.",
			 __FILE__,
			 __LINE__ );
      if( l_bHasGoal )
	throw Exception( E_NOT_IMPLEMENTED,
			 "The PDDL init block must come before the goal block.",
			 __FILE__,
			 __LINE__ );
      l_bHasInit = true;
      std::string l_sInitState = "( ";
      while( l_sFeatureStream.peek() == '(' )
      {
	l_sInitState += ReadParenthetical( l_sFeatureStream );
	l_sInitState += " ";
	EatWhitespace( l_sFeatureStream );
      }
      l_sInitState += ")";

      l_pRet->m_pState = new State( l_sInitState, 0, ( l_ObjectTypes.size() > 0 ? l_ObjectTypes : p_pDomain->GetConstantTypes() ), p_pDomain->GetAllowablePredicates() );
    }
    else if( CompareNoCase( l_sFeatureName, ":tasks" ) == 0 )
    {
      if( l_bHasGoal )
	throw Exception( E_NOT_IMPLEMENTED,
			 "A PDDL problem may not have multiple task blocks.",
			 __FILE__,
			 __LINE__ );
      l_bHasGoal = true;
      std::vector< HtnTaskHeadP > l_vTasks;
      while( l_sFeatureStream.peek() == '(' )
      {
	l_vTasks.push_back( HtnTaskHeadP( new HtnTaskHead( l_sFeatureStream, ( l_ObjectTypes.size() > 0 ? l_ObjectTypes : p_pDomain->GetConstantTypes() ) ) ) );
	EatWhitespace( p_sInput );
      }

      for( unsigned int i = l_vTasks.size(); i > 0; i-- )
	l_pRet->m_vOutstandingTasks.push_back( l_vTasks[i-1] );

      EatString( l_sFeatureStream, ")" );
    }
    else if( CompareNoCase( l_sFeatureName, ":goal" ) == 0 )
    {
      throw Exception( E_NOT_IMPLEMENTED,
		       "An HTN problem may not contain a goal block.",
		       __FILE__,
		       __LINE__ );
    }
    else if( CompareNoCase( l_sFeatureName, ":constraints" ) == 0 )
    {
      throw Exception( E_NOT_IMPLEMENTED,
		       "The constraints feature of PDDL is not supported.",
		       __FILE__,
		       __LINE__ );
    }
    else if( CompareNoCase( l_sFeatureName, ":metric" ) == 0 )
    {
      throw Exception( E_NOT_IMPLEMENTED,
		       "The metric feature of PDDL is not supported.",
		       __FILE__,
		       __LINE__ );
    }
    else
    {
      std::string l_sMessage;
      l_sMessage = "Unknown PDDL feature ";
      l_sMessage += l_sFeatureName;
      l_sMessage += ".";
      throw Exception( E_NOT_IMPLEMENTED,
		       l_sMessage,
		       __FILE__,
		       __LINE__ );
    }
  }

  EatString( p_sInput, ")" );

  if( !l_bHasInit )
    throw Exception( E_NOT_IMPLEMENTED,
		     "An htn-problem file must contain an initial state block.",
		     __FILE__,
		     __LINE__ );
  if( !l_bHasGoal )
    throw Exception( E_NOT_IMPLEMENTED,
		     "An htn-problem file must contain a tasks block.",
		     __FILE__,
		     __LINE__ );

  return l_pRet;
}

/**
 *  Retrieve a string containing the PDDL representation of this domain.
 *  \return A string containing the PDDL representation of this domain.
 */
std::string HtnProblem::ToPddl() const
{
  std::string l_sRet = "";

  l_sRet += "( define ( htn-problem " + m_sProbName + " )\n";
  l_sRet += "  ( :domain " + m_pDomain->GetDomainName() + " )\n";
  l_sRet += "  " + PrintRequirements( m_iRequirements ) + "\n";
  l_sRet += m_pState->ToPddl();
  l_sRet += "  ( :tasks\n";
  for( unsigned int i = m_vOutstandingTasks.size() - 1; i >= 0; i-- )
  {
    l_sRet += "    " + m_vOutstandingTasks[i]->ToStr() + "\n";
  }
  l_sRet += "  )\n";
  l_sRet += ")\n";

  return l_sRet;
}

/**
 *  Retrieve a pointer to the initial state.
 *  \return A pointer to the initial state.  This pointer has the same
 *   lifetime as this HtnProblem.
 */
const State * HtnProblem::GetCState() const
{
  return m_pState;
}

/**
 *  Retrieve a smart pointer to the first task to be accomplished.
 *  \return A smart pointer to the first task to be accomplished.
 */
HtnTaskHeadP HtnProblem::GetCTopTask() const
{
  if( m_vOutstandingTasks.empty() )
    throw Exception( E_INDEX_OUT_OF_BOUNDS,
		     "Bounds error.",
		     __FILE__,
		     __LINE__ );
  else
    return m_vOutstandingTasks.back();
}

/**
 *  Retrieve the number of tasks to be accomplished.
 *  \return The number of tasks to be accomplished.
 */
unsigned int HtnProblem::GetNumOutstandingTasks() const
{
  return m_vOutstandingTasks.size();
}

/**
 *  Determine whether or not this is equivalent to some other domain.
 *  Note that this does not verify that the associated domains are equivalent
 *   for efficiency.  However, it would be quite difficult to have everything
 *   else the same with different domains.
 *  \param p_Other IN The other domain.
 *  \return Whether or not the other domain is equivalent to this.
 */
bool HtnProblem::Equivalent( const HtnProblem & p_Other ) const
{
  if( m_iRequirements != p_Other.m_iRequirements )
    return false;
  if( m_vOutstandingTasks.size() != p_Other.m_vOutstandingTasks.size() )
    return false;
  if( !( *m_pState == *p_Other.m_pState ) )
    return false;
  if( CompareNoCase( m_sProbName, p_Other.m_sProbName ) != 0 )
    return false;

  for( unsigned int i = 0; i < m_vOutstandingTasks.size(); i++ )
  {
    if( !( *m_vOutstandingTasks[i] == *p_Other.m_vOutstandingTasks[i] ) )
      return false;
  }

  return true;
}

/**
 *  Retrieve a smart pointer to the domain associated with this problem.
 *  \return A reference-counted pointer to the domain associated with this 
 *   problem.
 */
const std::tr1::shared_ptr< HtnDomain > & HtnProblem::GetDomain() const
{
  return m_pDomain;
}

/**
 *  Retrieve a smart pointer to one of the tasks in this problem.
 *  \param p_iIndex IN The index of the desired task.  Note that these are
 *   stored in reverse order of how they need to be accomplished.
 *  \return A smart pointer to the requested task.
 */
HtnTaskHeadP HtnProblem::GetTask( unsigned int p_iIndex ) const
{
  if( p_iIndex < m_vOutstandingTasks.size() )
    return m_vOutstandingTasks[p_iIndex];
  else
    throw Exception( E_INDEX_OUT_OF_BOUNDS,
		     "Index out of bounds in HtnTaskHead::GetTask().",
		     __FILE__,
		     __LINE__ );
}

/**
 *  Retrieve the name of this problem.
 *  \return The name of this problem.
 */
const std::string & HtnProblem::GetName() const
{
  return m_sProbName;
}

/**
 *  Retrieve the requirements of this problem.
 *  \return The requirements of this problem.
 */
long HtnProblem::GetRequirements() const
{
  return m_iRequirements;
}

size_t HtnProblem::GetMemSizeMin() const
{
  size_t l_iSize = sizeof( HtnProblem ) + m_pState->GetMemSizeMin() + m_vOutstandingTasks.capacity() * sizeof( HtnTaskHeadP );
  return l_iSize;
}

size_t HtnProblem::GetMemSizeMax() const
{
  size_t l_iSize = sizeof( HtnProblem ) + m_pState->GetMemSizeMax() + m_vOutstandingTasks.capacity() * sizeof( HtnTaskHeadP );
  for( unsigned int i = 0; i < m_vOutstandingTasks.size(); i++ )
    l_iSize += m_vOutstandingTasks[i]->GetMemSizeMax();
  return l_iSize;
}
