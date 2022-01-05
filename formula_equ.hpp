#ifndef FORMULA_EQU_HPP__
#define FORMULA_EQU_HPP__

class FormulaEqu;
typedef std::tr1::shared_ptr< FormulaEqu > FormulaEquP;

class FormulaEqu : public Formula
{
public:

  FormulaEqu( std::stringstream & p_Stream,
	      const TypeTable & p_TypeTable );

  FormulaEqu( std::string p_sString,
	      const TypeTable & p_TypeTable );

  FormulaEqu( const FormulaEqu & p_Other );

  virtual ~FormulaEqu();

  virtual FormulaType GetType() const;

  virtual std::string ToStr() const;
  virtual std::string ToStrNoTyping() const;

  TermP GetCFirst() const;
  TermP GetCSecond() const;

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
  FormulaEqu();
  void ConstructorInternal( std::stringstream & p_Stream,
			    const TypeTable & p_TypeTable );

  TermP m_pFirstTerm;
  TermP m_pSecondTerm;
};

#endif//FORMULA_EQU_HPP__
