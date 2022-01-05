#ifndef OPERATOR_HPP__
#define OPERATOR_HPP__

class Operator
{
public:
  static Operator * FromPddl( std::stringstream & p_sInput, 
			      const std::set< std::string, StrLessNoCase > & p_sTypes, 
			      const std::vector< FormulaPred > & p_vAllowablePredicates );
  static Operator * FromShop( std::stringstream & p_sInput );

  Operator( const Operator & p_Other );

  virtual ~Operator();

  FormulaConjP GetCEffects() const;

  FormulaConjP GetCPreconditions() const;

  unsigned int GetNumParams() const;

  TermP GetCParam( unsigned int p_iIndex ) const;

  std::string GetName() const;

  std::string ToStr( bool p_bIsHtn = false, int p_iIndent = 0 ) const;

  bool Equivalent( const Operator * p_pOther ) const;

  FormulaPredP GetCHead() const;

  int GetCost() const;

  size_t GetMemSizeMin() const;
  size_t GetMemSizeMax() const;

private:
  Operator();

  FormulaConjP m_pPreconditions;
  FormulaConjP m_pEffects;
  FormulaPredP m_pHead;

  int m_iCost;
};

#endif//OPERATOR_HPP__
