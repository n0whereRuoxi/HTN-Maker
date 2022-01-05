#include <string>
#include <vector>
#include <sstream>
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

/** \file htn_task_head.hpp
 *  Declaration of the HtnTaskHead class.
 */

/** \file htn_task_head.cpp
 *  Definition of the HtnTaskHead class.
 */

/** \class HtnTaskHead
 *  A task in a Hierarchical Task Network.
 *  Although the semantics are different, the syntax is exactly the same as a
 *   predicate.
 */

/**
 *  Construct an HtnTaskHead from textual representation and assorted stuff.
 *  \param p_sInput INOUT A stream containing a textual representation of the 
 *   HtnTaskHead.  The stream will be advanced beyond it.
 *  \param p_TypeTable IN A table of mappings between terms and their types.
 *   If non-empty, only these terms may be used, and will be given their
 *   associated types.  Otherwise, any may be used and will be untyped.
 */
HtnTaskHead::HtnTaskHead( std::stringstream & p_sInput, 
			  const TypeTable & p_TypeTable )
  : FormulaPred( p_sInput, p_TypeTable, std::vector< FormulaPred >() )
{
}

/**
 *  Construct an HtnTaskHead from textual representation and assorted stuff.
 *  \param p_sString INOUT A textual representation of the  HtnTaskHead.
 *  \param p_TypeTable IN A table of mappings between terms and their types.
 *   If non-empty, only these terms may be used, and will be given their
 *   associated types.  Otherwise, any may be used and will be untyped.
 */
HtnTaskHead::HtnTaskHead( std::string p_sString, 
			  const TypeTable & p_TypeTable )
  : FormulaPred( p_sString, p_TypeTable, std::vector< FormulaPred >() )
{
}

/**
 *  Construct an HtnTaskHead as a copy of an existing one.
 *  \param p_Other IN The HtnTaskHead to copy.
 */
HtnTaskHead::HtnTaskHead( const HtnTaskHead & p_Other )
  : FormulaPred( p_Other )
{
}

/**
 *  Construct an HtnTaskHead as a copy of an existing predicate.
 *  \param p_Other IN The predicate to copy.
 */
HtnTaskHead::HtnTaskHead( const FormulaPred & p_Other )
  : FormulaPred( p_Other )
{
}

/**
 *  Destruct an HtnTaskHead.
 */
HtnTaskHead::~HtnTaskHead()
{
}

/**
 *  Retrieve the name of this task.
 *  \return The name of this task.
 */
std::string HtnTaskHead::GetName() const
{
  return GetRelation();
}

/**
 *  Retrieve the number of parameters of this task.
 *  \return The number of parameters of this task.
 */
unsigned int HtnTaskHead::GetNumParams() const
{
  return GetValence();
}

/**
 *  Retrieve a list of pointers to the variables used in this task.
 *  \return A list of pointers to the variables used in this task.  These 
 *   pointers have the same lifetime as the HtnTaskHead.
 */
std::vector< TermVariableP > HtnTaskHead::GetVariables() const
{
  std::vector< TermVariableP > l_vRet;

  for( unsigned int i = 0; i < GetNumParams(); i++ )
  {
    bool l_bFound = false;
    for( unsigned int j = 0; j < l_vRet.size(); j++ )
    {
      if( GetCParam( i )->GetType() == TT_VARIABLE &&
	  *l_vRet[j] == *GetCParam( i ) )
	l_bFound = true;
    }
    if( !l_bFound && GetCParam( i )->GetType() == TT_VARIABLE )
      l_vRet.push_back( std::tr1::dynamic_pointer_cast< TermVariable >( GetCParam( i ) ) );
  }

  return l_vRet;
}

/**
 *  Retrieve a textual representation of this task.
 *  \return A textual representation of this task.
 */
/*
std::string HtnTaskHead::ToStr() const
{
  std::string l_sRet = "";

  l_sRet += "( ";
  l_sRet += GetName();

  for( unsigned int i = 0; i < GetNumParams(); i++ )
  {
    l_sRet += " ";
    l_sRet += GetCParam( i )->ToStr();
  }

  l_sRet += " )";

  return l_sRet;
}
*/

/**
 *  Retrieve a smart pointer to a new HtnTaskHead that results from applying a
 *   Substitution to this.
 *  \param p_Sub IN The Substitution to apply.
 *  \param p_iRecurseLevel IN The depth of this Substitution attempt.
 *  \return A smart pointer to a new HtnTaskHead that results from applying a
 *   Substitution to this.
 */
FormulaP HtnTaskHead::AfterSubstitution( const Substitution & p_Sub,
					 unsigned int p_iRecurseLevel ) const
{
  FormulaPredP l_pAfter( std::tr1::dynamic_pointer_cast< FormulaPred >( FormulaPred::AfterSubstitution( p_Sub, p_iRecurseLevel ) ) );
  HtnTaskHeadP l_pRet( new HtnTaskHead( *l_pAfter ) );
  return l_pRet;
}

size_t HtnTaskHead::GetMemSizeMin() const
{
  return FormulaPred::GetMemSizeMin() + sizeof( HtnTaskHead ) - sizeof( FormulaPred );
}

size_t HtnTaskHead::GetMemSizeMax() const
{
  return FormulaPred::GetMemSizeMax() + sizeof( HtnTaskHead ) - sizeof( FormulaPred );
}
