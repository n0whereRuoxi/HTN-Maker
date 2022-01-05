#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <cassert>
#include <fstream>
#include <set>
#include <tr1/memory>
#include <ctime>

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

bool FindPlan( const std::tr1::shared_ptr< HtnDomain > & p_pDomain, 
	       const HtnSolution * p_pPartial,
	       unsigned int p_iDepth );
bool FindPlanOper( const std::tr1::shared_ptr< HtnDomain > & p_pDomain, 
		   const HtnSolution * p_pPartial,
		   unsigned int p_iDepth );
bool FindPlanMethod( const std::tr1::shared_ptr< HtnDomain > & p_pDomain, 
		     const HtnSolution * p_pPartial,
		     unsigned int p_iDepth );

bool g_bShowTrace;
bool g_bUseQValues;
bool g_bUpdateQValues;
bool g_bRandomSelection;
int g_iDebugLevel;
unsigned int g_iMaxDepth;

int main( int argc, char * argv[] )
{
#ifdef CATCH_EXCEPTS
  try{
#endif//CATCH_EXCEPTS

  std::string l_sDomainFile;
  std::string l_sProblemFile;
  try
  {
    TCLAP::CmdLine l_cCmd( "Find an HTN plan", ' ', "1.1" );

    TCLAP::UnlabeledValueArg<std::string> l_aDomainFile( "domain_file", "Path to the domain file.", true, "not_spec", "domain_file", l_cCmd );
    TCLAP::UnlabeledValueArg<std::string> l_aProblemFile( "problem_file", "Path to the problem file.", true, "not_spec", "problem_file", l_cCmd );
    TCLAP::SwitchArg l_aShowTrace( "t", "show_trace", "Show a full decomposition trace of the solution.", l_cCmd, false );
    TCLAP::SwitchArg l_aUseQValues( "q", "use_qvalues", "When decomposing a task, use the applicable method with lowest Q-value.", l_cCmd, false );
    TCLAP::SwitchArg l_aUpdateQValues( "u", "update_qvalues", "After finding a solution, update the Q-values of the methods used.", l_cCmd, false );
    TCLAP::SwitchArg l_aRandomSelection( "r", "random_selection", "Select applicable methods in random order.", l_cCmd, false );
    TCLAP::ValueArg<int> l_aDebugLevel( "d", "debug_level", "Determine how much debug information to print (0-10).", false, 0, "int", l_cCmd );
    TCLAP::ValueArg<unsigned int> l_aMaxDepth( "m", "max_depth", "Only pursue decomposition trees below this depth.", false, 99999, "unsigned int", l_cCmd );

    l_cCmd.parse( argc, argv );

    l_sDomainFile = l_aDomainFile.getValue();
    l_sProblemFile = l_aProblemFile.getValue();
    g_bShowTrace = l_aShowTrace.getValue();
    g_bUseQValues = l_aUseQValues.getValue();
    g_bUpdateQValues = l_aUpdateQValues.getValue();
    g_bRandomSelection = l_aRandomSelection.getValue();
    g_iDebugLevel = l_aDebugLevel.getValue();
    g_iMaxDepth = l_aMaxDepth.getValue();
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
    e.SetFileName( l_sProblemFile );
    throw e;
  }

  //  srand( time( 0 ) );

  if( g_bUseQValues )
    l_pDomain->SortMethods();

  if( !FindPlan( l_pDomain, l_pProblem, 0 ) )
    std::cout << "\nNo legal plans.\n";

  delete l_pProblem;

  if( g_bUpdateQValues )
  {
    std::ofstream fout( l_sDomainFile.c_str(), std::ios_base::out | std::ios_base::trunc );
    fout << l_pDomain->ToPddl() << "\n";
    fout.close();
  }

  return 0;

#ifdef CATCH_EXCEPTS
  }catch( Exception & e ){ std::cerr << "\n" << e.ToStr() << "\n"; return 1; }
#endif//CATCH_EXCEPTS
}

