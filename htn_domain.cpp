#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <cassert>
#include <set>
#include <tr1/memory>
#include <algorithm>

#include "exception.hpp"
#include "funcs.hpp"
#include "term.hpp"
#include "term_string.hpp"
#include "term_constant.hpp"
#include "term_variable.hpp"
#include "type_table.hpp"
#include "formula.hpp"
#include "formula_pred.hpp"
#include "formula_neg.hpp"
#include "formula_conj.hpp"
#include "operator.hpp"
#include "htn_task_head.hpp"
#include "htn_method.hpp"
#include "htn_domain.hpp"

/** \file htn_domain.hpp
 *  Declaration of the HtnDomain class.
 */

/** \file htn_domain.cpp
 *  Definition of the HtnDomain class.
 */

/** \class HtnDomain
 *  An Ordered Task Decomposition description of a planning domain.
 */

/** \var HtnDomain::m_vOperators
 *  A list of pointers to the planning operators in this domain.
 *  These need to be deallocated with the HtnDomain.
 */

/** \var HtnDomain::m_vMethods
 *  A list of pointers to the decomposition methods in this domain.
 *  These need to be deallocated with the HtnDomain.
 */

/** \var HtnDomain::m_sDomainName
 *  The name of this domain.
 */

/** \var HtnDomain::m_iRequirements
 *  The PDDL requirements of this domain.
 *  See the constants defined in funcs.hpp.
 */

/** \var HtnDomain::m_sAllowableTypes
 *  A list of types that may be used in this domain.
 *  If non-empty, all terms in the domain must be of one of these types.  
 *   Otherwise, any types (or no typing) are allowed.
 */

/** \var HtnDomain::m_ConstantTypes
 *  A list of constants in the domain with their types.
 *  If non-empty, only these constants may be used in problems for this domain.
 *   Otherwise, any may be used and they will be untyped.
 */

/** \var HtnDomain::m_vAllowablePredicates
 *  A list of allowable predicate symbols with the number and types of their
 *   arguments.
 *  If non-empty, only these predicate forms may be used in the domain.  
 *   Otherwise, any may be used.
 */

/**
 *  Retrieve a pointer to a new HtnDomain from its PDDL representation.
 *  \param p_sInput INOUT A stream containing a textual description of the
 *   domain in the PDDL language.  The stream is advanced beyond this.
 *  \return A pointer to a new HtnDomain based on the text.  The caller is
 *   responsible for deallocating it.
 */
