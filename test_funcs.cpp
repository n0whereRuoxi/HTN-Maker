#include <string>
#include <vector>
#include <cassert>
#include <sstream>
#include <fstream>
#include <iostream>
#include <set>
#include <tr1/memory>

#include "exception.hpp"
#include "funcs.hpp"
#include "string_table.hpp"
#include "term.hpp"
#include "term_string.hpp"
#include "term_constant.hpp"
#include "term_variable.hpp"
#include "term_table.hpp"
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

extern StringTable g_StrTable;
extern TermTable g_TermTable;

const std::vector< FormulaPred > g_NoPredicates;

void TestTermCreation()
{
  TermP l_pConst1 = g_TermTable.Lookup( "asdf" );
  TermP l_pConst2 = g_TermTable.Lookup( "asdf" );

  assert( l_pConst1->GetType() == TT_CONSTANT );
  assert( l_pConst2->GetType() == TT_CONSTANT );
  assert( *l_pConst1 == *l_pConst2 );

  assert( CompareNoCase( l_pConst1->ToStr(), "ASDF" ) == 0 );
  assert( CompareNoCase( l_pConst2->ToStr(), "ASDF" ) == 0 );


  TermP l_pVar1 = g_TermTable.Lookup( "?t1" );
  assert( l_pVar1->GetType() == TT_VARIABLE );
  assert( CompareNoCase( l_pVar1->ToStr(), "?T1" ) == 0 );
}

//FormulaPLess myFormulaPLess;
void TestFormulaCreation()
{
  FormulaPredP l_pForm1( new FormulaPred( "( in-truck ?pack truck3 )", TypeTable(), g_NoPredicates ) );
  FormulaNegP l_pForm2( new FormulaNeg( "( not ( in-truck ?pack truck3 ) )", TypeTable(), g_NoPredicates ) );
  FormulaPredP l_pFormTemp( new FormulaPred( "( at truck3 loc2 dummy )", TypeTable(), g_NoPredicates ) );

  //  assert( myFormulaPLess( l_pForm1, l_pForm2 ) );
  //  assert( !myFormulaPLess( l_pForm1, l_pFormTemp ) );
  //  assert( !myFormulaPLess( l_pForm2, l_pForm1 ) );
  //  assert( !myFormulaPLess( l_pForm2, l_pFormTemp ) );
  //  assert( myFormulaPLess( l_pFormTemp, l_pForm1 ) );
  //  assert( myFormulaPLess( l_pFormTemp, l_pForm2 ) );

  FormulaConjP l_pForm3( new FormulaConj( " ( and ( not ( in-truck ?pack truck3 ) ) ( in-truck ?pack truck3 ) ( at truck3 loc2 dummy ) )", TypeTable(), g_NoPredicates ) );

  assert( CompareNoCase( l_pForm1->GetRelation(), "IN-TRUCK" ) == 0 );
  assert( l_pForm1->GetValence() == 2 );
  assert( l_pForm1->GetCParam( 0 )->GetType() == TT_VARIABLE );
  assert( CompareNoCase( l_pForm1->GetCParam( 0 )->ToStr(), "?PACK" ) == 0 );
  assert( l_pForm1->GetCParam( 1 )->GetType() == TT_CONSTANT );
  assert( CompareNoCase( l_pForm1->GetCParam( 1 )->ToStr(), "TRUCK3" ) == 0 );

  FormulaPredP l_pForm4( new FormulaPred( l_pForm1->ToStr(), TypeTable(), g_NoPredicates ) );
  assert( *l_pForm4 == *l_pForm1 );

  assert( *l_pForm1 == *l_pForm2->GetCNegForm() );

  FormulaNegP l_pForm5( new FormulaNeg( l_pForm2->ToStr(), TypeTable(), g_NoPredicates ) );
  assert( *l_pForm5 == *l_pForm2 );

  bool l_bFoundNot = false,
    l_bFoundIn = false,
    l_bFoundAt = false;

  for( FormulaPVecCI i = l_pForm3->GetBeginConj();
       i != l_pForm3->GetEndConj();
       i++ )
  {
    if( (*i)->GetType() == FT_NEG )
    {
      assert( !l_bFoundNot );
      l_bFoundNot = true;
      assert( *(*i) == *l_pForm2 );
    }
    else if( (*i)->GetType() == FT_PRED &&
	     CompareNoCase( std::tr1::dynamic_pointer_cast< FormulaPred >( (*i) )->GetRelation(), "in-truck" ) == 0 )
    {
      assert( !l_bFoundIn );
      l_bFoundIn = true;
      assert( *(*i) == *l_pForm1 );
    }
    else if( (*i)->GetType() == FT_PRED &&
	     CompareNoCase( std::tr1::dynamic_pointer_cast< FormulaPred >( *i )->GetRelation(), "at" ) == 0 )
    {
      assert( !l_bFoundAt );
      l_bFoundAt = true;
      assert( std::tr1::dynamic_pointer_cast< FormulaPred >( *i )->GetValence() == 3 );
    }
    else
      assert( false );
  }

  assert( l_bFoundNot && l_bFoundIn && l_bFoundAt );

  FormulaConjP l_pForm6( new FormulaConj( l_pForm3->ToStr(), TypeTable(), g_NoPredicates ) );
  assert( *l_pForm6 == *l_pForm3 );

  assert( l_pForm1->GetVariables().size() == 1 );
  assert( CompareNoCase( l_pForm1->GetVariables()[0]->ToStr(), "?PACK" ) == 0 );
  assert( l_pForm2->GetVariables().size() == 1 );
  assert( CompareNoCase( l_pForm2->GetVariables()[0]->ToStr(), "?PACK" ) == 0 );
  assert( l_pForm3->GetVariables().size() == 1 );
  assert( CompareNoCase( l_pForm3->GetVariables()[0]->ToStr(), "?PACK" ) == 0 );
}

void TestSubstitutionCreation()
{
  Substitution l_Subs1;
  assert( l_Subs1.GetNumPairs() == 0 );

  TermP l_pConst1 = g_TermTable.Lookup( "asdf" );
  TermP l_pVar1 = g_TermTable.Lookup( "?t1" );
  TermP l_pConst2 = g_TermTable.Lookup( "qwer" );
  TermP l_pVar2 = g_TermTable.Lookup( "?t2" );

  // These should be deleted automatically.
  l_Subs1.AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( l_pVar1 ), l_pConst1 );

  assert( l_Subs1.GetNumPairs() == 1 );
  assert( CompareNoCase( l_Subs1.FindIndexByVar( std::tr1::dynamic_pointer_cast< TermVariable >( g_TermTable.Lookup( "?T1" ) ) )->second->ToStr(), "ASDF" ) == 0 );


  // These can be deleted immediately.
  l_Subs1.AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( l_pVar2 ), l_pConst2 );

  assert( l_Subs1.GetNumPairs() == 2 );
  assert( CompareNoCase( l_Subs1.FindIndexByVar( std::tr1::dynamic_pointer_cast< TermVariable >( g_TermTable.Lookup( "?T2" ) ) )->second->ToStr(), "QWER" ) == 0 );

  l_pVar1 = g_TermTable.Lookup( "?t1" );
  l_pVar2 = g_TermTable.Lookup( "?t2" );
  TermVariableP l_pVar3 = std::tr1::dynamic_pointer_cast< TermVariable >( g_TermTable.Lookup( "?t3" ) );
  l_pConst1 = g_TermTable.Lookup( "asdf" );
  l_pConst2 = g_TermTable.Lookup( "qwer" );
  TermConstantP l_pConst3 = std::tr1::dynamic_pointer_cast< TermConstant >( g_TermTable.Lookup( "zxcv" ) );

  assert( l_Subs1.FindIndexByVar( std::tr1::dynamic_pointer_cast< TermVariable >( l_pVar1 ) ) != l_Subs1.End() );
  assert( l_Subs1.FindIndexByVar( std::tr1::dynamic_pointer_cast< TermVariable >( l_pVar2 ) ) != l_Subs1.End() );
  assert( l_Subs1.FindIndexByVar( l_pVar3 ) == l_Subs1.End() );
  assert( l_Subs1.FindIndexByTerm( l_pConst3 ) == l_Subs1.End() );
  assert( l_Subs1.FindIndexByTerm( l_pConst2 ) != l_Subs1.End() );
  assert( l_Subs1.FindIndexByTerm( l_pConst1 ) != l_Subs1.End() );
  assert( l_Subs1.FindIndexByTerm( l_pVar1 ) == l_Subs1.End() );

  l_Subs1.RemovePair( l_Subs1.FindIndexByVar( std::tr1::dynamic_pointer_cast< TermVariable >( l_pVar1 ) ) );

  assert( l_Subs1.GetNumPairs() == 1 );
  assert( CompareNoCase( l_Subs1.FindIndexByVar( std::tr1::dynamic_pointer_cast< TermVariable >( l_pVar2 ) )->second->ToStr(), "QWER" ) == 0 );

  l_Subs1.ReplaceTerm( l_pConst2, l_pConst3 );

  assert( l_Subs1.GetNumPairs() == 1 );
  assert( CompareNoCase( l_Subs1.FindIndexByVar( std::tr1::dynamic_pointer_cast< TermVariable >( l_pVar2 ) )->second->ToStr(), "ZXCV" ) == 0 );

  l_Subs1.AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( l_pVar1 ), l_pConst3 );

  assert( l_Subs1.GetNumPairs() == 2 );
  assert( CompareNoCase( l_Subs1.FindIndexByVar( std::tr1::dynamic_pointer_cast< TermVariable >( l_pVar2 ) )->second->ToStr(), "ZXCV" ) == 0 );
  assert( CompareNoCase( l_Subs1.FindIndexByVar( std::tr1::dynamic_pointer_cast< TermVariable >( l_pVar1 ) )->second->ToStr(), "ZXCV" ) == 0 );

  l_Subs1.ReplaceTerm( l_pVar1, l_pVar2 );

  assert( l_Subs1.GetNumPairs() == 1 );
  assert( CompareNoCase( l_Subs1.FindIndexByVar( std::tr1::dynamic_pointer_cast< TermVariable >( l_pVar2 ) )->second->ToStr(), "ZXCV" ) == 0 );

  l_Subs1.AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( l_pVar1 ), l_pVar3 );

  assert( l_Subs1.GetNumPairs() == 2 );
  assert( CompareNoCase( l_Subs1.FindIndexByVar( std::tr1::dynamic_pointer_cast< TermVariable >( l_pVar2 ) )->second->ToStr(), "ZXCV" ) == 0 );
  assert( CompareNoCase( l_Subs1.FindIndexByVar( std::tr1::dynamic_pointer_cast< TermVariable >( l_pVar1 ) )->second->ToStr(), "?T3" ) == 0 );

  l_Subs1.ReplaceTerm( l_pVar3, l_pVar1 );

  assert( l_Subs1.GetNumPairs() == 1 );
  assert( CompareNoCase( l_Subs1.FindIndexByVar( std::tr1::dynamic_pointer_cast< TermVariable >( l_pVar2 ) )->second->ToStr(), "ZXCV" ) == 0 );

  l_Subs1.AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( l_pVar1 ), l_pVar3 );

  assert( l_Subs1.GetNumPairs() == 2 );
  assert( CompareNoCase( l_Subs1.FindIndexByVar( std::tr1::dynamic_pointer_cast< TermVariable >( l_pVar2 ) )->second->ToStr(), "ZXCV" ) == 0 );
  assert( CompareNoCase( l_Subs1.FindIndexByVar( std::tr1::dynamic_pointer_cast< TermVariable >( l_pVar1 ) )->second->ToStr(), "?T3" ) == 0 );

  l_Subs1.ReplaceTerm( l_pVar1, l_pVar3 );

  assert( l_Subs1.GetNumPairs() == 1 );
  assert( CompareNoCase( l_Subs1.FindIndexByVar( std::tr1::dynamic_pointer_cast< TermVariable >( l_pVar2 ) )->second->ToStr(), "ZXCV" ) == 0 );
}

void TestTermAfterSubstitution()
{
  TermP l_pVar1 = g_TermTable.Lookup( "?t1" );
  TermP l_pVar2 = g_TermTable.Lookup( "?l1" );
  TermP l_pConst1 = g_TermTable.Lookup( "truck1" );
  TermP l_pConst2 = g_TermTable.Lookup( "truck2" );
  TermP l_pConst3 = g_TermTable.Lookup( "loc1" );

  Substitution l_Subs1;
  l_Subs1.AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( l_pVar1 ), l_pConst1 );
  l_Subs1.AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( l_pVar2 ), l_pConst2 );
  Substitution l_Subs2;

  TermP l_pTemp1 = l_pVar1->AfterSubstitution( l_Subs2, 0 );
  assert( CompareNoCase( l_pTemp1->ToStr(), "?T1" ) == 0 );
  l_pTemp1 = l_pVar1->AfterSubstitution( l_Subs1, 0 );
  assert( CompareNoCase( l_pTemp1->ToStr(), "TRUCK1" ) == 0 );
  l_pTemp1 = l_pVar2->AfterSubstitution( l_Subs2, 0 );
  assert( CompareNoCase( l_pTemp1->ToStr(), "?L1" ) == 0 );
  l_pTemp1 = l_pVar2->AfterSubstitution( l_Subs1, 0 );
  assert( CompareNoCase( l_pTemp1->ToStr(), "TRUCK2" ) == 0 );
  l_pTemp1 = l_pConst3->AfterSubstitution( l_Subs2, 0 );
  assert( CompareNoCase( l_pTemp1->ToStr(), "LOC1" ) == 0 );
  l_pTemp1 = l_pConst3->AfterSubstitution( l_Subs1, 0 );
  assert( CompareNoCase( l_pTemp1->ToStr(), "LOC1" ) == 0 );
}

