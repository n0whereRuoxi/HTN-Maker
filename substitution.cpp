
#include <vector>
#include <cassert>
#include <string>
#include <iostream>
#include <tr1/memory>
#include <utility>

#include "exception.hpp"
#include "term.hpp"
#include "term_string.hpp"
#include "term_constant.hpp"
#include "term_variable.hpp"
#include "substitution.hpp"

/** \file substitution.hpp
 *  Declaration of Substitution class.
 */

/** \file substitution.cpp
 *  Definition of Substitution class.
 */

/** \class Substitution
 *  A substitution from TermVariables into Terms in first-order logic.
 */

/** \var Substitution::m_mPairs
 *  A mapping of TermVariable / Term pairs to be substituted.
 *  These are stored as reference-counted pointers because there tends to be a
 *   lot of copying Substitutions and this saves space.
 */

/**
 *  Construct a new, empty Substitution.
 */
Substitution::Substitution()
{
}

/**
 *  Construct a new Substitution as a copy of an existing one.
 *  \param p_sOther IN The Substitution to copy.
 */
Substitution::Substitution( const Substitution & p_sOther )
  : m_mPairs( p_sOther.m_mPairs )
{
}

/**
 *  Empty destructor, because reference-counted pointers take care of members.
 */
Substitution::~Substitution()
{
}

/**
 *  Retrieve the number of PairVarTerms in this Substitution.
 *  \return The number of PairVarTerms in this Substitution.
 */
unsigned int Substitution::GetNumPairs() const
{
  return m_mPairs.size();
}

/**
 *  Add a copy of a new PairVarTerm to this Substitution by its components.
 *  Throws E_SUBS_VAR_DOUBLE if there is already a PairVarTerm with this 
 *   TermVariable.
 *  Does nothing if the TermVariable and Term are equal.
 *  \param p_pVar IN A pointer to the TermVariable part of the PairVarterm to 
 *   add.
 *  \param p_pTerm IN A pointer to the Term part of the PairVarTerm to add.
 */
void Substitution::AddPair( const TermVariableP & p_pVar, 
			    const TermP & p_pTerm )
{
  SubMap::const_iterator l_Iter = m_mPairs.find( p_pVar );
  if( l_Iter != m_mPairs.end() )
  {
    if( l_Iter->second != p_pTerm )
    {
      throw Exception( E_SUBS_VAR_DOUBLE,
		       "Attempt to add " + p_pVar->ToStr() + " -> " + p_pTerm->ToStr() + " to a substitution conflicts with existing " + l_Iter->first->ToStr() + " -> " + l_Iter->second->ToStr() + ".",
		       __FILE__,
		       __LINE__ );
    }
  }
  else if( p_pVar != p_pTerm )
    m_mPairs.insert( std::make_pair( p_pVar, p_pTerm ) );
}

/**
 *  Retrieve the index of the PairVarTerm with a certain TermVariable.
 *  If there is no such PairVarTerm, the number of PairVarTermss is returned.
 *  Note that there may be at most one PairVarTerm with a given TermVariable.
 *  \param p_pVar IN A pointer to the variable for which to search.
 *  \return An iterator to the PairVarTerm with the TermVariable, or end() if 
 *   it is not found.
 */
SubMap::const_iterator Substitution::FindIndexByVar( const TermVariableP & p_pVar ) const
{
  return m_mPairs.find( p_pVar );
}

/**
 *  Retrieve the index of the first PairVarTerm with a certain Term.
 *  If there is no such PairVarTerm, the number of PairVarTerms is returned.
 *  Note that this finds only the first of potentially many.
 *  \param p_pTerm IN A pointer to the Term for which to search.
 *  \return An iterator to the first PairVarTerm with the Term, or end() if it
 *   is not found.
 */
SubMap::const_iterator Substitution::FindIndexByTerm( const TermP & p_pTerm ) const
{
  for( SubMap::const_iterator i = m_mPairs.begin(); i != m_mPairs.end(); i++ )
  {
    if( i->second == p_pTerm )
      return i;
  }
  return m_mPairs.end();
}

/**
 *  Replace instances of a specific Term in this Substitution with another, but
 *   only in the Term part of each PairVarTerm.
 *  This may remove redundant or irrelevant PairVarTerms, resuling in stored
 *   indices no longer being valid.
 *  \param p_pOld IN A pointer to the Term to replace.  The caller retains
 *   control and responsibility for deallocating it.
 *  \param p_pNew IN A pointer to the replacement Term.  The caller retains
 *   control and responsibility for deallocating it.
 */
void Substitution::ReplaceTermTermPart( const TermP & p_pOld,
					const TermP & p_pNew )
{
  if( p_pOld == p_pNew )
    return;
  for( SubMap::iterator l_iTermIndex = m_mPairs.begin();
       l_iTermIndex != m_mPairs.end();
       l_iTermIndex++ )
  {
    if( l_iTermIndex->second == p_pOld )
    {
      if( l_iTermIndex->first == p_pNew )
      {
	SubMap::iterator l_iRemove = l_iTermIndex;
	l_iTermIndex--;
	m_mPairs.erase( l_iRemove );
      }
      else
	l_iTermIndex->second = p_pNew;
    }
  }
}

/**
 *  Replace instances of a specific TermVariable in this Substitution with 
 *   another, but only in the TermVariable part of each PairVarTerm.
 *  This may remove redundant or irrelevant PairVarTerms, resuling in stored
 *   indices no longer being valid.
 *  \param p_pOld IN A pointer to the Term to replace.  The caller retains
 *   control and responsibility for deallocating it.
 *  \param p_pNew IN A pointer to the replacement Term.  The caller retains
 *   control and responsibility for deallocating it.
 */