HtnDomain * HtnDomain::FromPddl( std::stringstream & p_sInput )
{
  HtnDomain * l_pRet = new HtnDomain();
  l_pRet->m_iRequirements = PDDL_REQ_STRIPS | PDDL_REQ_HTN;

  EatWhitespace( p_sInput );
  EatString( p_sInput, "(" );
  EatWhitespace( p_sInput );
  EatString( p_sInput, "define" );
  EatWhitespace( p_sInput );
  EatString( p_sInput, "(" );
  EatWhitespace( p_sInput );
  EatString( p_sInput, "domain" );
  EatWhitespace( p_sInput );
  l_pRet->m_sDomainName = ReadString( p_sInput );
  EatWhitespace( p_sInput );
  EatString( p_sInput, ")" );
  EatWhitespace( p_sInput );

  bool l_bHasRequirements = false,
    l_bHasTypes = false,
    l_bHasConstants = false,
    l_bHasPredicates = false,
    l_bHasActions = false;

  while( p_sInput.peek() == '(' )
  {
    std::string l_sFeature = ReadParenthetical( p_sInput );
    std::stringstream l_sFeatureStream( l_sFeature );
    EatString( l_sFeatureStream, "(" );
    EatWhitespace( l_sFeatureStream );
    std::string l_sFeatureName = ReadString( l_sFeatureStream );

    if( CompareNoCase( l_sFeatureName, ":action" ) == 0 )
    {
      l_bHasActions = true;
      l_sFeatureStream.seekg( 0, std::ios::beg );
      l_pRet->m_vOperators.push_back( Operator::FromPddl( l_sFeatureStream, l_pRet->m_sAllowableTypes, l_pRet->m_vAllowablePredicates ) );
    }
    else if( CompareNoCase( l_sFeatureName, ":requirements" ) == 0 )
    {
      if( l_bHasRequirements )
	throw Exception( E_NOT_IMPLEMENTED,
			 "A PDDL domain may not have multiple requirements blocks.",
			 __FILE__,
			 __LINE__ );
      if( l_bHasTypes )
	throw Exception( E_NOT_IMPLEMENTED,
			 "The PDDL requirements block must come before the types block.",
			 __FILE__,
			 __LINE__ );
      if( l_bHasConstants )
	throw Exception( E_NOT_IMPLEMENTED,
			 "The PDDL requirements block must come before the constants block.",
			 __FILE__,
			 __LINE__ );
      if( l_bHasPredicates )
	throw Exception( E_NOT_IMPLEMENTED,
			 "The PDDL requirements block must come before the predicates block.",
			 __FILE__,
			 __LINE__ );
      if( l_bHasActions )
	throw Exception( E_NOT_IMPLEMENTED,
			 "The PDDL requirements block must come before any actions.",
			 __FILE__,
			 __LINE__ );
      l_bHasRequirements = true;
      l_pRet->m_iRequirements = ParseRequirements( l_sFeature, true );
    }
    else if( CompareNoCase( l_sFeatureName, ":types" ) == 0 )
    {
      if( l_bHasTypes )
	throw Exception( E_NOT_IMPLEMENTED,
			 "A PDDL domain may not have multiple types blocks.",
			 __FILE__,
			 __LINE__ );
      if( l_bHasConstants )
	throw Exception( E_NOT_IMPLEMENTED,
			 "The PDDL types block must come before the constants block.",
			 __FILE__,
			 __LINE__ );
      if( l_bHasPredicates )
	throw Exception( E_NOT_IMPLEMENTED,
			 "The PDDL types block must come before the predicates block.",
			 __FILE__,
			 __LINE__ );
      if( l_bHasActions )
	throw Exception( E_NOT_IMPLEMENTED,
			 "The PDDL types block must come before any actions.",
			 __FILE__,
			 __LINE__ );
      if( !( l_pRet->m_iRequirements & PDDL_REQ_TYPING ) )
	throw Exception( E_NOT_IMPLEMENTED,
			 "The PDDL types block requires the PDDL typing requirement.",
			 __FILE__,
			 __LINE__ );
      l_bHasTypes = true;
      EatWhitespace( l_sFeatureStream );
      while( l_sFeatureStream.peek() != ')' )
      {
	std::string l_sNewType = ReadString( l_sFeatureStream );
	if( l_pRet->m_sAllowableTypes.find( l_sNewType ) == l_pRet->m_sAllowableTypes.end() )
	{
	  l_pRet->m_sAllowableTypes.insert( l_sNewType );
	}
	EatWhitespace( l_sFeatureStream );
      }
    }
    else if( CompareNoCase( l_sFeatureName, ":constants" ) == 0 )
    {
      if( l_bHasConstants )
	throw Exception( E_NOT_IMPLEMENTED,
			 "A PDDL domain may not have multiple constants blocks.",
			 __FILE__,
			 __LINE__ );
      if( l_bHasPredicates )
	throw Exception( E_NOT_IMPLEMENTED,
			 "The PDDL constants block must come before the predicates block.",
			 __FILE__,
			 __LINE__ );
      if( l_bHasActions )
	throw Exception( E_NOT_IMPLEMENTED,
			 "The PDDL constants block must come before any actions.",
			 __FILE__,
			 __LINE__ );
      l_bHasConstants = true;
      EatWhitespace( l_sFeatureStream );
      while( l_sFeatureStream.peek() != ')' )
      {
	std::string l_sConstant = ReadString( l_sFeatureStream );
	EatWhitespace( l_sFeatureStream );
	std::string l_sTyping = "";
	if( l_pRet->m_iRequirements & PDDL_REQ_TYPING )
	{
	  EatString( l_sFeatureStream, "-" );
	  EatWhitespace( l_sFeatureStream );
	  l_sTyping = ReadString( l_sFeatureStream );
	  EatWhitespace( l_sFeatureStream );
	}

	if( l_pRet->m_ConstantTypes.find( l_sConstant ) == l_pRet->m_ConstantTypes.end() )
	  l_pRet->m_ConstantTypes[l_sConstant] = l_sTyping;
	else
	  throw Exception( E_NOT_IMPLEMENTED,
			   "A constant may not be declared twice.",
			   __FILE__,
			   __LINE__ );
      }
    }
    else if( CompareNoCase( l_sFeatureName, ":predicates" ) == 0 )
    {
      if( l_bHasPredicates )
	throw Exception( E_NOT_IMPLEMENTED,
			 "A PDDL domain may not have multiple predicates blocks.",
			 __FILE__,
			 __LINE__ );
      if( l_bHasActions )
	throw Exception( E_NOT_IMPLEMENTED,
			 "The PDDL predicates block must come before any actions.",
			 __FILE__,
			 __LINE__ );
      l_bHasPredicates = true;

      EatWhitespace( l_sFeatureStream );
      while( l_sFeatureStream.peek() != ')' )
      {
	TypeTable l_TempTypes;
	std::string l_sNewPred = "";
	EatString( l_sFeatureStream, "(" );
	EatWhitespace( l_sFeatureStream );
	std::string l_sRelation = ReadString( l_sFeatureStream );
	l_sNewPred += "( " + l_sRelation;
	EatWhitespace( l_sFeatureStream );
	while( l_sFeatureStream.peek() != ')' )
	{
	  std::string l_sVar = ReadString( l_sFeatureStream );
	  EatWhitespace( l_sFeatureStream );
	  l_sNewPred += " " + l_sVar;
	  if( l_pRet->m_iRequirements & PDDL_REQ_TYPING )
	  {
	    EatString( l_sFeatureStream, "-" );
	    EatWhitespace( l_sFeatureStream );
	    std::string l_sTyping = ReadString( l_sFeatureStream );
	    EatWhitespace( l_sFeatureStream );
	    l_TempTypes[l_sVar] = l_sTyping;
	    if( l_pRet->m_sAllowableTypes.size() != 0 )
	    {
	      if( l_pRet->m_sAllowableTypes.find( l_sTyping ) == l_pRet->m_sAllowableTypes.end() )
		throw Exception( E_NOT_IMPLEMENTED,
				 "An undeclared type was used in a predicate declaration.",
				 __FILE__,
				 __LINE__ );
	    }
	  }
	}
	EatString( l_sFeatureStream, ")" );
	EatWhitespace( l_sFeatureStream );
	l_sNewPred += " )";
	for( unsigned int i = 0; i < l_pRet->m_vAllowablePredicates.size(); i++ )
	{
	  if( CompareNoCase( l_pRet->m_vAllowablePredicates[i].GetRelation(),
			     l_sRelation ) == 0 )
	    throw Exception( E_NOT_IMPLEMENTED,
			     "A predicate was declared twice.",
			     __FILE__,
			     __LINE__ );
	}
	l_pRet->m_vAllowablePredicates.push_back( FormulaPred( l_sNewPred, l_TempTypes, std::vector< FormulaPred >() ) );
      }
    }
    else if( CompareNoCase( l_sFeatureName, ":functions" ) == 0 )
    {
      throw Exception( E_NOT_IMPLEMENTED,
		       "Functions are not supported.",
		       __FILE__,
		       __LINE__ );
    }
    else if( CompareNoCase( l_sFeatureName, ":constraints" ) == 0 )
    {
      throw Exception( E_NOT_IMPLEMENTED,
		       "Constraints are not supported.",
		       __FILE__,
		       __LINE__ );
    }
    else if( CompareNoCase( l_sFeatureName, ":method" ) == 0 )
    {
      l_bHasActions = true;
      l_sFeatureStream.seekg( 0, std::ios::beg );
      l_pRet->m_vMethods.push_back( HtnMethod::FromPddl( l_sFeatureStream, l_pRet->m_sAllowableTypes, l_pRet->m_vAllowablePredicates, l_pRet->m_iRequirements ) );
    }
    else
    {
      std::string l_sMessage;
      l_sMessage += "Unrecognized PDDL feature: ";
      l_sMessage += l_sFeature;
      throw Exception( E_NOT_IMPLEMENTED,
		       l_sMessage,
		       __FILE__,
		       __LINE__ );
    }
    EatWhitespace( p_sInput );
  }

  EatString( p_sInput, ")" );

  if( ( l_pRet->m_iRequirements & PDDL_REQ_TYPING ) && !l_bHasTypes )
    throw Exception( E_NOT_IMPLEMENTED,
		     "The pddl typing requirement means that you must have a pddl types block.",
		     __FILE__,
		     __LINE__ );

  return l_pRet;
}