void TestFormulaAfterSubstitution()
{
  TermVariableP l_pVar1 = std::tr1::dynamic_pointer_cast< TermVariable >( g_TermTable.Lookup( "?pack" ) );
  TermVariableP l_pVar2 = std::tr1::dynamic_pointer_cast< TermVariable >( g_TermTable.Lookup( "?truck" ) );
  TermConstantP l_pConst1 = std::tr1::dynamic_pointer_cast< TermConstant >( g_TermTable.Lookup( "truck3" ) );
  TermConstantP l_pConst2 = std::tr1::dynamic_pointer_cast< TermConstant >( g_TermTable.Lookup( "truck2" ) );
  TermConstantP l_pConst3 = std::tr1::dynamic_pointer_cast< TermConstant >( g_TermTable.Lookup( "p0" ) );

  FormulaPredP l_pForm1( new FormulaPred( "( in-truck p0 ?truck )", TypeTable(), g_NoPredicates ) );
  FormulaNegP l_pForm2( new FormulaNeg( "( not ( in-truck p0 truck3 ) )", TypeTable(), g_NoPredicates ) );
  FormulaConjP l_pForm3( new FormulaConj( " ( and ( not ( in-truck ?pack truck3 ) ) ( in-truck p0 ?truck ) ( at ?truck loc2 dummy ) )", TypeTable(), g_NoPredicates ) );

  assert( CompareNoCase( l_pForm1->GetRelation(), "IN-TRUCK" ) == 0 );
  assert( l_pForm1->GetValence() == 2 );
  assert( l_pForm1->GetCParam( 0 )->GetType() == TT_CONSTANT );
  assert( CompareNoCase( l_pForm1->GetCParam( 0 )->ToStr(), "P0" ) == 0 );
  assert( l_pForm1->GetCParam( 1 )->GetType() == TT_VARIABLE );
  assert( CompareNoCase( l_pForm1->GetCParam( 1 )->ToStr(), "?TRUCK" ) == 0 );

  Substitution l_Subs1;
  Substitution l_Subs2;
  l_Subs2.AddPair( l_pVar2, l_pConst1 );

  FormulaP l_pTemp1( l_pForm1->AfterSubstitution( l_Subs1, 0 ) );
  assert( *l_pTemp1 == *l_pForm1 );

  l_pTemp1 = l_pForm1->AfterSubstitution( l_Subs2, 0 );
  assert( CompareNoCase( std::tr1::dynamic_pointer_cast< FormulaPred >( l_pTemp1 )->GetRelation(), "IN-TRUCK" ) == 0 );
  assert( std::tr1::dynamic_pointer_cast< FormulaPred >( l_pTemp1 )->GetValence() == 2 );
  assert( std::tr1::dynamic_pointer_cast< FormulaPred >( l_pTemp1 )->GetCParam( 0 )->GetType() == TT_CONSTANT );
  assert( CompareNoCase( std::tr1::dynamic_pointer_cast< FormulaPred >( l_pTemp1 )->GetCParam( 0 )->ToStr(), "P0" ) == 0 );
  assert( std::tr1::dynamic_pointer_cast< FormulaPred >( l_pTemp1 )->GetCParam( 1 )->GetType() == TT_CONSTANT );
  assert( CompareNoCase( std::tr1::dynamic_pointer_cast< FormulaPred >( l_pTemp1 )->GetCParam( 1 )->ToStr(), "TRUCK3" ) == 0 );
  

  FormulaP l_pTemp2( l_pForm2->AfterSubstitution( l_Subs1, 0 ) );
  assert( *l_pTemp2 == *l_pForm2 );

  l_pTemp2 = l_pForm2->AfterSubstitution( l_Subs2, 0 );
  assert( *l_pTemp2 == *l_pForm2 );
  assert( *std::tr1::dynamic_pointer_cast< FormulaNeg >( l_pTemp2 )->GetCNegForm() == *l_pTemp1 );

  FormulaP l_pTemp3( l_pForm3->AfterSubstitution( l_Subs1, 0 ) );
  assert( *l_pTemp3 == *l_pForm3 );

  l_pTemp3 = l_pForm3->AfterSubstitution( l_Subs2, 0 );

  assert( !l_pForm1->IsGround() );
  assert( l_pTemp1->IsGround() );
  assert( l_pForm2->IsGround() );
  assert( l_pTemp2->IsGround() );
  assert( !l_pForm3->IsGround() );
  assert( !l_pTemp3->IsGround() );
}

void TestFormulaImplication()
{
  TypeTable l_TypeTable;
  FormulaP l_pForms[14] = { 
    FormulaP( new FormulaPred( "( in-truck p1 t2 )", l_TypeTable, g_NoPredicates ) ),
    FormulaP( new FormulaPred( "( at t2 l00 )", l_TypeTable, g_NoPredicates ) ),
    FormulaP( new FormulaPred( "( at p1 l10 )", l_TypeTable, g_NoPredicates ) ),
    FormulaP( new FormulaNeg( "( not ( in-truck p1 t2 ) )", l_TypeTable, g_NoPredicates ) ),
    FormulaP( new FormulaNeg( "( not ( at t2 l00 ) )", l_TypeTable, g_NoPredicates ) ),
    FormulaP( new FormulaNeg( "( not ( at p1 l10 ) )", l_TypeTable, g_NoPredicates ) ),
    FormulaP( new FormulaConj( "( and ( in-truck p1 t2 ) )", l_TypeTable, g_NoPredicates ) ),
    FormulaP( new FormulaConj( "( and ( not ( in-truck p1 t2 ) ) )", l_TypeTable, g_NoPredicates ) ),
    FormulaP( new FormulaConj( "( and ( at t2 l00 ) )", l_TypeTable, g_NoPredicates ) ),
    FormulaP( new FormulaConj( "( and ( not ( at t2 l00 ) ) )", l_TypeTable, g_NoPredicates ) ),
    FormulaP( new FormulaConj( "( and ( at p1 l10 ) )", l_TypeTable, g_NoPredicates ) ),
    FormulaP( new FormulaConj( "( and ( not ( at p1 l10 ) ) )", l_TypeTable, g_NoPredicates ) ),
    FormulaP( new FormulaConj( "( and ( in-truck p1 t2 ) ( at t2 l00 ) )", l_TypeTable, g_NoPredicates ) ),
    FormulaP( new FormulaConj( "( and ( in-truck p1 t2 ) ( at t2 l00 ) ( not ( at p1 l10 ) ) )", l_TypeTable, g_NoPredicates ) ) };

  for( int i = 0; i < 14; i++ )
  {
    for( int j = 0; j < 14; j++ )
    {
      bool l_bImp = l_pForms[i]->Implies( l_pForms[j] );

      if( i == j ||
	  ( i == 0 && j == 6 ) ||
	  ( i == 1 && j == 8 ) ||
	  ( i == 2 && j == 10 ) ||
	  ( i == 3 && j == 7 ) ||
	  ( i == 4 && j == 9 ) ||
	  ( i == 5 && j == 11 ) ||
	  ( i == 6 && j == 0 ) ||
	  ( i == 7 && j == 3 ) ||
	  ( i == 8 && j == 1 ) ||
	  ( i == 9 && j == 4 ) ||
	  ( i == 10 && j == 2 ) ||
	  ( i == 11 && j == 5 ) ||
	  ( i == 12 && j == 0 ) ||
	  ( i == 12 && j == 1 ) ||
	  ( i == 12 && j == 6 ) ||
	  ( i == 12 && j == 8 ) ||
	  ( i == 13 && j == 0 ) ||
	  ( i == 13 && j == 1 ) ||
	  ( i == 13 && j == 5 ) ||
	  ( i == 13 && j == 6 ) ||
	  ( i == 13 && j == 8 ) ||
	  ( i == 13 && j == 11 ) ||
	  ( i == 13 && j == 12 ) )
	assert( l_bImp == true );
      else
	assert( l_bImp == false );
    }
  }

}

void TestOperatorCreation()
{
  std::string l_sStr = "(:action !DRIVE-TRUCK :parameters (?truck ?loc-from ?loc-to ?city) :precondition (and (TRUCK ?truck) (LOCATION ?loc-from) (LOCATION ?loc-to) (CITY ?city) (at ?truck ?loc-from) (in-city ?loc-from ?city) (in-city ?loc-to ?city)) :effect (and (not (at ?truck ?loc-from)) (at ?truck ?loc-to)))";
  std::stringstream l_sStream( l_sStr );
  Operator * l_pOper = Operator::FromPddl( l_sStream, std::set< std::string, StrLessNoCase >(), std::vector< FormulaPred >() );

  assert( l_pOper->GetName() == "!DRIVE-TRUCK" );
  assert( l_pOper->GetNumParams() == 4 );
  assert( l_pOper->GetCParam( 0 )->GetType() == TT_VARIABLE );
  assert( CompareNoCase( l_pOper->GetCParam( 0 )->ToStr(), "?TRUCK" ) == 0 );
  assert( l_pOper->GetCParam( 1 )->GetType() == TT_VARIABLE );
  assert( CompareNoCase( l_pOper->GetCParam( 1 )->ToStr(), "?LOC-FROM" ) == 0 );
  assert( l_pOper->GetCParam( 2 )->GetType() == TT_VARIABLE );
  assert( CompareNoCase( l_pOper->GetCParam( 2 )->ToStr(), "?LOC-TO" ) == 0 );
  assert( l_pOper->GetCParam( 3 )->GetType() == TT_VARIABLE );
  assert( CompareNoCase( l_pOper->GetCParam( 3 )->ToStr(), "?CITY" ) == 0 );

  assert( l_pOper->GetCPreconditions()->GetType() == FT_CONJ );
  
  FormulaConjP l_pPreconditions = std::tr1::dynamic_pointer_cast< FormulaConj >( l_pOper->GetCPreconditions() );
  bool l_bTruck = false,
    l_bLocFrom = false,
    l_bLocTo = false,
    l_bCity = false,
    l_bAt = false,
    l_bFromIn = false,
    l_bToIn = false;
  for( FormulaPVecCI i = l_pPreconditions->GetBeginConj();
       i != l_pPreconditions->GetEndConj();
       i++ )
  {
    assert( (*i)->GetType() == FT_PRED );
    FormulaPredP l_pThisPred = std::tr1::dynamic_pointer_cast< FormulaPred >( *i );

    if( CompareNoCase( l_pThisPred->GetRelation(), "truck" ) == 0 )
    {
      assert( !l_bTruck );
      l_bTruck = true;
      assert( l_pThisPred->GetValence() == 1 );
      assert( l_pThisPred->GetCParam( 0 )->GetType() == TT_VARIABLE );
      assert( CompareNoCase( l_pThisPred->GetCParam( 0 )->ToStr(), "?truck" ) == 0 );
    }
    else if( CompareNoCase( l_pThisPred->GetRelation(), "location" ) == 0 )
    {
      assert( l_pThisPred->GetValence() == 1 );
      assert( l_pThisPred->GetCParam( 0 )->GetType() == TT_VARIABLE );
      if( CompareNoCase( l_pThisPred->GetCParam( 0 )->ToStr(), "?loc-from" ) == 0 )
      {
	assert( !l_bLocFrom );
	l_bLocFrom = true;
      }
      else if( CompareNoCase( l_pThisPred->GetCParam( 0 )->ToStr(), "?loc-to" ) == 0 )
      {
	assert( !l_bLocTo );
	l_bLocTo = true;
      }
      else
	assert( false );
    }
    else if( CompareNoCase( l_pThisPred->GetRelation(), "city" ) == 0 )
    {
      assert( !l_bCity );
      l_bCity = true;
      assert( l_pThisPred->GetValence() == 1 );
      assert( l_pThisPred->GetCParam( 0 )->GetType() == TT_VARIABLE );
      assert( CompareNoCase( l_pThisPred->GetCParam( 0 )->ToStr(), "?city" ) == 0 );
    }
    else if( CompareNoCase( l_pThisPred->GetRelation(), "at" ) == 0 )
    {
      assert( !l_bAt );
      l_bAt = true;
      assert( l_pThisPred->GetValence() == 2 );
      assert( l_pThisPred->GetCParam( 0 )->GetType() == TT_VARIABLE );
      assert( CompareNoCase( l_pThisPred->GetCParam( 0 )->ToStr(), "?truck" ) == 0 );
      assert( l_pThisPred->GetCParam( 1 )->GetType() == TT_VARIABLE );
      assert( CompareNoCase( l_pThisPred->GetCParam( 1 )->ToStr(), "?loc-from" ) == 0 );
    }
    else if( CompareNoCase( l_pThisPred->GetRelation(), "in-city" ) == 0 )
    {
      assert( l_pThisPred->GetValence() == 2 );
      assert( l_pThisPred->GetCParam( 0 )->GetType() == TT_VARIABLE );
      assert( l_pThisPred->GetCParam( 1 )->GetType() == TT_VARIABLE );
      assert( CompareNoCase( l_pThisPred->GetCParam( 1 )->ToStr(), "?city" ) == 0 );
      if( CompareNoCase( l_pThisPred->GetCParam( 0 )->ToStr(), "?loc-from" ) == 0 )
      {
	assert( !l_bFromIn );
	l_bFromIn = true;
      }
      else if( CompareNoCase( l_pThisPred->GetCParam( 0 )->ToStr(), "?loc-to" ) == 0 )
      {
	assert( !l_bToIn );
	l_bToIn = true;
      }
      else
	assert( false );
    }
    else
      assert( false );
  }

  assert( l_bTruck && l_bLocFrom && l_bLocTo && l_bCity && l_bAt && l_bFromIn && l_bToIn );


  assert( l_pOper->GetCEffects()->GetType() == FT_CONJ );

  FormulaConjP l_pEffects = std::tr1::dynamic_pointer_cast< FormulaConj >( l_pOper->GetCEffects() );
  bool l_bNot = false;
  l_bAt = false;

  for( FormulaPVecCI i = l_pEffects->GetBeginConj();
       i != l_pEffects->GetEndConj();
       i++ )
  {
    if( (*i)->GetType() == FT_NEG )
    {
      assert( !l_bNot );
      l_bNot = true;
      assert( std::tr1::dynamic_pointer_cast< FormulaNeg >( *i )->GetCNegForm()->GetType() == FT_PRED );
      assert( CompareNoCase( std::tr1::dynamic_pointer_cast< FormulaPred >( std::tr1::dynamic_pointer_cast< FormulaNeg >( *i )->GetCNegForm() )->GetRelation(), "at" ) == 0 );
      assert( std::tr1::dynamic_pointer_cast< FormulaPred >( std::tr1::dynamic_pointer_cast< FormulaNeg >( *i )->GetCNegForm() )->GetValence() == 2 );
      assert( std::tr1::dynamic_pointer_cast< FormulaPred >( std::tr1::dynamic_pointer_cast< FormulaNeg >( *i )->GetCNegForm() )->GetCParam( 0 )->GetType() == TT_VARIABLE );
      assert( CompareNoCase( std::tr1::dynamic_pointer_cast< FormulaPred >( std::tr1::dynamic_pointer_cast< FormulaNeg >( *i )->GetCNegForm() )->GetCParam( 0 )->ToStr(), "?truck" ) == 0 );
      assert( std::tr1::dynamic_pointer_cast< FormulaPred >( std::tr1::dynamic_pointer_cast< FormulaNeg >( *i )->GetCNegForm() )->GetCParam( 1 )->GetType() == TT_VARIABLE );
      assert( CompareNoCase( std::tr1::dynamic_pointer_cast< FormulaPred >( std::tr1::dynamic_pointer_cast< FormulaNeg >( *i )->GetCNegForm() )->GetCParam( 1 )->ToStr(), "?loc-from" ) == 0 );
    }
    else if( (*i)->GetType() == FT_PRED )
    {
      assert( !l_bAt );
      l_bAt = true;
      assert( CompareNoCase( std::tr1::dynamic_pointer_cast< FormulaPred >( *i )->GetRelation(), "at" ) == 0 );
      assert( std::tr1::dynamic_pointer_cast< FormulaPred >( *i )->GetValence() == 2 );
      assert( std::tr1::dynamic_pointer_cast< FormulaPred >( *i )->GetCParam( 0 )->GetType() == TT_VARIABLE );
      assert( CompareNoCase( std::tr1::dynamic_pointer_cast< FormulaPred >( *i )->GetCParam( 0 )->ToStr(), "?truck" ) == 0 );
      assert( std::tr1::dynamic_pointer_cast< FormulaPred >( *i )->GetCParam( 1 )->GetType() == TT_VARIABLE );
      assert( CompareNoCase( std::tr1::dynamic_pointer_cast< FormulaPred >( *i )->GetCParam( 1 )->ToStr(), "?loc-to" ) == 0 );
    }
  }

  assert( l_bNot && l_bAt );

  delete l_pOper;
}

