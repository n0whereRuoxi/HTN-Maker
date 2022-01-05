#include <vector>
#include <string>
#include <cassert>
#include <sstream>
#include <set>
#include <iostream>
#include <algorithm>
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

/** \file state.hpp
 *  Declaration of State class.
 */

/** \file state.cpp
 *  Definition of State class.
 */

/** \class State
 *  A state (complete set of predicates that hold simultaneously) in a planning
 *   context.
 */

/** \var State::m_vAtoms
 *  A doubly-indexed list of smart pointers to all predicates that hold in 
 *   this State.
 *  Technically, this is a vector of vectors of smart pointers to predicates.
 *   Each of the internal vectors contains smart pointers to predicates with 
 *   the same predicate symbol.
 *  This is done to make looking up whether or not a given predicate holds in 
 *   this State faster.
 *  \todo The lookup could potentially be even faster if we used hashtables
 *   instead of vectors.
 */

/** \var State::m_vConstants
 *  A list of all constants used in the atoms of this State.
 *  This is filled in as-needed and invalidated when necessary to prevent
 *   the overhead in computing it many times.
 */

/** \var State::m_iStateNum
 *  The index of this State in a plan.
 *  \todo Is this really necessary?
 */

/**
 *  A functor to order a vector of vectors of predicates from smallest vector
 *   to largest vector.
 *  Order among two vectors of the same size is meaningless, but we define it
 *   so that equality testing becomes easier.
 */
struct AtomsCompare
{
  bool operator()( const std::vector< FormulaPredP > & p_Vec1,
		   const std::vector< FormulaPredP > & p_Vec2 ) const
  {
    if( p_Vec1.size() < p_Vec2.size() )
      return true;
    if( p_Vec1.size() > p_Vec2.size() )
      return false;
    return p_Vec1[0]->GetRelationIndex() < p_Vec2[0]->GetRelationIndex();
  }
} g_AtomsComparer;


/**
 *  Construct a State from a stream containing its textual representation.
 *  \param p_sStream INOUT A stream containing a textual representation of the
 *   State.  The stream will be advanced beyond this representation.
 *  \param p_iStateNum IN The number of this State.
 *  \param p_TypeTable IN A table of usable terms with their associated types.
 *   If this is non-empty, only terms that appear in it may be used in the
 *   predicates of this state, and they will use their associated type.  
 *   Otherwise, any term may be used and will be untyped.
 *  \param p_vAllowablePredicates IN A list of predicate symbols and their
 *   associated arities that may be used.  If non-empty, only those predicates
 *   may appear in the state.  Otherwise, any can be used.
 */
State::State( std::stringstream & p_sStream,
	      unsigned int p_iStateNum,
	      const TypeTable & p_TypeTable,
	      const std::vector< FormulaPred > & p_vAllowablePredicates )
{
  ConstructorInternal( p_sStream, p_TypeTable, p_vAllowablePredicates );
  m_iStateNum = p_iStateNum;
}

/**
 *  Construct a State from its textual representation.
 *  \param p_sString IN A textual representation of the State.
 *  \param p_iStateNum IN The number of this State.
 *  \param p_TypeTable IN A table of usable terms with their associated types.
 *   If this is non-empty, only terms that appear in it may be used in the
 *   predicates of this state, and they will use their associated type.  
 *   Otherwise, any term may be used and will be untyped.
 *  \param p_vAllowablePredicates IN A list of predicate symbols and their
 *   associated arities that may be used.  If non-empty, only those predicates
 *   may appear in the state.  Otherwise, any can be used.
 */
State::State( std::string p_sString,
	      unsigned int p_iStateNum,
	      const TypeTable & p_TypeTable,
	      const std::vector< FormulaPred > & p_vAllowablePredicates )
{
  std::stringstream l_Stream( p_sString );
  ConstructorInternal( l_Stream, p_TypeTable, p_vAllowablePredicates );
  m_iStateNum = p_iStateNum;
}

/**
 *  Construct a State as a copy of an existing State.
 *  \param p_Other IN The State to copy.
 */
State::State( const State & p_Other )
{
  for( unsigned int i = 0; i < p_Other.m_vAtoms.size(); i++ )
  {
    m_vAtoms.push_back( std::vector< FormulaPredP >() );
    for( unsigned int j = 0; j < p_Other.m_vAtoms[i].size(); j++ )
      m_vAtoms[i].push_back( p_Other.m_vAtoms[i][j] );
  }
  m_iStateNum = p_Other.m_iStateNum;
}

/**
 *  Destruct this State.
 */
State::~State()
{
}

/**
 *  Construct a State from a stream containing its textual representation.
 *  \param p_Stream INOUT A stream containing a textual representation of the
 *   State.  The stream will be advanced beyond this representation.
 *  \param p_TypeTable IN A table of usable terms with their associated types.
 *   If this is non-empty, only terms that appear in it may be used in the
 *   predicates of this state, and they will use their associated type.  
 *   Otherwise, any term may be used and will be untyped.
 *  \param p_vAllowablePredicates IN A list of predicate symbols and their
 *   associated arities that may be used.  If non-empty, only those predicates
 *   may appear in the state.  Otherwise, any can be used.
 */
void State::ConstructorInternal( std::stringstream & p_Stream, 
				 const TypeTable & p_TypeTable,
				 const std::vector< FormulaPred > & p_vAllowablePredicates )
{
  EatWhitespace( p_Stream );
  EatString( p_Stream, "(" );
  EatWhitespace( p_Stream );

  // Special case for where a list is an implicit conjunction.
  if( p_Stream.peek() != '(' )
  {
    if( p_Stream.peek() == ')' )
    {
      EatString( p_Stream, ")" );
      return;
    }
    EatString( p_Stream, "and" );
  }

  EatWhitespace( p_Stream );

  while( p_Stream.peek() != ')' )
  {
    FormulaPredP l_pNewAtom( new FormulaPred( p_Stream, p_TypeTable, p_vAllowablePredicates ) );

    if( !l_pNewAtom->IsGround() )
    {
      std::string l_sMessage;
      l_sMessage = "\"";
      l_sMessage += l_pNewAtom->ToStr();
      l_sMessage += "\" is a member of a state, but not an atom.";
      throw Exception( E_STATE_NOT_ATOM,
		       l_sMessage,
		       __FILE__,
		       __LINE__ );
    }

    bool l_bFound = false;
    for( unsigned int i = 0; i < m_vAtoms.size() && !l_bFound; i++ )
    {
      if( PredRelationsAreEqual( *m_vAtoms[i][0], *l_pNewAtom ) )
      {
	l_bFound = true;
	bool l_bFound2 = false;
	for( unsigned int j = 0; j < m_vAtoms[i].size() && !l_bFound2; j++ )
	{
	  if( *m_vAtoms[i][j] == *l_pNewAtom )
	    l_bFound2 = true;
	}
	if( !l_bFound2 )
	  m_vAtoms[i].push_back( l_pNewAtom );
      }
    }
    if( !l_bFound )
    {
      std::vector< FormulaPredP > l_vNew;
      l_vNew.push_back( l_pNewAtom );
      m_vAtoms.push_back( l_vNew );
    }

    EatWhitespace( p_Stream );
  }

  EatString( p_Stream, ")" );

  std::sort( m_vAtoms.begin(), m_vAtoms.end(), g_AtomsComparer );
}

