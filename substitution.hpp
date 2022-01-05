#ifndef SUBSTITUTION_HPP__
#define SUBSTITUTION_HPP__

#include <map>

typedef std::map< TermVariableP, TermP > SubMap;

class Substitution
{
public:
  Substitution();
  Substitution( const Substitution & p_sOther );
  virtual ~Substitution();

  unsigned int GetNumPairs() const;

  void AddPair( const TermVariableP & p_pVar, 
		const TermP & p_pTerm );

  SubMap::const_iterator FindIndexByVar( const TermVariableP & p_pVar ) const;
  SubMap::const_iterator FindIndexByTerm( const TermP & p_pTerm ) const;

  void ReplaceTerm( const TermP & p_pOld, const TermP & p_pNew );

  void RemovePair( SubMap::const_iterator p_iIndex );

  std::string ToStr() const;

  SubMap::const_iterator Begin() const;
  SubMap::const_iterator End() const;

  bool Equal( const Substitution & p_Other ) const;

  size_t GetMemSizeMin() const;
  size_t GetMemSizeMax() const;

private:
  void ReplaceTermTermPart( const TermP & p_pOld, 
			    const TermP & p_pNew );
  void ReplaceTermVarPart( const TermVariableP & p_pOld,
			   const TermVariableP & p_pNew );

  SubMap m_mPairs;
};

#endif//SUBSTITUTION_HPP__
