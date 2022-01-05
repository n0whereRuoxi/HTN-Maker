This file provides basic documentation for the ch-htn-tools suite of programs for HTN planning.  Support may be available by email at <chadhogg@gmail.com>.  The rest of this document is structured as follows:

1: Licensing Information
2: Installation Instructions
3: Components Of This Suite
4: Command Usage
4.1: Usage of htn-solver2
4.2: Usage of htn-maker
4.3: Usage of htn-maker (nondeterministic)
4.4: Simulation of Q-Maker, Q-Reinforce, and Q-SHOP

###############################################################################
# 1: Licensing                                                                #
###############################################################################

All of the source files in this distribution are Copyright Chad Hogg 2009.
This work was supported in part by the National Science Foundation, grant 0642882.

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program (see COPYING.txt).  If not, see <http://www.gnu.org/licenses/>.

###############################################################################
# 2: Installation                                                             #
###############################################################################

This program uses GNU autotools to aid compilation.  It is known to work in the following distributions of GNU/Linux: Debian 5.0, Ubuntu 9.04, and Red Hat Server 5.3.0.3, but should be amenable to other flavors of UNIX as well.  It may be possible to build with Microsoft tools as well, although this would require creating a project file for Visual Studio.  To build this software as intended you should need only an implementation of make, a C++ compiler, and a version of the C++ standard library that includes the extensions specified in C++ Technical Report 1.

To build this software, first run `./configure` to test your system configuration for these dependencies.  Then run `make` to build the object files.  If you wish to build only a single command (see list below), instead run `make <command-name>`.

This software is not intended to be installed to system directories.  If you wish to do so, simply copy the library and commands to wherever you would like them to be installed.

###############################################################################
# 3: Components                                                               #
###############################################################################

libhtntools.la - A library that contains all of the generic code used in the commands.  It will be automatically built as a dependency of any of the commands below.

htn-solver2 - An implementation of an Ordered Task Decomposition planner in the style of SHOP.  An OTD planner uses HTN methods to decompose an initial task network chaining forward from an initial state.  For more details, see "SHOP: Simple Hierarchical Ordered Planner" by D. S. Nau, Y. Cao, A. Lotem, and H. Munoz-Avila published in the Proceedings of the International Joint Conference on Artificial Intelligence in 1999.  Detailed usage instructions are below.

htn-solver - An earlier implementation of an OTD planner, left here for historical reasons but not supported.

htn-maker - An algorithm to learn HTN methods from non-hierarchical plans and annotated tasks.  For more details, see "HTN-Maker: Learning HTNs with Minimal Additional Knowledge Engineering Required" by C. Hogg, H. Munoz-Avila, and U. Kuter published in Proceedings of the AAAI Conference on Artificial Intelligence in 2008.  Detailed usage instructions are below.

bw-gen - A problem generator for the Blocks World domain.  This is old and unsupported.

htndiff - Determine whether or not two HTN domain descriptions are equivalent.

pddld2shopd - Convert an HTN domain description in PDDL format to SHOP format.

shopd2pddld - Convert an HTN domain description in SHOP format to PDDL format.  Note that this will only work for a very restricted SHOP format with features that are supported.

shopp2pddlp - Convert an HTN problem in SHOP format to PDDL format.  Note that this will only work for a very restricted SHOP format with features that are supported.

tester - A test suite, which will not be usable or meaningful outside of my development environment.

vanilla_ice - A very generic, inefficient forward-chaining state-space classical planner.

verifier_strips - Confirm that a plan is a solution to a classical planning problem.

###############################################################################
# 4: Usage                                                                    #
###############################################################################

Currently, only the htn-solver2 and htn-maker commands and variants thereof are expected to be used by anyone.  Documentation for others will be created as necessary.

  #############################################################################
  # 4.1: HTN-Solver2                                                          #
  #############################################################################

The htn-solver2 command requires an HTN domain description file and an HTN problem file in a bastardization of the Planning Domain Description Language.  The main components of the domain description are actions (as in the standard) and methods, which use a similar syntax providing the parameters, free variables, precondition, and subtasks.  The only non-STRIPS features supported are typing and equality.  Providing a list of predicates and constants is recommended for debugging but not required.  The main components of the problem file are the initial state and tasks to complete.