/**
 *  Retrieve the number of atoms in this State.
 *  \return The number of atoms in this State.
 */
unsigned int State::GetNumAtoms() const
{
  unsigned int l_iNumAtoms = 0;
  for( unsigned int i = 0; i < m_vAtoms.size(); i++ )
    l_iNumAtoms += m_vAtoms[i].size();
  return l_iNumAtoms;
}

/**
 *  Retrieve the index of this State in its plan.
 */
int State::GetStateNum() const
{
  return m_iStateNum;
}

/**
 *  Move the elements of one vector into another, then deallocate the first.
 *  \param p_pDst INOUT A pointer to a vector to which elements will be added.
 *  \param p_pSrc INOUT A pointer to a vector from which elements will be 
 *   moved, and which will be deallocated.
 */
void CopyVectorsAndDelete( std::vector< Substitution * > * p_pDst,
			   std::vector< Substitution * > * p_pSrc )
{
  for( unsigned int l_iVecIndex = 0; l_iVecIndex < p_pSrc->size(); l_iVecIndex++ )
  {
    p_pDst->push_back( p_pSrc->at( l_iVecIndex ) );
  }
  delete p_pSrc;
}

/**
 *  Find the substitutions that make a vector of formulas hold in this State.
 *  In particular, try to match the first formula in that vector, which
 *   must be a predicate, against a specific atom in this State that has the
 *   same predicate symbol and arity.  If they cannot be unified, then return
 *   with an empty list.  Otherwise, recurse to the next conjunct.
 *  \param p_pPrecs IN A vector of smart pointers to the formulas to be 
 *   instantiated.
 *  \param p_pSub IN The partial Substitution of which this will find 
 *   extensions.
 *  \param p_pAtom IN A smart pointer to a predicate that is in this State and
 *   has the same predicate symbol and arity as the current formula.
 *  \param p_vRelVars IN A list of variables of which we actually care about
 *   all possible substitutions.
 *  \param p_vRet OUT A list of Substitutions that make all formulas in the
 *   vector ground and satisfied in this State.
 */
void State::GetInstantiationsDoublePredicate( const FormulaPVec & p_pPrecs,
					      const Substitution * p_pSub,
					      const FormulaPredP & p_pAtom,
					      const std::set< TermVariableP > & p_vRelVars,
					      std::vector< Substitution * > & p_vRet ) const
{
  FormulaPredP p_pCurConj = std::tr1::dynamic_pointer_cast< FormulaPred >( p_pPrecs[0] );
  Substitution l_NewSubs( *p_pSub );
  std::set< TermVariableP > l_vNewRelVars( p_vRelVars );
  bool l_bBad = false;
  for( unsigned int l_iParam = 0; l_iParam < p_pAtom->GetValence() && !l_bBad; l_iParam++ )
  {
    TermP l_pAtomParam = p_pAtom->GetCParam( l_iParam );
    TermP l_pConjParam = p_pCurConj->GetCParam( l_iParam );
    if( l_pConjParam->GetType() == TT_CONSTANT )
    {
      if( !( *l_pConjParam == *l_pAtomParam ) )
	l_bBad = true;
    }
    else
    {
      SubMap::const_iterator l_iConjSubsIndex = l_NewSubs.FindIndexByVar( std::tr1::dynamic_pointer_cast< TermVariable >( l_pConjParam ) );
      if( l_iConjSubsIndex != l_NewSubs.End() )
      {
	if( !( *l_pAtomParam == *l_iConjSubsIndex->second ) )
	  l_bBad = true;
      }
      else
      {
	if( ( l_pConjParam->HasTyping() && !l_pAtomParam->HasTyping() )
	    || ( !l_pConjParam->HasTyping() && l_pAtomParam->HasTyping() ) )
	  throw Exception( E_NOT_IMPLEMENTED,
			   "Either all terms must be typed, or none.",
			   __FILE__,
			   __LINE__ );
	if( l_pConjParam->HasTyping() && CompareNoCase( l_pConjParam->GetTyping(), l_pAtomParam->GetTyping() ) != 0 )
	  l_bBad = true;
	else
	  l_NewSubs.AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( l_pConjParam ), l_pAtomParam );
	l_vNewRelVars.erase( std::tr1::dynamic_pointer_cast< TermVariable >( l_pConjParam ) );
      }
    }
  }

  if( !l_bBad )
    CopyVectorsAndDelete( &p_vRet, GetInstantiations( p_pPrecs,
						      &l_NewSubs,
						      l_vNewRelVars ) );
}

/**
 *  Retrieve a list of Substitutions that make a vector of formulas hold in 
 *   this State.
 *  In particular, try unifying the first element of the vector, which must be
 *   a predicate, with each of the atoms in this State that have the same 
 *   predicate symbol and arity, then recursing through the next conjunct.
 *  \param p_pPrecs IN A vector of smart pointers to formulas to be 
 *   instantiated.
 *  \param p_pSub IN The partial Substitution of which this will find 
 *   extensions.
 *  \param p_vRelVars IN A list of variables of which we actually care about
 *   all possible substitutions.
 *  \return A list of Substitutions that make all elements of the vector 
 *   ground and satisfied in this State.
 */
std::vector< Substitution * > * 
  State::GetInstantiationsPredicate( const FormulaPVec & p_pPrecs,
				     const Substitution * p_pSub,
				     const std::set< TermVariableP > & p_vRelVars ) const
{
  FormulaPredP p_pCurConj = std::tr1::dynamic_pointer_cast< FormulaPred >( p_pPrecs[0] );
  std::vector<Substitution *> * l_pRet = new std::vector<Substitution *>;
  for( unsigned int i = 0; i < m_vAtoms.size(); i++ )
  {
    if( PredRelationsAreEqual( *m_vAtoms[i][0], *p_pCurConj ) )
    {
      for( unsigned int j = 0; j < m_vAtoms[i].size(); j++ )
      {
	GetInstantiationsDoublePredicate( p_pPrecs,
					  p_pSub,
					  m_vAtoms[i][j],
					  p_vRelVars,
					  *l_pRet );
	if( l_pRet->size() > 0 && p_vRelVars.size() == 0 )
	  break;
      }
    }
    if( l_pRet->size() > 0 && p_vRelVars.size() == 0 )
      break;
  }

  return l_pRet;
}

