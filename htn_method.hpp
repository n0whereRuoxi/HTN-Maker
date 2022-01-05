#ifndef HTN_METHOD_HPP__
#define HTN_METHOD_HPP__

class HtnMethod
{
public:
  static HtnMethod * FromPddl( std::stringstream & p_sInput, 
			       const std::set< std::string, StrLessNoCase > & p_sTypes, 
			       const std::vector< FormulaPred > & p_vAllowablePredicates,
			       long p_iRequirements );
  static HtnMethod * FromShop( std::stringstream & p_sInput );

  HtnMethod( const HtnMethod & p_Other );

  virtual ~HtnMethod();

  HtnTaskHeadP GetCHead() const;
  FormulaConjP GetCPreconditions() const;
  unsigned int GetNumSubtasks() const;
  HtnTaskHeadP GetCSubtask( unsigned int p_iIndex ) const;

  bool Subsumes( const HtnMethod * p_pOther ) const;

  std::vector< TermVariableP > GetVariables() const;
  std::set< TermVariableP > GetRelVars() const;
  std::tr1::shared_ptr< const std::vector< TermVariableP > > GetRelevantVariables() const;

  std::string ToStr() const;
  std::string ToPddl( long p_iRequirements ) const;

  bool SubtasksArePartiallyLinked() const;

  HtnMethod * AfterSubstitution( const Substitution & p_sSub,
				 unsigned int p_iDepth ) const;

  void AddNdCheckers();

  double GetQValue() const;
  int GetQCount() const;
  void UpdateQValue( double p_fNewCost );
  const std::string & GetId() const;
  void SetId( const std::string & p_sId );

  size_t GetMemSizeMin() const;
  size_t GetMemSizeMax() const;

private:
  HtnMethod();

  std::string m_sId;
  HtnTaskHeadP m_pHead;
  FormulaConjP m_pPreconditions;
  std::vector< HtnTaskHeadP > m_vSubtasks;
  TypeTable m_TypeTable;
  double m_fQValue;
  int m_iQCount;
  mutable std::tr1::shared_ptr< std::vector< TermVariableP > > m_pVariables;
};

#endif//HTN_METHOD_HPP__