std::string g_sInitStateStr =
    "( "
    "(AIRPLANE a0) "
    "(CITY c0) "
    "(CITY c1) "
    "(CITY c2) "
    "(TRUCK t0) "
    "(TRUCK t1) "
    "(TRUCK t2) "
    "(LOCATION l00) "
    "(in-city l00 c0) "
    "(LOCATION l10) "
    "(in-city l10 c1) "
    "(LOCATION l20) "
    "(LOCATION l21) "
    "(in-city l21 c2) "
    "(in-city l20 c2) "
    "(AIRPORT l00) "
    "(AIRPORT l10) "
    "(AIRPORT l20) "
    "(OBJ p0) "
    "(at t0 l00) "
    "(at t1 l10) "
    "(at t2 l20) "
    "(at p0 l10) "
    "(at a0 l00) "
    ")";

void TestStateCreation()
{
  State * l_pInitState = new State( g_sInitStateStr, 0, TypeTable(), std::vector< FormulaPred >() );
  State * l_pOtherState = new State( *l_pInitState );

  assert( *l_pInitState == *l_pOtherState );
  assert( l_pInitState->GetStateNum() == 0 );

  delete l_pOtherState;
  delete l_pInitState;
}

void TestStateConsistency()
{
  TypeTable l_TypeTable;
  std::vector< FormulaPred > l_Formulas;
  State * l_pInitState = new State( g_sInitStateStr, 0, l_TypeTable, l_Formulas );

  FormulaP l_pForm1( new FormulaPred( "(at p0 l10)", l_TypeTable, g_NoPredicates ) );
  FormulaP l_pForm2( new FormulaPred( "(at p0 l20)", l_TypeTable, g_NoPredicates ) );
  FormulaP l_pForm3( new FormulaConj( "(and (in-city l20 c2) (at t0 l00))", l_TypeTable, g_NoPredicates ) );
  FormulaP l_pForm4( new FormulaNeg( "(not (in-city l20 c3))", l_TypeTable, g_NoPredicates ) );
  FormulaP l_pForm5( new FormulaNeg( "(not (OBJ p0))", l_TypeTable, g_NoPredicates ) );
  FormulaP l_pForm6( new FormulaPred( "(location l20)", l_TypeTable, g_NoPredicates ) );
  FormulaP l_pForm7( new FormulaPred( "(at ?p ?l )", l_TypeTable, g_NoPredicates ) );
  FormulaP l_pForm8( new FormulaPred( "(dummy ?dummy ?dummy ?dummy ?dummy)", l_TypeTable, g_NoPredicates ) );
  FormulaP l_pForm9( new FormulaConj( "(and (at a0 l00) (at p0 ?l) )", l_TypeTable, g_NoPredicates ) );
  FormulaP l_pForm10( new FormulaConj( "(and (at a0 l10) (at p0 ?l) )", l_TypeTable, g_NoPredicates ) );
  FormulaP l_pForm11( new FormulaConj( "(and (at a0 l00) (not (at p0 ?l) ) )", l_TypeTable, g_NoPredicates ) );
  FormulaP l_pForm12( new FormulaConj( "(and (not (at a0 l00) ) (at p0 ?l) )", l_TypeTable, g_NoPredicates ) );
  FormulaP l_pForm13( new FormulaConj( "(and (not (at a0 l10) ) (at p0 ?l) )", l_TypeTable, g_NoPredicates ) );

  assert( l_pInitState->IsConsistent( l_pForm1 ) );
  assert( l_pInitState->CouldBeConsistent( l_pForm1 ) );
  assert( !l_pInitState->IsConsistent( l_pForm2 ) );
  assert( !l_pInitState->CouldBeConsistent( l_pForm2 ) );
  assert( l_pInitState->IsConsistent( l_pForm3 ) );
  assert( l_pInitState->CouldBeConsistent( l_pForm3 ) );
  assert( l_pInitState->IsConsistent( l_pForm4 ) );
  assert( l_pInitState->CouldBeConsistent( l_pForm4 ) );
  assert( !l_pInitState->IsConsistent( l_pForm5 ) );
  assert( !l_pInitState->CouldBeConsistent( l_pForm5 ) );
  assert( l_pInitState->IsConsistent( l_pForm6 ) );
  assert( l_pInitState->CouldBeConsistent( l_pForm6 ) );
  assert( !l_pInitState->IsConsistent( l_pForm7 ) );
  assert( l_pInitState->CouldBeConsistent( l_pForm7 ) );
  assert( !l_pInitState->IsConsistent( l_pForm8 ) );
  assert( !l_pInitState->CouldBeConsistent( l_pForm8 ) );
  assert( !l_pInitState->IsConsistent( l_pForm9 ) );
  assert( l_pInitState->CouldBeConsistent( l_pForm9 ) );
  assert( !l_pInitState->IsConsistent( l_pForm10 ) );
  assert( !l_pInitState->CouldBeConsistent( l_pForm10 ) );
  assert( !l_pInitState->IsConsistent( l_pForm11 ) );
  assert( l_pInitState->CouldBeConsistent( l_pForm11 ) );
  assert( !l_pInitState->IsConsistent( l_pForm12 ) );
  assert( !l_pInitState->CouldBeConsistent( l_pForm12 ) );
  assert( !l_pInitState->IsConsistent( l_pForm13 ) );
  assert( l_pInitState->CouldBeConsistent( l_pForm13 ) );

  delete l_pInitState;
}

void TestStateGetInstantiations()
{
  State * l_pInitState = new State( g_sInitStateStr, 0, TypeTable(), std::vector< FormulaPred >() );
  std::stringstream l_sOpStream( "(:action !DRIVE-TRUCK :parameters (?truck ?loc-from ?loc-to ?city) :precondition (and (TRUCK ?truck) (LOCATION ?loc-from) (LOCATION ?loc-to) (CITY ?city) (at ?truck ?loc-from) (in-city ?loc-from ?city) (in-city ?loc-to ?city)) :effect (and (not (at ?truck ?loc-from)) (at ?truck ?loc-to)))" );
  Operator * l_pOp = Operator::FromPddl( l_sOpStream, std::set< std::string, StrLessNoCase >(), std::vector< FormulaPred >() );

  Substitution l_Subs;
  std::vector<Substitution *> * l_pSubs = l_pInitState->GetInstantiations( l_pOp, &l_Subs );

  assert( l_pSubs->size() == 4 );

  assert( l_pSubs->at( 0 )->GetNumPairs() == 4 );
  assert( l_pSubs->at( 1 )->GetNumPairs() == 4 );
  assert( l_pSubs->at( 2 )->GetNumPairs() == 4 );
  assert( l_pSubs->at( 3 )->GetNumPairs() == 4 );

  bool l_bL00 = false;
  bool l_bL10 = false;
  bool l_bL20 = false;
  bool l_bL21 = false;
  TermVariableP l_pVarLocTo( std::tr1::dynamic_pointer_cast< TermVariable >( g_TermTable.Lookup( "?LOC-TO" ) ) );
  TermVariableP l_pVarLocFrom( std::tr1::dynamic_pointer_cast< TermVariable >( g_TermTable.Lookup( "?LOC-FROM" ) ) );
  TermVariableP l_pVarTruck( std::tr1::dynamic_pointer_cast< TermVariable >( g_TermTable.Lookup( "?TRUCK" ) ) );
  TermVariableP l_pVarCity( std::tr1::dynamic_pointer_cast< TermVariable >( g_TermTable.Lookup( "?CITY" ) ) );
  for( int i = 0; i < 4; i ++ )
  {
    assert( l_pSubs->at( i )->FindIndexByVar( l_pVarLocTo ) != l_pSubs->at( i )->End() );
    if( CompareNoCase( l_pSubs->at( i )->FindIndexByVar( l_pVarLocTo )->second->ToStr(), "L00" ) == 0 )
    {
      assert( !l_bL00 );
      l_bL00 = true;
      assert( CompareNoCase( l_pSubs->at( i )->FindIndexByVar( l_pVarLocFrom )->second->ToStr(), "L00" ) == 0 );
      assert( CompareNoCase( l_pSubs->at( i )->FindIndexByVar( l_pVarTruck )->second->ToStr(), "T0" ) == 0 );
      assert( CompareNoCase( l_pSubs->at( i )->FindIndexByVar( l_pVarCity )->second->ToStr(), "C0" ) == 0 );
    }
    else if( CompareNoCase( l_pSubs->at( i )->FindIndexByVar( l_pVarLocTo )->second->ToStr(), "L10" ) == 0 )
    {
      assert( !l_bL10 );
      l_bL10 = true;
      assert( CompareNoCase( l_pSubs->at( i )->FindIndexByVar( l_pVarLocFrom )->second->ToStr(), "L10" ) == 0 );
      assert( CompareNoCase( l_pSubs->at( i )->FindIndexByVar( l_pVarTruck )->second->ToStr(), "T1" ) == 0 );
      assert( CompareNoCase( l_pSubs->at( i )->FindIndexByVar( l_pVarCity )->second->ToStr(), "C1" ) == 0 );
    }
    else if( CompareNoCase( l_pSubs->at( i )->FindIndexByVar( l_pVarLocTo )->second->ToStr(), "L20" ) == 0 )
    {
      assert( !l_bL20 );
      l_bL20 = true;
      assert( CompareNoCase( l_pSubs->at( i )->FindIndexByVar( l_pVarLocFrom )->second->ToStr(), "L20" ) == 0 );
      assert( CompareNoCase( l_pSubs->at( i )->FindIndexByVar( l_pVarTruck )->second->ToStr(), "T2" ) == 0 );
      assert( CompareNoCase( l_pSubs->at( i )->FindIndexByVar( l_pVarCity )->second->ToStr(), "C2" ) == 0 );
    }
    else if( CompareNoCase( l_pSubs->at( i )->FindIndexByVar( l_pVarLocTo )->second->ToStr(), "L21" ) == 0 )
    {
      assert( !l_bL21 );
      l_bL21 = true;
      assert( CompareNoCase( l_pSubs->at( i )->FindIndexByVar( l_pVarLocFrom )->second->ToStr(), "L20" ) == 0 );
      assert( CompareNoCase( l_pSubs->at( i )->FindIndexByVar( l_pVarTruck )->second->ToStr(), "T2" ) == 0 );
      assert( CompareNoCase( l_pSubs->at( i )->FindIndexByVar( l_pVarCity )->second->ToStr(), "C2" ) == 0 );
    }
  }

  for( unsigned int i = 0; i < l_pSubs->size(); i++ )
    delete (*l_pSubs)[i];
  delete l_pSubs;

  delete l_pOp;
  delete l_pInitState;
}

