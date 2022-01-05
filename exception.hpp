#ifndef EXCEPTION_HPP__
#define EXCEPTION_HPP__

#include <string>
#include <vector>

enum ExceptionType
{
  E_PARSE_MISSING_STRING,
  E_PARSE_STREAM_FAIL,
  E_PARSE_BAD_STRING,
  E_TERM_TYPE_UNKNOWN,
  E_NOT_IMPLEMENTED,
  E_INDEX_OUT_OF_BOUNDS,
  E_SUBS_VAR_DOUBLE,
  E_SUBS_RECURSE,
  E_OPER_NOT_APPLICABLE,
  E_OPER_OVERLAP,
  E_BAD_CAST,
  E_FORMULA_TYPE_UNKNOWN,
  E_NEG_NOT_PRED,
  E_STATE_NOT_ATOM,
  E_DOMAIN_MATCH,
  E_METHOD_NOT_APPLICABLE,
  E_TYPING_MISMATCH,
};

class Exception : public std::exception
{
public:
  Exception( ExceptionType p_iType,
	     std::string p_sMessage,
	     std::string p_sSourceFileName,
	     unsigned int p_iSourceLineNum );

  Exception( const Exception & p_Other );
  virtual ~Exception() throw ();

  ExceptionType GetType() const;
  std::string GetMessage() const;
  std::string GetSourceFileName() const;
  unsigned int GetSourceLineNum() const;

  virtual std::string ToStr() const;

  static std::string StrFromType( ExceptionType p_iType );

private:
  ExceptionType m_iType;
  std::string m_sMessage;
  std::string m_sSourceFileName;
  unsigned int m_iSourceLineNum;
  std::string m_sBacktrace;
};

class FileReadException : public Exception
{
public:
  FileReadException( ExceptionType p_iType,
		     std::string p_sMessage,
		     std::string p_sSourceFileName,
		     unsigned int p_iSourceLineNum );
  FileReadException( const FileReadException & p_Other );
  virtual ~FileReadException() throw ();

  virtual std::string ToStr() const;

  void SetFileName( const std::string & p_sFileName );
  std::string GetFileName() const;

private:
  std::string m_sFileName;
};

class MissingStringException : public FileReadException
{
public:
  MissingStringException( std::string p_sMessage,
			  std::string p_sSourceFileName,
			  unsigned int p_iSourceLineNum,
			  std::string p_sMissingString );
  MissingStringException( const MissingStringException & p_Other );
  virtual ~MissingStringException() throw ();

  virtual std::string ToStr() const;

private:
  std::string m_sMissingString;
};

class StreamFailException : public FileReadException
{
public:
  StreamFailException( std::string p_sMessage,
		       std::string p_sSourceFileName,
		       unsigned int p_iSourceLineNum );
  StreamFailException( const StreamFailException & p_Other );
  virtual ~StreamFailException() throw ();
private:
};

class UnexpectedStringException : public FileReadException
{
public:
  UnexpectedStringException( std::string p_sMessage,
			     std::string p_sSourceFileName,
			     unsigned int p_iSourceLineNum );
  UnexpectedStringException( const UnexpectedStringException & p_Other );
  virtual ~UnexpectedStringException() throw ();
private:
};

#endif//EXCEPTION_HPP__
