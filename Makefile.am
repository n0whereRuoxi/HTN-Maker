
bin_PROGRAMS = tester vanilla_ice htn-maker htn-solver htn-solver2 bw-gen htndiff verifier_strips shopp2pddlp shopd2pddld pddld2shopd add-ids id-strips

lib_LTLIBRARIES = libhtntools.la

profiling = -rdynamic

libhtntools_la_SOURCES = \
	exception.cpp \
	funcs.cpp \
	string_table.cpp \
	type_table.cpp \
	term.cpp \
	term_string.cpp \
	term_variable.cpp \
	term_constant.cpp \
	term_variable_typed.cpp \
	term_constant_typed.cpp \
	term_table.cpp \
	substitution.cpp \
	formula.cpp \
	formula_pred.cpp \
	formula_equ.cpp \
	formula_neg.cpp \
	formula_conj.cpp \
	operator.cpp \
	state.cpp \
	strips_domain.cpp \
	strips_problem.cpp \
	strips_solution.cpp \
	htn_task_head.cpp \
	htn_task_descr.cpp \
	htn_task_list.cpp \
	htn_method.cpp \
	annotated_plan.cpp \
	htn_domain.cpp \
	partial_htn_method.cpp \
	htn_problem.cpp \
	htn_solution.cpp

libhtntools_la_LDFLAGS = -static ${profiling}

#CPPFLAGS =
CXXFLAGS =
#AM_CPPFLAGS =
AM_CXXFLAGS =

include_dir = -I ./include
release_flags = -O3 -ggdb -Wall -Wno-deprecated ${profiling} -D CATCH_EXCEPTS ${include_dir}

libhtntools_la_CPPFLAGS = ${release_flags}

tester_CPPFLAGS = ${release_flags}
vanilla_ice_CPPFLAGS = ${release_flags}
htn_maker_CPPFLAGS = ${release_flags}
htn_solver_CPPFLAGS = ${release_flags}
htn_solver2_CPPFLAGS = ${release_flags}
bw_gen_CPPFLAGS = ${release_flags}
htndiff_CPPFLAGS = ${release_flags}
verifier_strips_CPPFLAGS = ${release_flags}
shopp2pddlp_CPPFLAGS = ${release_flags}
shopd2pddld_CPPFLAGS = ${release_flags}
pddld2shopd_CPPFLAGS = ${release_flags}
add_ids_CPPFLAGS = ${release_flags}
id_strips_CPPFLAGS = ${release_flags}

tester_LDFLAGS = ${profiling}
vanilla_ice_LDFLAGS = ${profiling}
htn_maker_LDFLAGS = ${profiling}
htn_solver_LDFLAGS = ${profiling}
htn_solver2_LDFLAGS = ${profiling}
bw_gen_LDFLAGS = ${profiling}
htndiff_LDFLAGS = ${profiling}
verifier_strips_LDFLAGS = ${profiling}
shopp2pddlp_LDFLAGS = ${profiling}
shopd2pddld_LDFLAGS = ${profiling}
pddld2shopd_LDFLAGS = ${profiling}
add_ids_LDFLAGS = ${profiling}
id_strips_LDFLAGS = ${profiling}

noinst_HEADERS = \
	exception.hpp \
	funcs.hpp \
	string_table.hpp \
	type_table.hpp \
	term.hpp \
	term_string.hpp \
	term_constant.hpp \
	term_variable.hpp \
	term_variable_typed.hpp \
	term_constant_typed.hpp \
	term_table.hpp \
	substitution.hpp \
	formula.hpp \
	formula_pred.hpp \
	formula_equ.hpp \
	formula_neg.hpp \
	formula_conj.hpp \
	operator.hpp \
	state.hpp \
	strips_domain.hpp \
	strips_problem.hpp \
	strips_solution.hpp \
	htn_task_head.hpp \
	htn_task_descr.hpp \
	htn_task_list.hpp \
	htn_method.hpp \
	annotated_plan.hpp \
	htn_domain.hpp \
	partial_htn_method.hpp \
	htn_problem.hpp \
	htn_solution.hpp \
	test_funcs.hpp \
	include/tclap/ArgException.h \
	include/tclap/Arg.h \
	include/tclap/ArgTraits.h \
	include/tclap/CmdLine.h \
	include/tclap/CmdLineInterface.h \
	include/tclap/CmdLineOutput.h \
	include/tclap/Constraint.h \
	include/tclap/DocBookOutput.h \
	include/tclap/HelpVisitor.h \
	include/tclap/IgnoreRestVisitor.h \
	include/tclap/MultiArg.h \
	include/tclap/MultiSwitchArg.h \
	include/tclap/OptionalUnlabeledTracker.h \
	include/tclap/StandardTraits.h \
	include/tclap/StdOutput.h \
	include/tclap/SwitchArg.h \
	include/tclap/UnlabeledMultiArg.h \
	include/tclap/UnlabeledValueArg.h \
	include/tclap/ValueArg.h \
	include/tclap/ValuesConstraint.h \
	include/tclap/VersionVisitor.h \
	include/tclap/Visitor.h \
	include/tclap/XorHandler.h \
	include/tclap/ZshCompletionOutput.h

