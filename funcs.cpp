#include <string>
#include <sstream>
#include <cassert>
#include <iostream>
#include <fstream>

#include "exception.hpp"
#include "funcs.hpp"

/** \file funcs.hpp
 *  Declarations of various useful global functions.
 */

/** \file funcs.cpp
 *  Definitions of various useful global functions.
 */

/** \struct StrLessNoCase
 *  A functor for determining, in a case-insensitive way, whether one string is
 *   stricly less than another in an alphanumeric ordering.
 */

/** \struct StrEquNoCase
 *  A functor for determining, in a case-insensitive way, whether two strings
 *   are identical.
 */

/** \struct HashStr
 *  A functor for hashing a string.
 */

/**
 *  An instance of the HashStr functor for hashing strings.
 */
HashStr g_StrHasher;

/**
 *  Perform a case-insensitive comparison between two string objects.
 *  \param p_sFirst IN The first string to compare.
 *  \param p_sSecond IN The other string to compare.
 *  \return -1 if first is greater, 0 if equal, 1 if second is greater.
 *
 *  This is the most efficient implementation I could find.
 */
int CompareNoCase( const std::string & p_sFirst, const std::string & p_sSecond )
{
  const char * l_sFirst = p_sFirst.c_str();
  const char * l_sSecond = p_sSecond.c_str();
  int l_iFirstSize = p_sFirst.size();
  int l_iSecondSize = p_sSecond.size();

  int i = 0;
  while( i < l_iFirstSize && i < l_iSecondSize )
  {
    if( toupper( l_sFirst[i] ) != toupper( l_sSecond[i] ) )
      return ( toupper( l_sFirst[i] ) < toupper( l_sSecond[i] ) ) ? -1 : 1;
    i++;
  }
  if( l_iFirstSize == l_iSecondSize )
    return 0;
  else if( l_iFirstSize < l_iSecondSize )
    return -1;
  else
    return 1;
}

/**
 *  The body of the StrLessNoCase functor.
 *  \param p_sFirst IN The first string to be compared.
 *  \param p_sSecond IN The second string to be compared.
 *  \return Whether or not p_sFirst is strictly less than p_sSecond, by a
 *   case-insensitive alphabetic ordering.
 */
bool StrLessNoCase::operator()( const std::string & p_sFirst, const std::string & p_sSecond ) const
{
  return ( CompareNoCase( p_sFirst, p_sSecond ) == -1 );
}

/**
 *  The body of the StrEquNoCase functor.
 *  \param p_sFirst IN The first string to be compared.
 *  \param p_sSecond IN The second string to be compared.
 *  \return Whether or not p_sFirst and p_sSecond are identical, ignoring case.
 */
bool StrEquNoCase::operator()( const std::string & p_sFirst, const std::string & p_sSecond ) const
{
  return ( CompareNoCase( p_sFirst, p_sSecond ) == 0 );
}

/**
 *  Remove leading whitespace characters and comments from a stringstream.
 *  Comments are single-line and start with a semicolon.
 *  \param p_sInput INOUT The stream to remove whitespace from.
 */
void EatWhitespace( std::stringstream & p_sInput ) throw ( StreamFailException )
{
  if( p_sInput.eof() )
    return;
  if( p_sInput.fail() )
    throw StreamFailException( "Unexpected stream failure.",
			       __FILE__,
			       __LINE__ );

  char l_cTemp = p_sInput.peek();
  while( p_sInput.good() && ( l_cTemp == ' ' || l_cTemp == '\n' ||
			      l_cTemp == '\r' || l_cTemp == '\t' ||
			      l_cTemp == ';' ) )
  {
    p_sInput.get( l_cTemp );

    // Remove single-line comments
    if( l_cTemp == ';' )
    {
      while( p_sInput.good() && p_sInput.peek() != '\n' )
	p_sInput.get( l_cTemp );
    }

    l_cTemp = p_sInput.peek();
  }

  if( p_sInput.fail() )
    throw StreamFailException( "Unexpected stream failure.",
			       __FILE__,
			       __LINE__ );
}

/**
 *  Remove a given string from the front of a stringstream.
 *  \param p_sInput INOUT The stream to remove from.
 *  \param p_sString IN The string to remove.
 */
