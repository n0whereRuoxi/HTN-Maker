#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <cassert>
#include <iostream>
#include <set>
#include <tr1/memory>
#include <cstdlib>

#include "exception.hpp"
#include "funcs.hpp"
#include "string_table.hpp"
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
#include "strips_domain.hpp"
#include "strips_problem.hpp"
#include "strips_solution.hpp"
#include "htn_task_head.hpp"
#include "htn_task_descr.hpp"
#include "htn_method.hpp"
#include "htn_domain.hpp"
#include "htn_task_list.hpp"
#include "annotated_plan.hpp"
#include "partial_htn_method.hpp"
#include "htn_problem.hpp"
#include "htn_solution.hpp"
#include "test_funcs.hpp"

int main( int argc, char * argv[] )
{
#ifdef CATCH_EXCEPTS
  try{
#endif//CATCH_EXCEPTS

  if( argc < 2 )
  {
    std::cout << "\nProvide a list of numbers of test to run:";
    std::cout << "\n\t1\tTerm Creation";
    std::cout << "\n\t2\tFormula Creation";
    std::cout << "\n\t3\tSubstititution Creation";
    std::cout << "\n\t4\tTerm After Substitution";
    std::cout << "\n\t5\tFormula After Substitution";
    std::cout << "\n\t6\tFormula Implication";
    std::cout << "\n\t7\tOperator Creation";
    std::cout << "\n\t8\tState Creation";
    std::cout << "\n\t9\tState Consistency";
    std::cout << "\n\t10\tState Get Instantiations";
    std::cout << "\n\t11\tNext State";
    std::cout << "\n\t12\tRead Domain File";
    std::cout << "\n\t13\tRead Problem File";
    std::cout << "\n\t14\tStrips Solution";
    std::cout << "\n\t15\tHtn Task Head";
    std::cout << "\n\t16\tHtn Task Descr";
    std::cout << "\n\t17\tHtn Task List";
    std::cout << "\n\t18\tHtn Method";
    std::cout << "\n\t19\tAnnotated Plan";
    std::cout << "\n\t20\tHtn Domain";
    std::cout << "\n\t21\tPartial Htn Method";
    std::cout << "\n\t22\tHtn Solution";
    std::cout << "\n\t23\tString Table";
    std::cout << "\n\t24\tSubtasksAreLinked";
    std::cout << "\n\t25\tTyping";
    std::cout << "\n\n";
    return 0;
  }

  //  try
  //  {

  for( int i = 1; i < argc; i++ )
  {
    switch( atoi( argv[i] ) )
    {
    case 1:
      TestTermCreation();
      break;
    case 2:
      TestFormulaCreation();
      break;
    case 3:
      TestSubstitutionCreation();
      break;
    case 4:
      TestTermAfterSubstitution();
      break;
    case 5:
      TestFormulaAfterSubstitution();
      break;
    case 6:
      TestFormulaImplication();
      break;
    case 7:
      TestOperatorCreation();
      break;
    case 8:
      TestStateCreation();
      break;
    case 9:
      TestStateConsistency();
      break;
    case 10:
      TestStateGetInstantiations();
      break;
    case 11:
      TestNextState();
      break;
    case 12:
      TestReadSTRIPSDomainFile();
      break;
    case 13:
      TestReadSTRIPSProblemFile();
      break;
    case 14:
      TestStripsSolution();
      break;
    case 15:
      TestHtnTaskHead();
      break;
    case 16:
      TestHtnTaskDescr();
      break;
    case 17:
      TestHtnTaskList();
      break;
    case 18:
      TestHtnMethod();
      break;
    case 19:
      TestAnnotatedPlan();
      break;
    case 20:
      TestHtnDomain();
      break;
    case 21:
      TestPartialHtnMethod();
      break;
    case 22:
      TestHtnSolution();
      break;
    case 23:
      TestStringTable();
      break;
    case 24:
      TestSubtasksAreLinked();
      break;
    case 25:
      TestTyping();
      break;
    default:
      std::cout << "\n\t Test " << argv[i] << " unknown.";
      break;
    }
  }

  //  }
  //  catch( Exception e )
  //  {
  //    std::cerr << e.ToStr();
  //    throw e;
  //  }

  std::cout << "\n";

#ifdef CATCH_EXCEPTS
  }catch( Exception e ){ std::cerr << "\n" << e.ToStr() << "\n"; return 1; }
#endif//CATCH_EXCEPTS

  return 0;
}
