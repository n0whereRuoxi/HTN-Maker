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
#include "strips_domain.hpp"
#include "strips_problem.hpp"

/** \file strips_problem.hpp
 *  Declaration of the StripsProblem class.
 */

/** \file strips_problem.cpp
 *  Definition of the StripsProblem class.
 */

/** \class StripsProblem
 *  A classical (STRIPS) planning problem.
 */

/** \var StripsProblem::m_sName
 *  The name of this problem.
 */

/** \var StripsProblem::m_pDomain
 *  A pointer to the domain to which this problem is associated.
 */

/** \var StripsProblem::m_pInitState
 *  A pointer to the initial State in this problem.
 *  This will need to be deallocated with the StripsProblem.
 */

/** \var StripsProblem::m_pGoals
 *  A pointer to a Formula that specifies the goals of this planning problem.
 *  This should be either a predicate or a conjunction of predicates.
 *  This will need to be deallocated with the StripsProblem.
 */

/** \var StripsProblem::m_iRequirements
 *  The PDDL requirements of this problem.
 *  \todo They should be the same as those of the associated domain.
 */

/** \var StripsProblem::m_ObjectTypes
 *  A table of the objects in this problem with their types.  If neither this
 *   nor the constants table in the domain are empty, this must be a subset of
 *   it.  If this is empty, and the constants table in the domain is not, then
 *   all constants from the domain are determined to be objects in the problem.
 */

/**
 *  Construct a StripsProblem from a string containing its PDDL representation
 *   and a pointer to the domain to which is associated.
 *  \param p_sProblem IN A string containing the PDDL representation of the
 *   problem.
 *  \param p_pDomain IN A pointer to the domain associated with this problem.
 */
