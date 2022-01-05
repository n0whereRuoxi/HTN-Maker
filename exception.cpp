#include <sstream>
#include <cassert>
#include <execinfo.h>
#include <cxxabi.h>
#include <cstdlib>

#include "exception.hpp"

/** \file exception.hpp
 *  Declarations of class Exception and enumeration ExceptionType.
 */

/** \file exception.cpp
 *  Definition of class Exception.
 */

/** \enum ExceptionType
 *  Broad categories of exceptions, so that certain ones may be caught and 
 *   processed in certain ways.
 */

/** \var E_PARSE_MISSING_STRING
 *  Parsing a file failed because a required string was not found.
 */

/** \var E_PARSE_STREAM_FAIL
 *  Parsing a file failed because EOF or another error condition was 
 *   unexpectedly reached.
 */

/** \var E_PARSE_BAD_STRING
 *  Parsing a file failed because an unexpected string was encountered.
 */

/** \var E_TERM_TYPE_UNKNOWN
 *  A term has an unknown type.
 */

/** \var E_NOT_IMPLEMENTED
 *  Originally meant to cover reasonable features that had not (yet) been 
 *   implemented, but also used as a catch-all.
 */

/** \var E_INDEX_OUT_OF_BOUNDS
 *  Attempt to access a vector or array beyond its length.
 */

/** \var E_SUBS_VAR_DOUBLE
 *  Two pairs were added to a substitution with the same variable.
 *  This is semantically meaningless, because it would require replacing a
 *   single variable with multiple terms.
 */

/** \var E_SUBS_RECURSE
 *  A substitution was created that contains an infinite loop of replacements.
 */

/** \var E_OPER_NOT_APPLICABLE
 *  Attempt to apply an operator to a state in which its preconditions are not
 *   satisfied.
 */

/** \var E_OPER_OVERLAP
 *  Multiple structures are added to a PartialHtnMethod that cover the same 
 *   part of the plan from which it is being learned.
 */

/** \var E_BAD_CAST
 *  Attempt to cast something to an incompatible type.
 */

/** \var E_FORMULA_TYPE_UNKNOWN
 *  The type of a Formula is not recognized.
 */

/** \var E_NEG_NOT_PRED
 *  A Formula other than a FormulaPred or FormulaEqu is negated.
 *  This is fine in general first-order logic, but we require all formulas to
 *   be in a standard form for ease of computation.
 */

/** \var E_STATE_NOT_ATOM
 *  A state contains a Formula that is not a valid atom (a FormulaPred or a FormulaNeg containing a FormulaEqu.
 */

/** \var E_DOMAIN_MATCH
 *  The domain file used with a problem file is not the one expected.
 */

/** \var E_METHOD_NOT_APPLICABLE
 *  Attempt to apply an HtnMethod to a State in which the method's 
 *   preconditions can not be satisfied.
 */

/** \var E_TYPING_MISMATCH
 *  A term of a certain type is used with a term of a different type.
 */

/** \class Exception
 *  A throwable exception with information about what error occurred.
 *
 *  \todo Use subclasses with their own data members rather than types and
 *   strings, so they can be more easily inspected by catchers.
 */

/** \var Exception::m_iType
 *  The type of this Exception.
 */

/** \var Exception::m_sMessage
 *  A string describing what happened to the user.
 */

/** \var Exception::m_sSourceFileName
 *  The name of the file from which the Exception was thrown.
 *  This should always be __FILE__.
 */

/** \var Exception::m_iSourceLineNum
 *  The line number from which the Exception was thrown.
 *  This should always be __LINE__.
 */

std::string GetBacktrace()
{
  std::stringstream l_sRet;
  std::vector< void * > l_vAddresses( 200 );
  l_vAddresses.resize( backtrace( &(*l_vAddresses.begin()), l_vAddresses.size() ) );

  char * * l_pMangled = backtrace_symbols( &(*l_vAddresses.begin()), l_vAddresses.size() );
  if( l_pMangled )
  {
    for( size_t i = 2; i < l_vAddresses.size(); i++ )
    {
      std::string l_sMangled = l_pMangled[i];
      size_t start = l_sMangled.find( '(' );
      size_t end = l_sMangled.find( '+' );
      int l_iSuccess = -4;

      char * l_sCorrect = NULL;

      if( start != std::string::npos && end != std::string::npos )
	 l_sCorrect = abi::__cxa_demangle( l_sMangled.substr( start + 1, end - start - 1 ).c_str(), 0, 0, &l_iSuccess );

      if( l_iSuccess == 0 )
	l_sRet << "\t" << l_sCorrect << std::endl;
      else
	l_sRet << "\t" << l_pMangled[ i ] << std::endl;

      free( l_sCorrect );
    }
  }

  free( l_pMangled );

  return l_sRet.str();
}

/**
 *  Create an Exception, setting each of its data members.
 *  \param p_iType IN The type of this Exception.
 *  \param p_sMessage IN A message describing what happened to the user.
 *  \param p_sSourceFileName IN The name of the file from which the Exception 
 *   was thrown.  This should always be __FILE__.
 *  \param p_iSourceLineNum IN The line from which the Exception was thrown.
 *   This should always be __LINE__.
 */