EXTRA_DIST = \
	COPYING.txt \
	README.txt \
	examples/blocks-world/domain_strips.pddl \
	examples/blocks-world/domain_htn.pddl \
	examples/blocks-world/prob01-strips.pddl \
	examples/blocks-world/prob01-htn.pddl \
	examples/blocks-world/prob02-strips.pddl \
	examples/blocks-world/prob02-htn.pddl \
	examples/blocks-world/prob03-strips.pddl \
	examples/blocks-world/prob03-htn.pddl \
	examples/blocks-world/domain_partial_htn.pddl \
	examples/blocks-world/tasks.pddl \
	examples/blocks-world/prob01-solution.plan \
	examples/depots/domain_strips.pddl \
	examples/depots/domain_htn.pddl \
	examples/depots/prob01-strips.pddl \
	examples/depots/prob01-htn.pddl \
	examples/depots/prob02-strips.pddl \
	examples/depots/prob02-htn.pddl \
	examples/depots/prob03-strips.pddl \
	examples/depots/prob03-htn.pddl \
	examples/depots/domain_partial_htn.pddl \
	examples/depots/tasks.pddl \
	examples/depots/prob01-solution.plan \
	examples/logistics/domain_strips.pddl \
	examples/logistics/domain_htn.pddl \
	examples/logistics/prob01-strips.pddl \
	examples/logistics/prob01-htn.pddl \
	examples/logistics/prob02-strips.pddl \
	examples/logistics/prob02-htn.pddl \
	examples/logistics/prob03-strips.pddl \
	examples/logistics/prob03-htn.pddl \
	examples/logistics/domain_partial_htn.pddl \
	examples/logistics/tasks.pddl \
	examples/logistics/prob01-solution.plan \
	examples/rovers/domain_strips.pddl \
	examples/rovers/domain_htn.pddl \
	examples/rovers/prob01-strips.pddl \
	examples/rovers/prob01-htn.pddl \
	examples/rovers/prob02-strips.pddl \
	examples/rovers/prob02-htn.pddl \
	examples/rovers/prob03-strips.pddl \
	examples/rovers/prob03-htn.pddl \
	examples/rovers/domain_partial_htn.pddl \
	examples/rovers/tasks.pddl \
	examples/rovers/prob01-solution.plan \
	examples/satellite/domain_strips.pddl \
	examples/satellite/domain_htn.pddl \
	examples/satellite/prob01-strips.pddl \
	examples/satellite/prob01-htn.pddl \
	examples/satellite/prob02-strips.pddl \
	examples/satellite/prob02-htn.pddl \
	examples/satellite/prob03-strips.pddl \
	examples/satellite/prob03-htn.pddl \
	examples/satellite/domain_partial_htn.pddl \
	examples/satellite/tasks.pddl \
	examples/satellite/prob01-solution.plan


tester_SOURCES = test_funcs.cpp tester.cpp
tester_LDADD = libhtntools.la

vanilla_ice_SOURCES = vanilla_ice.cpp
vanilla_ice_LDADD = libhtntools.la

htn_maker_SOURCES = htn-maker.cpp
htn_maker_LDADD = libhtntools.la

htn_solver_SOURCES = htn-solver.cpp
htn_solver_LDADD = libhtntools.la

htn_solver2_SOURCES = htn-solver2.cpp
htn_solver2_LDADD = libhtntools.la

bw_gen_SOURCES = \
	bw-gen.cpp

htndiff_SOURCES = htndiff.cpp
htndiff_LDADD = libhtntools.la

verifier_strips_SOURCES = verifier_strips.cpp
verifier_strips_LDADD = libhtntools.la

shopp2pddlp_SOURCES = shopp2pddlp.cpp
shopp2pddlp_LDADD = libhtntools.la

shopd2pddld_SOURCES = shopd2pddld.cpp
shopd2pddld_LDADD = libhtntools.la

pddld2shopd_SOURCES = pddld2shopd.cpp
pddld2shopd_LDADD = libhtntools.la

add_ids_SOURCES = add-ids.cpp
add_ids_LDADD = libhtntools.la

id_strips_SOURCES = id-strips.cpp
id_strips_LDADD = libhtntools.la

include aminclude.am
