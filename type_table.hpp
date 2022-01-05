#ifndef TYPE_TABLE_HPP__
#define TYPE_TABLE_HPP__

#include <tr1/unordered_map>

class TypeTable : public std::tr1::unordered_map< std::string, std::string, HashStr, StrEquNoCase >
{
public:
  size_t GetMemSizeMin() const;
  size_t GetMemSizeMax() const;
};

TermP ReadTerm( const std::string & p_sTerm, 
		const TypeTable & p_TypeTable );

#endif//TYPE_TABLE_HPP__
