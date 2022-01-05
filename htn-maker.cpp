#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <cassert>
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

#define FLAG_REQUIRE_NEW                0x00000001
#define FLAG_VARIABLE_LINKAGE           0x00000002
#define FLAG_DROP_UNNEEDED              0x00000004
#define FLAG_FORCE_OPS_FIRST            0x00000008
#define FLAG_HARD_SQUELCH               0x00000010
#define FLAG_NO_SUBSUMPTION             0x00000020
#define FLAG_PARTIAL_GENERALIZATION     0x00000040
#define FLAG_ONLY_TASK_EFFECTS          0x00000080
#define FLAG_SOUNDNESS_CHECK            0x00000100
#define FLAG_ND_CHECKERS                0x00000200
#define FLAG_QVALUES                    0x00000400

void LearnMethods( AnnotatedPlan * p_pPlan,
		   HtnTaskList * p_pTasks,
		   HtnDomain * p_pHtnDomain );

void DoSubsumption( HtnDomain * p_pHtnDomain,
		    HtnMethod * p_pNewMethod );

void DoQValueUpdate( HtnDomain * p_pHtnDomain,
		     HtnMethod * p_pNewMethod );

void LearnFromExactSequence( unsigned int p_iInitState,
			     unsigned int p_iFinalState,
			     AnnotatedPlan * p_pPlan,
			     HtnDomain * p_pDomain,
			     const PartialHtnMethod * p_pPartial );

void LearnFromSubsequences( unsigned int p_iInitState,
			    unsigned int p_iFinalState,
			    AnnotatedPlan * p_pPlan,
			    HtnDomain * p_pDomain,
			    const std::vector< PartialHtnMethod * > & p_vPartials );

bool TrySolving( AnnotatedPlan * p_pPlan, 
		 unsigned int p_iInitState,
		 unsigned int p_iForState,
		 const std::tr1::shared_ptr< HtnTaskDescr > & p_pTask,
		 const Substitution * p_pTaskSubs,
		 const Substitution * p_pMasterSubs,
		 const HtnDomain * p_pDomain );

void MakeSoundnessCheckMethods( const HtnTaskList * p_pTasks,
				HtnDomain * p_pDomain );

void MakeTrivialNdCheckers( HtnDomain * p_pDomain );
void LearnNdCheckers( unsigned int p_iAction,
		      const StripsSolution * p_pPlan,
		      HtnDomain * p_pDomain );
void LearnOneNdChecker( unsigned int p_iInitState,
			unsigned int p_iFinalState,
			const Operator * p_pDesiredOp,
			const StripsSolution * p_pPlan,
			HtnDomain * p_pDomain );

unsigned long g_iFlags;
unsigned int g_iMaxMethodId;
char g_cMethodIdStr[8];

