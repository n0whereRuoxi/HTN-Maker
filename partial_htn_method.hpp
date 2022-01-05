#ifndef PARTIAL_HTN_METHOD__
#define PARTIAL_HTN_METHOD__

class PartialHtnMethod
{
public:
  PartialHtnMethod( const std::tr1::shared_ptr< HtnDomain > & p_pDomain, 
		    const std::tr1::shared_ptr< HtnTaskDescr > & p_pTaskDescr,
		    const Substitution * p_pTaskSubs,
		    unsigned int p_iFinalStateIndex );
  PartialHtnMethod( const PartialHtnMethod & p_Other );
  virtual ~PartialHtnMethod();

  bool SuppliesPrec( const FormulaConjP & p_pForm ) const;
  bool SuppliesEffect( const FormulaConjP & p_pForm ) const;
  bool SuppliesNewPrec( const FormulaConjP & p_pForm,
			const FormulaConjP & p_pOld ) const;
  bool SuppliesNewEffect( const FormulaConjP & p_pForm,
			  const FormulaConjP & p_pOld ) const;

  void AddOperator( const Operator * p_pOp, 
		    const Substitution * p_pRealSub,
		    unsigned int p_iBeforeStateNum,
		    bool p_bPartialGeneralization );

  void AddMethod( const HtnMethod * p_pMethod,
		  const Substitution * p_pRealSub,
		  unsigned int p_iBeforeStateNum,
		  unsigned int p_iAfterStateNum,
		  const FormulaConjP & p_pEffects,
		  bool p_bPartialGeneralization,
		  int p_iCost = 0 );

  HtnMethod * CreateMethod( bool p_bSafe,
			    bool p_bPartialGeneralization,
			    bool p_bQValues = false ) const;

  std::tr1::shared_ptr< HtnTaskDescr > GetCTaskDescr() const;
  const Substitution * GetCTaskSubs() const;
  const Substitution * GetCMasterSubs() const;

  FormulaConjP GetGroundEffects() const;
  FormulaConjP GetGroundPreconditions() const;

  FormulaConjP GetActualEffects() const;

  unsigned int GetInitStateNum() const;
  unsigned int GetFinalStateNum() const;
  unsigned int GetCurrentStateNum() const;

  void MarkMethodStart( unsigned int p_iInitStateNum );
  void Advance();

  void GetNewTaskVars();

  bool RemainingAddListSatisfied( const State * p_pState ) const;

  unsigned int GetNumRemainingAddList() const
  {
    return m_vRemainingAddList.size();
  }

  unsigned int GetNumRemainingPrecs() const
  {
    return m_vRemainingPrecs.size();
  }

  size_t GetMemSizeMin() const;
  size_t GetMemSizeMax() const;

private:

  void DoAddListUnification( const FormulaP & p_pNewEffects,
			     unsigned int p_iTaskAddListIndex,
			     Substitution * p_pSub );

  void UpdateAddsAndPrecs( const Substitution & p_ReplaceSub );

  void DoAddListTermReplacement( const TermVariableP & p_pOldVar,
				 const TermVariableP & p_pNewVar,
				 Substitution * p_pSub,
				 Substitution & p_ReplaceSub );

  void DoPrecondUnification( const FormulaPredP & p_pNewEffects,
			     unsigned int p_iPrecondIndex,
			     Substitution * p_pSub );

  bool DoesOperatorConflict( unsigned int p_iBeforeStateNum ) const;
  void CreateNewVars( const Substitution * p_pRealSub,
		      Substitution & p_NewSub,
		      bool p_bPartialGeneralization );
  void ProcessEffects( const FormulaConjP & p_pEffects,
		       const Substitution * p_pRealSub,
		       bool p_bPartialGeneralization,
		       Substitution & p_Subst );
  void ProcessPreconditions( const FormulaConjP & p_pPrecs,
			     const Substitution & p_Subst );
  bool DoesMethodConflict( unsigned int p_iBeforeStateNum,
			   unsigned int p_iAfterStateNum ) const;

  std::tr1::shared_ptr< HtnDomain > m_pDomain;
  std::tr1::shared_ptr< HtnTaskDescr > m_pTaskDescr;
  std::vector< FormulaP > m_vRemainingAddList;
  std::vector< FormulaP > m_vRemainingPrecs;
  Substitution m_TaskSubs;
  Substitution m_MasterSubs;
  std::vector< Operator * > m_vOperators;
  std::vector< Substitution * > m_vOperSubs;
  std::vector< unsigned int > m_vOperBeforeStates;
  std::vector< HtnMethod * > m_vMethods;
  std::vector< Substitution * > m_vMethodSubs;
  std::vector< FormulaConjP > m_vMethodEffects;
  std::vector< unsigned int > m_vMethodBeforeStates;
  std::vector< unsigned int > m_vMethodAfterStates;
  unsigned int m_iInitStateIndex;
  unsigned int m_iFinalStateIndex;
  unsigned int m_iCurrentStateIndex;
  int m_iTotalCost;
};

#endif//PARTIAL_HTN_METHOD__
