#ifndef HTN_DOMAIN_HPP__
#define HTN_DOMAIN_HPP__

class HtnDomain
{
public:
  static HtnDomain * FromPddl( std::stringstream & p_sInput );
  static HtnDomain * FromShop( std::stringstream & p_sInput );

  HtnDomain( const HtnDomain & p_Other );

  virtual ~HtnDomain();

  void AddOperator( Operator * p_pNewOper );
  void AddMethod( HtnMethod * p_pNewMethod );
  void RemoveOperator( unsigned int p_iIndex );
  void RemoveMethod( unsigned int p_iIndex );
  void ReplaceMethod( unsigned int p_iIndex, HtnMethod * p_pNewMethod );

  unsigned int GetNumOperators() const;
  unsigned int GetNumMethods() const;

  const Operator * GetCOperator( unsigned int p_iIndex ) const;
  const HtnMethod * GetCMethod( unsigned int p_iIndex ) const;

  std::string GetDomainName() const;

  std::string ToStr() const;
  std::string ToPddl() const;
  std::string ToShop() const;

  long GetRequirements() const;
  void AddRequirement( long p_iAdditionalRequirement );

  bool Equivalent( const HtnDomain * p_pOther ) const;

  const std::set< std::string, StrLessNoCase > & GetAllowableTypes() const;
  const TypeTable & GetConstantTypes() const;
  const std::vector< FormulaPred > & GetAllowablePredicates() const;

  void UpdateMethodQValue( unsigned int p_iMethodIndex,
			   double p_fNewCost );

  void SortMethods();
  void RandomizeMethodOrder();
  void SetMethodId( unsigned int p_iIndex, std::string p_sNewId );

  size_t GetMemSizeMin() const;
  size_t GetMemSizeMax() const;

private:
  HtnDomain();

  std::vector< Operator *> m_vOperators;
  std::vector< HtnMethod *> m_vMethods;
  std::string m_sDomainName;
  long m_iRequirements;
  std::set< std::string, StrLessNoCase > m_sAllowableTypes;
  TypeTable m_ConstantTypes;
  std::vector< FormulaPred > m_vAllowablePredicates;
};

#endif//HTN_DOMAIN_HPP__