void EatString( std::stringstream & p_sInput, std::string p_sString ) throw ( MissingStringException, StreamFailException )
{
  char l_cTemp;
  for( unsigned int i = 0; i < p_sString.length(); i++ )
  {
    p_sInput.get( l_cTemp );

    if( p_sInput.fail() )
    {
      std::string l_sMessage = "Unexpected stream failure, expecting \"";
      l_sMessage += p_sString;
      l_sMessage += "\".";
      throw StreamFailException( l_sMessage,
				 __FILE__,
				 __LINE__ );
    }

    if( p_sInput.eof() )
    {
      std::string l_sMessage = "Unexpected end of file, expecting \"";
      l_sMessage += p_sString;
      l_sMessage += "\".";
      throw MissingStringException( l_sMessage,
				    __FILE__,
				    __LINE__,
				    p_sString );
    }
    if( tolower( l_cTemp ) != tolower( p_sString[i] ) )
    {
      std::string l_sMessage = "Unexpected character '";
      l_sMessage += l_cTemp;
      l_sMessage += "', expecting \"";
      l_sMessage += p_sString;
      l_sMessage += "\".";
      throw MissingStringException( l_sMessage,
				    __FILE__,
				    __LINE__,
				    p_sString );
    }
  }
}

/**
 *  Read and the next string from a stringstream, deliminated by whitespace 
 *   and parentheses.
 *  \param p_sInput INOUT The stream to read from.  This is advanced.
 *  \return The complete first string in the stream.
 */
std::string ReadString( std::stringstream & p_sInput ) throw ( StreamFailException )
{
  std::string l_sRet = "";

  char l_cTemp;
  l_cTemp = p_sInput.peek();

  while( p_sInput.good()
	 && l_cTemp != '('
	 && l_cTemp != ')'
	 && l_cTemp != ' '
	 && l_cTemp != '\n'
	 && l_cTemp != '\t'
	 && l_cTemp != '\r' )
  {
    l_sRet += l_cTemp;
    p_sInput.get( l_cTemp );
    l_cTemp = p_sInput.peek();
  }
  if( p_sInput.fail() )
    throw StreamFailException( "Unexpected stream failure.",
			       __FILE__,
			       __LINE__ );
  return l_sRet;
}

/**
 *  Read a parenthetical phrase from a stringstream, throwing out any excess 
 *   whitespace and comments.
 *  \param p_sInput INOUT The stream to read from, which is advanced.
 *  \return A string containing the phrase.
 */
std::string ReadParenthetical( std::stringstream & p_sInput ) throw ( StreamFailException )
{
  std::string l_sRet = "";
  char l_cTemp;
  int l_iDepth = 0;

  EatWhitespace( p_sInput );
  EatString( p_sInput, "(" );
  l_sRet += "( ";
  l_iDepth++;
  EatWhitespace( p_sInput );

  while( p_sInput.good() && l_iDepth > 0 )
  {
    l_cTemp = p_sInput.peek();

    if( l_cTemp == '(' )
    {
      l_iDepth++;
      EatString( p_sInput, "(" );
      l_sRet += "(";
    }
    else if( l_cTemp == ')' )
    {
      l_iDepth--;
      EatString( p_sInput, ")" );
      l_sRet += ")";
    }
    else
    {
      l_sRet += ReadString( p_sInput );
      l_sRet += " ";
    }
    EatWhitespace( p_sInput );
  }
  if( p_sInput.fail() )
    throw StreamFailException( "Unexpected stream failure.",
			       __FILE__,
			       __LINE__ );
  
  return l_sRet;
}

/**
 *  Parse a PDDL requirements directive into a compact representation.
 *  The result is the bitwise disjunction of constants defined in funcs.hpp.
 *  \todo This should really not be making the determination about whether
 *   a requirement is legal or not.
 *  \param p_sInput IN A string containing the PDDl requirements directive.
 *  \param p_bAllowHtn IN Whether or not HTN-based requirements should be 
 *   allowed.
 *  \return A numerical representation of what requirements flags is set.
 */
