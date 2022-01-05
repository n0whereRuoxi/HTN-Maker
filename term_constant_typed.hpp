#ifndef TERM_CONSTANT_TYPED_HPP__
#define TERM_CONSTANT_TYPED_HPP__

class TermConstantTyped : public TermConstant
{
public:
  //  TermConstantTyped( const TermConstantTyped & p_tOther );
  virtual ~TermConstantTyped();

  virtual std::string ToStr() const;

  virtual bool Equal( const Term & p_Other ) const;

  /**
   *  Assert that a TermConstantTyped does have typing information.
   *  Return True.
   */
  virtual bool HasTyping() const
  {
    return true;
  }

  virtual std::string GetTyping() const;

  /**
   *  Retrieve the index into the global StringTable of this Term's typing.
   *  \return The index into the global StringTable of this Term's typing.
   */
  virtual unsigned int GetTypingStrTableIndex() const
  {
    return m_iTypingStrTableIndex;
  }

private:
  unsigned int m_iTypingStrTableIndex;

protected:
  TermConstantTyped( std::string p_sName, std::string p_sTyping );

  friend class TermTable;
};

#endif//TERM_CONSTANT_TYPED_HPP__