/**
 *  Retrieve a pointer to a new HtnDomain created from its SHOP representation.
 *  \param p_sInput INOUT A stream containing a textual description of the 
 *   domain in the SHOP formalism.  The stream is advanced beyond this.
 *  \return A pointer to a new HtnDomain created from the text.  The caller is
 *   responsible for deallocating it.
 */
HtnDomain * HtnDomain::FromShop( std::stringstream & p_sInput )
{
  HtnDomain * l_pRet = new HtnDomain();

  EatWhitespace( p_sInput );
  EatString( p_sInput, "(" );
  EatWhitespace( p_sInput );
  EatString( p_sInput, "defdomain" );

  EatWhitespace( p_sInput );

  l_pRet->m_sDomainName = ReadString( p_sInput );

  EatWhitespace( p_sInput );
  EatString( p_sInput, "(" );
  EatWhitespace( p_sInput );

  while( p_sInput.peek() == '(' )
  {
    std::streampos l_CurPos = p_sInput.tellg();
    EatString( p_sInput, "(" );
    EatWhitespace( p_sInput );
    EatString( p_sInput, ":" );
    if( p_sInput.peek() == 'o' || p_sInput.peek() == 'O' )
    {
      p_sInput.seekg( l_CurPos );
      l_pRet->m_vOperators.push_back( Operator::FromShop( p_sInput ) );
    }
    else if( p_sInput.peek() == 'm' || p_sInput.peek() == 'M' )
    {
      p_sInput.seekg( l_CurPos );
      l_pRet->m_vMethods.push_back( HtnMethod::FromShop( p_sInput ) );
    }
    else
    {
      assert( false );
    }
    EatWhitespace( p_sInput );
  }
  assert( p_sInput.peek() == ')' );

  l_pRet->m_iRequirements = PDDL_REQ_STRIPS | PDDL_REQ_HTN;

  return l_pRet;
}