int main( int argc, char * argv[] )
{
#ifdef CATCH_EXCEPTS
  try{
#endif//CATCH_EXCEPTS

  std::string l_sStripsDomainFile;
  std::string l_sTasksFile;
  std::string l_sProblemFile;
  std::string l_sSolutionFile;
  std::string l_sHtnDomainFile;

  try
  {
    TCLAP::CmdLine l_cCmd( "Learn an HTN domain from a plan trace", ' ', "1.1" );

    TCLAP::UnlabeledValueArg<std::string> l_aStripsDomainFile( "strips_domain_file", "Path to the STRIPS domain file.", true, "not_spec", "strips_domain_file", l_cCmd );
    TCLAP::UnlabeledValueArg<std::string> l_aTasksFile( "tasks_file", "Path to the tasks file.", true, "not_spec", "tasks_file", l_cCmd );
    TCLAP::UnlabeledValueArg<std::string> l_aProblemFile( "problem_file", "Path to the STRIPS problem file.", true, "not_spec", "problem_file", l_cCmd );
    TCLAP::UnlabeledValueArg<std::string> l_aSolutionFile( "solution_file", "Solution to the plan file.", true, "not_spec", "solution_file", l_cCmd );
    TCLAP::UnlabeledValueArg<std::string> l_aHtnDomainFile( "htn_domain_file", "Path to the HTN domain file.", true, "not_spec", "htn_domain_file", l_cCmd );

    TCLAP::SwitchArg l_aNoSubsumption( "", "no_subsumption", "Do not prune methods that are subsumed by another.", l_cCmd, false );
    TCLAP::SwitchArg l_aPartialGeneralization( "", "partial_generalization", "F2Aorce any matching constants to matching variables.", l_cCmd, false );
    TCLAP::SwitchArg l_aOnlyTaskEffects( "", "only_task_effects", "Remove only effects of task when adding a submethod.", l_cCmd, false );
    TCLAP::SwitchArg l_aRequireNew( "", "require_new", "Require that subtasks provide a precondition that it does not also add.", l_cCmd, false );
    TCLAP::SwitchArg l_aVariableLinkage( "", "var_linkage", "Require at least one var from each subtask to be linked to the method head.", l_cCmd, false );
    TCLAP::SwitchArg l_aDropUnneeded( "", "drop_unneeded", "Drop unneeded methods.  That is, if the preconditions of a subtask are fulfilled, the subtask could just be called.  This is an attempt to avoid the infinite recursion problem.", l_cCmd, false );
    TCLAP::SwitchArg l_aForceOpsFirst( "", "force_ops_first", "Force the first subtask of learned methods to be a primitive operator.", l_cCmd, false );
    TCLAP::SwitchArg l_aHardSquelch( "", "hard_squelch", "Only learn from subsequence if cannot be solved by any extension of that sequence backward.", l_cCmd, false );
    TCLAP::SwitchArg l_aSoundnessCheck( "", "soundness_check", "Include in methods a check to guarantee effects have been achieved.", l_cCmd, false );
    TCLAP::SwitchArg l_aNdCheckers( "", "nd_checkers", "Generate methods to force the desired outcome of a non-deterministic operator.", l_cCmd, false );
    TCLAP::SwitchArg l_aQValues( "", "qvalues", "Calculate initial Q-values for methods.", l_cCmd, false );

    l_cCmd.parse( argc, argv );

    l_sStripsDomainFile = l_aStripsDomainFile.getValue();
    l_sTasksFile = l_aTasksFile.getValue();
    l_sProblemFile = l_aProblemFile.getValue();
    l_sSolutionFile = l_aSolutionFile.getValue();
    l_sHtnDomainFile = l_aHtnDomainFile.getValue();
    if( l_aNoSubsumption.getValue() ) g_iFlags |= FLAG_NO_SUBSUMPTION;
    if( l_aPartialGeneralization.getValue() ) g_iFlags |= FLAG_PARTIAL_GENERALIZATION;
    if( l_aOnlyTaskEffects.getValue() ) g_iFlags |= FLAG_ONLY_TASK_EFFECTS;
    if( l_aRequireNew.getValue() ) g_iFlags |= FLAG_REQUIRE_NEW;
    if( l_aVariableLinkage.getValue() ) g_iFlags |= FLAG_VARIABLE_LINKAGE;
    if( l_aDropUnneeded.getValue() ) g_iFlags |= FLAG_DROP_UNNEEDED;
    if( l_aForceOpsFirst.getValue() ) g_iFlags |= FLAG_FORCE_OPS_FIRST;
    if( l_aHardSquelch.getValue() ) g_iFlags |= FLAG_HARD_SQUELCH;
    if( l_aSoundnessCheck.getValue() ) g_iFlags |= FLAG_SOUNDNESS_CHECK;
    if( l_aNdCheckers.getValue() ) g_iFlags |= FLAG_ND_CHECKERS;
    if( l_aQValues.getValue() ) g_iFlags |= FLAG_QVALUES;
  }
  catch( TCLAP::ArgException &e )
  {
    std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
    return 1;
  }

  std::tr1::shared_ptr< StripsDomain > l_pStripsDomain;
  std::tr1::shared_ptr< StripsProblem > l_pStripsProblem;
  AnnotatedPlan * l_pStripsPlan = 0;
  HtnTaskList * l_pHtnTaskList = 0;
  HtnDomain * l_pHtnDomain = 0;

  try
  {
    l_pStripsDomain = std::tr1::shared_ptr< StripsDomain >( new StripsDomain( ReadFile( l_sStripsDomainFile ) ) );
  }
  catch( FileReadException & e )
  {
    e.SetFileName( l_sStripsDomainFile );
    throw e;
  }

  try
  {
    l_pStripsProblem = std::tr1::shared_ptr< StripsProblem >( new StripsProblem( ReadFile( l_sProblemFile ), 
								   l_pStripsDomain ) );
  }
  catch( FileReadException & e )
  {
    e.SetFileName( l_sProblemFile );
    throw e;
  }

  try
  {
    l_pStripsPlan = new AnnotatedPlan( l_pStripsProblem, 
				       ReadFile( l_sSolutionFile ) );
  }
  catch( FileReadException & e )
  {
    e.SetFileName( l_sSolutionFile );
    throw e;
  }

  try
  {
    std::stringstream l_sDomainStream( ReadFile( l_sHtnDomainFile ) );
    l_pHtnDomain = HtnDomain::FromPddl( l_sDomainStream );
  }
  catch( FileReadException & e )
  {
    e.SetFileName( l_sHtnDomainFile );
    throw e;
  }

  try
  {
    l_pHtnTaskList = new HtnTaskList( std::tr1::shared_ptr< HtnDomain >( new HtnDomain( *l_pHtnDomain ) ), ReadFile( l_sTasksFile ) );
  }
  catch( FileReadException & e )
  {
    e.SetFileName( l_sTasksFile );
    throw e;
  }

  g_iMaxMethodId = 0;
  if( l_pHtnDomain->GetRequirements() & PDDL_REQ_METHOD_IDS )
  {
    for( unsigned int i = 0; i < l_pHtnDomain->GetNumMethods(); i++ )
    {
      if( (unsigned int)atoi( l_pHtnDomain->GetCMethod( i )->GetId().c_str() ) > g_iMaxMethodId )
	g_iMaxMethodId = atoi( l_pHtnDomain->GetCMethod( i )->GetId().c_str() );
    }
  }

  if( g_iFlags & FLAG_SOUNDNESS_CHECK )
    MakeSoundnessCheckMethods( l_pHtnTaskList,
			       l_pHtnDomain );

  if( g_iFlags & FLAG_ND_CHECKERS )
    MakeTrivialNdCheckers( l_pHtnDomain );

  LearnMethods( l_pStripsPlan,
		l_pHtnTaskList,
		l_pHtnDomain );

  std::cout << l_pHtnDomain->ToPddl() << "\n";

  delete l_pHtnDomain;  
  delete l_pHtnTaskList;
  delete l_pStripsPlan;

#ifdef CATCH_EXCEPTS
  }catch( Exception & e ){ std::cerr << "\n" << e.ToStr() << "\n"; return 1; }
#endif//CATCH_EXCEPTS

  return 0;
}

