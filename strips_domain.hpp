#ifndef STRIPS_DOMAIN_HPP__
#define STRIPS_DOMAIN_HPP__

class StripsDomain
{
public:

  StripsDomain( std::string p_sDomain );

  StripsDomain( const StripsDomain & p_Other );

 virtual ~StripsDomain();

  std::string GetName() const;

  unsigned int GetNumOpers() const;

  const Operator * GetCOper( unsigned int p_iIndex ) const;

  unsigned int GetOperIndexByName( std::string p_sOperName ) const;

  std::string ToPddl() const;

  long GetRequirements() const;

  const TypeTable & GetConstantTypes() const;
  const std::vector< FormulaPred > & GetAllowablePredicates() const;

  size_t GetMemSizeMin() const;
  size_t GetMemSizeMax() const;

private:
  std::string m_sName;
  std::vector< Operator * > m_vOps;
  long m_iRequirements;
  std::set< std::string, StrLessNoCase > m_sAllowableTypes;
  TypeTable m_ConstantTypes;
  std::vector< FormulaPred > m_vAllowablePredicates;
};

#endif//STRIPS_DOMAIN_HPP__