void TestNextState()
{
  State * l_pInitState = new State( g_sInitStateStr, 0, TypeTable(), std::vector< FormulaPred >() );
  std::stringstream l_sOpStream( "(:action !FLY-AIRPLANE :parameters (?airplane ?loc-from ?loc-to) :precondition (and (AIRPLANE ?airplane) (AIRPORT ?loc-from) (AIRPORT ?loc-to) (at ?airplane ?loc-from)) :effect (and (not (at ?airplane ?loc-from)) (at ?airplane ?loc-to)))" );
  Operator * l_pOp = Operator::FromPddl( l_sOpStream, std::set< std::string, StrLessNoCase >(), std::vector< FormulaPred >() );

  Substitution l_Subs;
  std::vector<Substitution *> * l_pSubs = l_pInitState->GetInstantiations( l_pOp, &l_Subs );

  assert( l_pSubs->size() == 3 );

  State * l_pOption1 = l_pInitState->NextState( l_pOp, (*l_pSubs)[0] );
  State * l_pOption2 = l_pInitState->NextState( l_pOp, (*l_pSubs)[1] );
  State * l_pOption3 = l_pInitState->NextState( l_pOp, (*l_pSubs)[2] );

  assert( l_pOption1->GetStateNum() == 1 );
  assert( l_pOption2->GetStateNum() == 1 );
  assert( l_pOption3->GetStateNum() == 1 );

  assert( l_pOption1->GetNumAtoms() == l_pInitState->GetNumAtoms() );
  assert( l_pOption2->GetNumAtoms() == l_pInitState->GetNumAtoms() );
  assert( l_pOption3->GetNumAtoms() == l_pInitState->GetNumAtoms() );

  FormulaP l_pForm0( new FormulaPred( "(at a0 l00)", TypeTable(), g_NoPredicates ) );
  FormulaP l_pForm1( new FormulaPred( "(at a0 l10)", TypeTable(), g_NoPredicates ) );
  FormulaP l_pForm2( new FormulaPred( "(at a0 l20)", TypeTable(), g_NoPredicates ) );

  assert( l_pOption1->IsConsistent( l_pForm0 ) );
  assert( !l_pOption1->IsConsistent( l_pForm1 ) );
  assert( !l_pOption1->IsConsistent( l_pForm2 ) );
  assert( !l_pOption2->IsConsistent( l_pForm0 ) );
  assert( l_pOption2->IsConsistent( l_pForm1 ) );
  assert( !l_pOption2->IsConsistent( l_pForm2 ) );
  assert( !l_pOption3->IsConsistent( l_pForm0 ) );
  assert( !l_pOption3->IsConsistent( l_pForm1 ) );
  assert( l_pOption3->IsConsistent( l_pForm2 ) );

  delete l_pOption3;
  delete l_pOption2;
  delete l_pOption1;

  for( unsigned int i = 0; i < l_pSubs->size(); i++ )
    delete (*l_pSubs)[i];
  delete l_pSubs;

  delete l_pOp;
  delete l_pInitState;
}

void TestReadSTRIPSDomainFile()
{
  StripsDomain * l_pDomain = new StripsDomain( ReadFile( "tests/test-domain.pddl" ) );
  assert( l_pDomain->GetName() == "logistics-strips" );
  assert( l_pDomain->GetNumOpers() == 6 );
  assert( l_pDomain->GetCOper( 0 )->GetName() == "!LOAD-TRUCK" );
  assert( l_pDomain->GetCOper( 1 )->GetName() == "!LOAD-AIRPLANE" );
  assert( l_pDomain->GetCOper( 2 )->GetName() == "!UNLOAD-TRUCK" );
  assert( l_pDomain->GetCOper( 3 )->GetName() == "!UNLOAD-AIRPLANE" );
  assert( l_pDomain->GetCOper( 4 )->GetName() == "!DRIVE-TRUCK" );
  assert( l_pDomain->GetCOper( 5 )->GetName() == "!FLY-AIRPLANE" );

  StripsDomain * l_pOther = new StripsDomain( *l_pDomain );

  assert( l_pOther->GetName() == "logistics-strips" );
  assert( l_pOther->GetNumOpers() == 6 );
  assert( l_pOther->GetCOper( 0 )->GetName() == "!LOAD-TRUCK" );
  assert( l_pOther->GetCOper( 1 )->GetName() == "!LOAD-AIRPLANE" );
  assert( l_pOther->GetCOper( 2 )->GetName() == "!UNLOAD-TRUCK" );
  assert( l_pOther->GetCOper( 3 )->GetName() == "!UNLOAD-AIRPLANE" );
  assert( l_pOther->GetCOper( 4 )->GetName() == "!DRIVE-TRUCK" );
  assert( l_pOther->GetCOper( 5 )->GetName() == "!FLY-AIRPLANE" );

  assert( l_pDomain->GetOperIndexByName( "!FLY-AIRPLANE" ) == 5 );
  assert( l_pDomain->GetOperIndexByName( "!UNLOAD-AIRPLANE" ) == 3 );
  assert( l_pDomain->GetOperIndexByName( "!LOAD-AIRPLANE" ) == 1 );
  assert( l_pDomain->GetOperIndexByName( "!LOAD-TRUCK" ) == 0 );
  assert( l_pDomain->GetOperIndexByName( "!UNLOAD-TRUCK" ) == 2 );
  assert( l_pDomain->GetOperIndexByName( "!DRIVE-TRUCK" ) == 4 );

  assert( l_pOther->GetOperIndexByName( "!FLY-AIRPLANE" ) == 5 );
  assert( l_pOther->GetOperIndexByName( "!UNLOAD-AIRPLANE" ) == 3 );
  assert( l_pOther->GetOperIndexByName( "!LOAD-AIRPLANE" ) == 1 );
  assert( l_pOther->GetOperIndexByName( "!LOAD-TRUCK" ) == 0 );
  assert( l_pOther->GetOperIndexByName( "!UNLOAD-TRUCK" ) == 2 );
  assert( l_pOther->GetOperIndexByName( "!DRIVE-TRUCK" ) == 4 );

  delete l_pOther;
  delete l_pDomain;
}

void TestReadSTRIPSProblemFile()
{
  std::tr1::shared_ptr< StripsDomain > l_pDomain( new StripsDomain( ReadFile( "tests/test-domain.pddl" ) ) );
  StripsProblem * l_pProblem = new StripsProblem( ReadFile( "tests/test-prob.pddl" ),
						  l_pDomain );
  StripsProblem * l_pProb2 = new StripsProblem( *l_pProblem );

  assert( l_pProblem->GetName() == "logistics-c1-s1-p2-a1" );
  assert( l_pProb2->GetName() == "logistics-c1-s1-p2-a1" );
  assert( l_pProblem->GetCDomain()->GetName() == "logistics-strips" );
  assert( l_pProb2->GetCDomain()->GetName() == "logistics-strips" );

  FormulaPredP l_pForm1( new FormulaPred( "( AIRPLANE a0 )", TypeTable(), g_NoPredicates ) );

  assert( l_pProblem->GetCInitState()->IsConsistent( std::tr1::dynamic_pointer_cast< Formula >( l_pForm1 ) ) );
  assert( l_pProb2->GetCInitState()->IsConsistent( std::tr1::dynamic_pointer_cast< Formula >( l_pForm1 ) ) );
  assert( CompareNoCase( l_pProblem->GetCGoals()->ToStr(), "( and ( AT P0 L00 ) ( AT P1 L00 ) )" ) == 0 );
  assert( CompareNoCase( l_pProb2->GetCGoals()->ToStr(), "( and ( AT P0 L00 ) ( AT P1 L00 ) )" ) == 0 );

  delete l_pProb2;
  delete l_pProblem;
}

void TestStripsSolution()
{
  std::tr1::shared_ptr< StripsDomain > l_pDomain( new StripsDomain( ReadFile( "tests/test-domain.pddl" ) ) );
  std::tr1::shared_ptr< StripsProblem > l_pProblem( new StripsProblem( ReadFile( "tests/test-prob2.pddl" ), l_pDomain ) );

  StripsSolution * l_pSolution = new StripsSolution( l_pProblem );

  assert( l_pSolution->GetCProblem()->GetName() == l_pProblem->GetName() );
  assert( l_pSolution->GetPlanLength() == 0 );
  assert( !l_pSolution->IsComplete() );
  assert( l_pSolution->ContainsState( *l_pProblem->GetCInitState() ) );

  Substitution * l_pSubs1 = new Substitution();
  l_pSubs1->AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( g_TermTable.Lookup( "?obj" ) ),
			 g_TermTable.Lookup( "p0" ) );
  l_pSubs1->AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( g_TermTable.Lookup( "?truck" ) ),
			 g_TermTable.Lookup( "t0" ) );
  l_pSubs1->AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( g_TermTable.Lookup( "?loc" ) ),
			 g_TermTable.Lookup( "l00" ) );
  
  assert( !l_pSolution->ApplyOperator( 0, l_pSubs1 ) );
  assert( l_pSolution->GetPlanLength() == 1 );
  assert( !l_pSolution->IsComplete() );
  assert( l_pSolution->ContainsState( *l_pSolution->GetCFollowState( 0 ) ) );

  StripsSolution * l_pSolution2 = new StripsSolution( *l_pSolution );

  Substitution * l_pSubs2 = new Substitution();
  l_pSubs2->AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( g_TermTable.Lookup( "?obj" ) ),
			 g_TermTable.Lookup( "p1" ) );
  l_pSubs2->AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( g_TermTable.Lookup( "?truck" ) ), 
			 g_TermTable.Lookup( "t0" ) );
  l_pSubs2->AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( g_TermTable.Lookup( "?loc" ) ),
			 g_TermTable.Lookup( "l00" ) );

  assert( !l_pSolution->ApplyOperator( 0, l_pSubs2 ) );
  assert( l_pSolution->GetPlanLength() == 2 );
  assert( !l_pSolution->IsComplete() );

  Substitution * l_pSubs3 = new Substitution();
  l_pSubs3->AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( g_TermTable.Lookup( "?truck" ) ),
			 g_TermTable.Lookup( "t0" ) );
  l_pSubs3->AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( g_TermTable.Lookup( "?loc-from" ) ),
			 g_TermTable.Lookup( "l00" ) );
  l_pSubs3->AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( g_TermTable.Lookup( "?loc-to" ) ),
			 g_TermTable.Lookup( "l01" ) );
  l_pSubs3->AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( g_TermTable.Lookup( "?city" ) ),
			 g_TermTable.Lookup( "c0" ) );

  assert( !l_pSolution2->ApplyOperator( 4, l_pSubs3 ) );
  assert( l_pSolution2->GetPlanLength() == 2 );
  assert( !l_pSolution2->IsComplete() );

  assert( !l_pSolution->ContainsState( *l_pSolution2->GetCFollowState( 1 ) ) );
  assert( !l_pSolution2->ContainsState( *l_pSolution->GetCFollowState( 1 ) ) );

  Substitution * l_pSubs4 = new Substitution();
  l_pSubs4->AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( g_TermTable.Lookup( "?truck" ) ),
			 g_TermTable.Lookup( "t0" ) );
  l_pSubs4->AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( g_TermTable.Lookup( "?loc-from" ) ),
			 g_TermTable.Lookup( "l01" ) );
  l_pSubs4->AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( g_TermTable.Lookup( "?loc-to" ) ),
			 g_TermTable.Lookup( "l00" ) );
  l_pSubs4->AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( g_TermTable.Lookup( "?city" ) ),
			 g_TermTable.Lookup( "c0" ) );

  assert( l_pSolution2->ApplyOperator( 4, l_pSubs4 ) );
  assert( l_pSolution2->GetPlanLength() == 3 );
  assert( !l_pSolution2->IsComplete() );

  assert( !l_pSolution->ApplyOperator( 4, new Substitution( *l_pSubs3 ) ) );
  assert( l_pSolution->GetPlanLength() == 3 );
  assert( !l_pSolution->IsComplete() );

  Substitution * l_pSubs5 = new Substitution();
  l_pSubs5->AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( g_TermTable.Lookup( "?obj" ) ),
			 g_TermTable.Lookup( "p1" ) );
  l_pSubs5->AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( g_TermTable.Lookup( "?truck" ) ),
			 g_TermTable.Lookup( "t0" ) );
  l_pSubs5->AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( g_TermTable.Lookup( "?loc" ) ),
			 g_TermTable.Lookup( "l01" ) );

  assert( !l_pSolution->ApplyOperator( 2, l_pSubs5 ) );
  assert( l_pSolution->GetPlanLength() == 4 );
  assert( !l_pSolution->IsComplete() );

  Substitution * l_pSubs6 = new Substitution();
  l_pSubs6->AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( g_TermTable.Lookup( "?obj" ) ),
			 g_TermTable.Lookup( "p0" ) );
  l_pSubs6->AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( g_TermTable.Lookup( "?truck" ) ),
			 g_TermTable.Lookup( "t0" ) );
  l_pSubs6->AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( g_TermTable.Lookup( "?loc" ) ),
			 g_TermTable.Lookup( "l01" ) );

  assert( !l_pSolution->ApplyOperator( 2, l_pSubs6 ) );
  assert( l_pSolution->GetPlanLength() == 5 );
  assert( l_pSolution->IsComplete() );

  StripsSolution * l_pSolution3 = new StripsSolution( l_pProblem,
						      ReadFile( "tests/test-sol.pddl" ) );

  assert( l_pSolution3->GetPlanLength() == 5 );
  assert( l_pSolution3->IsComplete() );

  delete l_pSolution3;
  delete l_pSolution2;
  delete l_pSolution;
}

