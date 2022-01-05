#ifndef FUNCS_HPP__
#define FUNCS_HPP__

int CompareNoCase( const std::string & p_sFirst, const std::string & p_sSecond );

struct StrLessNoCase
{
  bool operator()( const std::string & p_sFirst, const std::string & p_sSecond ) const;
};

struct StrEquNoCase
{
  bool operator()( const std::string & p_sFirst, const std::string & p_sSecond ) const;
};

void EatWhitespace( std::stringstream & p_sInput ) throw ( StreamFailException );

void EatString( std::stringstream & p_sInput, std::string p_sString ) throw ( MissingStringException, StreamFailException );

std::string ReadString( std::stringstream & p_sInput ) throw ( StreamFailException );

std::string ReadParenthetical( std::stringstream & p_sInput ) throw ( StreamFailException );

#define PDDL_REQ_STRIPS                           0x00000001
#define PDDL_REQ_TYPING                           0x00000002
#define PDDL_REQ_NEGATIVE_PRECONDITIONS           0x00000004
#define PDDL_REQ_DISJUNCTIVE_PRECONDITIONS        0x00000008
#define PDDL_REQ_EQUALITY                         0x00000010
#define PDDL_REQ_EXISTENTIAL_PRECONDITIONS        0x00000020
#define PDDL_REQ_UNIVERSAL_PRECONDITIONS          0x00000040
#define PDDL_REQ_CONDITIONAL_EFFECTS              0x00000080
#define PDDL_REQ_FLUENTS                          0x00000100
#define PDDL_REQ_DURATIVE_ACTIONS                 0x00000200
#define PDDL_REQ_DERIVED_PREDICATES               0x00000400
#define PDDL_REQ_TIMED_INITIAL_LITERALS           0x00000800
#define PDDL_REQ_PREFERENCES                      0x00001000
#define PDDL_REQ_CONSTRAINTS                      0x00002000
#define PDDL_REQ_HTN                              0x00004000
#define PDDL_REQ_METHOD_IDS                       0x00008000
#define PDDL_REQ_QVALUES                          0x00010000

#define PDDL_REQ_QUANTIFIED_PRECONDITIONS \
  ( PDDL_REQ_EXISTENTIAL_PRECONDITIONS | PDDL_REQ_UNIVERSAL_PRECONDITIONS )
#define PDDL_REQ_ADL \
  ( PDDL_REQ_STRIPS | PDDL_REQ_TYPING | PDDL_REQ_NEGATIVE_PRECONDITIONS | \
    PDDL_REQ_DISJUNCTIVE_PRECONDITIONS | PDDL_REQ_EQUALITY | \
    PDDL_REQ_EXISTENTIAL_PRECONDITIONS | PDDL_REQ_UNIVERSAL_PRECONDITIONS | \
    PDDL_REQ_CONDITIONAL_EFFECTS )

long ParseRequirements( std::string p_sInput, bool p_bAllowHtn ) throw ( UnexpectedStringException );

std::string PrintRequirements( long l_iReqs );

std::string ReadFile( std::string p_sFileName ) throw ( StreamFailException );

void IncreaseNextVarId( unsigned int p_iNewMinimum );
std::string MakeVarId();
std::string MakeTempOldId();
std::string MakeTempNewId();
std::string MakeOldId();

struct HashStr
{
  size_t operator() ( const std::string & x ) const;
};


#endif//FUNCS_HPP__
