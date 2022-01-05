#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <set>
#include <tr1/memory>
#include <cstdlib>

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
#include "formula_neg.hpp"
#include "formula_conj.hpp"
#include "operator.hpp"
#include "htn_task_head.hpp"
#include "htn_method.hpp"
#include "htn_domain.hpp"

int main( int argc, char * argv[] )
{
  if( argc != 3 )
  {
    std::cout << "Usage: htndiff file1 file2\n";
    exit( 1 );
  }

  try {

  HtnDomain * l_pHtnDomain1 = 0, * l_pHtnDomain2 = 0;

  try
  {
    std::stringstream l_sDomainStream( ReadFile( argv[1] ) );
    l_pHtnDomain1 = HtnDomain::FromPddl( l_sDomainStream );
  }
  catch( FileReadException & e )
  {
    e.SetFileName( argv[1] );
    throw e;
  }

  try
  {
    std::stringstream l_sDomainStream( ReadFile( argv[2] ) );
    l_pHtnDomain2 = HtnDomain::FromPddl( l_sDomainStream );
  }
  catch( FileReadException & e )
  {
    e.SetFileName( argv[2] );
    throw e;
  }

  if( l_pHtnDomain1->Equivalent( l_pHtnDomain2 ) )
    std::cout << "true\n";
  else
    std::cout << "false\n";

  delete l_pHtnDomain2;
  delete l_pHtnDomain1;

  }catch( Exception & e )
  {
    std::cerr << e.ToStr();
    throw e;
  }

  return 0;
}