void TestHtnTaskHead()
{
  HtnTaskHead * l_pTask1 = new HtnTaskHead( "( DELIVER-PKG p1 l00 )", TypeTable() );
  HtnTaskHead * l_pTask2 = new HtnTaskHead( *l_pTask1 );

  assert( CompareNoCase( l_pTask1->GetName(), "DELIVER-PKG" ) == 0 );
  assert( CompareNoCase( l_pTask2->GetName(), "DELIVER-PKG" ) == 0 );
  assert( l_pTask1->GetNumParams() == 2 );
  assert( l_pTask2->GetNumParams() == 2 );
  assert( CompareNoCase( l_pTask1->GetCParam( 0 )->ToStr(), "P1" ) == 0 );
  assert( CompareNoCase( l_pTask2->GetCParam( 0 )->ToStr(), "P1" ) == 0 );
  assert( CompareNoCase( l_pTask1->GetCParam( 1 )->ToStr(), "L00" ) == 0 );
  assert( CompareNoCase( l_pTask2->GetCParam( 1 )->ToStr(), "L00" ) == 0 );
  assert( l_pTask1->IsGround() );
  assert( l_pTask2->IsGround() );

  delete l_pTask2;
  delete l_pTask1;
}

void TestHtnTaskDescr()
{
  std::string l_sStr = "(:task ACQUIRE-TRUCK :parameters (?truck ?src ?dst) :precondition (and (TRUCK ?truck) (LOCATION ?src) (LOCATION ?dst) (CITY ?city) (in-city ?src ?city) (in-city ?dst ?city) (at ?truck ?src)) :effect (and (not (at ?truck ?src) ) (at ?truck ?dst)))";
  std::tr1::shared_ptr< HtnTaskDescr > l_pTask1( new HtnTaskDescr( l_sStr, std::set< std::string, StrLessNoCase >(), std::vector< FormulaPred >() ) );

  assert( CompareNoCase( l_pTask1->GetCHead()->GetName(), "ACQUIRE-TRUCK" ) == 0 );
  assert( l_pTask1->GetCHead()->GetNumParams() == 3 );
  assert( CompareNoCase( l_pTask1->GetCHead()->GetCParam( 0 )->ToStr(), "?TRUCK" ) == 0 );
  assert( CompareNoCase( l_pTask1->GetCHead()->GetCParam( 1 )->ToStr(), "?SRC" ) == 0 );
  assert( CompareNoCase( l_pTask1->GetCHead()->GetCParam( 2 )->ToStr(), "?DST" ) == 0 );
  //  assert( CompareNoCase( l_pTask1->GetCPreconditions()->ToStr(), "( and ( TRUCK ?TRUCK ) ( LOCATION ?SRC ) ( LOCATION ?DST ) ( CITY ?CITY ) ( IN-CITY ?SRC ?CITY ) ( IN-CITY ?DST ?CITY ) ( AT ?TRUCK ?SRC ) )" ) == 0 );
  //  assert( CompareNoCase( l_pTask1->GetCEffects()->ToStr(), "( and ( not ( AT ?TRUCK ?SRC ) ) ( AT ?TRUCK ?DST ) )" ) == 0 );
}

void TestHtnTaskList()
{
  std::stringstream l_sDomainStream( ReadFile( "tests/test-htn-domain.txt" ) );
  std::tr1::shared_ptr< HtnDomain > l_pDomain( HtnDomain::FromShop( l_sDomainStream ) );
  HtnTaskList * l_pTasks = new HtnTaskList( l_pDomain,
					    ReadFile( "tests/test-tasks.pddl" ) );
  HtnTaskList * l_pTasks2 = new HtnTaskList( *l_pTasks );

  assert( l_pTasks->size() == 3 );
  assert( CompareNoCase( l_pTasks->at( 0 )->GetCHead()->GetName(), "ACQUIRE-TRUCK" ) == 0 );
  assert( CompareNoCase( l_pTasks->at( 1 )->GetCHead()->GetName(), "ACQUIRE-PLANE" ) == 0 );
  assert( CompareNoCase( l_pTasks->at( 2 )->GetCHead()->GetName(), "DELIVER-PACKAGE" ) == 0 );

  assert( l_pTasks2->size() == 3 );
  assert( CompareNoCase( l_pTasks2->at( 0 )->GetCHead()->GetName(), "ACQUIRE-TRUCK" ) == 0 );
  assert( CompareNoCase( l_pTasks2->at( 1 )->GetCHead()->GetName(), "ACQUIRE-PLANE" ) == 0 );
  assert( CompareNoCase( l_pTasks2->at( 2 )->GetCHead()->GetName(), "DELIVER-PACKAGE" ) == 0 );

  delete l_pTasks2;
  delete l_pTasks;
}

