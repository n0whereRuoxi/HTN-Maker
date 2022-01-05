#ifndef FORMULA_PRED_HPP__
#define FORMULA_PRED_HPP__

class FormulaPred;
typedef std::tr1::shared_ptr< FormulaPred > FormulaPredP;

class FormulaPred : public Formula
{
public:

  FormulaPred( std::stringstream & p_Stream, 
	       const TypeTable & p_TypeTable,
	       const std::vector< FormulaPred > & p_vAllowablePredicates );

  FormulaPred( const FormulaPred & p_Other );

  FormulaPred( std::string p_sString,
	       const TypeTable & p_TypeTable,
	       const std::vector< FormulaPred > & p_vAllowablePredicates );

  virtual ~FormulaPred();

  virtual FormulaType GetType() const;

  virtual std::string ToStr() const;
  virtual std::string ToStrNoTyping() const;

  std::string GetRelation() const;

  unsigned int GetValence() const;

  TermP GetCParam( unsigned int p_iIndex ) const;

  virtual FormulaP AfterSubstitution( const Substitution & p_sSubs,
				      unsigned int p_iDepth ) const;

  virtual bool IsGround() const;

  virtual bool Implies( const FormulaP & p_pOther ) const;

  virtual std::vector< TermVariableP > GetVariables() const;
  virtual std::vector< TermConstantP > GetConstants() const;

  unsigned int GetRelationIndex() const
  {
    return m_iRelation;
  }

  virtual bool Equal( const Formula & p_Other ) const;

  virtual size_t GetMemSizeMin() const;
  virtual size_t GetMemSizeMax() const;

private:
  FormulaPred();
  void ConstructorInternal( std::stringstream & p_Stream, 
			    const TypeTable & p_TypeTable,
			    const std::vector< FormulaPred > & p_vAllowablePredicates );

  std::vector< TermP > m_vParams;

  unsigned int m_iRelation;
};

struct HashPredicate
{
  size_t operator() ( const FormulaPred & x ) const;
};

struct HashPredicatePointer
{
  size_t operator() ( const FormulaPred * x ) const;
};

struct EqualPredicate
{
  bool operator() ( const FormulaPred & x, const FormulaPred & y ) const;
};

struct EqualPredicatePointer
{
  bool operator() ( const FormulaPred * x, const FormulaPred * y ) const;
};

bool PredRelationsAreEqual( const FormulaPred & p_First,
			    const FormulaPred & p_Second );

#endif//FORMULA_PRED_HPP__