StripsProblem::StripsProblem( std::string p_sProblem,
			      const std::tr1::shared_ptr< StripsDomain > & p_pDomain )
  : m_pDomain( p_pDomain )
{
  m_pInitState = NULL;
  m_iRequirements = PDDL_REQ_STRIPS;

  std::stringstream l_sStream( p_sProblem );

  EatWhitespace( l_sStream );
  EatString( l_sStream, "(" );
  EatWhitespace( l_sStream );
  EatString( l_sStream, "define" );
  EatWhitespace( l_sStream );
  EatString( l_sStream, "(" );
  EatWhitespace( l_sStream );
  EatString( l_sStream, "problem" );
  EatWhitespace( l_sStream );

  m_sName = ReadString( l_sStream );

  EatWhitespace( l_sStream );
  EatString( l_sStream, ")" );

  EatWhitespace( l_sStream );
  EatString( l_sStream, "(" );
  EatWhitespace( l_sStream );
  EatString( l_sStream, ":domain" );
  EatWhitespace( l_sStream );
  
  std::string l_sDomainName = ReadString( l_sStream );

  if( CompareNoCase( l_sDomainName, m_pDomain->GetName() ) != 0 )
    throw Exception( E_DOMAIN_MATCH,
		     "The domain for the specified problem is not the domain passed in.",
		     __FILE__,
		     __LINE__ );
    
  EatWhitespace( l_sStream );
  EatString( l_sStream, ")" );
  EatWhitespace( l_sStream );

  bool l_bHasRequirements = false,
    l_bHasObjects = false,
    l_bHasInit = false,
    l_bHasGoal = false;

  while( l_sStream.peek() == '(' )
  {
    std::string l_sFeature = ReadParenthetical( l_sStream );
    EatWhitespace( l_sStream );

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
      m_iRequirements = ParseRequirements( l_sFeature, false );
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
	if( m_pDomain->GetRequirements() & PDDL_REQ_TYPING )
	{
	  EatString( l_sFeatureStream, "-" );
	  EatWhitespace( l_sFeatureStream );
	  l_sTyping = ReadString( l_sFeatureStream );
	  EatWhitespace( l_sFeatureStream );
	}

	if( m_ObjectTypes.find( l_sNewObj ) == m_ObjectTypes.end() )
	{
	  if( m_pDomain->GetConstantTypes().size() > 0 )
	  {
	    if( m_pDomain->GetConstantTypes().find( l_sNewObj ) == 
		m_pDomain->GetConstantTypes().end() )
	      throw Exception( E_NOT_IMPLEMENTED,
			       "Object " + l_sNewObj + " is not a declared constant in the domain file.",
			       __FILE__,
			       __LINE__ );
	    else if( CompareNoCase( (*(m_pDomain->GetConstantTypes().find( l_sNewObj ))).second, l_sTyping ) != 0 )
	      throw Exception( E_NOT_IMPLEMENTED,
			       "Object " + l_sNewObj + " is not of the same type as constant " + l_sNewObj + " in the domain file.",
			       __FILE__,
			       __LINE__ );
	  }
	  m_ObjectTypes[l_sNewObj] = l_sTyping;
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

      m_pInitState = new State( l_sInitState, 0, ( m_ObjectTypes.size() > 0 ? m_ObjectTypes : p_pDomain->GetConstantTypes() ), p_pDomain->GetAllowablePredicates() );
    }
    else if( CompareNoCase( l_sFeatureName, ":goal" ) == 0 )
    {
      if( l_bHasGoal )
	throw Exception( E_NOT_IMPLEMENTED,
			 "A PDDL problem may not have multiple goal blocks.",
			 __FILE__,
			 __LINE__ );
      l_bHasGoal = true;
      std::stringstream l_sGoalStream( ReadParenthetical( l_sFeatureStream ) );
      m_pGoals = FormulaP( NewFormula( l_sGoalStream, ( m_ObjectTypes.size() > 0 ? m_ObjectTypes : p_pDomain->GetConstantTypes() ), p_pDomain->GetAllowablePredicates() ) );
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

  EatString( l_sStream, ")" );

  if( !l_bHasInit )
    throw Exception( E_NOT_IMPLEMENTED,
		     "A problem file must contain an initial state block.",
		     __FILE__,
		     __LINE__ );
  if( !l_bHasGoal )
    throw Exception( E_NOT_IMPLEMENTED,
		     "A problem file must contain a goal block.",
		     __FILE__,
		     __LINE__ );
}

/**
 *  Construct a StripsProblem as a copy of an existing one.
 *  \param p_Other IN The StripsProblem to copy.
 */
StripsProblem::StripsProblem( const StripsProblem & p_Other )
  : m_pDomain( p_Other.m_pDomain ),
    m_pGoals( p_Other.m_pGoals )
{
  m_sName = p_Other.m_sName;
  m_pInitState = new State( *p_Other.m_pInitState );
  m_iRequirements = p_Other.m_iRequirements;
  m_ObjectTypes = p_Other.m_ObjectTypes;
}

/**
 *  Destruct this StripsProblem.
 */
StripsProblem::~StripsProblem()
{
  delete m_pInitState;
}

/**
 *  Retrieve the name of this StripsProblem.
 *  \return The name of this StripsProblem.
 */
std::string StripsProblem::GetName() const
{
  return m_sName;
}

/**
 *  Retrieve a pointer to the domain associated with this problem.
 *  \return A pointer to the domain associated with this problem.
 */
std::tr1::shared_ptr< StripsDomain > StripsProblem::GetCDomain() const
{
  return m_pDomain;
}

/**
 *  Retrieve a pointer to the initial state of this problem.
 *  \return A pointer to the initial state of this problem.  This pointer has
 *   the same lifetime as the StripsProblem.
 */
const State * StripsProblem::GetCInitState() const
{
  return m_pInitState;
}

/**
 *  Retrieve a pointer to the goals of this problem.
 *  \return A pointer to the goals of this problem.  This pointer has the same
 *   lifetime as the StripsProblem.
 */
FormulaP StripsProblem::GetCGoals() const
{
  return m_pGoals;
}

/**
 *  Retrieve a string containing the PDDL representation of this StripsProblem.
 *  \return A string containing the PDDL representation of this StripsProblem.
 */
std::string StripsProblem::ToPddl() const
{
  std::string l_sRet = "";

  l_sRet += "( define ( problem " + m_sName + " )\n";
  l_sRet += "  ( :domain " + m_pDomain->GetName() + " )\n";
  l_sRet += "  " + PrintRequirements( m_iRequirements ) + "\n";
  l_sRet += m_pInitState->ToPddl();
  l_sRet += "  ( :goal\n";
  l_sRet += m_pGoals->ToStr();
  l_sRet += "  )\n";
  l_sRet += ")\n";

  return l_sRet;
}

/**
 *  Retrieve a table of objects in this problem with their associated types.
 *  \return A table of objects in this problem with their associated types.
 */
const TypeTable & StripsProblem::GetObjectTypes() const
{
  if( m_ObjectTypes.size() > 0 )
    return m_ObjectTypes;
  else
    return m_pDomain->GetConstantTypes();
}

size_t StripsProblem::GetMemSizeMin() const
{
  size_t l_iSize = sizeof( StripsProblem ) + m_pInitState->GetMemSizeMin() + m_ObjectTypes.GetMemSizeMin();
  return l_iSize;
}

size_t StripsProblem::GetMemSizeMax() const
{
  size_t l_iSize = sizeof( StripsProblem ) + m_pInitState->GetMemSizeMax() + m_pGoals->GetMemSizeMax() + m_ObjectTypes.GetMemSizeMax();
  return l_iSize;
}