void Substitution::ReplaceTermVarPart( const TermVariableP & p_pOld,
				       const TermVariableP & p_pNew )
{
  if( p_pOld == p_pNew )
    return;

  SubMap::iterator l_iOldIndex = m_mPairs.find( p_pOld );
  SubMap::iterator l_iNewIndex = m_mPairs.find( p_pNew );

  if( l_iOldIndex != m_mPairs.end() )
  {
    if( l_iNewIndex != m_mPairs.end() )
    {
      if( l_iOldIndex->second == l_iNewIndex->second )
	m_mPairs.erase( l_iOldIndex );
      else
	throw Exception( E_SUBS_VAR_DOUBLE,
			 "Attempt to substitute " + p_pNew->ToStr() + " for " + p_pOld->ToStr() + " in a substitution would create " + l_iNewIndex->first->ToStr() + " -> " + l_iNewIndex->second->ToStr() + " and " + l_iNewIndex->first->ToStr() + " -> " + l_iOldIndex->second->ToStr() + ".",
			 __FILE__,
			 __LINE__ );
    }
    else if( l_iOldIndex->second == p_pNew )
      m_mPairs.erase( l_iOldIndex );
    else
    {
      TermP l_Term = l_iOldIndex->second;
      m_mPairs.erase( l_iOldIndex );
      m_mPairs.insert( std::make_pair( p_pNew, l_Term ) );
    }
  }
}

/**
 *  Replace instances of a specific Term in this Substitution with another.
 *  This may remove redundant or irrelevant PairVarTerms, resuling in stored
 *   indices no longer being valid.
 *  \param p_pOld IN A pointer to the Term to replace.  The caller retains
 *   control and responsibility for deallocating it.  Afterwards, this should
 *   no longer appear anywhere in the Substitution.
 *  \param p_pNew IN A pointer to the replacement Term.  The caller retains
 *   control and responsibility for deallocating it.
 */
void Substitution::ReplaceTerm( const TermP & p_pOld, 
				const TermP & p_pNew )
{
  ReplaceTermTermPart( p_pOld, p_pNew );

  if( p_pOld->GetType() == TT_VARIABLE )
  {
    if( p_pNew->GetType() == TT_VARIABLE )
    {
      ReplaceTermVarPart( std::tr1::dynamic_pointer_cast< TermVariable >( p_pOld ), std::tr1::dynamic_pointer_cast< TermVariable >( p_pNew ) );
    }
    else
    {
      throw Exception( E_NOT_IMPLEMENTED,
		       "May not replace a variable with something else.",
		       __FILE__,
		       __LINE__ );
    }
  }
}

/**
 *  Remove from this Substitution the PairVarTerm at the specified index.
 *  Throws E_INDEX_OUT_OF_BOUNDS if p_iIndex is not in the vector.
 *  \param p_iIndex IN An iterator to the PairVarTerm to remove.
 */
void Substitution::RemovePair( SubMap::const_iterator p_iIndex )
{
  if( p_iIndex == m_mPairs.end() )
    throw Exception( E_INDEX_OUT_OF_BOUNDS,
		     "Bounds error.",
		     __FILE__,
		     __LINE__ );

  for( SubMap::iterator l_Iter = m_mPairs.begin();
       l_Iter != m_mPairs.end();
       l_Iter++ )
  {
    if( l_Iter == p_iIndex )
    {
      m_mPairs.erase( l_Iter );
      return;
    }
  }
}

/**
 *  Print this Substitution to a string representation.
 *  \return A string representation of this Substitution.
 */
std::string Substitution::ToStr() const
{
  std::string l_sRet;
  l_sRet += "(\n";
  
  for( SubMap::const_iterator i = m_mPairs.begin();
       i != m_mPairs.end();
       i++ )
  {
    l_sRet += " ";
    l_sRet += i->first->ToStr() + " -> " + i->second->ToStr();
    l_sRet += "\n";
  }
  l_sRet += ")\n";

  return l_sRet;
}

/**
 *  Retrieve an iterator to the first pair.
 *  \return An iterator to the first pair.
 */
SubMap::const_iterator Substitution::Begin() const
{
  return m_mPairs.begin();
}

/**
 *  Retrieve an iterator beyond the last pair.
 *  \return An iterator beyond the last pair.
 */
SubMap::const_iterator Substitution::End() const
{
  return m_mPairs.end();
}

/**
 *  Determine whether or not this Substitution is equal to another.
 *  It is if it contains precisely the same pairs.
 *  \param p_Other IN The other Substitution.
 *  \return Whether or not the other Substitution is equal to this one.
 */
bool Substitution::Equal( const Substitution & p_Other ) const
{
  if( m_mPairs.size() != p_Other.m_mPairs.size() )
    return false;
  for( SubMap::const_iterator i = m_mPairs.begin(); i != m_mPairs.end(); i++ )
  {
    SubMap::const_iterator j = p_Other.m_mPairs.find( (*i).first );
    if( j == p_Other.m_mPairs.end() || !(*i).second->Equal( *(*j).second ) )
      return false;
  }
  return true;
}

size_t Substitution::GetMemSizeMin() const
{
  size_t l_iSize = sizeof( Substitution ) + m_mPairs.size() * ( sizeof( TermVariableP ) + sizeof( TermP ) );
  return l_iSize;
}

size_t Substitution::GetMemSizeMax() const
{
  size_t l_iSize = sizeof( Substitution ) + m_mPairs.size() * ( sizeof( TermVariableP ) + sizeof( TermP ) );
  for( SubMap::const_iterator i = m_mPairs.begin(); i != m_mPairs.end(); i++ )
    l_iSize += (*i).first->GetMemSizeMax() + (*i).second->GetMemSizeMax();
  return l_iSize;
}
