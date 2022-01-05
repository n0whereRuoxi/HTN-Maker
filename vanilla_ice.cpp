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
    std::cerr << "vanilla_ice <domain-file> <problem-file> <log-level>\n";
    std::cerr << "Valid log levels:\n";
    std::cerr << "\t0 Only print plan\n";
    std::cerr << "\t1 Print number of nodes at each depth\n";
    std::cerr << "\t2 Print each state and action\n";
    return 1;
  }

#ifdef CATCH_EXCEPTS
  try{
#endif//CATCH_EXCEPTS

  int l_iLogLevel = atoi( argv[3] );

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

  if( l_pProblem->GetCInitState()->IsConsistent( l_pProblem->GetCGoals() ) )
  {
    std::cout << "\nGoals are initially satisfied; empty plan is correct.\n";
    return 0;
  }


  std::vector< StripsSolution * > l_vQueue;
  l_vQueue.push_back( new StripsSolution( l_pProblem ) );

  unsigned int l_iCurDepth = 0;
  unsigned int l_iCount = 0;

  Substitution l_EmptySub;


  while( !l_vQueue.empty() )
  {
    StripsSolution * l_pCurrentSol = l_vQueue[0];
    l_vQueue.erase( l_vQueue.begin() );

    if( l_pCurrentSol->GetPlanLength() > l_iCurDepth && l_iLogLevel >= 1 )
    {
      std::cout << "\nProcessed "
		<< "all extensions of "
		<< l_iCount
		<< " "
		<< l_iCurDepth
		<< "-length plans without success ...";
      std::cout.flush();
      assert( l_pCurrentSol->GetPlanLength() == l_iCurDepth + 1 );
      l_iCurDepth++;
      l_iCount = 0;
    }

    l_iCount++;

    for( unsigned int i = 0; i < l_pDomain->GetNumOpers(); i++ )
    {
      std::vector<Substitution *> * l_pSubs = 0;
      if( l_pCurrentSol->GetPlanLength() == 0 )
	l_pSubs = l_pCurrentSol->GetCProblem()->GetCInitState()->GetInstantiations( l_pDomain->GetCOper( i ), &l_EmptySub );
      else
	l_pSubs = l_pCurrentSol->GetCFollowState( l_pCurrentSol->GetPlanLength() - 1 )->GetInstantiations( l_pDomain->GetCOper( i ), &l_EmptySub );

      while( !l_pSubs->empty() )
      {
	StripsSolution * l_pNewSol = new StripsSolution( *l_pCurrentSol );
	bool l_bIsRepeat = l_pNewSol->ApplyOperator( i, l_pSubs->at( 0 ) );
	l_pSubs->erase( l_pSubs->begin() );

	if( l_iLogLevel >= 2 )
	{
	  std::cout << "\n\n*************************************\n";
	  std::cout << "Extending the following partial plan:\n\n";
	  std::cout << "Initial State:\n";
	  std::cout << l_pCurrentSol->GetCProblem()->GetCInitState()->ToStr();

	  for( unsigned int k = 0; k < l_pCurrentSol->GetPlanLength(); k++ )
	  {
	    std::cout << "\nAction " << k << ":\t( "
		      << l_pCurrentSol->GetCOperator( k )->GetName();
	    for( unsigned int j = 0; j < l_pCurrentSol->GetCOperator( k )->GetNumParams(); j++ )
	    {
	      std::cout << " " << l_pCurrentSol->GetCOperator( k )->GetCParam( j )->AfterSubstitution( *l_pCurrentSol->GetCSubstitution( k ), 0 )->ToStr();
	    }
	    std::cout << " )\n";

	    std::cout << "\nState " << k << "\n";
	    std::cout << l_pCurrentSol->GetCFollowState( k )->ToStr();
	    std::cout << "\n";
	  }

	  std::cout << "\nNew Action:\t( "
		    << l_pNewSol->GetCOperator( l_pNewSol->GetPlanLength() - 1 )->GetName();
	  for( unsigned int j = 0; j < l_pNewSol->GetCOperator( l_pNewSol->GetPlanLength() - 1 )->GetNumParams(); j++ )
	  {
	    std::cout << " " << l_pNewSol->GetCOperator( l_pNewSol->GetPlanLength() - 1 )->GetCParam( j )->AfterSubstitution( *l_pNewSol->GetCSubstitution( l_pNewSol->GetPlanLength() - 1 ), 0 )->ToStr();
	  }

	  std::cout << " )\n";

	  std::cout << "\nResulting State:\n";
	  std::cout << l_pNewSol->GetCFollowState( l_pNewSol->GetPlanLength() - 1 )->ToStr();
	  std::cout << "\n";

	  if( l_bIsRepeat )
	  {
	    std::cout << "\nThis branch loops and thus will be terminated.\n";
	  }
	}

	if( l_pNewSol->GetCFollowState( l_pNewSol->GetPlanLength() - 1 )->IsConsistent( l_pProblem->GetCGoals() ) )
	{
	  std::cout << "\nPlan found!\n\n";
	  
	  for( unsigned int j = 0; j < l_pNewSol->GetPlanLength(); j++ )
	  {
	    std::cout << "\t( " << l_pNewSol->GetCOperator( j )->GetName() << " ";

	    for( unsigned int k = 0; k < l_pNewSol->GetCOperator( j )->GetNumParams(); k++ )
	    {
	      std::cout << l_pNewSol->GetCOperator( j )->GetCParam( k )->AfterSubstitution( *l_pNewSol->GetCSubstitution( j ), 0 )->ToStr() << " ";
	    }

	    std::cout << ")\n";
	  }
	  return 0;
	}

	if( l_bIsRepeat )
	  delete l_pNewSol;
	else
	  l_vQueue.push_back( l_pNewSol );
      }

      delete l_pSubs;
    }
    delete l_pCurrentSol;
  }

  std::cout << "\nNo plans found.\n";

  return 0;

#ifdef CATCH_EXCEPTS
  }catch( Exception & e ){ std::cout << "\n" << e.ToStr() << "\n"; return 1; }
#endif//CATCH_EXCEPTS
}