std::vector< PartialHtnMethod * > * GetPartials( const HtnDomain * p_pDomain,
						 const AnnotatedPlan * p_pPlan,
						 const HtnTaskList * p_pTasks,
						 unsigned int p_iStateIndex )
{
  std::vector< PartialHtnMethod * > * l_pRet = new std::vector< PartialHtnMethod * >;

  for( unsigned int j = 0; j < p_pTasks->size(); j++ )
  {
    Substitution l_EmptySub;

    std::set< TermVariableP > l_vRelVars;
    std::vector< TermVariableP > l_vTemp = p_pTasks->at( j )->GetCHead()->GetVariables();
    for( unsigned int i = 0; i < l_vTemp.size(); i++ )
      l_vRelVars.insert( l_vTemp[i] );
    l_vTemp = p_pTasks->at( j )->GetCPreconditions()->GetVariables();
    for( unsigned int i = 0; i < l_vTemp.size(); i++ )
      l_vRelVars.insert( l_vTemp[i] );
    l_vTemp = p_pTasks->at( j )->GetCEffects()->GetVariables();
    for( unsigned int i = 0; i < l_vTemp.size(); i++ )
      l_vRelVars.insert( l_vTemp[i] );

    std::vector< Substitution * > * l_pSubs = p_pPlan->GetCState( p_iStateIndex )->GetInstantiations( FormulaPredP( p_pTasks->at( j )->GetCHead() ), p_pTasks->at( j )->GetCEffects(), &l_EmptySub, l_vRelVars );

    for( unsigned int k = 0; k < l_pSubs->size(); k++ )
    {
      FormulaP l_pGroundEffects( p_pTasks->at( j )->GetCEffects()->AfterSubstitution( *l_pSubs->at( k ), 0 ) );
      // The effects must have just become true.
      if( !p_pPlan->GetCState( p_iStateIndex - 1 )->IsConsistent( l_pGroundEffects ) )
      {
	PartialHtnMethod * l_pNewPart = new PartialHtnMethod( std::tr1::shared_ptr< HtnDomain >( new HtnDomain( *p_pDomain ) ), p_pTasks->at( j ), l_pSubs->at( k ), p_iStateIndex );
	l_pRet->push_back( l_pNewPart );
      }
      delete l_pSubs->at( k );
    }

    delete l_pSubs;
  }

  return l_pRet;
}

void LearnMethods( AnnotatedPlan * p_pPlan,
		   HtnTaskList * p_pTasks,
		   HtnDomain * p_pHtnDomain )
{
  // Iterate forward through the solution trace.
  for( unsigned int l_iForState = 1;
       l_iForState < p_pPlan->GetPlanLength() + 1; 
       l_iForState++ )
  {
    // Add new partial methods for tasks that may be ending in this state.
    std::vector< PartialHtnMethod * > * l_pPartials = 
      GetPartials( p_pHtnDomain,
		   p_pPlan,
		   p_pTasks,
		   l_iForState );

    // Process all of those partials back to each prior state, starting
    //   with the most recent states to encourage composition of learned
    //   methods.
    LearnFromSubsequences( 0,
			   l_iForState,
			   p_pPlan,
			   p_pHtnDomain,
			   *l_pPartials );

    for( unsigned int l_iCurPartial = 0; 
	 l_iCurPartial < l_pPartials->size(); 
	 l_iCurPartial++ )
      delete l_pPartials->at( l_iCurPartial );
    delete l_pPartials;

    // Add non-trivial ND-check methods
    if( g_iFlags & FLAG_ND_CHECKERS )
      LearnNdCheckers( l_iForState - 1,
		       p_pPlan,
		       p_pHtnDomain );
  }

}

void DoSubsumption( HtnDomain * p_pHtnDomain,
		    HtnMethod * p_pNewMethod )
{
  bool l_bFound = false;
  for( unsigned int m = 0; m < p_pHtnDomain->GetNumMethods() && !l_bFound; m++ )
  {
    if( p_pHtnDomain->GetCMethod( m )->Subsumes( p_pNewMethod ) )
      l_bFound = true;
  }
  if( l_bFound )
    delete p_pNewMethod;
  else
  {
    bool l_bInserted = false;
    for( unsigned int m = 0; m < p_pHtnDomain->GetNumMethods(); m++ )
    {
      if( p_pNewMethod->Subsumes( p_pHtnDomain->GetCMethod( m ) ) )
      {
	if( !l_bInserted )
	{
	  l_bInserted = true;
	  p_pHtnDomain->ReplaceMethod( m, p_pNewMethod );
	}
	else
	{
	  p_pHtnDomain->RemoveMethod( m );
	  m--;
	}
      }
    }
    if( !l_bInserted )
      p_pHtnDomain->AddMethod( p_pNewMethod );
  }
}

void DoQValueUpdate( HtnDomain * p_pHtnDomain,
		     HtnMethod * p_pNewMethod )
{
  bool l_bFound = false;
  for( unsigned int m = 0; m < p_pHtnDomain->GetNumMethods() && !l_bFound; m++ )
  {
    if( p_pHtnDomain->GetCMethod( m )->Subsumes( p_pNewMethod ) &&
	p_pNewMethod->Subsumes( p_pHtnDomain->GetCMethod( m ) ) )
    {
      l_bFound = true;
      p_pHtnDomain->UpdateMethodQValue( m, (int)(p_pNewMethod->GetQValue() + 0.5) );
    }
  }
  if( l_bFound )
    delete p_pNewMethod;
  else
    p_pHtnDomain->AddMethod( p_pNewMethod );
}

