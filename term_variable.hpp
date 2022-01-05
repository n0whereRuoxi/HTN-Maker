#ifndef TERM_VARIABLE_HPP__
#define TERM_VARIABLE_HPP__

class TermVariable;
typedef std::tr1::shared_ptr< TermVariable > TermVariableP;

class TermVariable : public TermString
{
public:
  //  TermVariable( const TermVariable & p_tOther );
  virtual ~TermVariable();

  virtual TermType GetType() const;

  virtual TermP AfterSubstitution( const Substitution & p_sSubs,
							  unsigned int p_iDepth ) const;

  virtual bool Equal( const Term & p_Other ) const;

protected:
  TermVariable( std::string p_sName );

  friend class TermTable;
};

#endif//TERM_VARIABLE_HPP__
