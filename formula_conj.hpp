#ifndef FORMULA_CONJ_HPP__
#define FORMULA_CONJ_HPP__

class FormulaConj;
typedef std::tr1::shared_ptr< FormulaConj > FormulaConjP;

class FormulaConj : public Formula
{
public:

  FormulaConj( std::stringstream & p_sStream, 
	       const TypeTable & p_TypeTable,
	       const std::vector< FormulaPred > & p_vAllowablePredicates );

  FormulaConj( const FormulaConj & p_Other );

  FormulaConj( std::string p_sString, 
	       const TypeTable & p_TypeTable,
	       const std::vector< FormulaPred > & p_vAllowablePredicates );

  FormulaConj( const FormulaPVec & p_Vec );

  virtual ~FormulaConj();

  virtual FormulaType GetType() const;

  virtual std::string ToStr() const;
  virtual std::string ToStrNoTyping() const;

  FormulaPVecCI GetBeginConj() const;
  FormulaPVecCI GetEndConj() const;
  unsigned int GetNumConjs() const;

  virtual FormulaP AfterSubstitution( const Substitution & p_sSubs,
				      unsigned int p_iDepth ) const;

  virtual bool IsGround() const;

  virtual bool Implies( const FormulaP & p_pOther ) const;

  virtual std::vector< TermVariableP > GetVariables() const;
  virtual std::vector< TermConstantP > GetConstants() const;

  virtual bool Equal( const Formula & p_Other ) const;

  virtual size_t GetMemSizeMin() const;
  virtual size_t GetMemSizeMax() const;

private:
  FormulaConj();
  void ConstructorInternal( std::stringstream & p_Stream, 
			    const TypeTable & p_TypeTable,
			    const std::vector< FormulaPred > & p_vAllowablePredicates );

  FormulaPVec m_vConjuncts;

protected:
  //  void RemoveConj( unsigned int p_iIndex );
  //  void AddConjCopy( const FormulaP & p_pNew );
};

#endif//FORMULA_CONJ_HPP__
