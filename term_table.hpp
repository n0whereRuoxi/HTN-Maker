#ifndef TERM_TABLE_HPP__
#define TERM_TABLE_HPP__

#include <tr1/unordered_map>

class TermTable
{
public:
  virtual ~TermTable();
  TermP Lookup( std::string l_sKey );
  TermP Lookup( std::string l_sKey, std::string l_sTyping );
  size_t GetMemSizeMin() const;
  size_t GetMemSizeMax() const;
private:
  static Term * NewTerm( std::stringstream & p_Stream );
  static Term * NewTerm( std::string p_sString ) throw ( MissingStringException );

  static Term * NewTerm( std::string p_sName, std::string p_sTyping ) throw ( MissingStringException );

  std::tr1::unordered_map< std::string, TermP, HashStr, StrEquNoCase > m_mTermLookup;
};

#endif//TERM_TABLE_HPP__