double DoThisQValue( const std::tr1::shared_ptr< HtnDomain > & p_pDomain,
		     const DecompPart * p_pThisStep )
{
  const std::string & l_sCurrentName = p_pThisStep->GetName();
  const std::string & l_sCurrentId = p_pThisStep->GetId();

  std::string l_sRealName = l_sCurrentName.substr( 2, l_sCurrentName.find_first_of( ' ', 2 ) - 2 );

  if( l_sRealName[0] == '!' )
  {
    for( unsigned int i = 0; i < p_pDomain->GetNumOperators(); i++ )
    {
      if( p_pDomain->GetCOperator( i )->GetName() == l_sRealName )
	return p_pDomain->GetCOperator( i )->GetCost();
    }
    throw Exception( E_INDEX_OUT_OF_BOUNDS,
		     "Could not find operator " + l_sRealName + "\n",
		     __FILE__,
		     __LINE__ );
  }

  if( l_sCurrentId == "" )
    throw Exception( E_NOT_IMPLEMENTED,
		     "Attempt to use Q-values without method ids.",
		     __FILE__,
		     __LINE__ );

  double l_dCost = 0.0;

  const std::vector< DecompPart * > & l_vChildren = p_pThisStep->GetChildren();
  for( unsigned int i = 0; i < l_vChildren.size(); i++ )
  {
    l_dCost += DoThisQValue( p_pDomain, l_vChildren[i] );
  }

  for( unsigned int i = 0; i < p_pDomain->GetNumMethods(); i++ )
  {
    if( p_pDomain->GetCMethod( i )->GetId() == l_sCurrentId )
    {
      p_pDomain->UpdateMethodQValue( i, l_dCost );
      return l_dCost;
    }
  }

  throw Exception( E_INDEX_OUT_OF_BOUNDS,
		   "Could not find method id " + l_sCurrentId + "\n",
		   __FILE__,
		   __LINE__ );
}

void UpdateQValues( const std::tr1::shared_ptr< HtnDomain > & p_pDomain,
		    const HtnSolution * p_pPartial )
{
  if( !g_bUpdateQValues )
    return;
  std::vector< DecompPart * > l_vSolutionForest = p_pPartial->GetDecompTree();

  for( unsigned int i = 0; i < l_vSolutionForest.size(); i++ )
    DoThisQValue( p_pDomain, l_vSolutionForest[i] );
}

bool FindPlan( const std::tr1::shared_ptr< HtnDomain > & p_pDomain, 
	       const HtnSolution * p_pPartial,
	       unsigned int p_iDepth )
{
  if( p_pPartial->IsComplete() )
  {
    std::cout << "\nPlan found!\nNo tasks to complete.\n";
    return true;
  }

  if( p_pPartial->GetCTopTask()->GetName()[0] == '!' )
    return FindPlanOper( p_pDomain, p_pPartial, p_iDepth + 1 );
  else
    return FindPlanMethod( p_pDomain, p_pPartial, p_iDepth + 1 );
}

