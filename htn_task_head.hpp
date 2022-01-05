#ifndef HTN_TASK_HEAD_HPP__
#define HTN_TASK_HEAD_HPP__

class HtnTaskHead;
typedef std::tr1::shared_ptr< HtnTaskHead > HtnTaskHeadP;


class HtnTaskHead : public FormulaPred
{
public:
  HtnTaskHead( std::stringstream & p_sInput, 
	       const TypeTable & p_TypeTable );
  HtnTaskHead( std::string p_sString, 
	       const TypeTable & p_TypeTable );

  HtnTaskHead( const HtnTaskHead & p_Other );
  HtnTaskHead( const FormulaPred & p_Other );

  virtual ~HtnTaskHead();

  std::string GetName() const;

  unsigned int GetNumParams() const;

  std::vector< TermVariableP > GetVariables() const;

  //  std::string ToStr() const;

  FormulaP AfterSubstitution( const Substitution & p_Sub,
			      unsigned int p_iRecurseLevel ) const;

  virtual size_t GetMemSizeMin() const;
  virtual size_t GetMemSizeMax() const;

};

#endif//HTN_TASK_HEAD_HPP__