void LearnFromSubsequences( unsigned int p_iInitState,
			    unsigned int p_iFinalState,
			    AnnotatedPlan * p_pPlan,
			    HtnDomain * p_pDomain,
			    const std::vector< PartialHtnMethod * > & p_vPartials )
{
  if( p_iInitState + 1 < p_iFinalState )
    LearnFromSubsequences( p_iInitState + 1,
			   p_iFinalState,
			   p_pPlan,
			   p_pDomain,
			   p_vPartials );

  for( unsigned int i = 0; i < p_vPartials.size(); i++ )
  {
    bool l_bLearn = true;
    if( g_iFlags & FLAG_HARD_SQUELCH )
    {
      for( unsigned int j = 0; j <= p_iInitState && l_bLearn; j++ )
      {
	if( TrySolving( p_pPlan, 
			j,
			p_iFinalState,
			p_vPartials[i]->GetCTaskDescr(),
			p_vPartials[i]->GetCTaskSubs(),
			p_vPartials[i]->GetCMasterSubs(),
			p_pDomain ) )
	  l_bLearn = false;
      }
    }

    if( l_bLearn )
      LearnFromExactSequence( p_iInitState,
			      p_iFinalState,
			      p_pPlan,
			      p_pDomain,
			      p_vPartials[i] );
  }
}

