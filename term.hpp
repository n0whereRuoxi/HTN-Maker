#ifndef TERM_HPP__
#define TERM_HPP__

class Substitution;

#define MAX_SUBS_DEPTH 10

enum TermType
{
  TT_CONSTANT,
  TT_VARIABLE,
  TT_FUNCTION,
};

class Term;
typedef std::tr1::shared_ptr< Term > TermP;

class Term
{
public:
  virtual ~Term();

  /**
   *  Determine the TermType of this Term.
   *  \return The TermType associated with this Term.
   */
  virtual TermType GetType() const = 0;
  
  virtual TermP AfterSubstitution( const Substitution & p_sSub,
				   unsigned int p_iDepth ) const = 0;

  /**
   *  Print this Term to a string.
   *  \return A string representation of this Term.
   */
  virtual std::string ToStr() const = 0;

  /**
   *  Print this Term to a string, ignoring typing information.
   *  \return A string representation of this Term, without typing.
   */
  virtual std::string ToStrNoTyping() const = 0;

  /**
   *  Determine whether or not this Term is equal to another.
   *  \param p_Other IN The Term to which this should be compared.
   *  \return True if they are equal, otherwise false.
   */
  virtual bool Equal( const Term & p_Other ) const = 0;

  /**
   *  Determine whether or not this Term has an associated type.
   *  This refers to the semantics of the Term, not which TermType it is.
   *  \return Whether or not the Term has an associated type.
   */
  virtual bool HasTyping() const
  {
    return false;
  }

  /**
   *  Get the type associated with this Term.
   *  This refers to the semantics of the Term, not which TermType it is.
   *  If Term::HasTyping() returns false, this will throw E_NOT_IMPLEMENTED.
   *  \return The type associated with this Term.
   */
  virtual std::string GetTyping() const
  {
    throw Exception( E_NOT_IMPLEMENTED,
		     "Attempt to read typing information from a non-typed term.",
		     __FILE__,
		     __LINE__ );
  }

  /**
   *  Get the index into the global StringTable for the type associated with
   *   this Term.
   *  This refers to the semantics of the Term, not which TermType it is.
   *  If Term::HasTyping() returns false, this will throw E_NOT_IMPLEMENTED.
   *  \return The index of the type associated with this Term.
   */
  virtual unsigned int GetTypingStrTableIndex() const
  {
    throw Exception( E_NOT_IMPLEMENTED,
		     "Attempt to read typing information from a non-typed term.",
		     __FILE__,
		     __LINE__ );
  }

  virtual size_t GetMemSizeMin() const = 0;
  virtual size_t GetMemSizeMax() const = 0;

protected:
  TermP GetThis() const;

private:
  std::tr1::weak_ptr< Term > m_pThis;

  friend class TermTable;
};

bool operator==( const Term & p_First, const Term & p_Second );

#endif//TERM_HPP__