void TestHtnMethod()
{
  std::stringstream l_sMethod1Stream( "( :method ( ACQUIRE-PLANE ?auto_36 ?auto_37 ) ( ( AIRPLANE ?auto_36 ) ( AIRPORT ?auto_81 ) ( AIRPORT ?auto_37 ) ( at ?auto_36 ?auto_81 ) ) ( ( !FLY-AIRPLANE ?auto_36 ?auto_81 ?auto_37 ) ) )" );
  HtnMethod * l_pMethod1 = HtnMethod::FromShop( l_sMethod1Stream );

  std::stringstream l_sMethod2Stream( "( :method ( ACQUIRE-PLANE ?plane ?dst ) ( ( AIRPLANE ?plane ) ( at ?plane ?src ) ( AIRPORT ?src ) ( AIRPORT ?dst ) ) ( ( !FLY-AIRPLANE ?plane ?src ?dst ) ) )" );
  HtnMethod * l_pMethod2 = HtnMethod::FromShop( l_sMethod2Stream );

  std::stringstream l_sMethod3Stream( "( :method ( ACQUIRE-PLANE ?plane ?dst ) ( ( AIRPLANE ?plane ) ( at ?plane ?src ) ( AIRPORT ?src ) ( AIRPORT ?dst ) ( LOCATION ?loc ) ) ( ( !FLY-AIRPLANE ?plane ?src ?dst ) ) )" );
  HtnMethod * l_pMethod3 = HtnMethod::FromShop( l_sMethod3Stream );

  std::stringstream l_sMethod4Stream( "( :method ( ACQUIRE-PLANE ?plane ?dst ) ( ( AIRPLANE ?plane ) ( at ?plane ?src ) ( AIRPORT ?src ) ( AIRPORT ?dst ) ) ( ( !FLY-AIRPLANE ?plane ?src ?src ) ( !FLY-AIRPLANE ?plane ?src ?dst ) ) )" );
  HtnMethod * l_pMethod4 = HtnMethod::FromShop( l_sMethod4Stream );

  std::stringstream l_sMethod5Stream( "( :method ( ACQUIRE-PLANE ?plane ?dst ) ( ( AIRPLANE ?plane ) ( at ?plane ?src ) ( AIRPORT ?src ) ( AIRPORT ?dst ) ) ( ( !FLY-AIRPLANE ?plane ?src ?src ) ) )" );
  HtnMethod * l_pMethod5 = HtnMethod::FromShop( l_sMethod5Stream );

  assert( CompareNoCase( l_pMethod1->GetCHead()->GetName(), "ACQUIRE-PLANE" ) == 0 );
  assert( l_pMethod1->GetCHead()->GetNumParams() == 2 );
  assert( CompareNoCase( l_pMethod1->GetCHead()->GetCParam( 0 )->ToStr(), "?AUTO_36" ) == 0 );
  assert( CompareNoCase( l_pMethod1->GetCHead()->GetCParam( 1 )->ToStr(), "?AUTO_37" ) == 0 );
  //  assert( CompareNoCase( l_pMethod1->GetCPreconditions()->ToStr(), "( and ( AIRPLANE ?AUTO_36 ) ( AIRPORT ?AUTO_81 ) ( AIRPORT ?AUTO_37 ) ( AT ?AUTO_36 ?AUTO_81 ) )" ) == 0 );
  assert( l_pMethod1->GetNumSubtasks() == 1 );
  assert( CompareNoCase( l_pMethod1->GetCSubtask( 0 )->ToStr(), "( !FLY-AIRPLANE ?AUTO_36 ?AUTO_81 ?AUTO_37 )" ) == 0 );

  assert( l_pMethod1->Subsumes( l_pMethod1 ) );
  assert( l_pMethod1->Subsumes( l_pMethod2 ) );
  assert( l_pMethod1->Subsumes( l_pMethod3 ) );
  assert( !l_pMethod1->Subsumes( l_pMethod4 ) );
  assert( !l_pMethod1->Subsumes( l_pMethod5 ) );

  assert( l_pMethod2->Subsumes( l_pMethod1 ) );
  assert( l_pMethod2->Subsumes( l_pMethod2 ) );
  assert( l_pMethod2->Subsumes( l_pMethod3 ) );
  assert( !l_pMethod2->Subsumes( l_pMethod4 ) );
  assert( !l_pMethod2->Subsumes( l_pMethod5 ) );

  assert( !l_pMethod3->Subsumes( l_pMethod1 ) );
  assert( !l_pMethod3->Subsumes( l_pMethod2 ) );
  assert( l_pMethod3->Subsumes( l_pMethod3 ) );
  assert( !l_pMethod3->Subsumes( l_pMethod4 ) );
  assert( !l_pMethod3->Subsumes( l_pMethod5 ) );

  assert( !l_pMethod4->Subsumes( l_pMethod1 ) );
  assert( !l_pMethod4->Subsumes( l_pMethod2 ) );
  assert( !l_pMethod4->Subsumes( l_pMethod3 ) );
  assert( l_pMethod4->Subsumes( l_pMethod4 ) );
  assert( !l_pMethod4->Subsumes( l_pMethod5 ) );

  assert( !l_pMethod5->Subsumes( l_pMethod1 ) );
  assert( !l_pMethod5->Subsumes( l_pMethod2 ) );
  assert( !l_pMethod5->Subsumes( l_pMethod3 ) );
  assert( !l_pMethod5->Subsumes( l_pMethod4 ) );
  assert( l_pMethod5->Subsumes( l_pMethod5 ) );

  std::stringstream l_sMethod6Stream( "( :method ( METHOD1 ?a ?b ) ( ( A ?a ) ( B ?b ) ( C ?c ) ( D ?d ) ) ( ( !OPER1 ?a ?b ) ) )" );
  HtnMethod * l_pMethod6 = HtnMethod::FromShop( l_sMethod6Stream );

  std::stringstream l_sMethod7Stream( "( :method ( METHOD1 ?1 ?2 ) ( ( A ?1 ) ( B ?2 ) ( C ?1 ) ) ( ( !OPER1 ?1 ?2 ) ) )" );
  HtnMethod * l_pMethod7 = HtnMethod::FromShop( l_sMethod7Stream );

  std::stringstream l_sMethod8Stream( "( :method ( METHOD1 ?aa ?bb ) ( ( A ?aa ) ( B ?bb ) ( C ?aa ) ( D ?dd ) ) ( ( !OPER1 ?aa ?bb ) ) )" );
  HtnMethod * l_pMethod8 = HtnMethod::FromShop( l_sMethod8Stream );

  std::stringstream l_sMethod9Stream( "( :method ( METHOD1 ?11 ?22 ) ( ( A ?11 ) ( B ?22 ) ( C ?33 ) ( A ?44 ) ) ( ( !OPER1 ?11 ?22 ) ) )" );
  HtnMethod * l_pMethod9 = HtnMethod::FromShop( l_sMethod9Stream );

  assert( l_pMethod6->Subsumes( l_pMethod6 ) );
  assert( !l_pMethod6->Subsumes( l_pMethod7 ) );
  assert( l_pMethod6->Subsumes( l_pMethod8 ) );
  assert( !l_pMethod6->Subsumes( l_pMethod9 ) );

  assert( !l_pMethod7->Subsumes( l_pMethod6 ) );
  assert( l_pMethod7->Subsumes( l_pMethod7 ) );
  assert( l_pMethod7->Subsumes( l_pMethod8 ) );
  assert( !l_pMethod7->Subsumes( l_pMethod9 ) );

  assert( !l_pMethod8->Subsumes( l_pMethod6 ) );
  assert( !l_pMethod8->Subsumes( l_pMethod7 ) );
  assert( l_pMethod8->Subsumes( l_pMethod8 ) );
  assert( !l_pMethod8->Subsumes( l_pMethod9 ) );

  assert( l_pMethod9->Subsumes( l_pMethod6 ) );
  assert( l_pMethod9->Subsumes( l_pMethod7 ) );
  assert( l_pMethod9->Subsumes( l_pMethod8 ) );
  assert( l_pMethod9->Subsumes( l_pMethod9 ) );

  std::string l_FlyStrings[7];
  l_FlyStrings[0] = 
    "( :method ( DELIVER-PKG ?a_390 ?a_391 ) \n"
    "  ( \n"
    "    ( OBJ ?a_390 ) \n"
    "    ( AIRPLANE ?a_471 ) \n"
    "    ( LOCATION ?a_391 ) \n"
    "    ( in ?a_390 ?a_471 ) \n"
    "    ( AIRPORT ?a_473 ) \n"
    "    ( AIRPORT ?a_391 ) \n"
    "    ( at ?a_471 ?a_473 ) \n"
    "    ( different ?a_473 ?a_391 ) \n"
    "  ) \n"
    "  ( \n"
    "    ( !FLY-AIRPLANE ?a_471 ?a_473 ?a_391 ) \n"
    "    ( DELIVER-PKG ?a_390 ?a_391 ) \n"
    "  ) \n"
    ") \n";
  l_FlyStrings[1] =
    "( :method ( DELIVER-PKG ?b_4 ?b_5 ) \n"
    "  ( \n"
    "    ( OBJ ?b_4 ) \n"
    "    ( AIRPLANE ?b_193 ) \n"
    "    ( LOCATION ?b_5 ) \n"
    "    ( AIRPORT ?b_194 ) \n"
    "    ( AIRPORT ?b_5 ) \n"
    "    ( different ?b_194 ?b_5 ) \n"
    "    ( LOCATION ?b_195 ) \n"
    "    ( TRUCK ?b_196 ) \n"
    "    ( LOCATION ?b_184 ) \n"
    "    ( CITY ?b_197 ) \n"
    "    ( in-city ?b_184 ?b_197 ) \n"
    "    ( in-city ?b_195 ?b_197 ) \n"
    "    ( different ?b_184 ?b_195 ) \n"
    "    ( LOCATION ?b_198 ) \n"
    "    ( at ?b_4 ?b_198 ) \n"
    "    ( LOCATION ?b_199 ) \n"
    "    ( CITY ?b_200 ) \n"
    "    ( at ?b_196 ?b_199 ) \n"
    "    ( in-city ?b_199 ?b_200 ) \n"
    "    ( in-city ?b_198 ?b_200 ) \n"
    "    ( different ?b_199 ?b_198 ) \n"
    "    ( AIRPORT ?b_202 ) \n"
    "    ( AIRPORT ?b_195 ) \n"
    "    ( at ?b_193 ?b_202 ) \n"
    "    ( different ?b_202 ?b_195 ) \n"
    "  ) \n"
    "  ( \n"
    "    ( !FLY-AIRPLANE ?b_193 ?b_202 ?b_195 ) \n"
    "    ( DELIVER-PKG ?b_4 ?b_5 ) \n"
    "  ) \n"
    ") \n";
  l_FlyStrings[2] = 
    "( :method ( DELIVER-PKG ?c_36 ?c_37 ) \n"
    "  ( \n"
    "    ( OBJ ?c_36 ) \n"
    "    ( AIRPLANE ?c_124 ) \n"
    "    ( LOCATION ?c_37 ) \n"
    "    ( AIRPORT ?c_125 ) \n"
    "    ( AIRPORT ?c_37 ) \n"
    "    ( different ?c_125 ?c_37 ) \n"
    "    ( LOCATION ?c_126 ) \n"
    "    ( at ?c_36 ?c_126 ) \n"
    "    ( AIRPORT ?c_128 ) \n"
    "    ( AIRPORT ?c_126 ) \n"
    "    ( at ?c_124 ?c_128 ) \n"
    "    ( different ?c_128 ?c_126 ) \n"
    "  ) \n"
    "  ( \n"
    "    ( !FLY-AIRPLANE ?c_124 ?c_128 ?c_126 ) \n"
    "    ( DELIVER-PKG ?c_36 ?c_37 ) \n"
    "  ) \n"
    ") \n";
  l_FlyStrings[3] =
    "( :method ( DELIVER-PKG ?d_78 ?d_79 ) \n"
    "  ( \n"
    "    ( OBJ ?d_78 ) \n"
    "    ( TRUCK ?d_222 ) \n"
    "    ( LOCATION ?d_79 ) \n"
    "    ( LOCATION ?d_223 ) \n"
    "    ( CITY ?d_224 ) \n"
    "    ( at ?d_222 ?d_223 ) \n"
    "    ( in-city ?d_223 ?d_224 ) \n"
    "    ( in-city ?d_79 ?d_224 ) \n"
    "    ( different ?d_223 ?d_79 ) \n"
    "    ( LOCATION ?d_225 ) \n"
    "    ( at ?d_222 ?d_225 ) \n"
    "    ( AIRPLANE ?d_226 ) \n"
    "    ( in ?d_78 ?d_226 ) \n"
    "    ( AIRPORT ?d_228 ) \n"
    "    ( AIRPORT ?d_225 ) \n"
    "    ( at ?d_226 ?d_228 ) \n"
    "    ( different ?d_228 ?d_225 ) \n"
    "  ) \n"
    "  ( \n"
    "    ( !FLY-AIRPLANE ?d_226 ?d_228 ?d_225 ) \n"
    "    ( DELIVER-PKG ?d_78 ?d_79 ) \n"
    "  ) \n"
    ") \n";
  l_FlyStrings[4] =
    "( :method ( DELIVER-PKG ?e_38 ?e_39 ) \n"
    "  ( \n"
    "    ( OBJ ?e_38 ) \n"
    "    ( TRUCK ?e_243 ) \n"
    "    ( LOCATION ?e_39 ) \n"
    "    ( LOCATION ?e_244 ) \n"
    "    ( CITY ?e_245 ) \n"
    "    ( at ?e_243 ?e_244 ) \n"
    "    ( in-city ?e_244 ?e_245 ) \n"
    "    ( in-city ?e_39 ?e_245 ) \n"
    "    ( different ?e_244 ?e_39 ) \n"
    "    ( LOCATION ?e_246 ) \n"
    "    ( at ?e_243 ?e_246 ) \n"
    "    ( AIRPLANE ?e_247 ) \n"
    "    ( AIRPORT ?e_248 ) \n"
    "    ( AIRPORT ?e_246 ) \n"
    "    ( different ?e_248 ?e_246 ) \n"
    "    ( LOCATION ?e_249 ) \n"
    "    ( at ?e_38 ?e_249 ) \n"
    "    ( AIRPORT ?e_251 ) \n"
    "    ( AIRPORT ?e_249 ) \n"
    "    ( at ?e_247 ?e_251 ) \n"
    "    ( different ?e_251 ?e_249 ) \n"
    "  ) \n"
    "  ( \n"
    "    ( !FLY-AIRPLANE ?e_247 ?e_251 ?e_249 ) \n"
    "    ( DELIVER-PKG ?e_38 ?e_39 ) \n"
    "  ) \n"
    ") \n";
  l_FlyStrings[5] =
    "( :method ( DELIVER-PKG ?f_78 ?f_79 ) \n"
    "  ( \n"
    "    ( OBJ ?f_78 ) \n"
    "    ( TRUCK ?f_243 ) \n"
    "    ( LOCATION ?f_79 ) \n"
    "    ( LOCATION ?f_224 ) \n"
    "    ( CITY ?f_244 ) \n"
    "    ( in-city ?f_224 ?f_244 ) \n"
    "    ( in-city ?f_79 ?f_244 ) \n"
    "    ( different ?f_224 ?f_79 ) \n"
    "    ( LOCATION ?f_245 ) \n"
    "    ( LOCATION ?f_246 ) \n"
    "    ( CITY ?f_247 ) \n"
    "    ( at ?f_243 ?f_246 ) \n"
    "    ( in-city ?f_246 ?f_247 ) \n"
    "    ( in-city ?f_245 ?f_247 ) \n"
    "    ( different ?f_246 ?f_245 ) \n"
    "    ( AIRPLANE ?f_248 ) \n"
    "    ( in ?f_78 ?f_248 ) \n"
    "    ( AIRPORT ?f_250 ) \n"
    "    ( AIRPORT ?f_245 ) \n"
    "    ( at ?f_248 ?f_250 ) \n"
    "    ( different ?f_250 ?f_245 ) \n"
    "  ) \n"
    "  ( \n"
    "    ( !FLY-AIRPLANE ?f_248 ?f_250 ?f_245 ) \n"
    "    ( DELIVER-PKG ?f_78 ?f_79 ) \n"
    "  ) \n"
    ") \n";
  l_FlyStrings[6] =
    "( :method ( DELIVER-PKG ?g_38 ?g_39 ) \n"
    "  ( \n"
    "    ( OBJ ?g_38 ) \n"
    "    ( TRUCK ?g_266 ) \n"
    "    ( LOCATION ?g_39 ) \n"
    "    ( LOCATION ?g_224 ) \n"
    "    ( CITY ?g_267 ) \n"
    "    ( in-city ?g_224 ?g_267 ) \n"
    "    ( in-city ?g_39 ?g_267 ) \n"
    "    ( different ?g_224 ?g_39 ) \n"
    "    ( LOCATION ?g_268 ) \n"
    "    ( LOCATION ?g_269 ) \n"
    "    ( CITY ?g_270 ) \n"
    "    ( at ?g_266 ?g_269 ) \n"
    "    ( in-city ?g_269 ?g_270 ) \n"
    "    ( in-city ?g_268 ?g_270 ) \n"
    "    ( different ?g_269 ?g_268 ) \n"
    "    ( AIRPLANE ?g_271 ) \n"
    "    ( AIRPORT ?g_272 ) \n"
    "    ( AIRPORT ?g_268 ) \n"
    "    ( different ?g_272 ?g_268 ) \n"
    "    ( LOCATION ?g_273 ) \n"
    "    ( at ?g_38 ?g_273 ) \n"
    "    ( AIRPORT ?g_275 ) \n"
    "    ( AIRPORT ?g_273 ) \n"
    "    ( at ?g_271 ?g_275 ) \n"
    "    ( different ?g_275 ?g_273 ) \n"
    "  ) \n"
    "  ( \n"
    "    ( !FLY-AIRPLANE ?g_271 ?g_275 ?g_273 ) \n"
    "    ( DELIVER-PKG ?g_38 ?g_39 ) \n"
    "  ) \n"
    ") \n";

  std::stringstream l_FlyStreams[7];
  HtnMethod * l_FlyMethods[7];
  for( unsigned int i = 0; i < 7; i++ )
  {
    l_FlyStreams[i].str( l_FlyStrings[i] );
    l_FlyMethods[i] = HtnMethod::FromShop( l_FlyStreams[i] );
  }

  assert( l_FlyMethods[0]->Subsumes( l_FlyMethods[0] ) );
  // Because ( in ?390 ?470 ) cannot be guaranteed
  assert( !l_FlyMethods[0]->Subsumes( l_FlyMethods[1] ) );
  // Because ( in ?390 ?470 ) cannot be guaranteed
  assert( !l_FlyMethods[0]->Subsumes( l_FlyMethods[2] ) );
  // Because the subtasks cannot be unified
  assert( !l_FlyMethods[0]->Subsumes( l_FlyMethods[3] ) );
  // Because ( in ?390 ?470 ) cannot be guaranteed
  assert( !l_FlyMethods[0]->Subsumes( l_FlyMethods[4] ) );
  // Because the subtasks cannot be unified
  assert( !l_FlyMethods[0]->Subsumes( l_FlyMethods[5] ) );
  // Because ( in ?390 ?470 ) cannot be guaranteed
  assert( !l_FlyMethods[0]->Subsumes( l_FlyMethods[6] ) );

  // Because ( at ?4 ?198 ) cannot be guaranteed
  assert( !l_FlyMethods[1]->Subsumes( l_FlyMethods[0] ) );
  assert( l_FlyMethods[1]->Subsumes( l_FlyMethods[1] ) );
  // Because ( TRUCK ?196 ) cannot be guaranteed
  assert( !l_FlyMethods[1]->Subsumes( l_FlyMethods[2] ) );
  // Because ( AIRPORT ?5 ) cannot be guaranteed
  assert( !l_FlyMethods[1]->Subsumes( l_FlyMethods[3] ) );
  // Because ( AIRPORT ?5 ) cannot be guaranteed
  assert( !l_FlyMethods[1]->Subsumes( l_FlyMethods[4] ) );
  // Because ( at ?4 ?198 ) cannot be guaranteed
  assert( !l_FlyMethods[1]->Subsumes( l_FlyMethods[5] ) );
  // Because ( AIRPORT ?5 ) cannot be guaranteed
  assert( !l_FlyMethods[1]->Subsumes( l_FlyMethods[6] ) );

  // Because ( at ?36 ?126 ) cannot be guaranteed
  assert( !l_FlyMethods[2]->Subsumes( l_FlyMethods[0] ) );
  // Because the package must be where the plane is going
  assert( !l_FlyMethods[2]->Subsumes( l_FlyMethods[1] ) );
  assert( l_FlyMethods[2]->Subsumes( l_FlyMethods[2] ) );
  // Because ( at ?36 ?126 ) cannot be guaranteed
  assert( !l_FlyMethods[2]->Subsumes( l_FlyMethods[3] ) );
  // Because ( AIRPORT ?37 ) cannot be guaranteed
  assert( !l_FlyMethods[2]->Subsumes( l_FlyMethods[4] ) );
  // Because ( at ?36 ?126 ) cannot be guaranteed
  assert( !l_FlyMethods[2]->Subsumes( l_FlyMethods[5] ) );
  // Because ( AIRPORT ?37 ) cannot be guaranteed
  assert( !l_FlyMethods[2]->Subsumes( l_FlyMethods[6] ) );

  // Because ( TRUCK ?222 ) cannot be guaranteed
  assert( !l_FlyMethods[3]->Subsumes( l_FlyMethods[0] ) );
  // Because ( in ?78 ?226 ) cannot be guaranteed
  assert( !l_FlyMethods[3]->Subsumes( l_FlyMethods[1] ) );
  // Because ( in ?78 ?226 ) cannot be guaranteed
  assert( !l_FlyMethods[3]->Subsumes( l_FlyMethods[2] ) );
  assert( l_FlyMethods[3]->Subsumes( l_FlyMethods[3] ) );
  // Because ( in ?78 ?226 ) cannot be guaranteed
  assert( !l_FlyMethods[3]->Subsumes( l_FlyMethods[4] ) );
  // Because the truck must be where the plane is going
  assert( !l_FlyMethods[3]->Subsumes( l_FlyMethods[5] ) );
  // Because ( in ?78 ?226 ) cannot be guaranteed
  assert( !l_FlyMethods[3]->Subsumes( l_FlyMethods[6] ) );

  // Because ( at ?38 ?249 ) cannot be guaranteed
  assert( !l_FlyMethods[4]->Subsumes( l_FlyMethods[0] ) );
  // Because the package must be where the plane is going
  assert( !l_FlyMethods[4]->Subsumes( l_FlyMethods[1] ) );
  // Because ( TRUCK ?243 ) cannot be guaranteed
  assert( !l_FlyMethods[4]->Subsumes( l_FlyMethods[2] ) );
  // Because ( at ?38 ?249 ) cannot be guaranteed
  assert( !l_FlyMethods[4]->Subsumes( l_FlyMethods[3] ) );
  assert( l_FlyMethods[4]->Subsumes( l_FlyMethods[4] ) );
  // Because ( at ?38 ?249 ) cannot be guaranteed
  assert( !l_FlyMethods[4]->Subsumes( l_FlyMethods[5] ) );
  // Because it cannot be guaranteed that the truck is in the same city as
  //   the destination.
  assert( !l_FlyMethods[4]->Subsumes( l_FlyMethods[6] ) );

  // Because ( TRUCK ?243 ) cannot be guaranteed
  assert( !l_FlyMethods[5]->Subsumes( l_FlyMethods[0] ) );
  // Because ( in ?78 ?248 ) cannot be guaranteed
  assert( !l_FlyMethods[5]->Subsumes( l_FlyMethods[1] ) );
  // Because ( in ?78 ?248 ) cannot be guaranteed
  assert( !l_FlyMethods[5]->Subsumes( l_FlyMethods[2] ) );
  // Because where the plane is flown to is not in a specified city
  assert( !l_FlyMethods[5]->Subsumes( l_FlyMethods[3] ) );
  // Because ( in ?78 ?248 ) cannot be guaranteed
  assert( !l_FlyMethods[5]->Subsumes( l_FlyMethods[4] ) );
  //  assert( l_FlyMethods[5]->Subsumes( l_FlyMethods[5] ) );
  // Because ( in ?78 ?248 ) cannot be guaranteed
  assert( !l_FlyMethods[5]->Subsumes( l_FlyMethods[6] ) );

  // Because ( at ?38 ?273 ) cannot be guaranteed
  assert( !l_FlyMethods[6]->Subsumes( l_FlyMethods[0] ) );
  // Because the package cannot be guaranteed to be where the plane is going
  assert( !l_FlyMethods[6]->Subsumes( l_FlyMethods[1] ) );
  // Because ( TRUCK ?266 ) cannot be guaranteed
  assert( !l_FlyMethods[6]->Subsumes( l_FlyMethods[2] ) );
  // Because ( at ?38 ?273 ) cannot be guaranteed
  assert( !l_FlyMethods[6]->Subsumes( l_FlyMethods[3] ) );
  // Because the the truck cannot be guaranteed in a specific city
  assert( !l_FlyMethods[6]->Subsumes( l_FlyMethods[4] ) );
  // Because ( at ?38 ?273 ) cannot be guaranteed
  assert( !l_FlyMethods[6]->Subsumes( l_FlyMethods[5] ) );
  assert( l_FlyMethods[6]->Subsumes( l_FlyMethods[6] ) );


  for( unsigned int i = 0; i < 7; i++ ) delete l_FlyMethods[i];
  delete l_pMethod9;
  delete l_pMethod8;
  delete l_pMethod7;
  delete l_pMethod6;
  delete l_pMethod5;
  delete l_pMethod4;
  delete l_pMethod3;
  delete l_pMethod2;
  delete l_pMethod1;

  std::string l_MunozStrings[5];
  l_MunozStrings[0] = 
    "(:method (obj-at ?package ?loc) \n"
    "  ( \n"
    "    (obj-at ?package ?loc1) \n"
    "    (obj-at ?package ?loc0) \n"
    "    (IN-CITY ?loc ?city) \n"
    "    (IN-CITY ?loc1 ?city) \n"
    "    (TRUCK ?truck ?city) \n"
    "    (truck-at ?truck ?locTruck) \n"
    "    (same ?locTruck ?loc1) \n"
    "  ) \n"
    "  ( \n"
    "    (!load-truck ?package ?truck ?loc1) \n"
    "  ) \n"
    ") \n";
  l_MunozStrings[1] =
    "(:method (obj-at ?package ?loc) \n"
    "  ( \n"
    "    (obj-at ?package ?loc20) \n"
    "    (truck-at ?truck2 ?locTruck2) \n"
    "    (same ?locTruck2 ?loc2) \n"
    "    (IN-CITY ?loc1 ?city) \n"
    "    (obj-at ?package ?loc2) \n"
    "    (IN-CITY ?loc ?city) \n"
    "  ) \n"
    "  ( \n"
    "    (!load-truck ?package ?truck2 ?loc2) \n"
    "  ) \n"
    ") \n";
  l_MunozStrings[2] =
    "(:method (obj-at ?package6 ?loc) \n"
    "  ( \n"
    "    (same ?locTruck3 ?loc3) \n"
    "    (truck-at ?truck3 ?locTruck3) \n"
    "    (obj-at ?package6 ?loc3) \n"
    "    (obj-at ?package6 ?loc02) \n"
    "  ) \n"
    "  ( \n"
    "    (!load-truck ?package6 ?truck3 ?loc3) \n"
    "  ) \n"
    ") \n";
  l_MunozStrings[3] =
    "(:method (obj-at ?package6 ?loc3) \n"
    "  ( \n"
    "    (same ?locTruck3 ?loc3) \n"
    "    (truck-at ?truck3 ?locTruck3) \n"
    "    (obj-at ?package6 ?loc3) \n"
    "    (obj-at ?package6 ?loc3) \n"
    "  ) \n"
    "  ( \n"
    "    (!load-truck ?package6 ?truck3 ?loc3) \n"
    "  ) \n"
    ") \n";
  l_MunozStrings[4] =
    "(:method (obj-at ?package ?loc) \n"
    "  ( \n"
    "    (obj-at ?package ?loc020) \n"
    "    (obj-at ?package ?loc4) \n"
    "    (IN-CITY ?loc4 ?city) \n"
    "    (truck-at ?truck4 ?locTruck4) \n"
    "    (same ?locTruck4 ?loc4) \n"
    "  ) \n"
    "  ( \n"
    "    (!load-truck ?package ?truck4 ?loc4) \n"
    "  ) \n"
    ") \n";

  std::stringstream l_MunozStreams[5];
  HtnMethod * l_MunozMethods[5];
  for( unsigned int i = 0; i < 5; i++ )
  {
    l_MunozStreams[i].str( l_MunozStrings[i] );
    l_MunozMethods[i] = HtnMethod::FromShop( l_MunozStreams[i] );
  }

  assert( l_MunozMethods[0]->Subsumes( l_MunozMethods[0] ) );
  assert( !l_MunozMethods[0]->Subsumes( l_MunozMethods[1] ) );
  assert( !l_MunozMethods[0]->Subsumes( l_MunozMethods[2] ) );
  assert( !l_MunozMethods[0]->Subsumes( l_MunozMethods[3] ) );
  assert( !l_MunozMethods[0]->Subsumes( l_MunozMethods[4] ) );

  assert( l_MunozMethods[1]->Subsumes( l_MunozMethods[0] ) );
  assert( l_MunozMethods[1]->Subsumes( l_MunozMethods[1] ) );
  assert( !l_MunozMethods[1]->Subsumes( l_MunozMethods[2] ) );
  assert( !l_MunozMethods[1]->Subsumes( l_MunozMethods[3] ) );
  assert( !l_MunozMethods[1]->Subsumes( l_MunozMethods[4] ) );

  assert( l_MunozMethods[2]->Subsumes( l_MunozMethods[0] ) );
  assert( l_MunozMethods[2]->Subsumes( l_MunozMethods[1] ) );
  assert( l_MunozMethods[2]->Subsumes( l_MunozMethods[2] ) );
  assert( l_MunozMethods[2]->Subsumes( l_MunozMethods[3] ) );
  assert( l_MunozMethods[2]->Subsumes( l_MunozMethods[4] ) );

  for( unsigned int i = 0; i < 5; i++ ) delete l_MunozMethods[i];
}