long ParseRequirements( std::string p_sInput, bool p_bAllowHtn ) throw ( UnexpectedStringException )
{
  long l_iRet = 0;
  std::stringstream l_sFeatureStream( p_sInput );
  EatString( l_sFeatureStream, "(" );
  EatWhitespace( l_sFeatureStream );
  EatString( l_sFeatureStream, ":requirements" );
  EatWhitespace( l_sFeatureStream );
  while( l_sFeatureStream.peek() != ')' )
  {
    std::string l_sReq = ReadString( l_sFeatureStream );
    if( CompareNoCase( l_sReq, ":strips" ) == 0 )
    {
      l_iRet |= PDDL_REQ_STRIPS;
    }
    else if( CompareNoCase( l_sReq, ":typing" ) == 0 )
    {
      l_iRet |= PDDL_REQ_TYPING;
    }
    else if( CompareNoCase( l_sReq, ":negative-preconditions" ) == 0 )
    {
      l_iRet |= PDDL_REQ_NEGATIVE_PRECONDITIONS;
      throw Exception( E_NOT_IMPLEMENTED,
		       "The negative-preconditions requirement of PDDL is not supported.",
		       __FILE__,
		       __LINE__ );
    }
    else if( CompareNoCase( l_sReq, ":disjunctive-preconditions" ) == 0 )
    {
      l_iRet |= PDDL_REQ_DISJUNCTIVE_PRECONDITIONS;
      throw Exception( E_NOT_IMPLEMENTED,
		       "The disjunctive-preconditions requirement of PDDL is not supported.",
		       __FILE__,
		       __LINE__ );
    }
    else if( CompareNoCase( l_sReq, ":equality" ) == 0 )
    {
      l_iRet |= PDDL_REQ_EQUALITY;
    }
    else if( CompareNoCase( l_sReq, ":existential-preconditions" ) == 0 )
    {
      l_iRet |= PDDL_REQ_EXISTENTIAL_PRECONDITIONS;
      throw Exception( E_NOT_IMPLEMENTED,
		       "The existential-preconditions requirement of PDDL is not supported.",
		       __FILE__,
		       __LINE__ );
    }
    else if( CompareNoCase( l_sReq, ":universal-preconditions" ) == 0 )
    {
      l_iRet |= PDDL_REQ_UNIVERSAL_PRECONDITIONS;
      throw Exception( E_NOT_IMPLEMENTED,
		       "The universal-preconditions requirement of PDDL is not supported.",
		       __FILE__,
		       __LINE__ );
    }
    else if( CompareNoCase( l_sReq, ":quantified-preconditions" ) == 0 )
    {
      l_iRet |= PDDL_REQ_QUANTIFIED_PRECONDITIONS;
      throw Exception( E_NOT_IMPLEMENTED,
		       "The quantified-preconditions requirement of PDDL is not supported.",
		       __FILE__,
		       __LINE__ );
    }
    else if( CompareNoCase( l_sReq, ":conditional-effects" ) == 0 )
    {
      l_iRet |= PDDL_REQ_CONDITIONAL_EFFECTS;
      throw Exception( E_NOT_IMPLEMENTED,
		       "The conditional-effects requirement of PDDL is not supported.",
		       __FILE__,
		       __LINE__ );
    }
    else if( CompareNoCase( l_sReq, ":fluents" ) == 0 )
    {
      l_iRet |= PDDL_REQ_FLUENTS;
      throw Exception( E_NOT_IMPLEMENTED,
		       "The fluents requirement of PDDL is not supported.",
		       __FILE__,
		       __LINE__ );
    }
    else if( CompareNoCase( l_sReq, ":adl" ) == 0 )
    {
      l_iRet |= PDDL_REQ_ADL;
      throw Exception( E_NOT_IMPLEMENTED,
		       "The adl requirement of PDDL is not supported.",
		       __FILE__,
		       __LINE__ );
    }
    else if( CompareNoCase( l_sReq, ":durative-actions" ) == 0 )
    {
      l_iRet |= PDDL_REQ_DURATIVE_ACTIONS;
      throw Exception( E_NOT_IMPLEMENTED,
		       "The durative-actions requirement of PDDL is not supported.",
		       __FILE__,
		       __LINE__ );
    }
    else if( CompareNoCase( l_sReq, ":derived-predicates" ) == 0 )
    {
      l_iRet |= PDDL_REQ_DERIVED_PREDICATES;
      throw Exception( E_NOT_IMPLEMENTED,
		       "The derived-predicates requirement of PDDL is not supported.",
		       __FILE__,
		       __LINE__ );
    }
    else if( CompareNoCase( l_sReq, ":timed-initial-literals" ) == 0 )
    {
      l_iRet |= PDDL_REQ_TIMED_INITIAL_LITERALS;
      throw Exception( E_NOT_IMPLEMENTED,
		       "The timed-initial-literals requirement of PDDL is not supported.",
		       __FILE__,
		       __LINE__ );
    }
    else if( CompareNoCase( l_sReq, ":preferences" ) == 0 )
    {
      l_iRet |= PDDL_REQ_PREFERENCES;
      throw Exception( E_NOT_IMPLEMENTED,
		       "The preferences requirement of PDDL is not supported.",
		       __FILE__,
		       __LINE__ );
    }
    else if( CompareNoCase( l_sReq, ":constraints" ) == 0 )
    {
      l_iRet |= PDDL_REQ_CONSTRAINTS;
      throw Exception( E_NOT_IMPLEMENTED,
		       "The constraints requirement of PDDL is not supported.",
		       __FILE__,
		       __LINE__ );
    }
    else if( CompareNoCase( l_sReq, ":htn" ) == 0 )
    {
      l_iRet |= PDDL_REQ_HTN;
      if( p_bAllowHtn )
      {
	// This is expected.
      }
      else
      {
	throw Exception( E_NOT_IMPLEMENTED,
			 "The htn requirement of PDDL is not supported for STRIPS domains.",
			 __FILE__,
			 __LINE__ );
      }
    }
    else if( CompareNoCase( l_sReq, ":method-ids" ) == 0 )
    {
      l_iRet |= PDDL_REQ_METHOD_IDS;
      if( p_bAllowHtn )
      {
	// This is expected.
      }
      else
      {
	throw Exception( E_NOT_IMPLEMENTED,
			 "The method-ids requirement of PDDL is not supported for STRIPS domains.",
			 __FILE__,
			 __LINE__ );
      }
    }
    else if( CompareNoCase( l_sReq, ":q-values" ) == 0 )
    {
      l_iRet |= PDDL_REQ_QVALUES;
      if( p_bAllowHtn )
      {
	// This is expected.
      }
      else
      {
	throw Exception( E_NOT_IMPLEMENTED,
			 "The q-values requirement of PDDL is not supported for STRIPS domains.",
			 __FILE__,
			 __LINE__ );
      }
    }
    else
    {
      std::string l_sMessage;
      l_sMessage += "Unknown PDDL requirement ";
      l_sMessage += l_sReq;
      l_sMessage += " is not supported.";
      throw UnexpectedStringException( l_sMessage,
				       __FILE__,
				       __LINE__ );
		       
    }
    EatWhitespace( l_sFeatureStream );
  }
  return l_iRet;
}