/**
 *  Construct a default HtnDomain.
 *  This exists only to be called from HtnDomain::FromPddl() and 
 *   HtnDomain::FromShop().
 */
HtnDomain::HtnDomain()
{
}

/**
 *  Construct an HtnDomain as a copy of an existing one.
 *  \param p_Other IN The HtnDomain to copy.
 */
HtnDomain::HtnDomain( const HtnDomain & p_Other )
  : m_sAllowableTypes( p_Other.m_sAllowableTypes ),
    m_ConstantTypes( p_Other.m_ConstantTypes ),
    m_vAllowablePredicates( p_Other.m_vAllowablePredicates )
{
  for( unsigned int i = 0; i < p_Other.m_vMethods.size(); i++ )
    m_vMethods.push_back( new HtnMethod( *p_Other.m_vMethods[i] ) );
  for( unsigned int i = 0; i < p_Other.m_vOperators.size(); i++ )
    m_vOperators.push_back( new Operator( *p_Other.m_vOperators[i] ) );
  m_sDomainName = p_Other.m_sDomainName;
  m_iRequirements = p_Other.m_iRequirements;
}

/**
 *  Destruct an HtnDomain.
 */
HtnDomain::~HtnDomain()
{
  for( unsigned int i = 0; i < m_vMethods.size(); i++ )
    delete m_vMethods[i];
  for( unsigned int i = 0; i < m_vOperators.size(); i++ )
    delete m_vOperators[i];
}

/**
 *  Add a new planning Operator to the domain.
 *  \param p_pNewOper IN A pointer to the Operator to add.  The caller 
 *   relinquishes control over this pointer.
 *  \todo This should probably check to make sure the operator is not already
 *   in the list.
 */
void HtnDomain::AddOperator( Operator * p_pNewOper )
{
  m_vOperators.push_back( p_pNewOper );
}

/**
 *  Add a new decomposition HtnMethod to the domain.
 *  \param p_pNewMethod IN A pointer to the HtnMethod to add.  The caller 
 *   relinquishes control over this pointer.
 *  \todo This should probably check to make sure the method is not already
 *   in the list.
 */
void HtnDomain::AddMethod( HtnMethod * p_pNewMethod )
{
  m_vMethods.push_back( p_pNewMethod );
}

/**
 *  Remove an operator from the domain.
 *  This will potentially invalidate any stored indexes.
 *  \param p_iIndex IN The 0-based index of the operator to remove.
 */
