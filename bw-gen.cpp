#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <cassert>
#include <ctime>

int main( int argc, char * argv[] )
{
  if( argc < 3 || argc > 4 )
  {
    std::cerr << "\nUsage: bw_gen num_blocks pddl-file [htn-file]\n";
    exit( 1 );
  }

  srand( time(0) );

  std::vector< std::vector< unsigned int > > l_vInputTowers;
  std::vector< std::vector< unsigned int > > l_vOutputTowers;

  for( int i = 1; i <= atoi( argv[1] ); i++ )
  {
    unsigned int l_iTowerIndex = rand() % ( l_vInputTowers.size() + 1 );
    if( l_iTowerIndex == l_vInputTowers.size() )
      l_vInputTowers.push_back( std::vector< unsigned int >() );
    l_vInputTowers[l_iTowerIndex].push_back( i );
    l_iTowerIndex = rand() % ( l_vOutputTowers.size() + 1 );
    if( l_iTowerIndex == l_vOutputTowers.size() )
      l_vOutputTowers.push_back( std::vector< unsigned int >() );
    l_vOutputTowers[l_iTowerIndex].push_back( i );
  }

  for( unsigned int i = 0; i < l_vInputTowers.size(); i++ )
    std::random_shuffle( l_vInputTowers[i].begin(), l_vInputTowers[i].end() );
  for( unsigned int i = 0; i < l_vOutputTowers.size(); i++ )
    std::random_shuffle( l_vOutputTowers[i].begin(), l_vOutputTowers[i].end() );

  std::ofstream l_PddlFile( argv[2], std::ios_base::in | std::ios_base::trunc );
  assert( l_PddlFile.good() );

  l_PddlFile << "( define ( problem bw4-"
	     << argv[1]
	     << " )\n"
	     << "  ( :domain blocksworld )\n"
	     << "  ( :objects ";
  for( int i = 1; i <= atoi( argv[1] ); i++ )
    l_PddlFile << "b" << i << " ";
  l_PddlFile << " )\n";
  l_PddlFile << "  ( :init\n";
  l_PddlFile << "    ( arm-empty )\n";

  for( unsigned int i = 0; i < l_vInputTowers.size(); i++ )
  {
    l_PddlFile << "    ( on-table b" << l_vInputTowers[i][0] << " )\n";
    for( unsigned int j = 1; j < l_vInputTowers[i].size(); j++ )
      l_PddlFile << "    ( on b" << l_vInputTowers[i][j]
		 << " b" << l_vInputTowers[i][j-1] << " )\n";
    l_PddlFile << "    ( clear b" << l_vInputTowers[i][l_vInputTowers[i].size() - 1] << " )\n";
  }

  l_PddlFile << "  )\n"
	     << "  ( :goal \n"
	     << "    ( and \n";

  for( unsigned int i = 0; i < l_vOutputTowers.size(); i++ )
  {
    l_PddlFile << "      ( on-table b" << l_vOutputTowers[i][0] << " )\n";
    for( unsigned int j = 1; j < l_vOutputTowers[i].size(); j++ )
      l_PddlFile << "      ( on b" << l_vOutputTowers[i][j]
		 << " b" << l_vOutputTowers[i][j-1] << " )\n";
    l_PddlFile << "      ( clear b" << l_vOutputTowers[i][l_vOutputTowers[i].size() - 1] << " )\n";
  }

  l_PddlFile << "    )\n"
	     << "  )\n"
	     << ")\n";
  l_PddlFile.close();


  if( argc == 3 )
    return 0;

  std::ofstream l_HtnFile( argv[3], std::ios_base::in | std::ios_base::trunc );
  assert( l_HtnFile.good() );

  l_HtnFile << "( defproblem bw4-"
	    << argv[1]
	    << " blocksworld\n"
	    << "  (\n";
  l_HtnFile << "    ( arm-empty )\n";

  for( unsigned int i = 0; i < l_vInputTowers.size(); i++ )
  {
    l_HtnFile << "    ( on-table b" << l_vInputTowers[i][0] << " )\n";
    for( unsigned int j = 1; j < l_vInputTowers[i].size(); j++ )
      l_HtnFile << "    ( on b" << l_vInputTowers[i][j]
		<< " b" << l_vInputTowers[i][j-1] << " )\n";
    l_HtnFile << "    ( clear b" << l_vInputTowers[i][l_vInputTowers[i].size() - 1] << " )\n";
  }

  l_HtnFile << "  )\n"
	    << "  (\n";

  for( unsigned int i = 0; i < l_vOutputTowers.size(); i++ )
  {
    l_HtnFile << "    ( MAKE-"
	      << l_vOutputTowers[i].size()
	      << "PILE ";
    for( unsigned int j = l_vOutputTowers[i].size(); j > 0; j-- )
      l_HtnFile << "b" << l_vOutputTowers[i][j - 1] << " ";
    l_HtnFile << ")\n";
  }

  l_HtnFile << "  )\n"
	    << ")\n";

  l_HtnFile.close();

}
