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

void FindDecomps( std::vector< int > & p_vMethodIndices,
		  std::vector< std::vector< Substitution * > * > & p_vSubs,
		  const std::tr1::shared_ptr< HtnDomain > & p_pDomain,
		  const HtnSolution * p_pCurSol,
		  bool p_bEarlyStop );

void ReorderDecomps( std::vector< int > & p_vMethodIndices,
		     std::vector< std::vector< Substitution * > * > & p_vSubs,
		     const std::tr1::shared_ptr< HtnDomain > & p_pDomain,
		     bool p_bRandomOrder );

int main( int argc, char * argv[] )
{
#ifdef CATCH_EXCEPTS
  try{
#endif//CATCH_EXCEPTS

  std::string l_sDomainFile;
  std::string l_sProblemFile;
  int l_iMaxDecomps;
  bool l_bRandomOrder;
  bool l_bLoopDetection;
  bool l_bBreadthFirst;
  bool l_bEarlyStop;
  int l_iLogLevel;
  int l_iLDKeepLevel;
  try
  {
    TCLAP::CmdLine l_cCmd( "Find an HTN plan", ' ', "1.1" );

    TCLAP::UnlabeledValueArg<std::string> l_aDomainFile( "domain_file", "Path to the domain file.", true, "not_spec", "domain_file", l_cCmd );
    TCLAP::UnlabeledValueArg<std::string> l_aProblemFile( "problem_file", "Path to the problem file.", true, "not_spec", "problem_file", l_cCmd );
    TCLAP::ValueArg<int> l_aMaxDecomps( "m", "max_decomps", "Maximum depth of decompositions considered, -1 for unlimited.", false, -1, "int", l_cCmd );
    TCLAP::SwitchArg l_aRandomOrder( "r", "random_order", "Try methods in random order.  (Otherwise, from fewest precs to highest.)", l_cCmd, false );
    TCLAP::SwitchArg l_aLoopDetection( "l", "loop_detection", "Compare each new partial solution to all previous ones and save only if unique.", l_cCmd, false );
    TCLAP::SwitchArg l_aBreadthFirst( "b", "breadth_first", "Perform breadth-first, rather that depth-first, search.", l_cCmd, false );
    TCLAP::SwitchArg l_aEarlyStop( "e", "early_stop", "Use the first method found, without seeking others.", l_cCmd, false );
    TCLAP::ValueArg<int> l_aLogLevel( "d", "debug_level", "Level of debugging information to print (0-10).", false, 0, "int", l_cCmd );
    TCLAP::ValueArg<int> l_aLDKeepLevel( "k", "keep_level", "Percentage of max decomps below which nodes should be saved for loop detection (0-100).", false, 100, "int", l_cCmd );

    l_cCmd.parse( argc, argv );

    l_sDomainFile = l_aDomainFile.getValue();
    l_sProblemFile = l_aProblemFile.getValue();
    l_iMaxDecomps = l_aMaxDecomps.getValue();
    l_bRandomOrder = l_aRandomOrder.getValue();
    l_bLoopDetection = l_aLoopDetection.getValue();
    l_bBreadthFirst = l_aBreadthFirst.getValue();
    l_bEarlyStop = l_aEarlyStop.getValue();
    l_iLogLevel = l_aLogLevel.getValue();
    l_iLDKeepLevel = l_aLDKeepLevel.getValue();

    if( l_iLogLevel < 0 || l_iLogLevel > 10 )
    {
      std::cerr << "error: debug level must be in range (0-10)\n";
      return 1;
    }
    if( l_iLDKeepLevel < 0 || l_iLDKeepLevel > 100 )
    {
      std::cerr << "error: keep level must be in range (0-100)\n";
      return 1;
    }
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

  HtnSolution * l_pProblem = NULL;
  try
  {
    std::stringstream l_sProblemStream( ReadFile( l_sProblemFile ) );
    l_pProblem = HtnSolution::FromPddl( l_pDomain,
					l_sProblemStream );
  }
  catch( FileReadException & e )
  {
    e.SetFileName( l_sDomainFile );
    throw e;
  }

  if( l_pProblem->IsComplete() )
  {
    std::cout << "\nPlan found!\nNo tasks to complete.\n";
  }

  std::vector< HtnSolution * > l_vQueue;
  l_vQueue.push_back( l_pProblem );
  std::vector< HtnSolution * > l_vDead;

  Substitution l_EmptySub;

  unsigned int l_iNumGenerated = 0;
  unsigned int l_iNumDiscarded = 0;
  unsigned int l_iNumBacktracked = 0;
  unsigned int l_iNumFailed = 0;

  while( !l_vQueue.empty() )
  {
    HtnSolution * l_pCurrentSol;
    if( l_bBreadthFirst )
    {
      l_pCurrentSol = l_vQueue[0];
      l_vQueue.erase( l_vQueue.begin() );
    }
    else
    {
      l_pCurrentSol = l_vQueue.back();
      l_vQueue.pop_back();
    }
    bool l_bFoundDecomp = false;

    int l_iExtensions = 0;
    clock_t l_StartClock = 0;
    clock_t l_SeekStartClock = 0;
    clock_t l_SeekEndClock = 0;
    if( l_iLogLevel > 5 )
    {
      std::cout << "Seeking extensions of the following plan:\n";
      std::cout << l_pCurrentSol->ToStr();
      l_StartClock = clock();
    }

    if( l_pCurrentSol->GetCTopTask()->GetName()[0] == '!' )
    {
      int l_iOperIndex = -1;
      HtnTaskHeadP l_pTask( l_pCurrentSol->GetCTopTask() );

      if( l_iLogLevel > 5 )
      {
	l_SeekStartClock = clock();
      }
      for( unsigned int j = 0; j < l_pDomain->GetNumOperators() && l_iOperIndex == -1; j++ )
      {
	if( CompareNoCase( l_pDomain->GetCOperator( j )->GetName(),
			   l_pTask->GetName() ) == 0 )
	  l_iOperIndex = j;
      }
      if( l_iLogLevel > 5 )
      {
	l_SeekEndClock = clock();
      }

      if( l_iOperIndex != -1 )
      {
	Substitution l_OperSubs;
	for( unsigned int j = 0; j < l_pTask->GetNumParams(); j++ )
	  l_OperSubs.AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( l_pDomain->GetCOperator( l_iOperIndex )->GetCParam( j ) ), l_pTask->GetCParam( j ) );
	std::vector< Substitution * > * l_pAllOperSubs = l_pCurrentSol->GetCState()->GetInstantiations( l_pDomain->GetCOperator( l_iOperIndex ), &l_OperSubs );

	for( unsigned int k = 0; k < l_pAllOperSubs->size(); k++ )
	{
	  HtnSolution * l_pNewSolution = new HtnSolution( *l_pCurrentSol );
	  l_pNewSolution->ApplyOperator( l_iOperIndex, l_pAllOperSubs->at( k ) );
	  delete l_pAllOperSubs->at( k );

	  if( l_pNewSolution->IsComplete() )
	  {
	    std::cout << "\nPlan found!\n";
	    std::cout << l_pNewSolution->Print( false );

	    if( l_iLogLevel > 0 )
	    {
	      std::cout << "\nNodes Generated: " << l_iNumGenerated;
	      std::cout << "\nNodes Discarded: " << l_iNumDiscarded;
	      std::cout << "\nBacktrack Points: " << l_iNumBacktracked;
	      std::cout << "\nFailure Points: " << l_iNumFailed;
	      std::cout << "\n";
	    }

	    delete l_pCurrentSol;

	    for( unsigned int l = k + 1; l < l_pAllOperSubs->size(); l++ )
	      delete l_pAllOperSubs->at( l );
	    delete l_pAllOperSubs;

	    while( !l_vQueue.empty() )
	    {
	      delete l_vQueue.back();
	      l_vQueue.pop_back();
	    }
	    delete l_pNewSolution;
	    return 0;
	  }
	  if( l_iMaxDecomps < 0 || l_pNewSolution->GetNumDecomps() < l_iMaxDecomps )
	  {
	    bool l_bDuplicate = false;
	    if( l_bLoopDetection )
	    {
	      for( unsigned int q = 0; q < l_vQueue.size() && !l_bDuplicate; q++ )
	      {
		if( l_vQueue[q]->Equivalent( *l_pNewSolution ) &&
		    l_vQueue[q]->GetNumDecomps() <= 
		    l_pNewSolution->GetNumDecomps() )
		  l_bDuplicate = true;
	      }
	      for( unsigned int q = 0; q < l_vDead.size() && !l_bDuplicate; q++ )
	      {
		if( l_vDead[q]->Equivalent( *l_pNewSolution ) &&
		    l_vDead[q]->GetNumDecomps() <=
		    l_pNewSolution->GetNumDecomps() )
		  l_bDuplicate = true;
	      }
	    }
	    if( l_bDuplicate )
	    {
	      delete l_pNewSolution;
	      l_iNumDiscarded++;
	    }
	    else
	    {
	      l_vQueue.push_back( l_pNewSolution );
	      l_bFoundDecomp = true;
	      l_iExtensions++;
	    }
	  }
	  else
	    delete l_pNewSolution;
	}
	delete l_pAllOperSubs;
      }
      else
      {
	l_iNumBacktracked++;
      }
    }
    else
    {
      std::vector< int > l_vMethodIndices;
      std::vector< std::vector< Substitution * > * > l_vMethodSubs;

      if( l_iLogLevel > 5 )
      {
	l_SeekStartClock = clock();
      }
      FindDecomps( l_vMethodIndices,
		   l_vMethodSubs,
		   l_pDomain,
		   l_pCurrentSol,
		   l_bEarlyStop);

      if( l_iLogLevel > 5 )
      {
	l_SeekEndClock = clock();
      }

      ReorderDecomps( l_vMethodIndices,
		      l_vMethodSubs,
		      l_pDomain,
		      l_bRandomOrder );

      if( l_vMethodIndices.size() == 0 )
      {
	l_iNumBacktracked++;
	std::string l_sMethodName = l_pCurrentSol->GetCTopTask()->GetName();
	if( l_sMethodName.find( "-verify", 0 ) != std::string::npos ) 
	  l_iNumFailed++;
      }

      for( unsigned int l_iCurMethod = 0;
	   l_iCurMethod < l_vMethodIndices.size();
	   l_iCurMethod++ )
      {
	std::vector< Substitution * > * l_pInstances = l_vMethodSubs[l_iCurMethod];
	bool l_bFirstInst = true;
	unsigned int l_iRandInst = rand() % l_pInstances->size();
	for( unsigned int l_iCurInst = l_iRandInst;
	     l_bFirstInst || l_iCurInst != l_iRandInst;
	     l_iCurInst = ( l_iCurInst + 1 ) % l_pInstances->size() )
	{
	  l_bFirstInst = false;

	  l_iNumGenerated++;
	  HtnSolution * l_pNewSolution = new HtnSolution( *l_pCurrentSol );
	  l_pNewSolution->ApplyMethod( l_vMethodIndices[ l_iCurMethod ], l_pInstances->at( l_iCurInst ) );
	  if( l_pNewSolution->IsComplete() )
	  {
	    std::cout << "\nPlan found!\n";
	    std::cout << l_pNewSolution->Print( false );

	    if( l_iLogLevel > 0 )
	    {
	      std::cout << "\nNodes Generated: " << l_iNumGenerated;
	      std::cout << "\nNodes Discarded: " << l_iNumDiscarded;
	      std::cout << "\nBacktrack Points: " << l_iNumBacktracked;
	      std::cout << "\nFailure Points: " << l_iNumFailed;
	      std::cout << "\n";
	    }

	    delete l_pCurrentSol;
	    while( !l_vQueue.empty() )
	    {
	      delete l_vQueue.back();
	      l_vQueue.pop_back();
	    }
	    for( unsigned int del = l_iCurInst; del < l_pInstances->size(); del++ )
	      delete l_pInstances->at( del );
	    for( unsigned int del = l_iCurMethod + 1; del < l_vMethodSubs.size(); del++ )
	    {
	      for( unsigned int del2 = 0; del2 < l_vMethodSubs[del]->size(); del2++ )
		delete l_vMethodSubs[del]->at( del2 );
	      delete l_vMethodSubs[del];
	    }
	    delete l_pInstances;
	    delete l_pNewSolution;
	    return 0;
	  }
	  if( l_iMaxDecomps < 0 || l_pNewSolution->GetNumDecomps() < l_iMaxDecomps )
	  {
	    bool l_bDuplicate = false;
	    if( l_bLoopDetection )
	    {
	      for( unsigned int q = 0; q < l_vQueue.size() && !l_bDuplicate; q++ )
	      {
		if( l_vQueue[q]->Equivalent( *l_pNewSolution ) &&
		    l_vQueue[q]->GetNumDecomps() <= 
		    l_pNewSolution->GetNumDecomps() )
		  l_bDuplicate = true;
	      }
	      for( unsigned int q = 0; q < l_vDead.size() && !l_bDuplicate; q++ )
	      {
		if( l_vDead[q]->Equivalent( *l_pNewSolution ) &&
		    l_vDead[q]->GetNumDecomps() <=
		    l_pNewSolution->GetNumDecomps() )
		  l_bDuplicate = true;
	      }
	    }
	    if( l_bDuplicate )
	    {
	      delete l_pNewSolution;
	      l_iNumDiscarded++;
	    }
	    else
	    {
	      l_vQueue.push_back( l_pNewSolution );
	      l_bFoundDecomp = true;
	      l_iExtensions++;
	    }
	  }
	  else
	    delete l_pNewSolution;
	  delete l_pInstances->at( l_iCurInst );
	}
	delete l_pInstances;
      }
    }
    if( l_bLoopDetection && ( l_iMaxDecomps < 0 || l_pCurrentSol->GetNumDecomps() <= l_iLDKeepLevel * l_iMaxDecomps * 0.01 ) )
      l_vDead.push_back( l_pCurrentSol );
    else
      delete l_pCurrentSol;

    if( l_iLogLevel > 5 )
    {
      std::cout << l_iExtensions << " found in " << (double)( clock() - l_StartClock ) / CLOCKS_PER_SEC << " seconds.\n";
      std::cout << (double)( l_SeekEndClock - l_SeekStartClock ) / CLOCKS_PER_SEC << " seconds finding valid decompositions.\n\n";
    }
  }

  std::cout << "\nNo legal plans.\n";

  if( l_iLogLevel > 0 )
  {
    std::cout << "\nNodes Generated: " << l_iNumGenerated;
    std::cout << "\nNodes Discarded: " << l_iNumDiscarded;
    std::cout << "\nBacktrack Points: " << l_iNumBacktracked;
    std::cout << "\nFailure Points: " << l_iNumFailed;
    std::cout << "\n";
  }

  return 0;

#ifdef CATCH_EXCEPTS
  }catch( Exception e ){ std::cerr << "\n" << e.ToStr() << "\n"; return 1; }
#endif//CATCH_EXCEPTS
}