/**
 *  Retrieve a list of Substitutions that make a vector of formulas hold in 
 *   this State.
 *  In particular, try making the first element of the vector, which must be 
 *   an equality, hold.  If one of its parameters is a constant, the other 
 *   must be a  variable (because if the equality were ground, this would not 
 *   be called), so move on with a substitution that unifies the one parameter
 *   to the other.  If both of its parameters are the same variable, then it 
 *   will be satisfied no matter what Substitution is applied.  If the 
 *   parameters are different variables, then making them the same variable
 *   will make it satisfied.
 *  \param p_pPrecs IN A vector of smart pointers to the formulas to be 
 *   instantiated.
 *  \param p_pSub IN The partial Substitution of which this will find 
 *   extensions.
 *  \param p_vRelVars IN A list of variables of which we actually care about
 *   all possible substitutions.
 *  \return A list of Substitutions that make each formula in the vector
 *   ground and satisfied in this State.
 */
std::vector< Substitution * > * 
  State::GetInstantiationsEquality( const FormulaPVec & p_pPrecs,
				    const Substitution * p_pSub,
				    const std::set< TermVariableP > & p_vRelVars ) const
{
  std::vector< Substitution * > * l_pRet = new std::vector< Substitution * >;
  FormulaEquP p_pCurConj = std::tr1::dynamic_pointer_cast< FormulaEqu >( p_pPrecs[0] );

  if( p_pCurConj->GetCFirst()->HasTyping() && CompareNoCase( p_pCurConj->GetCFirst()->GetTyping(), p_pCurConj->GetCSecond()->GetTyping() ) != 0 )
  {
    // Nothing should be added, because this cannot be satisfied.
  }
  else if( p_pCurConj->GetCFirst()->GetType() == TT_CONSTANT )
  {
    // We should add the only substitution that satisfies this.
    Substitution l_NewSub( *p_pSub );
    l_NewSub.AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( p_pCurConj->GetCSecond() ), 
		      p_pCurConj->GetCFirst() );
    std::set< TermVariableP > l_vNewRelVars( p_vRelVars );
    l_vNewRelVars.erase( std::tr1::dynamic_pointer_cast< TermVariable >( p_pCurConj->GetCSecond() ) );
    CopyVectorsAndDelete( l_pRet, 
			  GetInstantiations( p_pPrecs,
					     &l_NewSub,
					     l_vNewRelVars ) );
  }
  else if( p_pCurConj->GetCSecond()->GetType() == TT_CONSTANT )
  {
    // We should add the only substitution that satisfies this.
    Substitution l_NewSub( *p_pSub );
    l_NewSub.AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( p_pCurConj->GetCFirst() ), 
		      p_pCurConj->GetCSecond() );
    std::set< TermVariableP > l_vNewRelVars( p_vRelVars );
    l_vNewRelVars.erase( std::tr1::dynamic_pointer_cast< TermVariable >( p_pCurConj->GetCFirst() ) );
    CopyVectorsAndDelete( l_pRet, 
			  GetInstantiations( p_pPrecs,
					     &l_NewSub,
					     l_vNewRelVars ) );
  }
  else if( p_pCurConj->GetCFirst()->Equal( *p_pCurConj->GetCSecond() ) )
  {
    // No new substitution is needed, because this will always be satisfied.
    CopyVectorsAndDelete( l_pRet,
			  GetInstantiations( p_pPrecs,
					     p_pSub,
					     p_vRelVars ) );
  }
  else
  {
    // The two variables must be made the same.
    Substitution l_NewSub( *p_pSub );
    l_NewSub.ReplaceTerm( p_pCurConj->GetCFirst(),
			  p_pCurConj->GetCSecond() );
    std::set< TermVariableP > l_vNewRelVars( p_vRelVars );
    l_vNewRelVars.erase( std::tr1::dynamic_pointer_cast< TermVariable >( p_pCurConj->GetCFirst() ) );
    CopyVectorsAndDelete( l_pRet,
			  GetInstantiations( p_pPrecs,
					     &l_NewSub,
					     l_vNewRelVars ) );
  }

  return l_pRet;
}

/**
 *  Retrieve a list of Substitutions that make a vector of formulas hold in 
 *   this State.
 *  In particular, find the Substitutions that will cause the first element of
 *   the vector, which must be a negation of an equality, to hold in this 
 *   State.
 *   This may require trying each pair of distinct objects of the same type.
 *  \param p_pPrecs IN A vector of smart pointers to the formulas to be 
 *   instantiated.
 *  \param p_pSub IN The partial Substitution of which this will find 
 *   extensions.
 *  \param p_vRelVars IN A list of variables of which we actually care about
 *   all possible substitutions.
 *  \return A list of Substitutions that make the members of the vector
 *   ground and satisfied in this State.
 */