bool FindPlanOper( const std::tr1::shared_ptr< HtnDomain > & p_pDomain, 
		   const HtnSolution * p_pPartial,
		   unsigned int p_iDepth )
{
  if( p_iDepth > g_iMaxDepth )
    return false;

  bool l_bSuccess = false;

  int l_iOperIndex = -1;
  HtnTaskHeadP l_pTask( p_pPartial->GetCTopTask() );

  for( unsigned int j = 0; j < p_pDomain->GetNumOperators() && l_iOperIndex == -1; j++ )
  {
    if( CompareNoCase( p_pDomain->GetCOperator( j )->GetName(),
		       l_pTask->GetName() ) == 0 )
      l_iOperIndex = j;
  }

  if( l_iOperIndex != -1 )
  {
    Substitution l_OperSubs;
    for( unsigned int j = 0; j < l_pTask->GetNumParams(); j++ )
      l_OperSubs.AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( p_pDomain->GetCOperator( l_iOperIndex )->GetCParam( j ) ), l_pTask->GetCParam( j ) );
    std::vector< Substitution * > * l_pAllOperSubs = p_pPartial->GetCState()->GetInstantiations( p_pDomain->GetCOperator( l_iOperIndex ), &l_OperSubs );

    for( unsigned int k = 0; k < l_pAllOperSubs->size() && !l_bSuccess; k++ )
    {
      HtnSolution * l_pNewSolution = new HtnSolution( *p_pPartial );
      l_pNewSolution->ApplyOperator( l_iOperIndex, l_pAllOperSubs->at( k ) );

      if( l_pNewSolution->IsComplete() )
      {
	std::cout << "\nPlan found!\n";
	std::cout << l_pNewSolution->Print( g_bShowTrace );
	l_bSuccess = true;
	UpdateQValues( p_pDomain, l_pNewSolution );
      }
      else if( l_pNewSolution->GetCTopTask()->GetName()[0] == '!' )
      {
	l_bSuccess = FindPlanOper( p_pDomain, l_pNewSolution, p_iDepth + 1 );
      }
      else
      {
	l_bSuccess = FindPlanMethod( p_pDomain, l_pNewSolution, p_iDepth + 1 );
      }

      delete l_pNewSolution;

    }

    for( unsigned int k = 0; k < l_pAllOperSubs->size(); k++ )
      delete l_pAllOperSubs->at( k );
    delete l_pAllOperSubs;
  }

  return l_bSuccess;
}

void RemoveIrrelevantBindings( std::vector< Substitution * > * p_pInstances,
			       const HtnMethod * p_pMethod )
{
  std::tr1::shared_ptr< const std::vector< TermVariableP > > l_pVars = p_pMethod->GetRelevantVariables();

  std::vector< Substitution * > l_vTemp( *p_pInstances );
  p_pInstances->clear();

  for( unsigned int i = 0; i < l_vTemp.size(); i++ )
  {
    Substitution * l_pCurSubst = l_vTemp[i];
    bool l_bFound = false;
    for( unsigned int j = 0; j < p_pInstances->size() && !l_bFound; j++ )
    {
      Substitution * l_pOldSubst = p_pInstances->at( j );
      bool l_bDiff = false;
      for( unsigned int k = 0; k < l_pVars->size() && !l_bDiff; k++ )
      {
	if( l_pCurSubst->FindIndexByVar( l_pVars->at( k ) )->second !=
	    l_pOldSubst->FindIndexByVar( l_pVars->at( k ) )->second )
	  l_bDiff = true;
      }
      if( !l_bDiff )
	l_bFound = true;
    }
    if( l_bFound )
      delete l_pCurSubst;
    else
      p_pInstances->push_back( l_pCurSubst );
  }
}