void LearnFromExactSequence( unsigned int p_iInitState,
			     unsigned int p_iFinalState,
			     AnnotatedPlan * p_pPlan,
			     HtnDomain * p_pDomain,
			     const PartialHtnMethod * p_pPartial )
{
  PartialHtnMethod * l_pCurPartial = 
    new PartialHtnMethod( *p_pPartial );
  l_pCurPartial->GetNewTaskVars();
  l_pCurPartial->MarkMethodStart( p_iInitState );
  bool l_bRecentHelped = false;

  if( g_iFlags & FLAG_DROP_UNNEEDED )
  {
    //todo Why are we doing this on instantiations, rather than just the list of methods in the domain?
    bool l_bDrop = false;
    for( int l_iCurMethod = p_pPlan->GetNumMethods() - 1;
	 l_iCurMethod >= 0 && !l_bDrop;
	 l_iCurMethod-- )
    {
      if( p_pPlan->GetMethodAfterState( l_iCurMethod ) <= p_iFinalState && p_pPlan->GetMethodBeforeState( l_iCurMethod ) >= p_iInitState && CompareNoCase( p_pPlan->GetCMethod( l_iCurMethod )->GetCHead()->GetName(), l_pCurPartial->GetCTaskDescr()->GetCHead()->GetName() ) == 0 )
      {
	Substitution l_Subs;
	for( unsigned int l_iCurParam = 0;
	     l_iCurParam < l_pCurPartial->GetCTaskDescr()->GetCHead()->GetNumParams();
	     l_iCurParam++ )
	{
	  TermVariableP l_pVar = std::tr1::dynamic_pointer_cast< TermVariable >( p_pPlan->GetCMethod( l_iCurMethod )->GetCHead()->GetCParam( l_iCurParam ) );
	  TermConstantP l_pTerm = std::tr1::dynamic_pointer_cast< TermConstant >( l_pCurPartial->GetCMasterSubs()->FindIndexByVar( std::tr1::dynamic_pointer_cast< TermVariable >( l_pCurPartial->GetCTaskSubs()->FindIndexByVar( std::tr1::dynamic_pointer_cast< TermVariable >( l_pCurPartial->GetCTaskDescr()->GetCHead()->GetCParam( l_iCurParam ) ) )->second ) )->second );
	  l_Subs.AddPair( l_pVar, l_pTerm );
	}

	std::set< TermVariableP > l_vRelVars;
	std::vector<Substitution *> * l_pTemp = p_pPlan->GetCState( p_iInitState )->GetInstantiations( p_pPlan->GetCMethod( l_iCurMethod )->GetCPreconditions(), &l_Subs, l_vRelVars );

	if( l_pTemp->size() > 0 )
	{
	  l_bDrop = true;
	  p_pPlan->AddMethodInst( p_pPlan->GetCMethod( l_iCurMethod ), l_pTemp->at( 0 ), p_iInitState, p_iFinalState, p_pPlan->GetCTaskDescr( l_iCurMethod ), p_pPlan->GetCMethodEffects( l_iCurMethod ), p_pPlan->GetMethodCost( l_iCurMethod ) );
	  //todo This may not exactly be the correct cost to use here, since we would have been learning a method with higher cost.

	  for( unsigned int i = 0; i < l_pTemp->size(); i++ )
	    delete l_pTemp->at( i );
	}
	delete l_pTemp;
      }
    }
    if( l_bDrop )
    {
      delete l_pCurPartial;
      return;
    }
  }

  while( l_pCurPartial->GetCurrentStateNum() > p_iInitState )
  {
    int l_iBestMethod = -1;

    for( unsigned int l_iCurMethod = 0; 
	 l_iCurMethod < p_pPlan->GetNumMethods(); 
	 l_iCurMethod++ )
    {
      if( p_pPlan->GetMethodAfterState( l_iCurMethod ) == l_pCurPartial->GetCurrentStateNum() && p_pPlan->GetMethodBeforeState( l_iCurMethod ) >= p_iInitState + ( g_iFlags & FLAG_FORCE_OPS_FIRST ? 1 : 0 ) )
      {
	bool l_bUseful = false;
	FormulaConjP l_pMethodEffects( std::tr1::dynamic_pointer_cast< FormulaConj >( p_pPlan->GetCTaskDescr( l_iCurMethod )->GetCEffects()->AfterSubstitution( *p_pPlan->GetCMethodSub( l_iCurMethod ), 0 ) ) );

	if( !( g_iFlags & FLAG_REQUIRE_NEW ) )
	{
	  if( l_pCurPartial->SuppliesPrec( l_pMethodEffects ) || l_pCurPartial->SuppliesEffect( l_pMethodEffects ) )
	    l_bUseful = true;
	}
	else
	{
	  FormulaConjP l_pMethodPrecs( std::tr1::dynamic_pointer_cast< FormulaConj >( p_pPlan->GetCMethod( l_iCurMethod )->GetCPreconditions()->AfterSubstitution( *p_pPlan->GetCMethodSub( l_iCurMethod ), 0 ) ) );
	  if( l_pCurPartial->SuppliesNewPrec( l_pMethodEffects, l_pMethodPrecs ) || l_pCurPartial->SuppliesNewEffect( l_pMethodEffects, l_pMethodPrecs ) )
	    l_bUseful = true;
	}
	if( l_bUseful )
	{
	  if( l_iBestMethod == -1 || p_pPlan->GetMethodBeforeState( l_iCurMethod ) < p_pPlan->GetMethodBeforeState( l_iBestMethod ) || ( p_pPlan->GetMethodBeforeState( l_iCurMethod ) == p_pPlan->GetMethodBeforeState( l_iBestMethod ) && p_pPlan->GetCMethod( l_iCurMethod )->GetNumSubtasks() < p_pPlan->GetCMethod( l_iBestMethod )->GetNumSubtasks() ) )
	  {
	    l_iBestMethod = l_iCurMethod;
	  }
	}
      }
    }
    if( l_iBestMethod != -1 )
    {
      l_pCurPartial->AddMethod( p_pPlan->GetCMethod( l_iBestMethod ), p_pPlan->GetCMethodSub( l_iBestMethod ), p_pPlan->GetMethodBeforeState( l_iBestMethod ), p_pPlan->GetMethodAfterState( l_iBestMethod ), ( g_iFlags & FLAG_ONLY_TASK_EFFECTS ? p_pPlan->GetCTaskDescr( l_iBestMethod )->GetCEffects() : p_pPlan->GetCMethodEffects( l_iBestMethod ) ), g_iFlags & FLAG_PARTIAL_GENERALIZATION, (int)p_pPlan->GetMethodCost( l_iBestMethod ) );
      l_bRecentHelped = true;
    }
    else
    {
      bool l_bUseful = false;
      FormulaConjP l_pOpEffects( std::tr1::dynamic_pointer_cast< FormulaConj >( p_pPlan->GetCOperator( l_pCurPartial->GetCurrentStateNum() - 1 )->GetCEffects()->AfterSubstitution( *p_pPlan->GetCSubstitution( l_pCurPartial->GetCurrentStateNum() - 1 ), 0 ) ) );

      if( l_pCurPartial->SuppliesPrec( l_pOpEffects ) || l_pCurPartial->SuppliesEffect( l_pOpEffects ) )
	l_bUseful = true;
      if( l_bUseful )
      {
	l_pCurPartial->AddOperator( p_pPlan->GetCOperator( l_pCurPartial->GetCurrentStateNum() - 1 ), p_pPlan->GetCSubstitution( l_pCurPartial->GetCurrentStateNum() - 1 ), l_pCurPartial->GetCurrentStateNum() - 1, g_iFlags & FLAG_PARTIAL_GENERALIZATION );
	l_bRecentHelped = true;
      }
      else
      {
	l_bRecentHelped = false;
	if( l_pCurPartial->GetCurrentStateNum() == p_iFinalState )
	{
	  while( l_pCurPartial->GetCurrentStateNum() > p_iInitState )
	    l_pCurPartial->Advance();
	}
	else
	  l_pCurPartial->Advance();
      }
    }
  }

  // Should I make a method?
  if( l_bRecentHelped )
  {
    FormulaConjP l_pLiftedPrecs( std::tr1::dynamic_pointer_cast< FormulaConj >( l_pCurPartial->GetCTaskDescr()->GetCPreconditions()->AfterSubstitution( *l_pCurPartial->GetCTaskSubs(), 0 ) ) );
    std::set< TermVariableP > l_vRelVars;
    std::vector< Substitution * > * l_pInstances = p_pPlan->GetCState( p_iInitState )->GetInstantiations( l_pLiftedPrecs, l_pCurPartial->GetCMasterSubs(), l_vRelVars );
    if( !l_pInstances->empty() && l_pCurPartial->RemainingAddListSatisfied( p_pPlan->GetCState( p_iInitState ) ) )
    {
      // Yes, create a method
      HtnMethod * l_pNewMethod = l_pCurPartial->CreateMethod( g_iFlags & FLAG_SOUNDNESS_CHECK, g_iFlags & FLAG_PARTIAL_GENERALIZATION, g_iFlags & FLAG_QVALUES );
      bool l_bDelete = false;
      FormulaP l_pLiftedTaskEffects( l_pCurPartial->GetCTaskDescr()->GetCEffects()->AfterSubstitution( *l_pCurPartial->GetCTaskSubs(), 0 ) );
      // If the preconditions of the method imply the desired effects, then the
      //  method serves no purpose.
      if( l_pNewMethod->GetCPreconditions()->Implies( l_pLiftedTaskEffects ) )
	l_bDelete = true;
      
      if( g_iFlags & FLAG_ND_CHECKERS )
	l_pNewMethod->AddNdCheckers();
      if( g_iFlags & FLAG_VARIABLE_LINKAGE && !l_pNewMethod->SubtasksArePartiallyLinked() )
	l_bDelete = true;
      if( l_bDelete )
      {
	delete l_pNewMethod;
      }
      else
      {
	if( p_pDomain->GetRequirements() & PDDL_REQ_METHOD_IDS )
	{
	  snprintf( g_cMethodIdStr, 8, "%d", ++g_iMaxMethodId );
	  l_pNewMethod->SetId( g_cMethodIdStr );
	}
	std::tr1::shared_ptr< HtnTaskDescr > l_pNewTaskDescr( l_pCurPartial->GetCTaskDescr()->AfterSubstitution( *l_pCurPartial->GetCTaskSubs(), 0 ) );
	FormulaConjP l_pMethodEffects( l_pCurPartial->GetActualEffects() );

	p_pPlan->AddMethodInst( l_pNewMethod, l_pInstances->at( 0 ), l_pCurPartial->GetInitStateNum(), l_pCurPartial->GetFinalStateNum(), l_pNewTaskDescr, l_pMethodEffects, l_pNewMethod->GetQValue() );

	if( l_pNewMethod->GetNumSubtasks() == 1 &&
	    *l_pNewMethod->GetCSubtask( 0 )  == 
	    *l_pNewMethod->GetCHead() )
	  delete l_pNewMethod;
	else if( g_iFlags & FLAG_QVALUES )
	  DoQValueUpdate( p_pDomain, l_pNewMethod );
	else if( !( g_iFlags & FLAG_NO_SUBSUMPTION ) )
	  DoSubsumption( p_pDomain, l_pNewMethod );
	else
	  p_pDomain->AddMethod( l_pNewMethod );
      }
    }
    for( unsigned int l_iInst = 0; l_iInst < l_pInstances->size(); l_iInst++ )
      delete l_pInstances->at( l_iInst );
    delete l_pInstances;
  }
  delete l_pCurPartial;
}