void HtnDomain::RemoveOperator( unsigned int p_iIndex )
{
  if( p_iIndex < m_vOperators.size() )
  {
    std::vector<Operator *>::iterator l_Iter = m_vOperators.begin();
    for( unsigned int i = 0; i < p_iIndex; i++ )
      l_Iter++;
    delete *l_Iter;
    m_vOperators.erase( l_Iter );
  }
  else
    throw Exception( E_INDEX_OUT_OF_BOUNDS,
		     "Bounds error.",
		     __FILE__,
		     __LINE__ );
}

/**
 *  Remove a method from the domain.
 *  This will potentially invalidate any stored indexes.
 *  \param p_iIndex IN The 0-based index of the method to remove.
 */
void HtnDomain::RemoveMethod( unsigned int p_iIndex )
{
  if( p_iIndex < m_vMethods.size() )
  {
    std::vector<HtnMethod *>::iterator l_Iter = m_vMethods.begin();
    for( unsigned int i = 0; i < p_iIndex; i++ )
      l_Iter++;
    delete *l_Iter;
    m_vMethods.erase( l_Iter );
  }
  else
    throw Exception( E_INDEX_OUT_OF_BOUNDS,
		     "Bounds error.",
		     __FILE__,
		     __LINE__ );
}

/**
 *  Replace a method in the domain with a new one.
 *  \param p_iIndex IN The index of the method that will be replaced.
 *  \param p_pNewMethod IN A pointer to the method to be inserted.  The caller
 *   relinquishes control of this pointer.
 */
void HtnDomain::ReplaceMethod( unsigned int p_iIndex, HtnMethod * p_pNewMethod )
{
  delete m_vMethods[p_iIndex];
  m_vMethods[p_iIndex] = p_pNewMethod;
}

/**
 *  Retrieve the number of operators in the domain.
 */
unsigned int HtnDomain::GetNumOperators() const
{
  return m_vOperators.size();
}

/**
 *  Retrieve the number of methods in the domain.
 */
unsigned int HtnDomain::GetNumMethods() const
{
  return m_vMethods.size();
}

/**
 *  Retrieve a pointer to one of the operators in the domain.
 *  \param p_iIndex IN The 0-based index of the requested operator.
 *  \return A pointer to the requested operator.  This pointer has the same
 *   lifetime as this HtnDomain.
 */
const Operator * HtnDomain::GetCOperator( unsigned int p_iIndex ) const
{
  if( p_iIndex < m_vOperators.size() )
    return m_vOperators[ p_iIndex ];
  else
    throw Exception( E_INDEX_OUT_OF_BOUNDS,
		     "Bounds error.",
		     __FILE__,
		     __LINE__ );
}

/**
 *  Retrieve a pointer to one of the methods in this domain.
 *  \param p_iIndex IN The 0-based index of the requested method.
 *  \return A pointer to the requested method.  This pointer has the same
 *   lifetime as this HtnDomain.
 */
const HtnMethod * HtnDomain::GetCMethod( unsigned int p_iIndex ) const
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
 *  Retrieve the name of the domain.
 */
std::string HtnDomain::GetDomainName() const
{
  return m_sDomainName;
}

/**
 *  Retrieve a string containing the SHOP representation of this domain.
 *  \return A string containing the SHOP representation of this domain.
 *  \todo Change the name of this function to ToShop.
 */
std::string HtnDomain::ToStr() const
{
  std::string l_sRet;

  l_sRet += "( defdomain ";
  l_sRet += m_sDomainName;
  l_sRet += "\n  (\n";

  for( unsigned int i = 0; i < m_vOperators.size(); i++ )
  {
    l_sRet += m_vOperators[i]->ToStr( true, 4 );
  }

  for( unsigned int i = 0; i < m_vMethods.size(); i++ )
  {
    l_sRet += m_vMethods[i]->ToStr();
  }

  l_sRet += "  )\n)\n";


  return l_sRet;
}

/**
 *  Retrieve a string containing the PDDL representation of this domain.
 *  \return A string containing the PDDL representation of this domain.
 */