void TestAnnotatedPlan()
{
  std::tr1::shared_ptr< StripsDomain > l_pDomain( new StripsDomain( ReadFile( "tests/test-domain.pddl" ) ) );
  std::tr1::shared_ptr< StripsProblem > l_pProblem( new StripsProblem( ReadFile( "tests/test-prob2.pddl" ), l_pDomain ) );

  AnnotatedPlan * l_pPlan = new AnnotatedPlan( l_pProblem,
					       ReadFile( "tests/test-sol.pddl" ) );

  std::tr1::shared_ptr< HtnTaskDescr > l_pTaskDescr( new HtnTaskDescr(  "(:task DELIVER-PACKAGE :parameters (?pkg ?dst) :precondition (and (PACKAGE ?pkg) (LOCATION ?dst)) :effect (and (at ?pkg ?dst)))", std::set< std::string, StrLessNoCase >(), std::vector< FormulaPred >() ) );

  std::stringstream l_sMethod1Stream( "( :method ( DELIVER-PACKAGE ?pkg ?dst ) ( ( PACKAGE ?pkg ) ( LOCATION ?dst ) ( TRUCK ?truck ) ( at ?truck ?dst ) ( in ?pkg ?truck ) ) ( ( !UNLOAD-TRUCK ?pkg ?truck ?dst ) ) )" );
  HtnMethod * l_pMethod1 = HtnMethod::FromShop( l_sMethod1Stream );

  Substitution * l_pSubs1 = new Substitution();
  l_pSubs1->AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( g_TermTable.Lookup( "?pkg" ) ),
			 g_TermTable.Lookup( "p1" ) );
  l_pSubs1->AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( g_TermTable.Lookup( "?dst" ) ),
			 g_TermTable.Lookup( "l01" ) );
  l_pSubs1->AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( g_TermTable.Lookup( "?truck" ) ),
			 g_TermTable.Lookup( "t0" ) );

  FormulaConjP l_pEffects1( new FormulaConj( "( and ( at ?pkg ?dst ) ( not ( in ?pkg ?truck ) ) )", TypeTable(), g_NoPredicates ) );

  l_pPlan->AddMethodInst( l_pMethod1,
			  l_pSubs1,
			  4,
			  5,
			  l_pTaskDescr,
			  l_pEffects1,
			  0 );

  std::stringstream l_sMethod2Stream( "( :method ( DELIVER-PACKAGE ?p ?d ) ( ( PACKAGE ?p ) ( LOCATION ?d ) ( LOCATION ?s ) ( CITY ?c ) ( TRUCK ?t ) ( in-city ?d ?c ) ( in-city ?s ?c ) ( at ?t ?s ) ( in ?p ?t ) ) ( ( !DRIVE-TRUCK ?t ?s ?d ?c ) ( !UNLOAD-TRUCK ?p ?t ?d ) ) )" );
  HtnMethod * l_pMethod2 = HtnMethod::FromShop( l_sMethod2Stream );

  Substitution * l_pSubs2 = new Substitution();
  l_pSubs2->AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( g_TermTable.Lookup( "?p" ) ),
			 g_TermTable.Lookup( "p1" ) );
  l_pSubs2->AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( g_TermTable.Lookup( "?d" ) ),
			 g_TermTable.Lookup( "l01" ) );
  l_pSubs2->AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( g_TermTable.Lookup( "?t" ) ),
			 g_TermTable.Lookup( "t0" ) );
  l_pSubs2->AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( g_TermTable.Lookup( "?s" ) ),
			 g_TermTable.Lookup( "l00" ) );
  l_pSubs2->AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( g_TermTable.Lookup( "?c" ) ),
			 g_TermTable.Lookup( "c0" ) );

  FormulaConjP l_pEffects2( new FormulaConj( "( and ( at ?p ?d ) ( not ( in ?p ?t ) ) ( at ?t ?d ) ( not ( at ?t ?s ) ) )", TypeTable(), g_NoPredicates ) );

  l_pPlan->AddMethodInst( l_pMethod2,
			  l_pSubs2,
			  2,
			  5,
			  l_pTaskDescr,
			  l_pEffects2,
			  5 );

  assert( l_pPlan->GetNumMethods() == 2 );
  assert( l_pPlan->GetMethodBeforeState( 0 ) == 4 );
  assert( l_pPlan->GetMethodAfterState( 0 ) == 5 );
  assert( l_pPlan->GetMethodBeforeState( 1 ) == 2 );
  assert( l_pPlan->GetMethodAfterState( 1 ) == 5 );

  delete l_pSubs2;
  delete l_pMethod2;
  delete l_pSubs1;
  delete l_pMethod1;
  delete l_pPlan;
}

