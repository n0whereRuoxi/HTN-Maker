#ifndef STRING_TABLE_HPP__
#define STRING_TABLE_HPP__

#include <tr1/unordered_map>

/* \todo When there are a large number of unique strings that should exist for
 *  only a small amount of time, this can waste a ton of space.  For example,
 *  HTN-Maker creates a large number of temporary variable names, which never
 *  expire.  In one run that was giving me problems, I found that 6,220,384
 *  strings were stored, at a total cost of over 395MB.  I cannot think of any
 *  way to garbage collect, because we do not know when the string is no longer
 *  used.  Perhaps, because we have the TermTable, this data structure is
 *  completely unnecessary.  More testing will be required.
 */

class StringTable
{
public:
  unsigned int Lookup( std::string l_sKey );
  std::string Lookup( unsigned int l_iKey );

  size_t GetMemSizeMin() const;
  size_t GetMemSizeMax() const;

private:
  std::vector< std::string > m_vStrLookup;
  std::tr1::unordered_map< std::string, unsigned int, HashStr, StrEquNoCase > m_mIntLookup;
};

#endif//STRING_TABLE_HPP__
