#ifndef HTN_SOLUTION_HPP__
#define HTN_SOLUTION_HPP__

class DecompPart
{
public:
  DecompPart( const std::string & p_sName,
	      const std::string & p_sId );

  DecompPart( const DecompPart & p_Other );

  virtual ~DecompPart();

  void AddChild( DecompPart * p_pNew );

  std::string Print( const std::string & p_sIndent ) const;

  const std::vector< DecompPart * > & GetChildren() const;
  const std::string & GetName() const;
  const std::string & GetId() const;

  size_t GetMemSizeMin() const;
  size_t GetMemSizeMax() const;

private:
  std::string m_sName;
  std::string m_sId;
  std::vector< DecompPart * > m_vChildren;
};

class HtnSolution : public HtnProblem
{
public:
  static HtnSolution * FromShop( const std::tr1::shared_ptr< HtnDomain > & p_pDomain,
				 std::stringstream & p_sInput );
  static HtnSolution * FromPddl( const std::tr1::shared_ptr< HtnDomain > & p_pDomain,
				 std::stringstream & p_sINput );

  static HtnSolution * FromProblem( const HtnProblem * p_pProblem );

  HtnSolution( const HtnSolution & p_Other );

  virtual ~HtnSolution();

  unsigned int GetPlanLength() const;
  const Operator * GetCOperator( unsigned int p_iIndex ) const;
  const Substitution * GetCSubstitution( unsigned int p_iIndex ) const;

  void ApplyMethod( unsigned int p_iMethodIndexInDomain, 
		    const Substitution * p_pNewSub );

  void ApplyOperator( unsigned int p_iOperIndexInDomain,
		      const Substitution * p_pNewSub );

  bool IsComplete() const;

  std::string Print( bool p_bIncludeTrace ) const;
  std::string ToStr() const;

  int GetNumDecomps() const;

  virtual size_t GetMemSizeMin() const;
  virtual size_t GetMemSizeMax() const;

  const std::vector< DecompPart * > & GetDecompTree() const;

private:
  HtnSolution( const std::tr1::shared_ptr< HtnDomain > & p_pDomain );

  State * m_pInitState;
  
  std::vector< unsigned int > m_vOperIndices;
  std::vector< Substitution * > m_vOperSubs;
  int m_iNumDecomps;

  std::vector< DecompPart * > m_vDecompTree;
  std::vector< DecompPart * > m_vParents;
};

#endif//HTN_SOLUTION_HPP__