Running `./htn-solver2 --help` will print a listing of program options, but the most likely usecase is simply `./htn-solver2 <domain-file> <problem-file>`.  The `-t` or `--show_trace` argument will cause the program to output the entire decomposition tree from the initial task network to the solution plan, but depends on each of the methods having an associated ID (supplied with the `:id` extension to PDDL.  Seting `-d` or `--debug_level` to a value higher than 1 will cause the planner to be progressively more verbose about what it is doing.

The `examples` directory contains descriptions and sample problems in five planning domains.

  #############################################################################
  # 4.2: HTN-Maker                                                            #
  #############################################################################

The htn-maker command requires a classical planning domain description file, an annotated tasks description file, a classical planning problem file for the given domain, a solution file for the given problem, and a partial HTN planning domain description file.  It returns the HTN planning domain description, possibly enhanced with new HTN methods learned by analyzing the problem / solution pair.  Each of these files use a modified PDDL representation, as in htn-solver2.

Running `./htn-maker --help` will print a listing of program options.  The five input files mentioned above are required.  In addition, there are quite a few options that change the way HTN-Maker operates.  Currently, we believe that the best combination of options is to use "--drop_unneeded" "--force_ops_first", "--only_task_effects", "--require_new", and "--soundness_check".

The `examples` directory contains sample input files in several planning domains.  From the `examples/blocks-world` directory, for example, you could run the command `../../htn-maker --drop_unneeded --force_ops_first --only_task_effects --require_new --soundness_check domain_strips.pddl tasks.pddl prob01-strips.pddl prob01-solution.plan domain_partial_htn.pddl`.

  #############################################################################
  # 4.3: HTN-Maker (with nondeterminism)                                      #
  #############################################################################

The nondeterministic version of HTN-Maker was introduced in the paper "Learning Hierarchical Task Networks for Nondeterministic Planning Domains" by Chad Hogg, Hector Munoz-Avila, and Ugur Kuter, which was published as part of the Proceedings of the Twenty-First International Joint Conference on Artificial Intelligence (IJCAI-09).

Input files for this version should represent nondeterministic operators by having one deterministic operator for each possible outcome.  Traces that include such operators must be generated by software outside of this suite.  HTN-Maker can be run directly on these traces with the addition of the --nd_checkers flag to those recommended for work with deterministic domains.  Afterward, the resulting HTN domain file must be edited to replace the deterministic operator components back into nondeterministic operators.  The learned domains can then be used with the ND-SHOP planner, which is not part of this software distribution.

The `examples/nd-blocks-world` directory sample input and output files in a nondeterministic version of the Blocks World domain in which the usual operators may fail.  The file `domain_strips.lisp` contains a description of the domain operators in ND-SHOP format, while `domain_strips.pddl` contains those operators split for use with HTN-Maker.  File `prob01-strips.pddl` contains a non-hierarchical problem and `prob01-solution.pddl` contains a trace that solves it.  Running `../../htn-maker --nd_checkers --drop_unneeded --force_ops_first --only_task_effects --require_new --soundness_check domain_strips.pddl tasks.pddl prob01-strips.pddl prob01-solution.plan domain_partial_htn.pddl` should produce output similar to file `prob01-output.pddl`.  This can then be processed into `prob01-output.lisp` for use with ND-SHOP.

  #############################################################################
  # 4.4: Q-Maker, Q-Reinforce, & Q-SHOP                                       #
  #############################################################################

The paper "Learning Methods to Generate Good Plans: Integrating HTN Learning and Reinforcement Learning" by Chad Hogg, Ugur Kuter, and Hector Munoz-Avila describes three algorithms for learning and using HTN methods with value annotations.  Our implementation of these three algorithms consists of HTN-Maker and HTN-Solver2 with varying command-line options.  This implementation differs slightly from the theoretical presentation in that the value of a method is always non-negative and the greedy agent selects the method with lowest value.  Also, this implementation is tailored to the convention that the reward returned from the use of method is the sum of the costs of the actions in the decomposition, although it should be straightforward to generalize it.

To learn methods with initial estimates of their Q-values (that is, the Q-Maker algorithm), run `htn-maker` with the `--qvalues` option in addition to any others.  To refine the Q-values of existing methods (that is, the Q-Reinforce algorithm), run `htn-solver2` with the `--use_qvalues`, `--update_qvalues`, and `--random_selection` options.  To use the Q-values, run `htn-solver2` with the `--use_qvalues` option.  The partial HTN domain file passed as the fifth argument of `htn-maker` must declare the PDDL requirements `:q-values` and `:method-ids`.  (These are not part of the PDDL specification, but are our extensions as are method structures.)  The files in the `examples/qvalues-blocks-world` directory provide samples.
