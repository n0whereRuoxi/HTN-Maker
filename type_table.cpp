#include <string>
#include <set>
#include <tr1/memory>

#include "exception.hpp"
#include "funcs.hpp"
#include "term.hpp"
#include "term_string.hpp"
#include "term_table.hpp"
#include "type_table.hpp"

/** \file type_table.hpp
 *  Declaration of TypeTable class.
 */

/** \file type_table.cpp
 *  Definition of TypeTable class.
 */

/** \class TypeTable
 *  A hash map from the string representing a Term to the type of the Term.
 *  Unlike the StringTable and TermTable classes, there is no intention that
 *   there will be a single one of these.  Rather, they follow around 
 *   declarations of the types of terms.
 */

/**
 *  The one and only TermTable, defined in term_table.cpp.
 */
extern TermTable g_TermTable;

/**
 *  Produce a Term from its string representation and a TypeTable.
 *  If the TypeTable is not empty, the Term must appear in it, and will be 
 *   given the associated type, or no type if the type is blank.  If it is
 *   empty, any Term may be created with no type.
 *  \param p_sTerm IN A string representation of the desired Term.
 *  \param p_TypeTable IN A table of allowable Terms with their associated 
 *   types (if any).
 *  \return If using a TermTable, a pointer into it.  Otherwise, a pointer to a
 *   new Term that the caller must deallocate.
 */
TermP ReadTerm( const std::string & p_sTerm, 
		const TypeTable & p_TypeTable )
{
  if( p_TypeTable.size() > 0 )
  {
    if( p_TypeTable.find( p_sTerm ) == p_TypeTable.end() )
      throw Exception( E_NOT_IMPLEMENTED,
		       "Attempt to use a term \"" + p_sTerm + "\" that was not declared.",
		       __FILE__,
		       __LINE__ );

    std::string l_sType = (*p_TypeTable.find( p_sTerm )).second;

    if( l_sType == "" )
      return g_TermTable.Lookup( p_sTerm );
    else
      return g_TermTable.Lookup( p_sTerm, l_sType );
  }
  else
  {
    return g_TermTable.Lookup( p_sTerm );
  }
}

size_t TypeTable::GetMemSizeMin() const
{
  size_t l_iSize = sizeof( TypeTable ) + bucket_count() * sizeof( std::string ) * 2;
  for( const_iterator i = begin(); i != end(); i++ )
    l_iSize += (*i).first.capacity() + (*i).second.capacity();
  return l_iSize;
}

size_t TypeTable::GetMemSizeMax() const
{
  return GetMemSizeMin();
}