std::vector< Substitution * > *
  State::GetInstantiationsNegation( const FormulaPVec & p_pPrecs,
				    const Substitution * p_pSub,
				    const std::set< TermVariableP > & p_vRelVars ) const
{
  FormulaNegP p_pCurConj = std::tr1::dynamic_pointer_cast< FormulaNeg >( p_pPrecs[0] );
  if( p_pCurConj->GetCNegForm()->GetType() != FT_EQU )
  {
    throw Exception( E_NOT_IMPLEMENTED,
			"For performance reasons, only equality formulas may be negated in preconditions.",
			__FILE__,
			__LINE__ );
  }

  std::vector< Substitution * > * l_pRet = new std::vector< Substitution * >;
  FormulaEquP l_pEquality( std::tr1::dynamic_pointer_cast< FormulaEqu >( p_pCurConj->GetCNegForm() ) );

  if( l_pEquality->GetCFirst()->GetType() == TT_CONSTANT )
  {
    // We should add every properly typed different constant.
    std::vector< TermConstantP > l_vConstants = GetConstants();
    for( unsigned int i = 0; i < l_vConstants.size(); i++ )
    {
      if( !l_vConstants[i]->Equal( *l_pEquality->GetCFirst() ) &&
	  ( !l_pEquality->GetCSecond()->HasTyping() ||
	    CompareNoCase( l_pEquality->GetCSecond()->GetTyping(),
			   l_vConstants[i]->GetTyping() ) == 0 ) )
      {
	Substitution l_NewSub( *p_pSub );
	l_NewSub.AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( l_pEquality->GetCSecond() ),
			  l_vConstants[i] );
	std::set< TermVariableP > l_vNewRelVars( p_vRelVars );
	l_vNewRelVars.erase( std::tr1::dynamic_pointer_cast< TermVariable >( l_pEquality->GetCSecond() ) );
	CopyVectorsAndDelete( l_pRet,
			      GetInstantiations( p_pPrecs,
						 &l_NewSub,
						 l_vNewRelVars ) );
      }
      if( l_pRet->size() > 0 && p_vRelVars.size() == 0 )
	break;
    }
  }
  else if( l_pEquality->GetCSecond()->GetType() == TT_CONSTANT )
  {
    // We should add every properly typed different constant.
    std::vector< TermConstantP > l_vConstants = GetConstants();
    for( unsigned int i = 0; i < l_vConstants.size(); i++ )
    {
      if( !l_vConstants[i]->Equal( *l_pEquality->GetCSecond() ) &&
	  ( !l_pEquality->GetCFirst()->HasTyping() ||
	    CompareNoCase( l_pEquality->GetCFirst()->GetTyping(),
			   l_vConstants[i]->GetTyping() ) == 0 ) )
      {
	Substitution l_NewSub( *p_pSub );
	l_NewSub.AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( l_pEquality->GetCFirst() ),
			  l_vConstants[i] );
	std::set< TermVariableP > l_vNewRelVars( p_vRelVars );
	l_vNewRelVars.erase( std::tr1::dynamic_pointer_cast< TermVariable >( l_pEquality->GetCFirst() ) );
	CopyVectorsAndDelete( l_pRet,
			      GetInstantiations( p_pPrecs,
						 &l_NewSub,
						 l_vNewRelVars ) );
      }
      if( l_pRet->size() > 0 && p_vRelVars.size() == 0 )
	break;
    }
  }
  else if( l_pEquality->GetCFirst()->Equal( *l_pEquality->GetCSecond() ) )
  {
    // Nothing should be added, because this cannot be satisfied.
  }
  else
  {
    // We should add every pair of properly-typed different constants.
    std::vector< TermConstantP > l_vConstants = GetConstants();
    for( unsigned int i = 0; i < l_vConstants.size(); i++ )
    {
      if( !l_pEquality->GetCFirst()->HasTyping() ||
	  CompareNoCase( l_pEquality->GetCFirst()->GetTyping(),
			 l_vConstants[i]->GetTyping() ) == 0 )
      {
	for( unsigned int j = 0; j < l_vConstants.size(); j++ )
	{
	  if( i != j && ( !l_pEquality->GetCSecond()->HasTyping() ||
			  CompareNoCase( l_pEquality->GetCSecond()->GetTyping(),
					 l_vConstants[j]->GetTyping() ) == 0 ) )
	  {
	    Substitution l_NewSub( *p_pSub );
	    l_NewSub.AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( l_pEquality->GetCFirst() ),
			      l_vConstants[i] );
	    l_NewSub.AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( l_pEquality->GetCSecond() ),
			      l_vConstants[j] );
	    std::set< TermVariableP > l_vNewRelVars( p_vRelVars );
	    l_vNewRelVars.erase( std::tr1::dynamic_pointer_cast< TermVariable >( l_pEquality->GetCFirst() ) );
	    l_vNewRelVars.erase( std::tr1::dynamic_pointer_cast< TermVariable >( l_pEquality->GetCSecond() ) );
	    CopyVectorsAndDelete( l_pRet,
				  GetInstantiations( p_pPrecs,
						     &l_NewSub,
						     l_vNewRelVars ) );
	  }
	  if( l_pRet->size() > 0 && p_vRelVars.size() == 0 )
	    break;
	}
      }
      if( l_pRet->size() > 0 && p_vRelVars.size() == 0 )
	break;
    }
  }

  return l_pRet;
}

/**
 *  A functor to order a vector of formulas from most specified to least.
 */
struct FormulaPMostSpecified
{
  /**
   *  Determine if the first formula is more specified than the second.
   *  What this means is that the first should have fewer possible
   *   instantiations than the second.
   *  This is only a heuristic, but seems to work well.
   *  \param p_pForm1 IN The first formula.
   *  \param p_pForm2 IN The second formula.
   *  \return Whether or not p_pForm1 is more specified than p_pForm2.
   */
  bool operator()( const FormulaP & p_pForm1,
		   const FormulaP & p_pForm2 ) const
  {
    if( p_pForm1->GetType() == FT_EQU && p_pForm2->GetType() == FT_PRED )
      return true;
    if( p_pForm1->GetType() == FT_EQU && p_pForm2->GetType() == FT_NEG )
      return true;
    if( p_pForm1->GetType() == FT_PRED && p_pForm2->GetType() == FT_EQU )
      return false;
    if( p_pForm1->GetType() == FT_PRED && p_pForm2->GetType() == FT_NEG )
      return true;
    if( p_pForm1->GetType() == FT_NEG && p_pForm2->GetType() == FT_EQU )
      return false;
    if( p_pForm1->GetType() == FT_NEG && p_pForm2->GetType() == FT_PRED )
      return false;

    if( p_pForm1->GetType() == FT_PRED )
    {
      const FormulaPredP & p_pPred1 = std::tr1::dynamic_pointer_cast< FormulaPred >( p_pForm1 );
      const FormulaPredP & p_pPred2 = std::tr1::dynamic_pointer_cast< FormulaPred >( p_pForm2 );

      unsigned int l_iNumInstances1 = 0;
      unsigned int l_iNumInstances2 = 0;
      for( unsigned int i = 0; i < m_pAtoms->size(); i++ )
      {
	if( p_pPred1->GetRelationIndex() == m_pAtoms->at(i)[0]->GetRelationIndex() )
	{
	  for( unsigned int j = 0; j < m_pAtoms->at(i).size(); j++ )
	  {
	    bool l_bReject = false;
	    for( unsigned int k = 0; k < p_pPred1->GetValence() && !l_bReject; k++ )
	    {
	      if( p_pPred1->GetCParam( k )->GetType() == TT_CONSTANT &&
		  !p_pPred1->GetCParam( k )->Equal( *m_pAtoms->at(i)[j]->GetCParam( k ) ) )
		l_bReject = true;
	    }
	    if( !l_bReject )
	      l_iNumInstances1++;
	  }
	}

	if( p_pPred2->GetRelationIndex() == m_pAtoms->at(i)[0]->GetRelationIndex() )
	{
	  for( unsigned int j = 0; j < m_pAtoms->at(i).size(); j++ )
	  {
	    bool l_bReject = false;
	    for( unsigned int k = 0; k < p_pPred2->GetValence() && !l_bReject; k++ )
	    {
	      if( p_pPred2->GetCParam( k )->GetType() == TT_CONSTANT &&
		  !p_pPred2->GetCParam( k )->Equal( *m_pAtoms->at(i)[j]->GetCParam( k ) ) )
		l_bReject = true;
	    }
	    if( !l_bReject )
	      l_iNumInstances2++;
	  }
	}
      }

      return ( l_iNumInstances1 < l_iNumInstances2 );

    }

    unsigned int l_iConsts1 = p_pForm1->GetConstants().size();
    unsigned int l_iConsts2 = p_pForm2->GetConstants().size();

    if( l_iConsts1 > l_iConsts2 )
      return true;
    if( l_iConsts2 > l_iConsts1 )
      return false;

    unsigned int l_iVars1 = p_pForm1->GetVariables().size();
    unsigned int l_iVars2 = p_pForm2->GetVariables().size();

    if( l_iVars1 < l_iVars2 )
      return true;
    return false;
  }
  const std::vector< std::vector< FormulaPredP > > * m_pAtoms;
} asdf;