bool FindPlanMethod( const std::tr1::shared_ptr< HtnDomain > & p_pDomain, 
		     const HtnSolution * p_pPartial,
		     unsigned int p_iDepth )
{
  if( p_iDepth > g_iMaxDepth )
    return false;

  bool l_bSuccess = false;

  std::vector< unsigned int> l_vMethodIndices;
  for( unsigned int l_iCurMethod = 0;
       l_iCurMethod < p_pDomain->GetNumMethods();
       l_iCurMethod ++ )
    l_vMethodIndices.push_back( l_iCurMethod );

  if( g_bRandomSelection )
    std::random_shuffle( l_vMethodIndices.begin(), l_vMethodIndices.end() );

  for( unsigned int i = 0;
       i < l_vMethodIndices.size() && !l_bSuccess;
       i++ )
  {
    unsigned int l_iCurMethod = l_vMethodIndices[i];
    if( CompareNoCase( p_pDomain->GetCMethod( l_iCurMethod )->GetCHead()->GetName(), p_pPartial->GetCTopTask()->GetName() ) == 0 )
    {
      Substitution l_PartSub;
      for( unsigned int i = 0; 
	   i < p_pPartial->GetCTopTask()->GetNumParams(); 
	   i++ )
      {
	l_PartSub.AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( p_pDomain->GetCMethod( l_iCurMethod )->GetCHead()->GetCParam( i ) ), p_pPartial->GetCTopTask()->GetCParam( i ) );
      }
      std::set< TermVariableP > l_vRelVars = p_pDomain->GetCMethod( l_iCurMethod )->GetRelVars();
      for( unsigned int i = 0;
	   i < p_pDomain->GetCMethod( l_iCurMethod )->GetCHead()->GetNumParams();
	   i++ )
	l_vRelVars.erase( std::tr1::dynamic_pointer_cast< TermVariable >( p_pDomain->GetCMethod( l_iCurMethod )->GetCHead()->GetCParam( i ) ) );
      std::vector< Substitution * > * l_pInstances = p_pPartial->GetCState()->GetInstantiations( p_pDomain->GetCMethod( l_iCurMethod )->GetCPreconditions(), &l_PartSub, l_vRelVars );

      if( g_iDebugLevel > 5 && !l_pInstances->empty() )
      {
	std::cout << "\nTrying method #" << l_iCurMethod << " for task " << p_pPartial->GetCTopTask()->ToStr() << ", " << l_pInstances->size() << " instances possible, depth " << p_iDepth << ".\n";
      }

      RemoveIrrelevantBindings( l_pInstances, p_pDomain->GetCMethod( l_iCurMethod ) );

      if( g_iDebugLevel > 6 && !l_pInstances->empty() )
	std::cout << l_pInstances->size() << " unique instances retained.\n";

      if( l_pInstances->size() > 0 )
      {
	bool l_bFirstInst = true;
	int l_iRandInst = rand() % l_pInstances->size();
	for( int l_iCurInst = l_iRandInst;
	     ( l_bFirstInst || l_iCurInst != l_iRandInst ) && !l_bSuccess;
	     l_iCurInst = ( l_iCurInst + 1 ) % l_pInstances->size() )
	{
	  l_bFirstInst = false;

	  if( g_iDebugLevel > 5 )
	    std::cout << "\nTrying substitution " << l_pInstances->at( l_iCurInst )->ToStr() << " for method #" << l_iCurMethod << " at depth " << p_iDepth << "\n";

	  HtnSolution * l_pNewSolution = new HtnSolution( *p_pPartial );
	  l_pNewSolution->ApplyMethod( l_iCurMethod, l_pInstances->at( l_iCurInst ) );
	  if( l_pNewSolution->IsComplete() )
	  {
	    std::cout << "\nPlan found!\n";
	    std::cout << l_pNewSolution->Print( g_bShowTrace );
	    l_bSuccess = true;
	    UpdateQValues( p_pDomain, l_pNewSolution );
	  }
	  else if( l_pNewSolution->GetCTopTask()->GetName()[0] == '!' )
	  {
	    l_bSuccess = FindPlanOper( p_pDomain, l_pNewSolution, p_iDepth + 1 );
	  }
	  else
	  {
	    l_bSuccess = FindPlanMethod( p_pDomain, l_pNewSolution, p_iDepth + 1 );
	  }

	  delete l_pNewSolution;
	}
      }


      for( unsigned int l_iCurInstance = 0;
	   l_iCurInstance < l_pInstances->size();
	   l_iCurInstance++ )
      {
	delete (*l_pInstances)[l_iCurInstance];
      }
      delete l_pInstances;
    }
  }

  if( g_iDebugLevel > 5 && !l_bSuccess )
    std::cout << "Backtracking from depth " << p_iDepth << "\n";

  return l_bSuccess;
}
