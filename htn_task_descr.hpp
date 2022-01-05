#ifndef HTN_TASK_DESCR_HPP__
#define HTN_TASK_DESCR_HPP__

class HtnTaskDescr
{
public:
  HtnTaskDescr( std::stringstream & p_sInput, 
		const std::set< std::string, StrLessNoCase > & p_sTypes,
		const std::vector< FormulaPred > & p_vAllowablePredicates );

  HtnTaskDescr( std::string p_sString, 
		const std::set< std::string, StrLessNoCase > & p_sTypes,
		const std::vector< FormulaPred > & p_vAllowablePredicates );

  HtnTaskDescr( const HtnTaskDescr & p_Other );

  virtual ~HtnTaskDescr();

  HtnTaskHeadP GetCHead() const;
  FormulaConjP GetCPreconditions() const;
  FormulaConjP GetCEffects() const;

  std::tr1::shared_ptr< HtnTaskDescr > AfterSubstitution( const Substitution & p_Sub,
							  unsigned int p_iRecurseLevel ) const;

  size_t GetMemSizeMin() const;
  size_t GetMemSizeMax() const;

private:
  void ConstructorInternal( std::stringstream & p_sInput, 
			    const std::set< std::string, StrLessNoCase > & p_sTypes,
			    const std::vector< FormulaPred > & p_vAllowablePredicates );

  HtnTaskHeadP m_pHead;
  FormulaConjP m_pPreconditions;
  FormulaConjP m_pEffects;
};

#endif//HTN_TASK_DESCR_HPP__
