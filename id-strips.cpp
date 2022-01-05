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

void Search( std::tr1::shared_ptr< StripsProblem > p_pProblem,
	     const State * p_pCurState,
	     std::vector< unsigned int > & p_vOperatorIndices,
	     std::vector< Substitution * > & p_vSubstitutions,
	     unsigned int p_iCurDepth,
	     unsigned int p_iMaxDepth )
{
  if( p_iCurDepth >= p_iMaxDepth )
  {
    std::cerr << "Hey, this should never happen!\n";
    return;
  }

  for( unsigned int i = 0; i < p_pProblem->GetCDomain()->GetNumOpers(); i++ )
  {
    Substitution l_EmptySub;
    std::vector<Substitution *> * l_pSubs = p_pCurState->GetInstantiations( p_pProblem->GetCDomain()->GetCOper( i ), &l_EmptySub );

    for( unsigned int j = 0; j < l_pSubs->size(); j++ )
    {
      State * l_pNewState = p_pCurState->NextState( p_pProblem->GetCDomain()->GetCOper( i ), l_pSubs->at( j ) );
      p_vOperatorIndices.push_back( i );
      p_vSubstitutions.push_back( l_pSubs->at( j ) );

      if( l_pNewState->IsConsistent( p_pProblem->GetCGoals() ) )
      {
	std::cout << "\nPlan found at depth " << p_iCurDepth + 1 << ".\n";

	for( unsigned int k = 0; k < p_vOperatorIndices.size(); k++ )
	{
	  std::cout << "\t( " << p_pProblem->GetCDomain()->GetCOper( p_vOperatorIndices[k] )->GetName() << " ";

	  for( unsigned int l = 0; l < p_pProblem->GetCDomain()->GetCOper( p_vOperatorIndices[k] )->GetNumParams(); l++ )
	  {
	    std::cout << p_pProblem->GetCDomain()->GetCOper( p_vOperatorIndices[k] )->GetCParam( l )->AfterSubstitution( *p_vSubstitutions[k], 0 )->ToStr() << " ";
	  }

	  std::cout << ")\n";
	}
	exit( 0 );
      }

      if( p_iCurDepth < p_iMaxDepth - 1 )
	Search( p_pProblem,
		l_pNewState,
		p_vOperatorIndices,
		p_vSubstitutions,
		p_iCurDepth + 1, 
		p_iMaxDepth );

      p_vOperatorIndices.pop_back();
      p_vSubstitutions.pop_back();
      delete l_pSubs->at( j );
      delete l_pNewState;
    }

    delete l_pSubs;
  }
}

int main( int argc, char * argv[] )
{
  if( argc != 3 )
  {
    std::cerr << "\nUsage: \n";
    std::cerr << "id_strips <domain-file> <problem-file>\n";
    return 1;
  }

#ifdef CATCH_EXCEPTS
  try{
#endif//CATCH_EXCEPTS

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
    std::cout << "\nPlan found at depth 0.\n";
    return 0;
  }

  std::vector< unsigned int > l_vOperatorIndices;
  std::vector< Substitution * > l_vSubstitutions;
  for( unsigned int i = 1; i < 100; i++ )
  {
    Search( l_pProblem,
	    l_pProblem->GetCInitState(),
	    l_vOperatorIndices,
	    l_vSubstitutions,
	    0,
	    i );
    std::cout << "Failed at depth " << i << "." << std::endl;
  }
  std::cout << "Gave up after trying depth 100.\n";
  return 1;

#ifdef CATCH_EXCEPTS
  }catch( Exception & e ){ std::cout << "\n" << e.ToStr() << "\n"; return 1; }
#endif//CATCH_EXCEPTS
}