void FindDecomps( std::vector< int > & p_vMethodIndices,
		  std::vector< std::vector< Substitution * > * > & p_vSubs,
		  const std::tr1::shared_ptr< HtnDomain > & p_pDomain,
		  const HtnSolution * p_pCurSol,
		  bool p_bEarlyStop )
{
  for( unsigned int l_iCurMethod = 0;
       l_iCurMethod < p_pDomain->GetNumMethods();
       l_iCurMethod++ )
  {
    if( CompareNoCase( p_pDomain->GetCMethod( l_iCurMethod )->GetCHead()->GetName(), p_pCurSol->GetCTopTask()->GetName() ) == 0 )
    {
      Substitution l_PartSub;
      for( unsigned int i = 0; i < p_pCurSol->GetCTopTask()->GetNumParams(); i++ )
      {
	l_PartSub.AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( p_pDomain->GetCMethod( l_iCurMethod )->GetCHead()->GetCParam( i ) ), p_pCurSol->GetCTopTask()->GetCParam( i ) );
      }
      std::set< TermVariableP > l_vRelVars = p_pDomain->GetCMethod( l_iCurMethod )->GetRelVars();
      for( unsigned int i = 0; i < p_pDomain->GetCMethod( l_iCurMethod )->GetCHead()->GetNumParams(); i++ )
	l_vRelVars.erase( std::tr1::dynamic_pointer_cast< TermVariable >( p_pDomain->GetCMethod( l_iCurMethod )->GetCHead()->GetCParam( i ) ) );
      std::vector< Substitution * > * l_pInstances = p_pCurSol->GetCState()->GetInstantiations( p_pDomain->GetCMethod( l_iCurMethod )->GetCPreconditions(), &l_PartSub, l_vRelVars );

      if( !l_pInstances->empty() )
      {
	p_vMethodIndices.push_back( l_iCurMethod );
	p_vSubs.push_back( l_pInstances );
	if( p_bEarlyStop )
	  return;
      }
    }
  }
}