/**
 *  Print a compact representation of a PDDL requirements directive to a 
 *   string.
 *  \param l_iReqs IN The internal representation of a PDDL requirements 
 *   directive.
 *  \return A string containing the PDDL representation.
 */
std::string PrintRequirements( long l_iReqs )
{
  std::string l_sRet = "( :requirements ";

  if( ( l_iReqs & PDDL_REQ_STRIPS ) 
      == PDDL_REQ_STRIPS )
    l_sRet += ":strips ";
  if( ( l_iReqs & PDDL_REQ_TYPING ) 
      == PDDL_REQ_TYPING )
    l_sRet += ":typing ";
  if( ( l_iReqs & PDDL_REQ_NEGATIVE_PRECONDITIONS ) 
      == PDDL_REQ_NEGATIVE_PRECONDITIONS )
    l_sRet += ":negative-preconditions ";
  if( ( l_iReqs & PDDL_REQ_DISJUNCTIVE_PRECONDITIONS )
      == PDDL_REQ_DISJUNCTIVE_PRECONDITIONS )
    l_sRet += ":disjunctive-preconditions ";
  if( ( l_iReqs & PDDL_REQ_EQUALITY )
      == PDDL_REQ_EQUALITY )
    l_sRet += ":equality ";
  if( ( l_iReqs & PDDL_REQ_EXISTENTIAL_PRECONDITIONS ) 
      == PDDL_REQ_EXISTENTIAL_PRECONDITIONS )
    l_sRet += ":existential-preconditions ";
  if( ( l_iReqs & PDDL_REQ_UNIVERSAL_PRECONDITIONS ) 
      == PDDL_REQ_UNIVERSAL_PRECONDITIONS )
    l_sRet += ":universal-preconditions ";
  if( ( l_iReqs & PDDL_REQ_CONDITIONAL_EFFECTS ) 
      == PDDL_REQ_CONDITIONAL_EFFECTS )
    l_sRet += ":conditional-effects ";
  if( ( l_iReqs & PDDL_REQ_FLUENTS ) 
      == PDDL_REQ_FLUENTS )
    l_sRet += ":fluents ";
  if( ( l_iReqs & PDDL_REQ_DURATIVE_ACTIONS ) 
      == PDDL_REQ_DURATIVE_ACTIONS )
    l_sRet += ":durative-actions ";
  if( ( l_iReqs & PDDL_REQ_DERIVED_PREDICATES ) 
      == PDDL_REQ_DERIVED_PREDICATES )
    l_sRet += ":derived-predicates ";
  if( ( l_iReqs & PDDL_REQ_TIMED_INITIAL_LITERALS ) 
      == PDDL_REQ_TIMED_INITIAL_LITERALS )
    l_sRet += ":timed-initial-literals ";
  if( ( l_iReqs & PDDL_REQ_PREFERENCES )
      == PDDL_REQ_PREFERENCES )
    l_sRet += ":preferences ";
  if( ( l_iReqs & PDDL_REQ_CONSTRAINTS ) 
      == PDDL_REQ_CONSTRAINTS )
    l_sRet += ":constraints ";
  if( ( l_iReqs & PDDL_REQ_HTN )
      == PDDL_REQ_HTN )
    l_sRet += ":htn ";
  if( ( l_iReqs & PDDL_REQ_METHOD_IDS )
      == PDDL_REQ_METHOD_IDS )
    l_sRet += ":method-ids ";
  if( ( l_iReqs & PDDL_REQ_QUANTIFIED_PRECONDITIONS )
      == PDDL_REQ_QUANTIFIED_PRECONDITIONS)
    l_sRet += ":quantified-preconditions ";
  if( ( l_iReqs & PDDL_REQ_ADL )
      == PDDL_REQ_ADL )
    l_sRet += ":adl ";
  if( ( l_iReqs & PDDL_REQ_QVALUES )
      == PDDL_REQ_QVALUES )
    l_sRet += ":q-values ";

  l_sRet += ")";

  return l_sRet;
}

