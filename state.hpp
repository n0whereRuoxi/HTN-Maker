#ifndef STATE_HPP__
#define STATE_HPP__

class State
{
public:

  State( std::stringstream & p_sStream,
	 unsigned int p_iStateNum,
	 const TypeTable & p_TypeTable,
	 const std::vector< FormulaPred > & p_vAllowablePredicates );

  State( std::string p_sString,
	 unsigned int p_iStateNum,
	 const TypeTable & p_TypeTable,
	 const std::vector< FormulaPred > & p_vAllowablePredicates );

  State( const State & p_Other );

  virtual ~State();

  int GetStateNum() const;

  unsigned int GetNumAtoms() const;

  bool Equal( const State & p_Other ) const;

  std::vector< TermConstantP > GetConstants() const;

  std::string ToStr() const;
  std::string ToPddl() const;

  std::vector<Substitution *> * GetInstantiations( const FormulaPVec & p_pPrecs,
						   const Substitution * p_pSub,
						   const std::set< TermVariableP > & p_vRelVars ) const;

  std::vector<Substitution *> * GetInstantiations( const FormulaConjP & p_pPrecs,
						   const Substitution * p_pSub,
						   const std::set< TermVariableP > & p_vRelVars ) const;

  std::vector<Substitution *> * GetInstantiations( const Operator * p_pOp,
						   const Substitution * p_pSub ) const;

  std::vector<Substitution *> * GetInstantiations( const FormulaPredP & p_pHead,
						   const FormulaConjP & p_pPrecs,
						   const Substitution * p_pSub,
						   const std::set< TermVariableP > & p_vRelVars ) const;

  bool IsConsistent( const FormulaP & p_pForm ) const;

  bool CouldBeConsistent( const FormulaP & p_pForm ) const;

  State * NextState( const Operator * p_pOp, const Substitution * p_pSub ) const;

  size_t GetMemSizeMin() const;
  size_t GetMemSizeMax() const;

private:
  void ApplyEffects( const FormulaP & p_pEff );

  void ConstructorInternal( std::stringstream & p_Stream, 
			    const TypeTable & p_TypeTable,
			    const std::vector< FormulaPred > & p_vAllowablePredicates );

  std::vector< Substitution * > *
  GetInstantiationsPredicate( const FormulaPVec & p_pPrecs,
			      const Substitution * p_pSub,
			      const std::set< TermVariableP > & p_vRelVars ) const;

  std::vector< Substitution * > *
  GetInstantiationsEquality( const FormulaPVec & p_pPrecs,
			     const Substitution * p_pSub,
			     const std::set< TermVariableP > & p_vRelVars ) const;

  std::vector< Substitution * > *
  GetInstantiationsNegation( const FormulaPVec & p_pPrecs,
			     const Substitution * p_pSub,
			     const std::set< TermVariableP > & p_vRelVars ) const;

  void GetInstantiationsDoublePredicate( const FormulaPVec & p_pPrecs,
					 const Substitution * p_pSub,
					 const FormulaPredP & p_pAtom,
					 const std::set< TermVariableP > & p_vRelVars,
					 std::vector< Substitution * > & p_vRet ) const;


  std::vector< std::vector< FormulaPredP > > m_vAtoms;

  mutable std::vector< TermConstantP > m_vConstants;

  int m_iStateNum;
};

bool operator==( const State & p_First, const State & p_Second );

#endif//STATE_HPP__