bool TrySolving( AnnotatedPlan * p_pPlan, 
		 unsigned int p_iInitState,
		 unsigned int p_iForState,
		 const std::tr1::shared_ptr< HtnTaskDescr > & p_pTask,
		 const Substitution * p_pTaskSubs,
		 const Substitution * p_pMasterSubs,
		 const HtnDomain *p_pDomain )
{
  const State * l_pState = p_pPlan->GetCState( p_iInitState );
  for( unsigned int i = 0; i < p_pDomain->GetNumMethods(); i++ )
  {
    const HtnMethod * l_pCurMethod = p_pDomain->GetCMethod( i );
    if( CompareNoCase( l_pCurMethod->GetCHead()->GetName(),
		       p_pTask->GetCHead()->GetName() ) == 0 )
    {
      Substitution l_FromMethodSubs;
      Substitution l_FromTaskSubs;
      for( unsigned int l_iCurParam = 0;
	   l_iCurParam < p_pTask->GetCHead()->GetNumParams();
	   l_iCurParam++ )
      {
	TermVariableP l_pVar = std::tr1::dynamic_pointer_cast< TermVariable >( l_pCurMethod->GetCHead()->GetCParam( l_iCurParam ) );
	TermP l_pTerm = p_pTask->GetCHead()->GetCParam( l_iCurParam );
	l_FromTaskSubs.AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( l_pTerm ), l_pVar );
	l_pTerm = p_pTaskSubs->FindIndexByVar( std::tr1::dynamic_pointer_cast< TermVariable >( l_pTerm ) )->second;
	l_pTerm = p_pMasterSubs->FindIndexByVar( std::tr1::dynamic_pointer_cast< TermVariable >( l_pTerm ) )->second;
	l_FromMethodSubs.AddPair( l_pVar, l_pTerm );
      }

      std::set< TermVariableP > l_vRelVars;
      std::vector<Substitution *> * l_pTemp = l_pState->GetInstantiations( l_pCurMethod->GetCPreconditions(), &l_FromMethodSubs, l_vRelVars );

      if( l_pTemp->size() > 0 )
      {
	FormulaConjP l_pMethodEffects( std::tr1::dynamic_pointer_cast< FormulaConj >( p_pTask->GetCEffects()->AfterSubstitution( l_FromTaskSubs, 0 ) ) );
	std::tr1::shared_ptr< HtnTaskDescr > l_pTask( p_pTask->AfterSubstitution( l_FromTaskSubs, 0 ) );
	p_pPlan->AddMethodInst( l_pCurMethod, l_pTemp->at( 0 ), p_iInitState, p_iForState, l_pTask, l_pMethodEffects, l_pCurMethod->GetQValue() );
	//todo What cost should we really be using here?  This seems like the only option, but does not quite mean what it should.

	for( unsigned int i = 0; i < l_pTemp->size(); i++ )
	  delete l_pTemp->at( i );
      
	return true;
      }
    }
  }
  return false;
}