/**
 *  Retrieve a list of Substitutions that make a vector of formulas hold in 
 *   this State.
 *  First, verify that it is possible to extend the passed substitution and
 *   remove any formulas that it grounds completely.  Then re-order the 
 *   remaining formulas from most specified to least specified, meaning that
 *   those at the front of the list should have fewer possible instantiations.
 *   Finally, call a specialized handler on the first element, which will
 *   recurse to this with the smaller vector and a larger substitution.
 *  \param p_pPrecs IN A vector of smart pointers to formulas to be 
 *   instantiated.
 *  \param p_pSub IN The partial Substitution of which this will find 
 *   extensions.
 *  \param p_vRelVars IN A list of those variables for which we actually want
 *   all possible replacements.
 *  \return A list of Substitutions that make each member of the vector 
 *   ground and satisfied in this State.
 */
std::vector<Substitution *> * State::GetInstantiations( const FormulaPVec & p_pPrecs,
							const Substitution * p_pSub,
							const std::set< TermVariableP > & p_vRelVars ) const
{
  FormulaPVec l_vNew;
  for( unsigned int i = 0; i < p_pPrecs.size(); i++ )
  {
    FormulaP l_pCurConj( p_pPrecs[i]->AfterSubstitution( *p_pSub, 0 ) );
    if( l_pCurConj->IsGround() )
    {
      if( !IsConsistent( l_pCurConj ) )
      {
	return new std::vector< Substitution * >;
      }
    }
    else
      l_vNew.push_back( l_pCurConj );
  }

  if( l_vNew.size() == 0 )
  {
    std::vector< Substitution * > * l_pRet = new std::vector< Substitution * >;
    l_pRet->push_back( new Substitution( *p_pSub ) );
    return l_pRet;
  }

  asdf.m_pAtoms = &m_vAtoms;
  std::sort< FormulaPVecI, FormulaPMostSpecified >( l_vNew.begin(), l_vNew.end(), asdf );

  switch( l_vNew[0]->GetType() )
  {
  case FT_PRED:
    return GetInstantiationsPredicate( l_vNew, p_pSub, p_vRelVars );
  case FT_EQU:
    return GetInstantiationsEquality( l_vNew, p_pSub, p_vRelVars );
  case FT_NEG:
    return GetInstantiationsNegation( l_vNew, p_pSub, p_vRelVars );
  case FT_CONJ:
    throw Exception( E_NOT_IMPLEMENTED,
		     "For performance reasons, a conjunction may not contain a conjunction.  Please refactor this.",
		     __FILE__,
		     __LINE__ );
  default:
    throw Exception( E_NOT_IMPLEMENTED,
		     "A conjunction contained an unknown formula type.",
		     __FILE__,
		     __LINE__ );
  }
}

/**
 *  Retrieve a list of Substitutions that make a Formula hold in this State.
 *  \param p_pPrecs IN A smart pointer to the overall Formula that is being 
 *   instantiated.
 *  \param p_pSub IN The partial Substitution of which this will find 
 *   extensions.
 *  \param p_vRelVars IN A list of those variables for which we actually want
 *   all possible replacements.
 *  \return A list of Substitutions that make the entire overall Formula 
 *   ground and satisfied in this State.
 */
std::vector<Substitution *> * State::GetInstantiations( const FormulaConjP & p_pPrecs,
							const Substitution * p_pSub,
							const std::set< TermVariableP > & p_vRelVars ) const
{
  FormulaPVec l_vNew;
  for( FormulaPVecCI i = p_pPrecs->GetBeginConj();
       i != p_pPrecs->GetEndConj();
       i++ )
  {
    l_vNew.push_back( *i );
  }
  return GetInstantiations( l_vNew, p_pSub, p_vRelVars );
}

/**
 *  Retrieve a list of Substitutions that make an Operator applicable in this
 *   State.
 *  \param p_pOp IN A pointer to the Operator to be instantiated.
 *  \param p_pSub IN The partial Substitution of which this will find 
 *   extensions.
 *  \return A list of Substitutions that make Operator ground and applicable in
 *   this State.
 */
std::vector<Substitution *> * State::GetInstantiations( const Operator * p_pOp,
							const Substitution * p_pSub ) const
{
  std::set< TermVariableP > l_vRelVars;
  for( unsigned int i = 0; i < p_pOp->GetCHead()->GetValence(); i++ )
  {
    if( p_pOp->GetCHead()->GetCParam( i )->GetType() == TT_VARIABLE )
    {
      l_vRelVars.insert( std::tr1::dynamic_pointer_cast< TermVariable >( p_pOp->GetCHead()->GetCParam( i ) ) );
    }
  }
  return GetInstantiations( p_pOp->GetCHead(), p_pOp->GetCPreconditions(), p_pSub, l_vRelVars );
}

/**
 *  Retrieve a list of Substitutions that make a head and preconditions 
 *   applicable in this State.
 *  What makes this a bit complicated is when a parameter of the head is not
 *   used in the preconditions, which means that nothing other than its type
 *   restricts what constants it might be bound to.
 *  \param p_pHead IN A smart pointer to the head to be instantiated.
 *  \param p_pPrecs IN A smart pointer to the preconditions to be instantiated.
 *  \param p_pSub IN The partial Substitution of which this will find 
 *   extensions.
 *  \param p_vRelVars IN A list of variables for which we actually want every
 *   posible replacement.
 *  \return A list of Substitutions that make the head and preconditions ground
 *   and applicable to this State.
 */