Exception::Exception( ExceptionType p_iType,
		      std::string p_sMessage,
		      std::string p_sSourceFileName,
		      unsigned int p_iSourceLineNum )
{
  m_iType = p_iType;
  m_sMessage = p_sMessage;
  m_sSourceFileName = p_sSourceFileName;
  m_iSourceLineNum = p_iSourceLineNum;
  m_sBacktrace = GetBacktrace();
}

/**
 *  Copy all data members of an existing Exception to another.
 *  \param p_Other IN An existing Exception to be copied.
 */
Exception::Exception( const Exception & p_Other )
{
  m_iType = p_Other.m_iType;
  m_sMessage = p_Other.m_sMessage;
  m_sSourceFileName = p_Other.m_sSourceFileName;
  m_iSourceLineNum = p_Other.m_iSourceLineNum;
  m_sBacktrace = p_Other.m_sBacktrace;
}

/**
 *  Does nothing.
 */
Exception::~Exception() throw ()
{
}

/**
 *  Get the type of this Exception.
 *  \return The type of this Execution.
 */
ExceptionType Exception::GetType() const
{
  return m_iType;
}

/**
 *  Get the message associated with this Exception.
 *  \return The message associated with this Exception.
 */
std::string Exception::GetMessage() const
{
  return m_sMessage;
}

/**
 *  Get the name of the file from which this Exception was thrown.
 *  \return The name of the file from which this Exception was thrown.
 */
std::string Exception::GetSourceFileName() const
{
  return m_sSourceFileName;
}

/**
 *  Get the line number from which this Exception was thrown.
 *  \return The line number from which this Exception was thrown.
 */
unsigned int Exception::GetSourceLineNum() const
{
  return m_iSourceLineNum;
}

/**
 *  Print a summary of the Exception for the user to a string.
 *  \return A summary of the Exception for the user.
 */
std::string Exception::ToStr() const
{
  std::stringstream l_sRet;

  l_sRet << "Exception Thrown at "
	 << m_sSourceFileName
	 << " : "
	 << m_iSourceLineNum
	 << "\n\tType:\t"
	 << StrFromType( m_iType )
	 << "\n\tMessage:\t"
	 << m_sMessage
	 << "\n";

  l_sRet << "Backtrace:\n" << m_sBacktrace << "\n";

  return l_sRet.str();
}

/**
 *  Get a string representation of an ExceptionType.
 *  \param p_iType IN An instance of ExceptionType.
 *  \return A string containing the name of the ExceptionType.
 *
 *  This is rather dumb, honestly.
 */
std::string Exception::StrFromType( ExceptionType p_iType )
{
  switch( p_iType )
  {
  case E_NOT_IMPLEMENTED:
    return "E_NOT_IMPLEMENTED";
  case E_TERM_TYPE_UNKNOWN:
    return "E_TERM_TYPE_UNKNOWN";
  case E_SUBS_RECURSE:
    return "E_SUBS_RECURSE";
  case E_BAD_CAST:
    return "E_BAD_CAST";
  case E_INDEX_OUT_OF_BOUNDS:
    return "E_INDEX_OUT_OF_BOUNDS";
  case E_SUBS_VAR_DOUBLE:
    return "E_SUBS_VAR_DOUBLE";
  case E_FORMULA_TYPE_UNKNOWN:
    return "E_FORMULA_TYPE_UNKNOWN";
  case E_PARSE_MISSING_STRING:
    return "E_PARSE_MISSING_STRING";
  case E_PARSE_STREAM_FAIL:
    return "E_PARSE_STREAM_FAIL";
  case E_PARSE_BAD_STRING:
    return "E_PARSE_BAD_STRING";
  case E_NEG_NOT_PRED:
    return "E_NEG_NOT_PRED";
  case E_STATE_NOT_ATOM:
    return "E_STATE_NOT_ATOM";
  case E_OPER_NOT_APPLICABLE:
    return "E_OPER_NOT_APPLICABLE";
  case E_DOMAIN_MATCH:
    return "E_DOMAIN_MATCH";
  case E_OPER_OVERLAP:
    return "E_OPER_OVERLAP";
  case E_METHOD_NOT_APPLICABLE:
    return "E_METHOD_NOT_APPLICABLE";
  case E_TYPING_MISMATCH:
    return "E_TYPING_MISMATCH";
  default:
    assert( false );
    return "E_UNKNOWN_ERROR";
  }
}

/** \class FileReadException
 *  An exception related to reading a file.
 */

/** \var FileReadException::m_sFileName
 *  The name of the file that was being read.
 */

/**
 *  Construct a FileReadException from components.
 *  \param p_iType IN The type of this exception.
 *  \param p_sMessage IN A message about this exception.
 *  \param p_sSourceFileName IN The name of the source file from which this was
 *   generated.
 *  \param p_iSourceLineNum IN The line number in the source file from which
 *   this was generated.
 */