void MakeSoundnessCheckMethods( const HtnTaskList * p_pTasks,
				HtnDomain * p_pDomain )
{
  for( unsigned int i = 0; i < p_pTasks->size(); i++ )
  {
    std::string l_sTaskName = p_pTasks->at(i)->GetCHead()->GetName();
    l_sTaskName += "-verify";

    bool l_bFound = false;
    for( unsigned int j = 0; j < p_pDomain->GetNumMethods() && !l_bFound; j++ )
    {
      if( CompareNoCase( p_pDomain->GetCMethod( j )->GetCHead()->GetName(),
			 l_sTaskName ) == 0 )
	l_bFound = true;
    }

    if( !l_bFound )
    {
      std::string l_sNewMethod = "( :method ";
      l_sNewMethod += l_sTaskName;
      l_sNewMethod += "\n";
      l_sNewMethod += "  :parameters\n";
      l_sNewMethod += "  (\n";
      for( unsigned int j = 0; j < p_pTasks->at(i)->GetCHead()->GetNumParams(); j++ )
	l_sNewMethod += " " + p_pTasks->at(i)->GetCHead()->GetCParam( j )->ToStr();
      l_sNewMethod += "  )\n";
      l_sNewMethod += "  :precondition\n";
      l_sNewMethod += "  " + p_pTasks->at( i )->GetCEffects()->ToStrNoTyping();
      l_sNewMethod += "  :subtasks\n";
      l_sNewMethod += "  ()\n";
      l_sNewMethod += ")\n";

      std::stringstream l_sVerifierStream( l_sNewMethod );
      HtnMethod * l_pVerifier = HtnMethod::FromPddl( l_sVerifierStream,
						     p_pDomain->GetAllowableTypes(),
						     p_pDomain->GetAllowablePredicates(),
						     p_pDomain->GetRequirements() );
      if( p_pDomain->GetRequirements() & PDDL_REQ_METHOD_IDS )
      {
	snprintf( g_cMethodIdStr, 8, "%d", ++g_iMaxMethodId );
	l_pVerifier->SetId( g_cMethodIdStr );
      }
      p_pDomain->AddMethod( l_pVerifier );
    }
  }
}

void MakeTrivialNdCheckers( HtnDomain * p_pDomain )
{
  if( ( g_iFlags & FLAG_ND_CHECKERS ) == 0 )
    throw Exception( E_NOT_IMPLEMENTED,
		     "Call to use ND checkers withouth appropriate cli flag.",
		     __FILE__,
		     __LINE__ );

  for( unsigned int i = 0; i < p_pDomain->GetNumOperators(); i++ )
  {
    const Operator * l_pCurOp = p_pDomain->GetCOperator( i );
    int l_iStrLen = l_pCurOp->GetName().length();
    if( l_iStrLen > 3 &&
	l_pCurOp->GetName().substr( l_iStrLen - 3, 3 ) == "-01" )
    {
      std::stringstream l_sNewMethod;
      l_sNewMethod << "( :method check-";
      l_sNewMethod << l_pCurOp->GetName().substr( 1, l_iStrLen - 4 );
      l_sNewMethod << "\n";
      l_sNewMethod << "  :parameters\n";
      l_sNewMethod << "  (\n";
      for( unsigned int j = 0; j < l_pCurOp->GetNumParams(); j++ )
      {
	l_sNewMethod << "    ";
	l_sNewMethod << l_pCurOp->GetCParam( j )->ToStr();
	l_sNewMethod << "\n";
      }
      l_sNewMethod << "  )\n";
      l_sNewMethod << "  :precondition\n";
      l_sNewMethod << l_pCurOp->GetCEffects()->ToStrNoTyping();
      l_sNewMethod << "\n";
      l_sNewMethod << "  :subtasks\n";
      l_sNewMethod << "  ( )\n";
      l_sNewMethod << ")\n";

      HtnMethod * l_pNewMethod = HtnMethod::FromPddl( l_sNewMethod, p_pDomain->GetAllowableTypes(), p_pDomain->GetAllowablePredicates(), p_pDomain->GetRequirements() );
      
      bool l_bFound = false;
      for( unsigned int j = 0; j < p_pDomain->GetNumMethods() && !l_bFound; j++ )
      {
	if( p_pDomain->GetCMethod( j )->Subsumes( l_pNewMethod ) &&
	    l_pNewMethod->Subsumes( p_pDomain->GetCMethod( j ) ) )
	  l_bFound = true;
      }
      if( !l_bFound )
      {
        if( p_pDomain->GetRequirements() & PDDL_REQ_METHOD_IDS )
	{
	  snprintf( g_cMethodIdStr, 8, "%d", ++g_iMaxMethodId );
	  l_pNewMethod->SetId( g_cMethodIdStr );
	}
	p_pDomain->AddMethod( l_pNewMethod );
      }
      else
	delete l_pNewMethod;
    }
  }
}

void LearnNdCheckers( unsigned int p_iAction,
		      const StripsSolution * p_pPlan,
		      HtnDomain * p_pDomain )
{
  if( ( g_iFlags & FLAG_ND_CHECKERS ) == 0 )
    throw Exception( E_NOT_IMPLEMENTED,
		     "Call to use ND checkers withouth appropriate cli flag.",
		     __FILE__,
		     __LINE__ );

  const Operator * l_pCurOp = p_pPlan->GetCOperator( p_iAction );
  unsigned int l_iNameLen = l_pCurOp->GetName().length();
  if( l_iNameLen < 4 )
    return;
  std::string l_sNameSuffix = l_pCurOp->GetName().substr( l_iNameLen - 3, 3 );
  if( l_sNameSuffix[0] == '-' && isdigit( l_sNameSuffix[1] ) &&
      isdigit( l_sNameSuffix[2] ) && l_sNameSuffix != "-01" )
  {
    const Operator * l_pDesiredOp = NULL;
    for( unsigned int i = 0; i < p_pDomain->GetNumOperators() && !l_pDesiredOp; i++ )
    {
      if( p_pDomain->GetCOperator( i )->GetName().length() == l_iNameLen &&
	  CompareNoCase( l_pCurOp->GetName().substr( 0, l_iNameLen - 3 ),
			 p_pDomain->GetCOperator( i )->GetName().substr( 0, l_iNameLen - 3 ) ) == 0 &&
	  p_pDomain->GetCOperator( i )->GetName().substr( l_iNameLen - 3, 3 ) == "-01" )
	l_pDesiredOp = p_pDomain->GetCOperator( i );
    }

    if( !l_pDesiredOp )
      throw Exception( E_NOT_IMPLEMENTED,
		       "Operator " + l_pCurOp->GetName() + " has no desired version.",
		       __FILE__,
		       __LINE__ );

    FormulaP l_pDesiredEffects( l_pDesiredOp->GetCEffects()->AfterSubstitution( *p_pPlan->GetCSubstitution( p_iAction ), 0 ) );

    bool l_bGotFirst = false;
    for( unsigned int i = p_iAction + 2; i < p_pPlan->GetPlanLength() + 1 && !l_bGotFirst; i++ )
    {
      /** \todo Might we only care about positive effects? */
      if( p_pPlan->GetCState( i )->IsConsistent( l_pDesiredEffects ) )
      {
	l_bGotFirst = true;
	LearnOneNdChecker( p_iAction + 1,
			   i,
			   l_pDesiredOp,
			   p_pPlan,
			   p_pDomain );
      }
    }

  }
}

