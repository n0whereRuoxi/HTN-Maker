#ifndef TERM_VARIABLE_TYPED_HPP__
#define TERM_VARIABLE_TYPED_HPP__

class TermVariableTyped : public TermVariable
{
public:
  //  TermVariableTyped( const TermVariableTyped & p_tOther );
  virtual ~TermVariableTyped();

  virtual std::string ToStr() const;

  virtual bool Equal( const Term & p_Other ) const;

  /**
   *  Assert that a TermVariableTyped does have typing information.
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
  TermVariableTyped( std::string p_sName, std::string p_sTyping );

  friend class TermTable;
};

#endif//TERM_VARIABLE_TYPED_HPP__