std::vector<Substitution *> * State::GetInstantiations( const FormulaPredP & p_pHead,
							const FormulaConjP & p_pPrecs,
							const Substitution * p_pSub,
							const std::set< TermVariableP > & p_vRelVars ) const
{
  std::vector< Substitution * > * l_pRet = new std::vector< Substitution * >;
  std::vector< TermConstantP > l_vConsts = GetConstants();
  std::vector< TermVariableP > l_vVars = p_pPrecs->GetVariables();

  std::vector< Substitution * > l_vParts;
  l_vParts.push_back( new Substitution( *p_pSub ) );

  while( !l_vParts.empty() )
  {
    Substitution * l_pOldSubs = l_vParts.back();
    l_vParts.pop_back();

    // If the head is not yet ground, we need to ground it.
    bool l_bAdded = false;
    for( unsigned int i = 0; i < p_pHead->GetValence() && !l_bAdded; i++ )
    {
      if( l_pOldSubs->FindIndexByVar( std::tr1::dynamic_pointer_cast< TermVariable >( p_pHead->GetCParam( i ) ) ) ==
	  l_pOldSubs->End() )
      {
	// Search for this variable in the preconditions.
	bool l_bInPrecs = false;
	for( unsigned int j = 0; j < l_vVars.size() && !l_bInPrecs; j++ )
	{
	  if( *l_vVars[j] == *p_pHead->GetCParam( i ) )
	    l_bInPrecs = true;
	}
	if( !l_bInPrecs )
	{
	  // If not in the preconditions, we must add every constant to which
	  //  it could unify.
	  l_bAdded = true;
	  for( unsigned int j = 0; j < l_vConsts.size(); j++ )
	  {
	    if( ( p_pHead->GetCParam( i )->HasTyping() && 
		  !l_vConsts[j]->HasTyping() ) ||
		( !p_pHead->GetCParam( i )->HasTyping() &&
		  l_vConsts[j]->HasTyping() ) )
	      throw Exception( E_NOT_IMPLEMENTED,
			       "Either all terms must be typed, or none.",
			       __FILE__,
			       __LINE__ );
	    if( p_pHead->GetCParam( i )->HasTyping() && CompareNoCase( p_pHead->GetCParam( i )->GetTyping(), l_vConsts[j]->GetTyping() ) != 0 )
	      continue;
	    Substitution * l_pNewSubs = new Substitution( *l_pOldSubs );
	    l_pNewSubs->AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( p_pHead->GetCParam( i ) ),
				 l_vConsts[j] );
	    l_vParts.push_back( l_pNewSubs );
	  }
	  // Otherwise, it will get taken care of by the preconditions.
	}
      }
    }

    if( l_bAdded )
      delete l_pOldSubs;
    else
    {
      std::set< TermVariableP > l_vNewRelVars( p_vRelVars );
      for( SubMap::const_iterator i = l_pOldSubs->Begin();
	   i != l_pOldSubs->End();
	   i++ )
	l_vNewRelVars.erase( (*i).first );
      std::vector< Substitution * > * l_pTemp = GetInstantiations( p_pPrecs, l_pOldSubs, l_vNewRelVars );
      delete l_pOldSubs;
      for( unsigned int i = 0; i < l_pTemp->size(); i++ )
	l_pRet->push_back( l_pTemp->at( i ) );
      delete l_pTemp;
    }
  }

  return l_pRet;
}

/**
 *  Determine whether or not a given Formula holds in this State.
 *  \param p_pForm IN A smart pointer to the Formula that might hold.
 *  \return Whether or not the given Formula holds in this State.
 */
bool State::IsConsistent( const FormulaP & p_pForm ) const
{
  if( !p_pForm->IsGround() )
    return false;

  switch( p_pForm->GetType() )
  {
  case FT_PRED:
    // A predicate holds if it appears in the list of atoms.
  {
    for( unsigned int i = 0; i < m_vAtoms.size(); i++ )
    {
      if( PredRelationsAreEqual( *m_vAtoms[i][0], *std::tr1::dynamic_pointer_cast< FormulaPred >( p_pForm ) ) )
      {
	for( unsigned int j = 0; j < m_vAtoms[i].size(); j++ )
	{
	  if( *m_vAtoms[i][j] == *std::tr1::dynamic_pointer_cast< FormulaPred >( p_pForm ) )
	    return true;
	}
	return false;
      }
    }
    return false;
  }
  case FT_EQU:
    // An equality holds if the two parameters are equal.
  {
    return std::tr1::dynamic_pointer_cast< FormulaEqu >( p_pForm )->GetCFirst()->Equal( *std::tr1::dynamic_pointer_cast< FormulaEqu >( p_pForm )->GetCSecond() );
  }
  case FT_NEG:
    // A negation holds if its opposite does not.
  {
    switch( std::tr1::dynamic_pointer_cast< FormulaNeg >( p_pForm )->GetCNegForm()->GetType() )
    {
    case FT_PRED:
    case FT_EQU:
      return !IsConsistent( std::tr1::dynamic_pointer_cast< FormulaNeg >( p_pForm )->GetCNegForm() );
    default:
      throw Exception( E_NEG_NOT_PRED,
		       "For now, only predicates and equalities may be negated.",
		       __FILE__,
		       __LINE__ );
    }
  }
  case FT_CONJ:
    // A conjunction holds if all of its conjuncts hold.
  {
    FormulaConjP l_pForm = std::tr1::dynamic_pointer_cast< FormulaConj >( p_pForm );
    for( FormulaPVecCI i = l_pForm->GetBeginConj();
	 i != l_pForm->GetEndConj();
	 i++ )
    {
      if( !IsConsistent( *i ) )
	return false;
    }
    return true;
  }
  default:
    throw Exception( E_FORMULA_TYPE_UNKNOWN,
		     "Unknown formula type.",
		     __FILE__,
		     __LINE__ );
  }
}

/**
 *  Determine whether or not a given Formula could be made to hold in this 
 *   State by applying a Substitution to it.
 *  This is not a strict test.  That is, it may return true when there is, in
 *   fact, no such Substitution.  However, if it returns false then none 
 *   exists.  Consider the Formula ( and ( = ?x ?y ) ( not ( = ?x ?y ) ) ).
 *   No possible Substitution could make this hold, but this function will not
 *   recognize this.  The purpose is to throw out partially-substituted 
 *   Formulas that can easily be ruled out.
 *  \param p_pForm IN A smart pointer to the Formula that might hold.
 *  \return Whether or not the given Formula could be made to hold in this
 *   State by applying a Substitution to it.
 */
