#ifndef HTN_PROBLEM_HPP__
#define HTN_PROBLEM_HPP__

class HtnProblem
{
public:
  static HtnProblem * FromShop( const std::tr1::shared_ptr< HtnDomain > & p_pDomain,
				std::stringstream & p_sInput );
  static HtnProblem * FromPddl( const std::tr1::shared_ptr< HtnDomain > & p_pDomain,
				std::stringstream & p_sInput );

  HtnProblem( const HtnProblem & p_Other );

  virtual ~HtnProblem();

  std::string ToPddl() const;

  const State * GetCState() const;
  HtnTaskHeadP GetCTopTask() const;
  unsigned int GetNumOutstandingTasks() const;

  bool Equivalent( const HtnProblem & p_Other ) const;

  const std::tr1::shared_ptr< HtnDomain > & GetDomain() const;
  HtnTaskHeadP GetTask( unsigned int p_iIndex ) const;
  const std::string & GetName() const;
  long GetRequirements() const;

  virtual size_t GetMemSizeMin() const;
  virtual size_t GetMemSizeMax() const;

protected:
  HtnProblem( const std::tr1::shared_ptr< HtnDomain > & p_pDomain );

  const std::tr1::shared_ptr< HtnDomain > m_pDomain;
  State * m_pState;
  std::vector< HtnTaskHeadP > m_vOutstandingTasks;
  std::string m_sProbName;
  long m_iRequirements;
};

#endif//HTN_PROBLEM_HPP__