void TestHtnDomain()
{
  std::stringstream l_sDomainStream( ReadFile( "tests/test-htn-domain.txt" ) );
  HtnDomain * l_pHtnDomain = HtnDomain::FromShop( l_sDomainStream );
  HtnDomain * l_pOtherDom = new HtnDomain( *l_pHtnDomain );

  assert( l_pHtnDomain->GetNumOperators() == l_pOtherDom->GetNumOperators() );
  assert( l_pHtnDomain->GetNumMethods() == l_pOtherDom->GetNumMethods() );
  l_pOtherDom->RemoveMethod( 5 );
  assert( l_pHtnDomain->GetNumMethods() == l_pOtherDom->GetNumMethods() + 1 );
  assert( l_pOtherDom->GetDomainName() == "logistics" );

  delete l_pOtherDom;
  delete l_pHtnDomain;
}

void TestPartialHtnMethod()
{
  std::tr1::shared_ptr< HtnTaskDescr > l_pTask1( new HtnTaskDescr( "(:task ACQUIRE-TRUCK :parameters (?truck ?src ?dst) :precondition (and (TRUCK ?truck) (LOCATION ?src) (LOCATION ?dst) (CITY ?city) (in-city ?src ?city) (in-city ?dst ?city) (at ?truck ?src)) :effect (and (not (at ?truck ?src) ) (at ?truck ?dst)))", std::set< std::string, StrLessNoCase >(), std::vector< FormulaPred >() ) );

  Substitution l_Subs1;
  l_Subs1.AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( g_TermTable.Lookup( "?truck" ) ),
		       g_TermTable.Lookup( "t0" ) );
  l_Subs1.AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( g_TermTable.Lookup( "?src" ) ),
		       g_TermTable.Lookup( "l00" ) );
  l_Subs1.AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( g_TermTable.Lookup( "?dst" ) ),
		       g_TermTable.Lookup( "l01" ) );
  l_Subs1.AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( g_TermTable.Lookup( "?city" ) ),
		       g_TermTable.Lookup( "c0" ) );

  std::stringstream l_sDomainStream( ReadFile( "tests/test-htn-domain2.pddl" ) );
  std::tr1::shared_ptr< HtnDomain > l_pHtnDomain( HtnDomain::FromShop( l_sDomainStream ) );
  PartialHtnMethod * l_pPart1 = new PartialHtnMethod( l_pHtnDomain, l_pTask1, &l_Subs1, 3 );
  PartialHtnMethod * l_pPart2 = new PartialHtnMethod( *l_pPart1 );

  delete l_pPart2;
  delete l_pPart1;
}

void TestHtnSolution()
{
  std::stringstream l_sDomainStream( ReadFile( "tests/test-htn-domain2.pddl" ) );
  std::tr1::shared_ptr< HtnDomain > l_pHtnDomain(  HtnDomain::FromShop( l_sDomainStream ) );
  std::stringstream l_sProblemStream( ReadFile( "tests/test-htn-prob.pddl" ) );
  HtnSolution * l_pSol1 = HtnSolution::FromShop( l_pHtnDomain,
						 l_sProblemStream );
  HtnSolution * l_pSol2 = new HtnSolution( *l_pSol1 );

  assert( l_pSol1->GetPlanLength() == 0 );
  assert( l_pSol2->GetPlanLength() == 0 );
  assert( l_pSol1->IsComplete() == false );
  assert( l_pSol1->IsComplete() == false );
  assert( CompareNoCase( l_pSol1->GetCTopTask()->GetName(), "DELIVER-PKG" ) == 0 );
  assert( l_pSol1->GetCTopTask()->GetNumParams() == 2 );
  assert( CompareNoCase( l_pSol1->GetCTopTask()->GetCParam( 0 )->ToStr(), "P0" ) == 0 );
  assert( CompareNoCase( l_pSol1->GetCTopTask()->GetCParam( 1 )->ToStr(), "L10" ) == 0 );

  Substitution * l_pSubs1 = new Substitution();
  l_pSubs1->AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( g_TermTable.Lookup( "?auto_171" ) ), 
			 g_TermTable.Lookup( "p0" ) );
  l_pSubs1->AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( g_TermTable.Lookup( "?auto_172" ) ), 
			 g_TermTable.Lookup( "l10" ) );
  l_pSubs1->AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( g_TermTable.Lookup( "?auto_281" ) ),
			 g_TermTable.Lookup( "a0" ) );
  l_pSubs1->AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( g_TermTable.Lookup( "?auto_283" ) ),
			 g_TermTable.Lookup( "l00" ) );
  l_pSubs1->AddPair( std::tr1::dynamic_pointer_cast< TermVariable >( g_TermTable.Lookup( "?auto_285" ) ),
			 g_TermTable.Lookup( "l10" ) );

  l_pSol1->ApplyMethod( 4, l_pSubs1 );

  delete l_pSubs1;
  delete l_pSol2;
  delete l_pSol1;
}

void TestStringTable()
{
  unsigned int l_iStr2 = g_StrTable.Lookup( "tiger" );
  assert( g_StrTable.Lookup( "TIGER" ) == l_iStr2 );
  assert( g_StrTable.Lookup( l_iStr2 ) == "TIGER" );
}

void TestSubtasksAreLinked()
{
  std::stringstream l_sMethod1Stream( "( :method ( TEST1 ?p1 ?p2 ) ( ( PREC1 ?p3 ) ) ( ( SUB1 ?p1 ) ) )" );
  HtnMethod * l_pMethod1 = HtnMethod::FromShop( l_sMethod1Stream );
  assert( l_pMethod1->SubtasksArePartiallyLinked() );

  delete l_pMethod1;
  l_sMethod1Stream.str( "( :method ( TEST1 ?p1 ?p2 ) ( ( PREC1 ?p3 ) ) ( ( SUB1 ?p3 ) ) )" );
  l_pMethod1 = HtnMethod::FromShop( l_sMethod1Stream );
  assert( !l_pMethod1->SubtasksArePartiallyLinked() );

  delete l_pMethod1;
  l_sMethod1Stream.str( "( :method ( TEST1 ?p1 ?p2 ) ( ( PREC1 ?p3 ?p2 ) ) ( ( SUB1 ?p3 ) ) )" );
  l_pMethod1 = HtnMethod::FromShop( l_sMethod1Stream );
  assert( l_pMethod1->SubtasksArePartiallyLinked() );

  delete l_pMethod1;
  l_sMethod1Stream.str( "( :method ( TEST1 ?p1 ?p2 ) ( ( PREC1 ?p3 ?p1 ) ( PREC2 ?p4 ?p3 ) ) ( ( SUB1 ?p4 ) ) )" );
  l_pMethod1 = HtnMethod::FromShop( l_sMethod1Stream );
  assert( l_pMethod1->SubtasksArePartiallyLinked() );

  delete l_pMethod1;
  l_sMethod1Stream.str( "( :method ( TEST1 ?p1 ?p2 ) ( ( DIFFERENT ?p3 ?p1 ) ) ( ( SUB1 ?p3 ) ) )" );
  l_pMethod1 = HtnMethod::FromShop( l_sMethod1Stream );
  assert( !l_pMethod1->SubtasksArePartiallyLinked() );

  delete l_pMethod1;
  l_sMethod1Stream.str( "( :method ( TEST1 ?p1 ?p2 ) ( ( PREC1 ?p3 ?p1 ) ) ( ( SUB1 ?p3 ) ( SUB2 ?p4 ) ) )" );
  l_pMethod1 = HtnMethod::FromShop( l_sMethod1Stream );
  assert( !l_pMethod1->SubtasksArePartiallyLinked() );

  delete l_pMethod1;
  l_sMethod1Stream.str( "( :method ( TEST1 ?p1 ?p2 ) ( ( PREC1 ?p3 ?p1 ) ( PREC2 ?p4 ?p2 ) ) ( ( SUB1 ?p3 ) ( SUB2 ?p4 ) ) )" );
  l_pMethod1 = HtnMethod::FromShop( l_sMethod1Stream );
  assert( l_pMethod1->SubtasksArePartiallyLinked() );

  delete l_pMethod1;
}

void TestTyping()
{
  TermP l_pTerm1 = g_TermTable.Lookup( "?x-typed", "truck" );
  TermP l_pTerm2 = g_TermTable.Lookup( "t1-typed", "truck" );
  TermP l_pTerm3 = g_TermTable.Lookup( "l01-typed", "location" );
  TermP l_pTerm4 = g_TermTable.Lookup( "p3" );

  assert( l_pTerm1->HasTyping() );
  assert( l_pTerm2->HasTyping() );
  assert( l_pTerm3->HasTyping() );
  assert( !l_pTerm4->HasTyping() );
  assert( CompareNoCase( l_pTerm1->GetTyping(), "truck" ) == 0 );
  assert( CompareNoCase( l_pTerm2->GetTyping(), "truck" ) == 0 );
  assert( CompareNoCase( l_pTerm3->GetTyping(), "location" ) == 0 );

  TypeTable l_TypeTable;
  l_TypeTable["?x-typed"] = "truck";
  l_TypeTable["?y-typed"] = "location";
  l_TypeTable["t1-typed"] = "truck";
  l_TypeTable["l01-typed"] = "location";
  l_TypeTable["p3-typed"] = "package";
  l_TypeTable["t2-typed"] = "truck";
  l_TypeTable["l02-typed"] = "location";

  FormulaP l_pFormula1( new FormulaConj( "( and ( at ?x-typed ?y-typed ) )", l_TypeTable, g_NoPredicates ) );
  FormulaP l_pFormula2( new FormulaPred( "( at ?x ?y )", TypeTable(), g_NoPredicates ) );
  FormulaP l_pFormula3( new FormulaNeg( "( not ( at ?x-typed ?y-typed ) )", l_TypeTable, g_NoPredicates ) );
  FormulaP l_pFormula4( new FormulaConj( "( and ( at t1-typed l01-typed ) ( in p3-typed t1-typed ) )", l_TypeTable, g_NoPredicates ) );
  FormulaP l_pFormula5( new FormulaPred( "( in p3-typed t1-typed )", l_TypeTable, g_NoPredicates ) );

  assert( l_pFormula4->Implies( l_pFormula5 ) );

  State * l_pState = new State( 
      "( \n"
      "  ( at t1-typed l01-typed )\n"
      "  ( in p3-typed t1-typed )\n"
      "  ( at t2-typed l02-typed )\n"
      ")\n",
      0,
      l_TypeTable,
      std::vector< FormulaPred >());

  assert( !l_pState->IsConsistent( l_pFormula1 ) );
  assert( !l_pState->IsConsistent( l_pFormula2 ) );
  assert( !l_pState->IsConsistent( l_pFormula3 ) );
  assert( l_pState->IsConsistent( l_pFormula4 ) );
  assert( l_pState->IsConsistent( l_pFormula5 ) );

  assert( l_pState->CouldBeConsistent( l_pFormula1 ) );
  assert( l_pState->CouldBeConsistent( l_pFormula3 ) );
  assert( l_pState->CouldBeConsistent( l_pFormula4 ) );
  assert( l_pState->CouldBeConsistent( l_pFormula5 ) );

  Substitution l_EmptySub;
  std::set< TermVariableP > l_sRelVars;
  l_sRelVars.insert( std::tr1::dynamic_pointer_cast< TermVariable >( std::tr1::dynamic_pointer_cast< FormulaPred >( *std::tr1::dynamic_pointer_cast< FormulaConj >( l_pFormula1 )->GetBeginConj() )->GetCParam( 0 ) ) );
  l_sRelVars.insert( std::tr1::dynamic_pointer_cast< TermVariable >( std::tr1::dynamic_pointer_cast< FormulaPred >( *std::tr1::dynamic_pointer_cast< FormulaConj >( l_pFormula1 )->GetBeginConj() )->GetCParam( 1 ) ) );

  std::vector< Substitution * > * l_pInsts = l_pState->GetInstantiations( std::tr1::dynamic_pointer_cast< FormulaConj >( l_pFormula1 ), &l_EmptySub, l_sRelVars );
  assert( l_pInsts->size() == 2 );
  assert( l_pInsts->at( 0 )->GetNumPairs() == 2 );
  assert( l_pInsts->at( 0 )->Begin()->first->HasTyping() );
  assert( l_pInsts->at( 0 )->Begin()->second->HasTyping() );
  assert( (++l_pInsts->at( 0 )->Begin())->first->HasTyping() );
  assert( (++l_pInsts->at( 0 )->Begin())->second->HasTyping() );

  for( unsigned int i = 0; i < l_pInsts->size(); i++ )
    delete l_pInsts->at( i );
  delete l_pInsts;

  delete l_pState;
}
