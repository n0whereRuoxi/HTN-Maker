#ifndef STRIPS_PROBLEM_HPP__
#define STRIPS_PROBLEM_HPP__

class StripsProblem
{
public:

  StripsProblem( std::string p_sProblem,
		 const std::tr1::shared_ptr< StripsDomain > & p_pDomain );

  StripsProblem( const StripsProblem & p_Other );

  virtual ~StripsProblem();

  std::string GetName() const;

  std::tr1::shared_ptr< StripsDomain > GetCDomain() const;

  const State * GetCInitState() const;

  FormulaP GetCGoals() const;

  std::string ToPddl() const;

  const TypeTable & GetObjectTypes() const;

  size_t GetMemSizeMin() const;
  size_t GetMemSizeMax() const;

private:
  std::string m_sName;
  std::tr1::shared_ptr< StripsDomain > m_pDomain;
  State * m_pInitState;
  FormulaP m_pGoals;
  long m_iRequirements;
  TypeTable m_ObjectTypes;
};

#endif//STRIPS_PROBLEM_HPP__
