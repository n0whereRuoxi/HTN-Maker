#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <cassert>
#include <fstream>
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
#include "formula_equ.hpp"
#include "formula_neg.hpp"
#include "formula_conj.hpp"
#include "operator.hpp"
#include "strips_domain.hpp"
#include "state.hpp"
#include "strips_problem.hpp"
#include "strips_solution.hpp"


int main( int argc, char * argv[] )
{
  if( argc != 4 || atoi( argv[3] ) < 0 || atoi( argv[3] ) > 2  )
  {
    std::cerr << "\nUsage: \n";
    std::cerr << "verifier_strips <domain-file> <problem-file> <solution-file>\n";
    return 1;
  }

  try{

    std::tr1::shared_ptr< StripsDomain > l_pDomain;
    try
    {
      l_pDomain = std::tr1::shared_ptr< StripsDomain >( new StripsDomain( ReadFile( argv[1] ) ) );
    }
    catch( FileReadException & e )
    {
      e.SetFileName( argv[1] );
      throw e;
    }

    std::tr1::shared_ptr< StripsProblem > l_pProblem;
    try
    {
      l_pProblem = std::tr1::shared_ptr< StripsProblem >( new StripsProblem( ReadFile( argv[2] ),
									     l_pDomain ) );
    }
    catch( FileReadException & e )
    {
      e.SetFileName( argv[2] );
      throw e;
    }

    StripsSolution * l_pSolution = NULL;
    try
    {
      l_pSolution = new StripsSolution( l_pProblem,
					ReadFile( argv[3] ) );
    }
    catch( FileReadException & e )
    {
      e.SetFileName( argv[3] );
      throw e;
    }

    State * l_pCurState = new State( *l_pProblem->GetCInitState() );
    for( unsigned int i = 0; i < l_pSolution->GetPlanLength(); i++ )
    {
      FormulaP l_pPrecs( l_pSolution->GetCOperator( i )->GetCPreconditions()->AfterSubstitution( *l_pSolution->GetCSubstitution( i ), 0 ) );
      if( !l_pCurState->IsConsistent( l_pPrecs ) )
      {
	std::cout << "FAILURE: Invalid action #" << i << ".\n";
	delete l_pCurState;
	delete l_pSolution;
	exit( 2 );
      }

      State * l_pNewState = l_pCurState->NextState( l_pSolution->GetCOperator( i ), l_pSolution->GetCSubstitution( i ) );
      delete l_pCurState;
      l_pCurState = l_pNewState;
    }

    if( !l_pCurState->IsConsistent( l_pProblem->GetCGoals() ) )
    {
      std::cout << "FAILURE: Does not achieve goals.\n";
      delete l_pCurState;
      delete l_pSolution;
      exit( 3 );
    }

    std::cout << "SUCCESS\n";
    delete l_pCurState;
    delete l_pSolution;

    return 0;

  }
  catch( Exception & e )
  {
    if( e.GetType() == E_OPER_NOT_APPLICABLE )
    {
      std::cout << "FAILURE: Invalid action.\n";
      exit( 2 );
    }
    std::cout << "\n" << e.ToStr() << "\n";
    return 1;
  }
}
