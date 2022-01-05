#include <string>
#include <vector>
#include <sstream>
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
#include "htn_task_head.hpp"
#include "htn_task_descr.hpp"
#include "operator.hpp"
#include "htn_method.hpp"
#include "htn_domain.hpp"
#include "htn_task_list.hpp"

/** \file htn_task_list.hpp
 *  Declaration of HtnTaskList class.
 */

/** \file htn_task_list.cpp
 *  Definition of HtnTaskList class.
 */

/** \class HtnTaskList
 *  A collection of smart pointers to HtnTaskDescr objects.
 */

/** \var HtnTaskList::m_sName
 *  The name of this task list.
 */

/** \var HtnTaskList::m_pDomain
 *  The HtnDomain for which these tasks will be used.
 *  From this we can get typing and other information.
 */

/**
 *  Construct an HtnTaskList from an associated domain and textual description.
 *  \param p_pDomain IN A pointer to the domain associated with these tasks.
 *  \param p_sString IN A textual representation of the list of tasks.
 */
HtnTaskList::HtnTaskList( const std::tr1::shared_ptr< HtnDomain > & p_pDomain,
			  std::string p_sString )
  : m_pDomain( p_pDomain )
{
  std::stringstream l_sStream( p_sString );

  EatWhitespace( l_sStream );
  EatString( l_sStream, "(" );
  EatWhitespace( l_sStream );
  EatString( l_sStream, "define" );

  EatWhitespace( l_sStream );
  EatString( l_sStream, "(" );
  EatWhitespace( l_sStream );
  EatString( l_sStream, "tasks" );
  EatWhitespace( l_sStream );

  while( l_sStream.peek() != ' ' && l_sStream.peek() != '\t' &&
	 l_sStream.peek() != '\n' && l_sStream.peek() != '\r' &&
	 l_sStream.peek() != ')' && l_sStream.peek() != '(' )
    m_sName += l_sStream.get();

  EatWhitespace( l_sStream );
  EatString( l_sStream, ")" );
  EatWhitespace( l_sStream );

  while( l_sStream.peek() == '(' )
  {
    push_back( std::tr1::shared_ptr< HtnTaskDescr >( new HtnTaskDescr( l_sStream, m_pDomain->GetAllowableTypes(), m_pDomain->GetAllowablePredicates() ) ) );
    EatWhitespace( l_sStream );
  }
  EatString( l_sStream, ")" );
}

/**
 *  Construct an HtnTaskList as a copy of an existing one.
 *  I cannot use the base class copy constructor because it would be a 
 *   shallow copy of the pointers to tasks.
 *  \param p_Other IN The existing HtnTaskList to copy.
 */
HtnTaskList::HtnTaskList( const HtnTaskList & p_Other )
  : std::vector< std::tr1::shared_ptr< HtnTaskDescr > >( p_Other ),
    m_pDomain( p_Other.m_pDomain )
{
  m_sName = p_Other.m_sName;
  m_pDomain = p_Other.m_pDomain;
}

/**
 *  Destruct an HtnTaskList.
 */
HtnTaskList::~HtnTaskList()
{
}

size_t HtnTaskList::GetMemSizeMin() const
{
  size_t l_iSize = sizeof( HtnTaskList ) + m_sName.capacity() + capacity() * sizeof( std::tr1::shared_ptr< HtnTaskDescr > );
  return l_iSize;
}

size_t HtnTaskList::GetMemSizeMax() const
{
  size_t l_iSize = GetMemSizeMin();
  for( unsigned int i = 0; i < size(); i++ )
    l_iSize += at( i )->GetMemSizeMax();
  return l_iSize;
}