std::string HtnDomain::ToPddl() const
{
  std::string l_sRet;

  l_sRet += "( define ( domain " + m_sDomainName + " )\n";
  l_sRet += "  " + PrintRequirements( m_iRequirements ) + "\n";

  if( m_iRequirements & PDDL_REQ_TYPING )
  {
    l_sRet += "  ( :types ";
    for( std::set< std::string, StrLessNoCase >::const_iterator l_SetIter = m_sAllowableTypes.begin(); l_SetIter != m_sAllowableTypes.end(); l_SetIter++ )
      l_sRet += *l_SetIter + " ";
    l_sRet += ")\n";
  }

  if( m_ConstantTypes.size() > 0 )
  {
    l_sRet += "  ( :constants\n";
    for( TypeTable::const_iterator l_Iter = m_ConstantTypes.begin();
	 l_Iter != m_ConstantTypes.end(); l_Iter++ )
      l_sRet += "    " + (*l_Iter).first + " - " + (*l_Iter).second + "\n";
    l_sRet += "  )\n";
  }

  if( m_vAllowablePredicates.size() > 0 )
  {
    l_sRet += "  ( :predicates\n";
    for( unsigned int i = 0; i < m_vAllowablePredicates.size(); i++ )
    {
      l_sRet += "    " + m_vAllowablePredicates[i].ToStr() + "\n";
    }
    l_sRet += "  )\n";
  }

  for( unsigned int i = 0; i < m_vOperators.size(); i++ )
  {
    l_sRet += m_vOperators[i]->ToStr( false, 2 ) + "\n";
  }
  for( unsigned int i = 0; i < m_vMethods.size(); i++ )
  {
    l_sRet += m_vMethods[i]->ToPddl( m_iRequirements ) + "\n";
  }
  l_sRet += ")\n";

  return l_sRet;
}

/**
 *  Retrieve a string containing the SHOP representation of this domain.
 *  \return A string containing the SHOP representation of this domain.
 *  \todo This is redundant.
 */
std::string HtnDomain::ToShop() const
{
  return ToStr();
}

/**
 *  Determine whether or not another domain is equivalent to this one.
 *  \param p_pOther A pointer to the other domain.
 *  \return Whether or not this and the other domain are equivalent.
 *  \todo This should also check the constants and allowable predicates and
 *   whatnot.
 */
bool HtnDomain::Equivalent( const HtnDomain * p_pOther ) const
{
  if( m_iRequirements != p_pOther->m_iRequirements )
    return false;
  if( m_vOperators.size() != p_pOther->m_vOperators.size() )
    return false;
  if( m_vMethods.size() != p_pOther->m_vMethods.size() )
    return false;
  if( m_sDomainName != p_pOther->m_sDomainName )
    return false;

  for( unsigned int i = 0; i < m_vOperators.size(); i++ )
  {
    bool l_bFound = false;
    for( unsigned int j = 0; j < p_pOther->m_vOperators.size() && !l_bFound; j++ )
    {
      if( m_vOperators[i]->Equivalent( p_pOther->m_vOperators[j] ) )
	l_bFound = true;
    }
    if( !l_bFound )
      return false;
  }

  for( unsigned int i = 0; i < m_vMethods.size(); i++ )
  {
    bool l_bFound = false;
    for( unsigned int j = 0; j < p_pOther->m_vMethods.size() && !l_bFound; j++ )
    {
      if( m_vMethods[i]->Subsumes( p_pOther->m_vMethods[j] ) &&
	  p_pOther->m_vMethods[j]->Subsumes( m_vMethods[i] ) )
	l_bFound = true;
    }
    if( !l_bFound )
      return false;
  }

  return true;
}

/**
 *  Retrieve a list of types that may be used in this domain.
 *  If the list is empty, any may be used.
 *  \return A list of types that may be used in this domain.
 */
const std::set< std::string, StrLessNoCase > & HtnDomain::GetAllowableTypes() const
{
  return m_sAllowableTypes;
}

/**
 *  Retrieve a list of constants that may be used in this domain.
 *  If the list is empty, any may be used.
 *  \return A list of constants that may be used in this domain.
 */
const TypeTable & HtnDomain::GetConstantTypes() const
{
  return m_ConstantTypes;
}

/**
 *  Retrieve a list of predicate symbols with the number and types of their
 *   parameters that may be used in this domain.
 *  If the list is empty, any may be used.
 *  \return A list of predicate symbols with the number and types of their
 *   parameters that may be used in this domain.
 */
const std::vector< FormulaPred > & HtnDomain::GetAllowablePredicates() const
{
  return m_vAllowablePredicates;
}

/**
 *  Retrieve the requirements of this domain.
 */
long HtnDomain::GetRequirements() const
{
  return m_iRequirements;
}

/**
 *  Update the Q-value of method at index by averaging in new cost.
 *  \param p_iMethodIndex IN The index of the method to be adjusted.
 *  \param p_fNewCost IN The cost of the method in this instance.
 */