FileReadException::FileReadException( ExceptionType p_iType,
				      std::string p_sMessage,
				      std::string p_sSourceFileName,
				      unsigned int p_iSourceLineNum )
  : Exception( p_iType,
	       p_sMessage,
	       p_sSourceFileName,
	       p_iSourceLineNum ),
    m_sFileName()
{
}

/**
 *  Construct a FileReadException from another.
 *  \param p_Other IN The exception to copy.
 */
FileReadException::FileReadException( const FileReadException & p_Other )
  : Exception( p_Other ),
    m_sFileName( p_Other.m_sFileName )
{
}

/**
 *  Destruct a FileReadException.
 */
FileReadException::~FileReadException() throw ()
{
}

/**
 *  Print this exception to a string.
 */
std::string FileReadException::ToStr() const
{
  return Exception::ToStr() + "While reading file " + m_sFileName + ".\n";
}

/**
 *  Set the name of the file that was being read.
 *  \param p_sFileName IN The name of the file that was being read.
 */
void FileReadException::SetFileName( const std::string & p_sFileName )
{
  m_sFileName = p_sFileName;
}

/**
 *  Retrieve the name of the file that was being read.
 *  \return The name of the file that was being read.
 */
std::string FileReadException::GetFileName() const
{
  return m_sFileName;
}

/** \class MissingStringException
 *  An exception for a string expected but not seen when parsing a file.
 */

/** \var MissingStringException::m_sMissingString
 *  The string that was expected.
 */

/**
 *  Construct a MissingStringException from components.
 *  \param p_sMessage IN A description of the the exception.
 *  \param p_sSourceFileName IN The name of the source file from which this
 *   was generated.
 *  \param p_iSourceLineNum IN The line in the source file from which this
 *   was generated.
 *  \param p_sMissingString IN The string that had been expected.
 */
MissingStringException::MissingStringException( std::string p_sMessage,
						std::string p_sSourceFileName,
						unsigned int p_iSourceLineNum,
						std::string p_sMissingString )
  : FileReadException( E_PARSE_MISSING_STRING,
		       p_sMessage,
		       p_sSourceFileName,
		       p_iSourceLineNum ),
    m_sMissingString( p_sMissingString )
{
}

/**
 *  Construct a MissingStringException from an existing one.
 *  \param p_Other IN The exception to copy.
 */
MissingStringException::MissingStringException( const MissingStringException & p_Other )
  : FileReadException( p_Other ),
    m_sMissingString( p_Other.m_sMissingString )
{
}

/**
 *  Destruct a MissingStringException.
 */
MissingStringException::~MissingStringException() throw ()
{
}

/**
 *  Print this exception to a string.
 */
std::string MissingStringException::ToStr() const
{
  return "Expected string " + m_sMissingString + ".\n";
}

/** \class StreamFailException
 *  An exception generated when an input stream is not readable.
 */

/**
 *  Construct a StreamFailException from components.
 *  \param p_sMessage IN A description of the exception.
 *  \param p_sSourceFileName IN The name of the source code file from which
 *   this was generated.
 *  \param p_iSourceLineNum IN The line in the source code file from which
 *   this was generated.
 */
StreamFailException::StreamFailException( std::string p_sMessage,
					  std::string p_sSourceFileName,
					  unsigned int p_iSourceLineNum )
  : FileReadException( E_PARSE_STREAM_FAIL,
		       p_sMessage,
		       p_sSourceFileName,
		       p_iSourceLineNum )
{
}

/**
 *  Construct a StreamFailException as a copy of an existing one.
 *  \param p_Other IN The exception to copy.
 */
StreamFailException::StreamFailException( const StreamFailException & p_Other )
  : FileReadException( p_Other )
{
}

/**
 *  Destruct a StreamFailException.
 */
StreamFailException::~StreamFailException() throw ()
{
}

/** \class UnexpectedStringException
 *  An exception generated when an unexpected string is found in input.
 */

/**
 *  Construct an UnexpectedStringException from components.
 *  \param p_sMessage IN A description of the exception.
 *  \param p_sSourceFileName IN The name of the source code file from which
 *   this was generated.
 *  \param p_iSourceLineNum IN The line in the source code file from which
 *   this was generated.
 */
UnexpectedStringException::UnexpectedStringException( std::string p_sMessage,
						      std::string p_sSourceFileName,
						      unsigned int p_iSourceLineNum )
  : FileReadException( E_PARSE_BAD_STRING,
		       p_sMessage,
		       p_sSourceFileName,
		       p_iSourceLineNum )
{
}

/**
 *  Construct an UnexpectedStringException as a copy of an existing one.
 *  \param p_Other IN The exception to copy.
 */
UnexpectedStringException::UnexpectedStringException( const UnexpectedStringException & p_Other )
  : FileReadException( p_Other )
{
}

/**
 *  Destruct an UnexpectedStringException.
 */
UnexpectedStringException::~UnexpectedStringException() throw ()
{
}
