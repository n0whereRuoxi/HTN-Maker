#ifndef TERM_CONSTANT_HPP__
#define TERM_CONSTANT_HPP__

class TermConstant;
typedef std::tr1::shared_ptr< TermConstant > TermConstantP;

class TermConstant : public TermString
{
public:
  TermConstant( const TermConstant & p_tOther );
  virtual ~TermConstant();

  virtual TermType GetType() const;

  virtual TermP AfterSubstitution( const Substitution & p_sSubs,
							  unsigned int p_iDepth ) const;

  virtual bool Equal( const Term & p_Other ) const;

protected:
  TermConstant( std::string p_sName );

  friend class TermTable;
};

#endif//TERM_CONSTANT_HPP__
