#ifndef ANNOTATED_PLAN_HPP__
#define ANNOTATED_PLAN_HPP__

class AnnotatedPlan : public StripsSolution
{
public:

  AnnotatedPlan( const std::tr1::shared_ptr< StripsProblem > & p_pProblem,
		 std::string p_sInput );

  AnnotatedPlan( const AnnotatedPlan & p_Other );

  virtual ~AnnotatedPlan();

  unsigned int GetNumMethods() const;

  const HtnMethod * GetCMethod( unsigned int p_iIndex ) const;
  const Substitution * GetCMethodSub( unsigned int p_iIndex ) const;
  unsigned int GetMethodBeforeState( unsigned int p_iIndex ) const;
  unsigned int GetMethodAfterState( unsigned int p_iIndex ) const;
  std::tr1::shared_ptr< HtnTaskDescr > GetCTaskDescr( unsigned int p_iIndex ) const;
  FormulaConjP GetCMethodEffects( unsigned int p_iIndex ) const;
  double GetMethodCost( unsigned int p_iIndex ) const;

  void AddMethodInst( const HtnMethod * p_pMethod, 
		      const Substitution * p_pSub,
		      unsigned int p_iBeforeStateNum,
		      unsigned int p_iAfterStateNum,
		      std::tr1::shared_ptr< HtnTaskDescr > p_pTaskDescr,
		      FormulaConjP p_pMethodEffects,
		      double p_fMethodCost );

private:
  std::vector< Substitution * > m_vMethodSubs;
  std::vector< HtnMethod * > m_vMethods;
  std::vector< unsigned int > m_vBeforeStates;
  std::vector< unsigned int > m_vAfterStates;
  std::vector< std::tr1::shared_ptr< HtnTaskDescr > > m_vTaskDescrs;
  std::vector< FormulaConjP > m_vMethodEffects;
  std::vector< double > m_vMethodCosts;
};

#endif//ANNOTATED_PLAN_HPP__