void LearnOneNdChecker( unsigned int p_iInitState,
			unsigned int p_iFinalState,
			const Operator * p_pDesiredOp,
			const StripsSolution * p_pPlan,
			HtnDomain * p_pDomain )
{
  //  const Operator * l_pOper = p_pPlan->GetCOperator( p_iInitState - 1 );
  const Substitution * l_pSubs = p_pPlan->GetCSubstitution( p_iInitState - 1 );
  std::stringstream l_sTaskDescr;
  l_sTaskDescr << "( :task check-";
  l_sTaskDescr << p_pDesiredOp->GetName().substr( 1, p_pDesiredOp->GetName().length() - 4 );
  l_sTaskDescr << "\n";
  l_sTaskDescr << "  :parameters\n";
  l_sTaskDescr << "  (\n";
  for( unsigned int i = 0; i < p_pDesiredOp->GetNumParams(); i++ )
    l_sTaskDescr << "    " << p_pDesiredOp->GetCParam( i )->ToStr() << "\n";
  l_sTaskDescr << "  )\n";
  l_sTaskDescr << "  :precondition\n";
  l_sTaskDescr << "  ( )\n";
  l_sTaskDescr << "  :effect\n";
  l_sTaskDescr << "  (\n";
  for( FormulaPVecCI i = p_pDesiredOp->GetCEffects()->GetBeginConj();
       i != p_pDesiredOp->GetCEffects()->GetEndConj();
       i++ )
  {
    if( ( *i )->GetType() == FT_PRED )
      l_sTaskDescr << "    " << ( *i )->ToStrNoTyping() << "\n";
  }
  l_sTaskDescr << "  )\n";
  l_sTaskDescr << ")\n";

  std::tr1::shared_ptr< HtnTaskDescr > l_pTempDescr( new HtnTaskDescr( l_sTaskDescr,
								       p_pDomain->GetAllowableTypes(),
								       p_pDomain->GetAllowablePredicates() ) );

  PartialHtnMethod l_PartMethod( std::tr1::shared_ptr< HtnDomain >( new HtnDomain( *p_pDomain ) ),
				 l_pTempDescr,
				 l_pSubs,
				 p_iFinalState );
  l_PartMethod.GetNewTaskVars();
  l_PartMethod.MarkMethodStart( p_iInitState );
  
  //  bool l_bRecentHelped = false;

  while( l_PartMethod.GetCurrentStateNum() > p_iInitState )
  {
    bool l_bUseful = false;
    FormulaConjP l_pOpEffects( std::tr1::dynamic_pointer_cast< FormulaConj >( p_pPlan->GetCOperator( l_PartMethod.GetCurrentStateNum() - 1 )->GetCEffects()->AfterSubstitution( *p_pPlan->GetCSubstitution( l_PartMethod.GetCurrentStateNum() - 1 ), 0 ) ) );

    if( l_PartMethod.SuppliesPrec( l_pOpEffects ) || l_PartMethod.SuppliesEffect( l_pOpEffects ) )
      l_bUseful = true;
    if( l_bUseful )
    {
      l_PartMethod.AddOperator( p_pPlan->GetCOperator( l_PartMethod.GetCurrentStateNum() - 1 ), p_pPlan->GetCSubstitution( l_PartMethod.GetCurrentStateNum() - 1 ), l_PartMethod.GetCurrentStateNum() - 1, g_iFlags & FLAG_PARTIAL_GENERALIZATION );
      //      l_bRecentHelped = true;
    }
    else
    {
      //      l_bRecentHelped = false;
      if( l_PartMethod.GetCurrentStateNum() == p_iFinalState )
      {
	while( l_PartMethod.GetCurrentStateNum() > p_iInitState )
	  l_PartMethod.Advance();
      }
      else
	l_PartMethod.Advance();
    }
  }

  // Should I make a method?
  //  if( l_bRecentHelped )
  {
    if( l_PartMethod.RemainingAddListSatisfied( p_pPlan->GetCState( p_iInitState ) ) )
    {
      HtnMethod * l_pNewMethod = l_PartMethod.CreateMethod( false, g_iFlags & FLAG_PARTIAL_GENERALIZATION );
      l_pNewMethod->AddNdCheckers();

      if( p_pDomain->GetRequirements() & PDDL_REQ_METHOD_IDS )
      {
	snprintf( g_cMethodIdStr, 8, "%d", ++g_iMaxMethodId );
	l_pNewMethod->SetId( g_cMethodIdStr );
      }

      if( !( g_iFlags & FLAG_NO_SUBSUMPTION ) )
	DoSubsumption( p_pDomain, l_pNewMethod );
      else
	p_pDomain->AddMethod( l_pNewMethod );
    }
  }
}