bool State::CouldBeConsistent( const FormulaP & p_pForm ) const
{
  switch( p_pForm->GetType() )
  {
  case FT_PRED:
    // A grounded predicate must hold as is, while an unground predicate may if
    //  the ground parts of it match something.

    if( p_pForm->IsGround() )
      return IsConsistent( p_pForm );
    else
    {
      for( unsigned int i = 0; i < m_vAtoms.size(); i++ )
      {
	if( PredRelationsAreEqual( *m_vAtoms[i][0], *std::tr1::dynamic_pointer_cast< FormulaPred >( p_pForm ) ) )
	{
	  for( unsigned int j = 0; j < m_vAtoms[i].size(); j++ )
	  {
	    bool l_bOk = true;
	    for( unsigned int k = 0; k < std::tr1::dynamic_pointer_cast< FormulaPred >( p_pForm )->GetValence() && l_bOk; k++ )
	    {
	      if( std::tr1::dynamic_pointer_cast< FormulaPred >( p_pForm )->GetCParam( k )->GetType() == TT_CONSTANT &&
		  m_vAtoms[i][j]->GetCParam( k )->GetType() == TT_CONSTANT &&
		  !std::tr1::dynamic_pointer_cast< FormulaPred >( p_pForm )->GetCParam( k )->Equal( *m_vAtoms[i][j]->GetCParam( k ) ) )
		l_bOk = false;
	    }
	    if( l_bOk )
	      return true;
	  }
	  return false;
	}
      }
      return false;
    }
  case FT_EQU:
    // A grounded equality must hold as is, while an ungrounded equality may
    //  always be made to hold.
    if( p_pForm->IsGround() )
      return IsConsistent( p_pForm );
    else
      return true;
  case FT_NEG:
    // A grounded negation must hold as is, while an ungrounded one may.
    // Note that this is not strict either.
    switch( std::tr1::dynamic_pointer_cast< FormulaNeg >( p_pForm )->GetCNegForm()->GetType() )
    {
    case FT_PRED:
    case FT_EQU:
      if( p_pForm->IsGround() )
	return !IsConsistent( std::tr1::dynamic_pointer_cast< FormulaNeg >( p_pForm )->GetCNegForm() );
      else
	return true;
    default:
      throw Exception( E_NEG_NOT_PRED,
		       "For now, only predicates and equalities may be negated.",
		       __FILE__,
		       __LINE__ );
    }
  case FT_CONJ:
  {
    // A conjunction may hold only if all of its conjuncts may.
    FormulaConjP l_pForm = std::tr1::dynamic_pointer_cast< FormulaConj >( p_pForm );
    for( FormulaPVecCI i = l_pForm->GetBeginConj();
	 i != l_pForm->GetEndConj();
	 i++ )
    {
      if( ( *i )->IsGround() &&
	  !IsConsistent( *i ) )
	return false;
    }
    return true;
  }
  default:
    throw Exception( E_FORMULA_TYPE_UNKNOWN,
		     "Unknown formula type.",
		     __FILE__,
		     __LINE__ );
  }
}

/**
 *  Retrieve a pointer to the State that results from applying an Operator
 *   to this State.
 *  \param p_pOp IN A pointer to the Operator to apply.
 *  \param p_pSub IN A pointer to a Substitution that grounds the Operator and
 *   makes it applicable to this State.
 *  \return A pointer to the new State.  The caller is responsible for 
 *   deallocating it.
 */
State * State::NextState( const Operator * p_pOp, const Substitution * p_pSub ) const
{
  FormulaP l_pPreNoVars( p_pOp->GetCPreconditions()->AfterSubstitution( *p_pSub, 0 ) );
  if( !l_pPreNoVars->IsGround() )
  {
    throw Exception( E_OPER_NOT_APPLICABLE,
		     "The selected operator instance is not ground.",
		     __FILE__,
		     __LINE__ );
  }
  if( !IsConsistent( l_pPreNoVars ) )
  {
    throw Exception( E_OPER_NOT_APPLICABLE,
		     "The selected operator is not applicable to the selected state.",
		     __FILE__,
		     __LINE__ );
  }

  State * l_pNext = new State( *this );
  l_pNext->m_iStateNum++;

  FormulaP l_pEffNoVars( p_pOp->GetCEffects()->AfterSubstitution( *p_pSub, 0 ) );

  l_pNext->ApplyEffects( l_pEffNoVars );
  std::sort( l_pNext->m_vAtoms.begin(), l_pNext->m_vAtoms.end(), g_AtomsComparer );

  return l_pNext;
}

/**
 *  Apply the effects of an Operator to this State.
 *  First, any negated predicates from the Formula are removed.  Then, any
 *   predicates in the Formula are added.
 *  \param p_pEff IN A smart pointer to the effects to apply.
 */
