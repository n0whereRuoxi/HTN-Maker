#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <cassert>
#include <fstream>
#include <set>
#include <tr1/memory>

#include <tclap/CmdLine.h>

#include "exception.hpp"
#include "funcs.hpp"
#include "term.hpp"
#include "term_string.hpp"
#include "term_constant.hpp"
#include "term_variable.hpp"
#include "type_table.hpp"
#include "substitution.hpp"
#include "formula.hpp"
#include "formula_pred.hpp"
#include "formula_equ.hpp"
#include "formula_neg.hpp"
#include "formula_conj.hpp"
#include "operator.hpp"
#include "state.hpp"
#include "htn_task_head.hpp"
#include "htn_method.hpp"
#include "htn_domain.hpp"
#include "htn_problem.hpp"
#include "htn_solution.hpp"

int main( int argc, char * argv[] )
{
#ifdef CATCH_EXCEPTS
  try{
#endif//CATCH_EXCEPTS

  std::string l_sDomainFile;
  try
  {
    TCLAP::CmdLine l_cCmd( "Add IDs to each method in an HTN domain.", ' ', "1.1" );

    TCLAP::UnlabeledValueArg<std::string> l_aDomainFile( "domain_file", "Path to the domain file.", true, "not_spec", "domain_file", l_cCmd );

    l_cCmd.parse( argc, argv );

    l_sDomainFile = l_aDomainFile.getValue();
  }
  catch( TCLAP::ArgException &e )
  {
    std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
    return 1;
  }

  std::tr1::shared_ptr< HtnDomain > l_pDomain;
  try
  {
    std::stringstream l_sDomainStream( ReadFile( l_sDomainFile ) );
    l_pDomain = std::tr1::shared_ptr< HtnDomain >( HtnDomain::FromPddl( l_sDomainStream ) );
  }
  catch( FileReadException & e )
  {
    e.SetFileName( l_sDomainFile );
    throw e;
  }

  unsigned int l_iIdNum = 100;
  char l_cStr[32];
  for( unsigned int i = 0; i < l_pDomain->GetNumMethods(); i++ )
  {
    snprintf( l_cStr, 32, "%d", l_iIdNum++ );
    l_pDomain->SetMethodId( i, l_cStr );
  }

  l_pDomain->AddRequirement( PDDL_REQ_METHOD_IDS );
  std::cout << l_pDomain->ToPddl();

  return 0;

#ifdef CATCH_EXCEPTS
  }catch( Exception & e ){ std::cerr << "\n" << e.ToStr() << "\n"; return 1; }
#endif//CATCH_EXCEPTS
}

