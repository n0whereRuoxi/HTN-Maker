#ifndef FORMULA_HPP__
#define FORMULA_HPP__

#include <set>

enum FormulaType
{
  FT_PRED,
  FT_NEG,
  FT_CONJ,
  //  FT_DISJ,
  //  FT_IMP,
  //  FT_EXIS,
  //  FT_UNIV,
  FT_EQU,
};

class FormulaPred;

class Formula;
typedef std::tr1::shared_ptr< Formula > FormulaP;

//struct FormulaPLess
//{
//  bool operator()( const FormulaP & p_pForm1, 
//		   const FormulaP & p_pForm2 ) const;
//};

typedef std::vector< FormulaP > FormulaPVec;
//typedef FormulaPVec FormulaPSet;
//typedef std::set< FormulaP, FormulaPLess > FormulaPSet;
typedef std::vector< FormulaP >::const_iterator FormulaPVecCI;
typedef std::vector< FormulaP >::iterator FormulaPVecI;
//typedef FormulaPVecCI FormulaPSetCI;
//typedef FormulaPVecI FormulaPSetI;
//typedef std::set< FormulaP, FormulaPLess >::const_iterator FormulaPSetCI;
//typedef std::set< FormulaP, FormulaPLess >::iterator FormulaPSetI;

class Formula
{
public:

  virtual ~Formula();

  /**
   *  Retrieve the general category of this Formula.
   *  \return The FormulaType of this Formula.
   */
  virtual FormulaType GetType() const = 0;

  /**
   *  Print this Formula to a string representation.
   *  \return A string representation of this Formula.
   */
  virtual std::string ToStr() const = 0;
  /**
   *  Print this Formula to a string representation, without showing any types
   *   of Terms.
   *  \return A string representation of this Formula, without types.
   */
  virtual std::string ToStrNoTyping() const = 0;

  /**
   *  Return a smart pointer to a new Formula that results from applying the 
   *   given Substitution to this Formula.
   *  \param p_sSub IN The Substitution to apply.
   *  \param p_iDepth IN The current Substitution depth.
   *  \return A smart pointer to a new Formula that results from applying the 
   *   given Substitution to this.
   */
  virtual FormulaP AfterSubstitution( const Substitution & p_sSub,
				      unsigned int p_iDepth ) const = 0;
  /**
   *  Determine whether or not this Formula is ground (contains no 
   *   TermVariables).
   *  \return Whether or not this Formula is ground.
   */
  virtual bool IsGround() const = 0;

  bool IsValidAtom() const;

  /**
   *  Determine whether or not this Formula implies some other Formula.
   *  For the sake of efficiency, this requires that both Formulas be in a
   *   standard form.  The following forms are allowed: predicates, negations 
   *   of predicates, equalities, negations of equalities, and conjunctions of
   *   the former.  That is, no conjunctions of conjunctions, no negations of 
   *   conjunctions, and no negations of negations.
   *  \param p_pOther IN A smart pointer to the Formula that this might imply.
   *  \return Whether or not this implies p_pOther.
   */
  virtual bool Implies( const FormulaP & p_pOther ) const = 0;

  /**
   *  Retrieve a list of all TermVariables appearing in this Formula.
   *  \return A list of all TermVariables appearing in this Formula.
   */
  virtual std::vector< TermVariableP > GetVariables() const = 0;

  /**
   *  Retrieve a list of all TermConstants appearing in this Formula.
   *  \return A list of all TermConstants appearing in this Formula.
   */
  virtual std::vector< TermConstantP > GetConstants() const = 0;

  /**
   *  Determine whether or not this Formula is equal to another.
   *  \param p_Other IN The Formula to compare this against.
   *  \return Whether or not this Formula is equal to p_Other.
   */
  virtual bool Equal( const Formula & p_Other ) const = 0;

  virtual size_t GetMemSizeMin() const = 0;
  virtual size_t GetMemSizeMax() const = 0;
};

FormulaP NewFormula( std::stringstream & p_Stream, 
		     const TypeTable & p_TypeTable,
		     const std::vector< FormulaPred > & p_vAllowablePredicates ) throw( MissingStringException );

bool operator==( const Formula & p_First, const Formula & p_Second );

#endif//FORMULA_HPP__
