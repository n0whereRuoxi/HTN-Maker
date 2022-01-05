#ifndef TERM_STRING_HPP__
#define TERM_STRING_HPP__

class TermString : public Term
{
public:
  //  TermString( const TermString & p_tOther );
  virtual ~TermString();

  virtual TermType GetType() const = 0;

  virtual std::string ToStr() const;
  virtual std::string ToStrNoTyping() const;

  virtual size_t GetMemSizeMin() const;
  virtual size_t GetMemSizeMax() const;

private:
  std::string m_sValue;

protected:
  TermString( std::string p_sName );

  friend class TermTable;
};

#endif//TERM_STRING_HPP__