void State::ApplyEffects( const FormulaP & p_pEff )
{
  if( !p_pEff->IsGround() )
    throw Exception( E_OPER_NOT_APPLICABLE,
		     "The selected operator instance is not ground.",
		     __FILE__,
		     __LINE__ );

  switch( p_pEff->GetType() )
  {
  case FT_PRED:
    {
      bool l_bFoundRow = false;
      for( unsigned int i = 0; i < m_vAtoms.size() && !l_bFoundRow; i++ )
      {
	if( PredRelationsAreEqual( *m_vAtoms[i][0], *std::tr1::dynamic_pointer_cast< FormulaPred >( p_pEff ) ) )
	{
	  l_bFoundRow = true;
	  bool l_bFoundCol = false;
	  for( unsigned int j = 0; j < m_vAtoms[i].size() && !l_bFoundCol; j++ )
	  {
	    if( *m_vAtoms[i][j] == *p_pEff )
	      l_bFoundCol = true;
	  }
	  if( !l_bFoundCol )
	    m_vAtoms[i].push_back( std::tr1::dynamic_pointer_cast< FormulaPred >( p_pEff ) );
	}
      }
      if( !l_bFoundRow )
      {
	std::vector< FormulaPredP > l_vNew;
	l_vNew.push_back( std::tr1::dynamic_pointer_cast< FormulaPred >( p_pEff ) );
	m_vAtoms.push_back( l_vNew );
      }
      break;
    }
  case FT_NEG:
    {
      if( std::tr1::dynamic_pointer_cast< FormulaNeg >( p_pEff )->GetCNegForm()->GetType() != FT_PRED )
	throw Exception( E_NEG_NOT_PRED,
			 "For now, only predicates may be negated.",
			 __FILE__,
			 __LINE__ );

      bool l_bFoundRow = false;
      for( unsigned int i = 0; i < m_vAtoms.size() && !l_bFoundRow; i++ )
      {
	if( PredRelationsAreEqual( *m_vAtoms[i][0], 
				   *std::tr1::dynamic_pointer_cast< FormulaPred >( std::tr1::dynamic_pointer_cast< FormulaNeg >( p_pEff )->GetCNegForm() ) ) )
	{
	  l_bFoundRow = true;
	  bool l_bFoundCol = false;
	  for( unsigned int j = 0; j < m_vAtoms[i].size() && !l_bFoundCol; j++ )
	  {
	    if( *m_vAtoms[i][j] == *std::tr1::dynamic_pointer_cast< FormulaNeg >( p_pEff )->GetCNegForm() )
	    {
	      l_bFoundCol = true;
	      m_vAtoms[i].erase( m_vAtoms[i].begin() + j );
	    }
	  }
	  if( m_vAtoms[i].empty() )
	    m_vAtoms.erase( m_vAtoms.begin() + i );
	}
      }
      break;
    }
  case FT_CONJ:
    {
      FormulaConjP l_pEff = std::tr1::dynamic_pointer_cast< FormulaConj >( p_pEff );
      for( FormulaPVecCI i = l_pEff->GetBeginConj();
	   i != l_pEff->GetEndConj();
	   i++ )
      {
	if( ( *i )->GetType() == FT_NEG )
	  ApplyEffects( *i );
      }

      for( FormulaPVecCI i = l_pEff->GetBeginConj();
	   i != l_pEff->GetEndConj();
	   i++ )
      {
	if( ( *i )->GetType() == FT_PRED )
	  ApplyEffects( *i );
      }
      break;
    }
  default:
    throw Exception( E_FORMULA_TYPE_UNKNOWN,
		     "Unknown formula type.",
		     __FILE__,
		     __LINE__ );
  }

  m_vConstants.clear();
}

/**
 *  Determine whether or not this State is equal to another, not counting 
 *   number.
 *  Two states are equal if they contain exactly the same atoms.
 *  \param p_Other IN The State that might be equal.
 *  \return Whether or not this State is equal to the other.
 */
bool State::Equal( const State & p_Other ) const
{
  if( m_vAtoms.size() != p_Other.m_vAtoms.size() )
    return false;
  for( unsigned int i = 0; i < m_vAtoms.size(); i++ )
  {
    if( m_vAtoms[i].size() != p_Other.m_vAtoms[i].size() )
      return false;
    for( unsigned int k = 0; k < m_vAtoms[i].size(); k++ )
    {
      bool l_bFoundCol = false;
      for( unsigned int l = 0; l < p_Other.m_vAtoms[i].size() && !l_bFoundCol; l++ )
      {
	if( *m_vAtoms[i][k] == *p_Other.m_vAtoms[i][l] )
	  l_bFoundCol = true;
      }
      if( !l_bFoundCol )
	return false;
    }
  }

  //  if( p_First.GetStateNum() != p_Second.GetStateNum() )
  //    return false;
  return true;
}

/**
 *  Retrieve a list of constants that appear in the atoms of this State.
 *  This will only be calculated the first time it is called, unless the state
 *   changes.
 *  \return A list of constants that appear in the atoms of this State.
 */
std::vector< TermConstantP > State::GetConstants() const
{
  if( m_vConstants.size() == 0 )
  {
    for( unsigned int i = 0; i < m_vAtoms.size(); i++ )
    {
      for( unsigned int j = 0; j < m_vAtoms[i].size(); j++ )
      {
	std::vector< TermConstantP > l_vTemp = m_vAtoms[i][j]->GetConstants();
	for( unsigned int k = 0; k < l_vTemp.size(); k++ )
	{
	  bool l_bFound = false;
	  for( unsigned int l = 0; l < m_vConstants.size() && !l_bFound; l++ )
	  {
	    if( *m_vConstants[l] == *l_vTemp[k] )
	      l_bFound = true;
	  }
	  if( !l_bFound )
	    m_vConstants.push_back( l_vTemp[k] );
	}
      }
    }
  }
  return m_vConstants;
}

/**
 *  Retrieve a string containing a textual representation of this State.
 *  \return A string containing a textual representation of this State.
 */
std::string State::ToStr() const
{
  std::string l_sRet = "( ";

  for( unsigned int i = 0; i < m_vAtoms.size(); i++ )
  {
    for( unsigned int j = 0; j < m_vAtoms[i].size(); j++ )
    {
      l_sRet += m_vAtoms[i][j]->ToStr();
      l_sRet += " ";
    }
  }

  l_sRet += ")";
  return l_sRet;
}

/**
 *  Retrieve a string containing a PDDL description of this State.
 *  \return A string containing a PDDL description of this State.
 */
std::string State::ToPddl() const
{
  std::string l_sRet = "  ( :init\n";

  for( unsigned int i = 0; i < m_vAtoms.size(); i++ )
  {
    for( unsigned int j = 0; j < m_vAtoms[i].size(); j++ )
    {
      l_sRet += "    " + m_vAtoms[i][j]->ToStr() + "\n";
    }
  }

  l_sRet += "  )\n";
  return l_sRet;
}

/**
 *  Determine whether or not two States are equal, ignoring number.
 *  \param p_First IN The first State.
 *  \param p_Second IN The second State.
 *  \return Whether or not the two States are equal, ignoring number.
 */
bool operator==( const State & p_First, const State & p_Second )
{
  return p_First.Equal( p_Second );
}

size_t State::GetMemSizeMin() const
{
  size_t l_iSize = sizeof( State ) + m_vAtoms.capacity() * sizeof( std::vector< FormulaPredP > ) + m_vConstants.capacity() * sizeof( TermConstantP );
  for( unsigned int i = 0; i < m_vAtoms.size(); i++ )
  {
    l_iSize += m_vAtoms[i].capacity() * sizeof( FormulaPredP );
  }
  return l_iSize;
}

size_t State::GetMemSizeMax() const
{
  size_t l_iSize = GetMemSizeMin();
  for( unsigned int i = 0; i < m_vAtoms.size(); i++ )
  {
    for( unsigned int j = 0; j < m_vAtoms[i].size(); j++ )
      l_iSize += m_vAtoms[i][j]->GetMemSizeMax();
  }
  return l_iSize;
}
