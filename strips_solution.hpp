#ifndef STRIPS_SOLUTION_HPP__
#define STRIPS_SOLUTION_HPP__

class StripsSolution
{
public:

  StripsSolution( const std::tr1::shared_ptr< StripsProblem > & p_pProblem );

  StripsSolution( const std::tr1::shared_ptr< StripsProblem > & p_pProblem,
		  std::string p_sInput );

  StripsSolution( const StripsSolution & p_Other );

  virtual ~StripsSolution();

  std::tr1::shared_ptr< StripsProblem > GetCProblem() const;

  bool ApplyOperator( unsigned int p_iOperIndexInDomain, 
		      Substitution * p_pNewSub );

  unsigned int GetPlanLength() const;

  const State * GetCState( unsigned int p_iIndex ) const;

  const State * GetCFollowState( unsigned int p_iIndex ) const;

  const Operator * GetCOperator( unsigned int p_iIndex ) const;

  const Substitution * GetCSubstitution( unsigned int p_iIndex ) const;

  bool ContainsState( const State & p_State ) const;

  bool IsComplete() const;

  virtual size_t GetMemSizeMin() const;
  virtual size_t GetMemSizeMax() const;

private:
  std::tr1::shared_ptr< StripsProblem > m_pProblem;
  std::vector< State * > m_vFollowStates;
  std::vector< unsigned int > m_vOperatorIndices;
  std::vector< Substitution * > m_vSubstitutions;
};

#endif//STRIPS_SOLUTION_HPP__
