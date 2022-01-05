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
#include "formula_conj.hpp"
#include "operator.hpp"
#include "htn_task_head.hpp"
#include "htn_method.hpp"
#include "htn_domain.hpp"

int main( int argc, char * argv[] )
{
  if( argc != 2 )
  {
    std::cerr << "\nUsage: \n";
    std::cerr << "shopd2pddld <shop-domain-file>\n";
    return 1;
  }

  std::stringstream l_sFileStream( ReadFile( argv[1] ) );
  HtnDomain * l_pDomain = HtnDomain::FromShop( l_sFileStream );
  std::cout << l_pDomain->ToPddl();
}
