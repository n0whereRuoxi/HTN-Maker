#include <vector>
#include <string>
#include <cassert>
#include <sstream>
#include <algorithm>
#include <set>
#include <tr1/memory>

#include "exception.hpp"
#include "funcs.hpp"
#include "term.hpp"
#include "term_string.hpp"
#include "term_constant.hpp"
#include "term_variable.hpp"
#include "type_table.hpp"
#include "substitution.hpp"
#include "formula.hpp"
#include "formula_pred.hpp"
#include "formula_neg.hpp"
#include "formula_conj.hpp"
#include "operator.hpp"
#include "strips_domain.hpp"

/** \file strips_domain.hpp
 *  Declaration of the StripsDomain class.
 */

/** \file strips_domain.cpp
 *  Definition of the StripsDomain class.
 */

/** \class StripsDomain
 *  A classical (STRIPS) planning domain.
 *  A few of the common extensions to classical planning are supported, as well
 *   as a few of my own.
 */

/** \var StripsDomain::m_sName
 *  The name of this domain.
 */

/** \var StripsDomain::m_vOps
 *  A list of pointers to the Operators in this domain.
 *  These will need to be deallocated with the domain.
 */

/** \var StripsDomain::m_iRequirements
 *  A bitflag listing the PDDL requirements of this domain.
 *  See ParseRequirements() and PrintRequirements(), as well as the PDDL_REQ_ 
 *   macros.
 */

/** \var StripsDomain::m_sAllowableTypes
 *  A list of types that may be used within this domain, if m_iRequirements
 *   includes PDDL_REQ_TYPING.
 */

/** \var StripsDomain::m_ConstantTypes
 *  A table of the constants that may be used in this domain with their 
 *   associated types.
 *  If m_iRequirements does not contain PDDL_REQ_TYPING, the types will be 
 *   blank.
 *  If this table is empty, and constants may be used.
 */

/** \var StripsDomain::m_vAllowablePredicates
 *  A list of predicate symbols with the number of types of their arguments 
 *   that may be used in this domain.
 *  If empty, any predicate may be used.
 */

/**
 *  Construct a StripsDomain from a string containing its textual 
 *   representation in PDDL.
 *  \param p_sDomain IN A string containing a textual representation of the
 *   domain.
 */