void HtnDomain::UpdateMethodQValue( unsigned int p_iMethodIndex,
				    double p_fNewCost )
{
  m_vMethods[p_iMethodIndex]->UpdateQValue( p_fNewCost );
}

size_t HtnDomain::GetMemSizeMin() const
{
  size_t l_iSize = sizeof( HtnDomain ) + m_vOperators.capacity() * sizeof( Operator * ) + m_vMethods.capacity() * sizeof( HtnMethod * ) + m_sDomainName.capacity() + m_sAllowableTypes.size() * sizeof( std::string ) + m_ConstantTypes.size() * sizeof( std::string ) * 2 + m_vAllowablePredicates.capacity() * sizeof( FormulaPred );
  for( unsigned int i = 0; i < m_vOperators.size(); i++ )
    l_iSize += m_vOperators[i]->GetMemSizeMin();
  for( unsigned int i = 0; i < m_vMethods.size(); i++ )
    l_iSize += m_vMethods[i]->GetMemSizeMin();
  for( std::set< std::string, StrLessNoCase >::const_iterator i = m_sAllowableTypes.begin(); i != m_sAllowableTypes.end(); i++ )
    l_iSize += (*i).capacity();
  for( TypeTable::const_iterator i = m_ConstantTypes.begin(); i != m_ConstantTypes.end(); i++ )
    l_iSize += (*i).first.capacity() + (*i).second.capacity();
  for( unsigned int i = 0; i < m_vAllowablePredicates.size(); i++ )
    l_iSize += m_vAllowablePredicates[i].GetMemSizeMin();
  return l_iSize;
}

size_t HtnDomain::GetMemSizeMax() const
{
  size_t l_iSize = sizeof( HtnDomain ) + m_vOperators.capacity() * sizeof( Operator * ) + m_vMethods.capacity() * sizeof( HtnMethod * ) + m_sDomainName.capacity() + m_sAllowableTypes.size() * sizeof( std::string ) + m_ConstantTypes.size() * sizeof( std::string ) * 2 + m_vAllowablePredicates.capacity() * sizeof( FormulaPred );
  for( unsigned int i = 0; i < m_vOperators.size(); i++ )
    l_iSize += m_vOperators[i]->GetMemSizeMax();
  for( unsigned int i = 0; i < m_vMethods.size(); i++ )
    l_iSize += m_vMethods[i]->GetMemSizeMax();
  for( std::set< std::string, StrLessNoCase >::const_iterator i = m_sAllowableTypes.begin(); i != m_sAllowableTypes.end(); i++ )
    l_iSize += (*i).capacity();
  for( TypeTable::const_iterator i = m_ConstantTypes.begin(); i != m_ConstantTypes.end(); i++ )
    l_iSize += (*i).first.capacity() + (*i).second.capacity();
  for( unsigned int i = 0; i < m_vAllowablePredicates.size(); i++ )
    l_iSize += m_vAllowablePredicates[i].GetMemSizeMax();
  return l_iSize;
}

struct HtnMethodStarLess
{
  bool operator() ( HtnMethod * p_pFirst, HtnMethod * p_pSecond )
  {
    return p_pFirst->GetQValue() < p_pSecond->GetQValue();
  }
};

/**
 *  Re-order the list of methods from lowest Q-value to highest.
 *  This should be a stable sort.
 */
void HtnDomain::SortMethods()
{
  static HtnMethodStarLess sorter;
  std::stable_sort( m_vMethods.begin(), m_vMethods.end(), sorter );
}

void HtnDomain::RandomizeMethodOrder()
{
  std::random_shuffle( m_vMethods.begin(), m_vMethods.end() );
}

void HtnDomain::SetMethodId( unsigned int p_iIndex,
			     std::string p_sNewId )
{
  if( p_iIndex >= m_vMethods.size() )
    throw Exception( E_INDEX_OUT_OF_BOUNDS,
		     "Method index out of bounds.\n",
		     __FILE__,
		     __LINE__ );
  if( m_vMethods[p_iIndex]->GetId() != "" )
    throw Exception( E_NOT_IMPLEMENTED,
		     "Attempt to overwrite a method ID.\n",
		     __FILE__,
		     __LINE__ );
  m_vMethods[p_iIndex]->SetId( p_sNewId );
}

void HtnDomain::AddRequirement( long p_iAdditionalRequirement )
{
  m_iRequirements |= p_iAdditionalRequirement;
}
