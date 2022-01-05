#ifndef FORMULA_NEG_HPP__
#define FORMULA_NEG_HPP__

class FormulaNeg;
typedef std::tr1::shared_ptr< FormulaNeg > FormulaNegP;

class FormulaNeg : public Formula
{
public:

  FormulaNeg( std::stringstream & p_sStream, 
	      const TypeTable & p_TypeTable,
	      const std::vector< FormulaPred > & p_vAllowablePredicates );

  FormulaNeg( std::string p_sString, 
	      const TypeTable & p_TypeTable,
	      const std::vector< FormulaPred > & p_vAllowablePredicates );

  FormulaNeg( const FormulaNeg & p_Other );

  virtual ~FormulaNeg();

  virtual FormulaType GetType() const;

  virtual std::string ToStr() const;
  virtual std::string ToStrNoTyping() const;

  FormulaP GetCNegForm() const;

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
  FormulaNeg();
  void ConstructorInternal( std::stringstream & p_Stream, 
			    const TypeTable & p_TypeTable,
			    const std::vector< FormulaPred > & p_vAllowablePredicates );

  FormulaP m_pNegForm;
};

#endif//FORMULA_NEG_HPP__