StripsDomain::StripsDomain( std::string p_sDomain )
{
  m_iRequirements = PDDL_REQ_STRIPS;
  std::stringstream l_sStream( p_sDomain );
  EatWhitespace( l_sStream );
  EatString( l_sStream, "(" );
  EatWhitespace( l_sStream );
  EatString( l_sStream, "define" );

  EatWhitespace( l_sStream );
  EatString( l_sStream, "(" );
  EatWhitespace( l_sStream );
  EatString( l_sStream, "domain" );
  EatWhitespace( l_sStream );

  m_sName = ReadString( l_sStream );

  EatWhitespace( l_sStream );
  EatString( l_sStream, ")" );
  EatWhitespace( l_sStream );

  bool l_bHasRequirements = false,
    l_bHasTypes = false,
    l_bHasConstants = false,
    l_bHasPredicates = false,
    l_bHasActions = false;

  while( l_sStream.peek() == '(' )
  {
    std::string l_sFeature = ReadParenthetical( l_sStream );

    std::stringstream l_sFeatureStream( l_sFeature );
    EatString( l_sFeatureStream, "(" );
    EatWhitespace( l_sFeatureStream );
    std::string l_sFeatureName = ReadString( l_sFeatureStream );

    if( CompareNoCase( l_sFeatureName, ":action" ) == 0 )
    {
      l_bHasActions = true;
      l_sFeatureStream.seekg( 0, std::ios::beg );
      m_vOps.push_back( Operator::FromPddl( l_sFeatureStream, m_sAllowableTypes, m_vAllowablePredicates ) );
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
      m_iRequirements = ParseRequirements( l_sFeature, false );
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
      if( !( m_iRequirements & PDDL_REQ_TYPING ) )
	throw Exception( E_NOT_IMPLEMENTED,
			 "The PDDL types block requires the pddl types requirement.",
			 __FILE__,
			 __LINE__ );
      l_bHasTypes = true;
      EatWhitespace( l_sFeatureStream );
      while( l_sFeatureStream.peek() != ')' )
      {
	std::string l_sNewType = ReadString( l_sFeatureStream );
	if( m_sAllowableTypes.find( l_sNewType ) == m_sAllowableTypes.end() )
	{
	  m_sAllowableTypes.insert( l_sNewType );
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
	if( m_iRequirements & PDDL_REQ_TYPING )
	{
	  EatString( l_sFeatureStream, "-" );
	  EatWhitespace( l_sFeatureStream );
	  l_sTyping = ReadString( l_sFeatureStream );
	  EatWhitespace( l_sFeatureStream );
	}

	if( m_ConstantTypes.find( l_sConstant ) == m_ConstantTypes.end() )
	  m_ConstantTypes[l_sConstant] = l_sTyping;
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
	  if( m_iRequirements & PDDL_REQ_TYPING )
	  {
	    EatString( l_sFeatureStream, "-" );
	    EatWhitespace( l_sFeatureStream );
	    std::string l_sTyping = ReadString( l_sFeatureStream );
	    EatWhitespace( l_sFeatureStream );
	    l_TempTypes[l_sVar] = l_sTyping;
	    if( m_sAllowableTypes.size() != 0 )
	    {
	      if( m_sAllowableTypes.find( l_sTyping ) == m_sAllowableTypes.end() )
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
	for( unsigned int i = 0; i < m_vAllowablePredicates.size(); i++ )
	{
	  if( CompareNoCase( m_vAllowablePredicates[i].GetRelation(),
			     l_sRelation ) == 0 )
	    throw Exception( E_NOT_IMPLEMENTED,
			     "A predicate was declared twice.",
			     __FILE__,
			     __LINE__ );
	}
	m_vAllowablePredicates.push_back( FormulaPred( l_sNewPred, l_TempTypes, std::vector< FormulaPred >() ) );
      }
    }
    else if( CompareNoCase( l_sFeatureName, ":functions" ) == 0 )
    {
      throw new Exception( E_NOT_IMPLEMENTED,
			   "Functions are not supported.",
			   __FILE__,
			   __LINE__ );
    }
    else if( CompareNoCase( l_sFeatureName, ":constraints" ) == 0 )
    {
      throw new Exception( E_NOT_IMPLEMENTED,
			   "Constraints are not supported.",
			   __FILE__,
			   __LINE__ );
    }
    else if( CompareNoCase( l_sFeatureName, ":method" ) == 0 )
    {
      throw new Exception( E_NOT_IMPLEMENTED,
			   "A STRIPS domain may not contain methods.",
			   __FILE__,
			   __LINE__ );
    }
    else
    {
      std::string l_sMessage;
      l_sMessage += "Unrecognized PDDL feature: ";
      l_sMessage += l_sFeature;
      throw new Exception( E_NOT_IMPLEMENTED,
			   l_sMessage,
			   __FILE__,
			   __LINE__ );
    }

    EatWhitespace( l_sStream );
  }
  EatString( l_sStream, ")" );

  if( !l_bHasRequirements )
    m_iRequirements = PDDL_REQ_STRIPS;

  if( ( m_iRequirements & PDDL_REQ_TYPING ) && !l_bHasTypes )
    throw Exception( E_NOT_IMPLEMENTED,
		     "The pddl typing requirement means that you must have a pddl types block.",
		     __FILE__,
		     __LINE__ );
}

/**
 *  Construct a StripsDomain as a copy of an existing one.
 *  \param p_Other IN The StripsDomain to copy.
 */
StripsDomain::StripsDomain( const StripsDomain & p_Other )
  : m_sAllowableTypes( p_Other.m_sAllowableTypes )
{
  for( unsigned int i = 0; i < p_Other.GetNumOpers(); i++ )
  {
    m_vOps.push_back( new Operator( *p_Other.GetCOper( i ) ) );
  }
  m_sName = p_Other.GetName();
  m_iRequirements = p_Other.m_iRequirements;
}

/**
 *  Destruct a StripsDomain.
 */
StripsDomain::~StripsDomain()
{
  for( unsigned int i = 0; i < m_vOps.size(); i++ )
    delete m_vOps[i];
}

/**
 *  Retrieve the name of this domain.
 *  \return The name of this domain.
 */
std::string StripsDomain::GetName() const
{
  return m_sName;
}

/**
 *  Retrieve the number of Operators in this domain.
 *  \return The number of Operators in this domain.
 */
unsigned int StripsDomain::GetNumOpers() const
{
  return m_vOps.size();
}

/**
 *  Retrieve a pointer to one of the Operators in this domain.
 *  \param p_iIndex IN The 0-based index of the requested Operator.
 *  \return A pointer to the requested Operator.
 */
const Operator * StripsDomain::GetCOper( unsigned int p_iIndex ) const
{
  if( p_iIndex >= m_vOps.size() )
    throw Exception( E_INDEX_OUT_OF_BOUNDS,
		     "Bounds error.",
		     __FILE__,
		     __LINE__ );
  return m_vOps[ p_iIndex ];
}

/**
 *  Retrieve the 0-based index of an Operator in the domain from its name.
 *  \param p_sOperName IN A string containing the name of the requested
 *   Operator.
 *  \return The 0-based index of the Operator in the domain.
 */
unsigned int StripsDomain::GetOperIndexByName( std::string p_sOperName ) const
{
  for( unsigned int i = 0; i < m_vOps.size(); i++ )
  {
    if( CompareNoCase( m_vOps[ i ]->GetName(), p_sOperName ) == 0 )
      return i;
  }
  throw Exception( E_INDEX_OUT_OF_BOUNDS,
		   "Operator not found in list.",
		   __FILE__,
		   __LINE__ );
}

/**
 *  Retrieve a string containing the PDDL representation of this domain.
 *  \return A string containing the PDDL representation of this domain.
 */
std::string StripsDomain::ToPddl() const
{
  std::string l_sRet = "";

  l_sRet += "( define ( domain " + m_sName + " )\n";
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
    l_sRet += "  ( :constants ";
    for( TypeTable::const_iterator l_Iter = m_ConstantTypes.begin();
	 l_Iter != m_ConstantTypes.end(); l_Iter++ )
      l_sRet += "    " + (*l_Iter).first + " - " + (*l_Iter).second + "\n";
    l_sRet += "  )\n";
  }

  if( m_vAllowablePredicates.size() > 0 )
  {
    l_sRet += "  ( :predicates ";
    for( unsigned int i = 0; i < m_vAllowablePredicates.size(); i++ )
    {
      l_sRet += "    " + m_vAllowablePredicates[i].ToStr() + "\n";
    }
    l_sRet += "  )\n";
  }

  for( unsigned int i = 0; i < m_vOps.size(); i++ )
  {
    l_sRet += m_vOps[i]->ToStr( false, 2 ) + "\n";
  }
  l_sRet += ")\n";

  return l_sRet;
}

/**
 *  Retrieve a (possibly empty) table of usable constants in the domain with
 *   their types.
 *  \return A table of usable constants in the domain with their types.
 */
const TypeTable & StripsDomain::GetConstantTypes() const
{
  return m_ConstantTypes;
}

/**
 *  Retrieve a list of the predicate symbols with number and types of
 *   parameters that may be used in the domain.
 *  \return A list of the predicate symbols with number and types of parameters
 *   that may be used in the domain.
 */
const std::vector< FormulaPred > & StripsDomain::GetAllowablePredicates() const
{
  return m_vAllowablePredicates;
}

/**
 *  Retrieve the requirements of this domain.
 *  \return The requirements of this domain.
 */
long StripsDomain::GetRequirements() const
{
  return m_iRequirements;
}

size_t StripsDomain::GetMemSizeMin() const
{
  size_t l_iSize = sizeof( StripsDomain ) + m_vOps.capacity() * sizeof( Operator * ) + m_sAllowableTypes.size() * sizeof( std::string ) + (m_vAllowablePredicates.capacity() - m_vAllowablePredicates.size()) * sizeof( FormulaPred );
  for( std::set< std::string, StrLessNoCase >::const_iterator i = m_sAllowableTypes.begin(); i != m_sAllowableTypes.end(); i++ )
    l_iSize += (*i).capacity();
  for( unsigned int i = 0; i < m_vOps.size(); i++ )
    l_iSize += m_vOps[i]->GetMemSizeMin();
  for( unsigned int i = 0; i < m_vAllowablePredicates.size(); i++ )
    l_iSize += m_vAllowablePredicates[i].GetMemSizeMin();
  return l_iSize;
}

size_t StripsDomain::GetMemSizeMax() const
{
  size_t l_iSize = sizeof( StripsDomain ) + m_vOps.capacity() * sizeof( Operator * ) + m_sAllowableTypes.size() * sizeof( std::string ) + (m_vAllowablePredicates.capacity() - m_vAllowablePredicates.size()) * sizeof( FormulaPred );
  for( std::set< std::string, StrLessNoCase >::const_iterator i = m_sAllowableTypes.begin(); i != m_sAllowableTypes.end(); i++ )
    l_iSize += (*i).capacity();
  for( unsigned int i = 0; i < m_vOps.size(); i++ )
    l_iSize += m_vOps[i]->GetMemSizeMax();
  for( unsigned int i = 0; i < m_vAllowablePredicates.size(); i++ )
    l_iSize += m_vAllowablePredicates[i].GetMemSizeMax();
  return l_iSize;
}
