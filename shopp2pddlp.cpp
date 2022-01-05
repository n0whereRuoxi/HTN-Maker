#include <iostream>
#include <vector>
#include <sstream>
#include <set>
#include <tr1/memory>

#include "exception.hpp"
#include "funcs.hpp"
#include "term.hpp"
#include "term_string.hpp"
#include "term_variable.hpp"
#include "term_constant.hpp"
#include "type_table.hpp"
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

int main( int argc, char * argv[] )
{
  if( argc != 3 )
  {
    std::cerr << "\nUsage: \n";
    std::cerr << "shopd2pddld <pddl-domain-file> <shop-problem-file>\n";
    return 1;
  }

  std::stringstream l_sDomainStream( ReadFile( argv[1] ) );
  std::tr1::shared_ptr< HtnDomain > l_pDomain( HtnDomain::FromPddl( l_sDomainStream ) );
  std::stringstream l_sProblemStream( ReadFile( argv[2] ) );
  HtnProblem * l_pProblem = HtnProblem::FromShop( l_pDomain, l_sProblemStream );
  std::cout << l_pProblem->ToPddl();
}