/**
 *  Read the contents of a file into a string.
 *  \param p_sFileName IN The path to a file.
 *  \return The contents of that file.
 *  \todo This could probably be optimized by reading more than one char
 *   at a time.
 */
std::string ReadFile( std::string p_sFileName ) throw ( StreamFailException )
{
  std::ifstream fin( p_sFileName.c_str() );

  if( !fin.is_open() || !fin.good() )
    throw StreamFailException( "Reading file " + p_sFileName + " failed.",
			       __FILE__,
			       __LINE__ );

  std::string l_sRet = "";

  while( !fin.eof() )
    l_sRet += (char)fin.get();

  fin.close();
  return l_sRet;
}

/**
 *  The suffix for the next automatic variable to be created.
 */
unsigned int g_iNextVarIdSuffix = 0;

/**
 *  Set the suffix for the next automatic variable ID to be created to a 
 *   minimum threshhold.
 *  This is important because otherwise IDs may overlap, which will be a 
 *   problem if they have different types and are stored in the table.
 *  \param p_iNewMinimum IN The new minimum value of g_iNextVarIdSuffix.
 */
void IncreaseNextVarId( unsigned int p_iNewMinimum )
{
  if( g_iNextVarIdSuffix <= p_iNewMinimum )
    g_iNextVarIdSuffix = p_iNewMinimum + 1;
}

/**
 *  Create a new, unique variable name of the form "?auto_[number]".
 *  The [number] part is the current value of g_iNextVarIdSuffix, which is
 *   incremented.
 *  \return A string containing a new unique variable name.
 */
std::string MakeVarId()
{
  char l_cArray[32];
  sprintf( l_cArray, "?auto_%d", g_iNextVarIdSuffix++ );
  return l_cArray;
}

/**
 *  Create a new, unique variable of the form "?temp_[number]".
 *  These are used for temporary substitutions.
 *  \return A string containing a new unique variable name.
 */
std::string MakeTempOldId()
{
  static unsigned int x = 0;
  char l_cArray[32];
  sprintf( l_cArray, "?temp_old_%d", x++ );
  return l_cArray;
}

/**
 *  Create a new, unique variable of the form "?temp_[number]".
 *  These are used for temporary substitutions.
 *  \return A string containing a new unique variable name.
 */
std::string MakeTempNewId()
{
  static unsigned int x = 0;
  char l_cArray[32];
  sprintf( l_cArray, "?temp_new_%d", x++ );
  return l_cArray;
}

/**
 *  Create a new, unique variable of the form "?old_[number]".
 *  These are used for different temporary substitutions than those from
 *   MakeVarId().
 *  \return A string containing a new unique variable name.
 */
std::string MakeOldId()
{
  static unsigned int x = 0;
  char l_cArray[32];
  sprintf( l_cArray, "?old_%d", x++ );
  return l_cArray;
}

/**
 *  The body of the HashStr functor.
 *  \param x IN The string to be hashed.
 *  \return The hash value of the input string.
 */
size_t HashStr::operator() ( const std::string & x ) const
{
  size_t l_iHash = 5381;
  int c;
  const char * l_sFirst = x.c_str();

  while( ( c = *l_sFirst++ ) ) // Intentional assignment
    l_iHash = ( ( l_iHash << 5 ) + l_iHash ) + toupper( c );

  return l_iHash;
}