void ReorderDecomps( std::vector< int > & p_vMethodIndices,
		     std::vector< std::vector< Substitution * > * > & p_vSubs,
		     const std::tr1::shared_ptr< HtnDomain > & p_pDomain,
		     bool p_bRandomOrder )
{
  std::vector< int > l_vMethodIndicesTemp = p_vMethodIndices;
  std::vector< std::vector< Substitution * > * > l_vMethodSubsTemp = p_vSubs;
  p_vMethodIndices.clear();
  p_vSubs.clear();

  while( !l_vMethodIndicesTemp.empty() )
  {
    unsigned int l_iWorst = 0;
    if( p_bRandomOrder )
      l_iWorst = rand() % l_vMethodIndicesTemp.size();
    else
    {
      for( unsigned int i = 1; i < l_vMethodIndicesTemp.size(); i++ )
      {
	if( p_pDomain->GetCMethod( l_vMethodIndicesTemp[i] )->GetCPreconditions()->GetNumConjs() > p_pDomain->GetCMethod( l_vMethodIndicesTemp[l_iWorst] )->GetCPreconditions()->GetNumConjs() )
	  l_iWorst = i;
      }
    }
      
    p_vMethodIndices.push_back( l_vMethodIndicesTemp[l_iWorst] );
    p_vSubs.push_back( l_vMethodSubsTemp[l_iWorst] );

    std::vector< int >::iterator l_Iter = l_vMethodIndicesTemp.begin();
    for( unsigned int i = 0; i < l_iWorst; i++ )
      l_Iter++;
    l_vMethodIndicesTemp.erase( l_Iter );
    std::vector< std::vector< Substitution * > * >::iterator l_Iter2 = l_vMethodSubsTemp.begin();
    for( unsigned int i = 0; i < l_iWorst; i++ )
      l_Iter2++;
    l_vMethodSubsTemp.erase( l_Iter2 );
  }
}
