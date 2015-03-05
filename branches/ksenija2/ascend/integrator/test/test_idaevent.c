#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <ascend/general/env.h>
#include <ascend/general/ospath.h>
#include <ascend/general/list.h>
#include <ascend/general/ltmatrix.h>

#include <ascend/general/platform.h>
#include <ascend/utilities/ascEnvVar.h>
#include <ascend/utilities/error.h>

#include <ascend/compiler/ascCompiler.h>
#include <ascend/compiler/module.h>
#include <ascend/compiler/parser.h>
#include <ascend/compiler/library.h>
#include <ascend/compiler/symtab.h>
#include <ascend/compiler/simlist.h>
#include <ascend/compiler/instquery.h>
#include <ascend/compiler/parentchild.h>
#include <ascend/compiler/atomvalue.h>
#include <ascend/compiler/relation_io.h>
#include <ascend/compiler/reverse_ad.h>
#include <ascend/compiler/relation_util.h>
#include <ascend/compiler/mathinst.h>
#include <ascend/compiler/watchpt.h>
#include <ascend/compiler/initialize.h>
#include <ascend/compiler/name.h>
#include <ascend/compiler/visitinst.h>
#include <ascend/compiler/functype.h>
#include <ascend/compiler/safe.h>
#include <ascend/compiler/qlfdid.h>
#include <ascend/compiler/instance_io.h>
#include <ascend/compiler/packages.h>

#include <ascend/compiler/slvreq.h>

#include <ascend/system/system.h>
#include <ascend/system/slv_client.h>
#include <ascend/solver/solver.h>
#include <ascend/system/slv_server.h>
#include <ascend/system/slv_param.h>
#include <ascend/integrator/integrator.h>

#include <test/common.h>
#include <test/test_globals.h>

/* a simple integrator reporter for testing */
static int test_ida_reporter_init(struct IntegratorSystemStruct *integ) {
	return 0;
}

static int test_ida_reporter_write(struct IntegratorSystemStruct *integ) {
	/*double val;
	val = var_value(integ->y[0]);
	CONSOLE_DEBUG("y[0] = %g", val);*/
	return 1; /* no interrupt */
}

static int test_ida_reporter_writeobs(struct IntegratorSystemStruct *integ) {
	/*CONSOLE_DEBUG("x = %f", var_value(integ->x));*/
	return 0;
}

static int test_ida_reporter_close(struct IntegratorSystemStruct *integ) {
	return 0;
}

static IntegratorReporter test_ida_reporter = { test_ida_reporter_init,
		test_ida_reporter_write, test_ida_reporter_writeobs,
		test_ida_reporter_close };

static void test_hysteron(){

	Asc_CompilerInit(1);

	/* set paths relative to test executable */
	Asc_PutEnv(ASC_ENV_LIBRARY "=models");
	Asc_PutEnv(ASC_ENV_SOLVERS "=solvers/ida" OSPATH_DIV "solvers/lrslv:solvers/qrslv");
	CU_TEST_FATAL(0 == package_load("lrslv",NULL));
	CU_TEST_FATAL(0 == package_load("qrslv",NULL));

	/* load the file */
#define FILESTEM "hysteron_event"
	char path[PATH_MAX] = "ksenija/" FILESTEM ".a4c";
	{
		int status;
		Asc_OpenModule(path, &status);
		CU_ASSERT_FATAL(status == 0);
	}

	/* parse it */
	CU_ASSERT(0 == zz_parse());

	/* find the model */
	CU_ASSERT(FindType(AddSymbol(FILESTEM))!=NULL);

	/* instantiate it */
	struct Instance *siminst = SimsCreateInstance(AddSymbol(FILESTEM), AddSymbol("sim1"), e_normal, NULL);
	CU_ASSERT_FATAL(siminst!=NULL);
#undef FILESTEM

	CONSOLE_DEBUG("RUNNING ON_LOAD");

	/** Call on_load */
	struct Name *name = CreateIdName(AddSymbol("on_load"));
	enum Proc_enum pe = Initialize(GetSimulationRoot(siminst),name,"sim1", ASCERR, WP_STOPONERR, NULL, NULL);
	CU_ASSERT(pe==Proc_all_ok);

	/* create the integrator */

	g_use_dersyntax = 1;
	slv_system_t sys = system_build(GetSimulationRoot(siminst));
	CU_ASSERT_FATAL(sys != NULL);

	IntegratorSystem *integ = integrator_new(sys,siminst);

	CU_ASSERT_FATAL(0 == integrator_set_engine(integ,"IDA"));
	CONSOLE_DEBUG("Assigned integrator '%s'...",integ->internals->name);

	slv_parameters_t p;
	CU_ASSERT(0 == integrator_params_get(integ,&p));
	/* TODO set some parameters? */

	/* perform problem analysis */
	CU_ASSERT_FATAL(0 == integrator_analyse(integ));

	/* TODO assign an integrator reporter */
	integrator_set_reporter(integ, &test_ida_reporter);

	integrator_set_minstep(integ,0.00001);
	integrator_set_maxstep(integ,0.01);
	integrator_set_stepzero(integ,0.0001);
	integrator_set_maxsubsteps(integ,200);

	/* set a linearly-distributed samplelist */
	double start = 0, end = 30;
	int num = 60;
	dim_type d;
	SetDimFraction(d,D_TIME,CreateFraction(1,1));
	SampleList *samplelist = samplelist_new(num+1, &d);
	double val = start;
	double inc = (end-start)/(num);
	unsigned long i;
	for(i=0; i<=num; ++i){
		samplelist_set(samplelist,i,val);
		val += inc;
	}
	integrator_set_samples(integ,samplelist);

	CU_ASSERT_FATAL(0 == integrator_solve(integ, 0, samplelist_length(samplelist)-1));

	integrator_free(integ);
	samplelist_free(samplelist);

	CU_ASSERT_FATAL(NULL != sys);
	system_destroy(sys);
	system_free_reused_mem();

	struct Instance *simroot = GetSimulationRoot(siminst);
	CU_TEST(simroot != NULL);
	struct Instance *ix = ChildByChar(simroot,AddSymbol("x"));
	struct Instance *ix0 = ChildByChar(simroot,AddSymbol("x0"));
	CU_TEST(ix != NULL);
	CU_TEST(ix0 != NULL);

	CONSOLE_DEBUG("Final x = %e",RealAtomValue(ix));
	CONSOLE_DEBUG("Final x0 = %e",RealAtomValue(ix0));

	CU_TEST(fabs(RealAtomValue(ix) - 0.7756) < 1e-4);
	CU_TEST(fabs(RealAtomValue(ix0) - -3.0353) < 1e-4);

	/* destroy all that stuff */
	CU_ASSERT(siminst != NULL);

	solver_destroy_engines();
	integrator_free_engines();
	sim_destroy(siminst);
	Asc_CompilerDestroy();
}

static void test_test1(){

	Asc_CompilerInit(1);

	Asc_PutEnv(ASC_ENV_LIBRARY "=models");
	Asc_PutEnv(ASC_ENV_SOLVERS "=solvers/ida" OSPATH_DIV "solvers/lrslv:solvers/qrslv");
	CU_TEST_FATAL(0 == package_load("lrslv",NULL));
	CU_TEST_FATAL(0 == package_load("qrslv",NULL));

#define FILESTEM "test_event1"
	char path[PATH_MAX] = "ksenija/" FILESTEM ".a4c";
	{
		int status;
		Asc_OpenModule(path, &status);
		CU_ASSERT_FATAL(status == 0);
	}

	CU_ASSERT(0 == zz_parse());

	CU_ASSERT(FindType(AddSymbol(FILESTEM))!=NULL);

	struct Instance *siminst = SimsCreateInstance(AddSymbol(FILESTEM), AddSymbol("sim1"), e_normal, NULL);
	CU_ASSERT_FATAL(siminst!=NULL);
#undef FILESTEM

	CONSOLE_DEBUG("RUNNING ON_LOAD");

	struct Name *name = CreateIdName(AddSymbol("on_load"));
	enum Proc_enum pe = Initialize(GetSimulationRoot(siminst),name,"sim1", ASCERR, WP_STOPONERR, NULL, NULL);
	CU_ASSERT(pe==Proc_all_ok);


	g_use_dersyntax = 1;
	slv_system_t sys = system_build(GetSimulationRoot(siminst));
	CU_ASSERT_FATAL(sys != NULL);

	IntegratorSystem *integ = integrator_new(sys,siminst);

	CU_ASSERT_FATAL(0 == integrator_set_engine(integ,"IDA"));
	CONSOLE_DEBUG("Assigned integrator '%s'...",integ->internals->name);

	slv_parameters_t p;
	CU_ASSERT(0 == integrator_params_get(integ,&p));

	CU_ASSERT_FATAL(0 == integrator_analyse(integ));

	integrator_set_reporter(integ, &test_ida_reporter);

	integrator_set_minstep(integ,0.00001);
	integrator_set_maxstep(integ,0.01);
	integrator_set_stepzero(integ,0.0001);
	integrator_set_maxsubsteps(integ,200);

	double start = 0, end = 30;
	int num = 100;
	dim_type d;
	SetDimFraction(d,D_TIME,CreateFraction(1,1));
	SampleList *samplelist = samplelist_new(num+1, &d);
	double val = start;
	double inc = (end-start)/(num);
	unsigned long i;
	for(i=0; i<=num; ++i){
		samplelist_set(samplelist,i,val);
		val += inc;
	}
	integrator_set_samples(integ,samplelist);

	CU_ASSERT_FATAL(0 == integrator_solve(integ, 0, samplelist_length(samplelist)-1));

	integrator_free(integ);
	samplelist_free(samplelist);

	CU_ASSERT_FATAL(NULL != sys);
	system_destroy(sys);
	system_free_reused_mem();

	struct Instance *simroot = GetSimulationRoot(siminst);
	CU_TEST(simroot != NULL);
	struct Instance *ix = ChildByChar(simroot,AddSymbol("x"));
	CU_TEST(ix != NULL);

	CONSOLE_DEBUG("Final x = %e",RealAtomValue(ix));

	CU_TEST(fabs(RealAtomValue(ix) - 16.2269) < 1e-4);

	CU_ASSERT(siminst != NULL);

	solver_destroy_engines();
	integrator_free_engines();
	sim_destroy(siminst);
	Asc_CompilerDestroy();
}

static void test_test2(){

	Asc_CompilerInit(1);

	Asc_PutEnv(ASC_ENV_LIBRARY "=models");
	Asc_PutEnv(ASC_ENV_SOLVERS "=solvers/ida" OSPATH_DIV "solvers/lrslv:solvers/qrslv");
	CU_TEST_FATAL(0 == package_load("lrslv",NULL));
	CU_TEST_FATAL(0 == package_load("qrslv",NULL));

#define FILESTEM "test_event2"
	char path[PATH_MAX] = "ksenija/" FILESTEM ".a4c";
	{
		int status;
		Asc_OpenModule(path, &status);
		CU_ASSERT_FATAL(status == 0);
	}

	CU_ASSERT(0 == zz_parse());

	CU_ASSERT(FindType(AddSymbol(FILESTEM))!=NULL);

	struct Instance *siminst = SimsCreateInstance(AddSymbol(FILESTEM), AddSymbol("sim1"), e_normal, NULL);
	CU_ASSERT_FATAL(siminst!=NULL);
#undef FILESTEM

	CONSOLE_DEBUG("RUNNING ON_LOAD");

	struct Name *name = CreateIdName(AddSymbol("on_load"));
	enum Proc_enum pe = Initialize(GetSimulationRoot(siminst),name,"sim1", ASCERR, WP_STOPONERR, NULL, NULL);
	CU_ASSERT(pe==Proc_all_ok);


	g_use_dersyntax = 1;
	slv_system_t sys = system_build(GetSimulationRoot(siminst));
	CU_ASSERT_FATAL(sys != NULL);

	IntegratorSystem *integ = integrator_new(sys,siminst);

	CU_ASSERT_FATAL(0 == integrator_set_engine(integ,"IDA"));
	CONSOLE_DEBUG("Assigned integrator '%s'...",integ->internals->name);

	slv_parameters_t p;
	CU_ASSERT(0 == integrator_params_get(integ,&p));

	CU_ASSERT_FATAL(0 == integrator_analyse(integ));

	integrator_set_reporter(integ, &test_ida_reporter);

	integrator_set_minstep(integ,0.00001);
	integrator_set_maxstep(integ,0.01);
	integrator_set_stepzero(integ,0.0001);
	integrator_set_maxsubsteps(integ,200);

	double start = 0, end = 5;
	int num = 100;
	dim_type d;
	SetDimFraction(d,D_TIME,CreateFraction(1,1));
	SampleList *samplelist = samplelist_new(num+1, &d);
	double val = start;
	double inc = (end-start)/(num);
	unsigned long i;
	for(i=0; i<=num; ++i){
		samplelist_set(samplelist,i,val);
		val += inc;
	}
	integrator_set_samples(integ,samplelist);

	CU_ASSERT_FATAL(0 == integrator_solve(integ, 0, samplelist_length(samplelist)-1));

	integrator_free(integ);
	samplelist_free(samplelist);

	CU_ASSERT_FATAL(NULL != sys);
	system_destroy(sys);
	system_free_reused_mem();

	struct Instance *simroot = GetSimulationRoot(siminst);
	CU_TEST(simroot != NULL);
	struct Instance *ix = ChildByChar(simroot,AddSymbol("x"));
	CU_TEST(ix != NULL);

	CONSOLE_DEBUG("Final x = %e",RealAtomValue(ix));

	CU_TEST(fabs(RealAtomValue(ix) - 208.966) < 1e-4);

	CU_ASSERT(siminst != NULL);

	solver_destroy_engines();
	integrator_free_engines();
	sim_destroy(siminst);
	Asc_CompilerDestroy();
}


static void test_successive(){

	Asc_CompilerInit(1);

	/* set paths relative to test executable */
	Asc_PutEnv(ASC_ENV_LIBRARY "=models");
	Asc_PutEnv(ASC_ENV_SOLVERS "=solvers/ida" OSPATH_DIV "solvers/lrslv:solvers/qrslv");
	CU_TEST_FATAL(0 == package_load("lrslv",NULL));
	CU_TEST_FATAL(0 == package_load("qrslv",NULL));

	/* load the file */
#define FILESTEM "successive"
	char path[PATH_MAX] = "ksenija/" FILESTEM ".a4c";
	{
		int status;
		Asc_OpenModule(path, &status);
		CU_ASSERT_FATAL(status == 0);
	}

	/* parse it */
	CU_ASSERT(0 == zz_parse());

	/* find the model */
	CU_ASSERT(FindType(AddSymbol(FILESTEM))!=NULL);

	/* instantiate it */
	struct Instance *siminst = SimsCreateInstance(AddSymbol(FILESTEM), AddSymbol("sim1"), e_normal, NULL);
	CU_ASSERT_FATAL(siminst!=NULL);
#undef FILESTEM

	CONSOLE_DEBUG("RUNNING ON_LOAD");

	/** Call on_load */
	struct Name *name = CreateIdName(AddSymbol("on_load"));
	enum Proc_enum pe = Initialize(GetSimulationRoot(siminst),name,"sim1", ASCERR, WP_STOPONERR, NULL, NULL);
	CU_ASSERT(pe==Proc_all_ok);

	/* create the integrator */

	g_use_dersyntax = 1;
	slv_system_t sys = system_build(GetSimulationRoot(siminst));
	CU_ASSERT_FATAL(sys != NULL);

	IntegratorSystem *integ = integrator_new(sys,siminst);

	CU_ASSERT_FATAL(0 == integrator_set_engine(integ,"IDA"));
	CONSOLE_DEBUG("Assigned integrator '%s'...",integ->internals->name);

	slv_parameters_t p;
	CU_ASSERT(0 == integrator_params_get(integ,&p));
	/* TODO set some parameters? */

	/* perform problem analysis */
	CU_ASSERT_FATAL(0 == integrator_analyse(integ));

	/* TODO assign an integrator reporter */
	integrator_set_reporter(integ, &test_ida_reporter);

	integrator_set_minstep(integ,0.00001);
	integrator_set_maxstep(integ,0.01);
	integrator_set_stepzero(integ,0.0001);
	integrator_set_maxsubsteps(integ,200);

	/* set a linearly-distributed samplelist */
	double start = 0, end = 10;
	int num = 100;
	dim_type d;
	SetDimFraction(d,D_TIME,CreateFraction(1,1));
	SampleList *samplelist = samplelist_new(num+1, &d);
	double val = start;
	double inc = (end-start)/(num);
	unsigned long i;
	for(i=0; i<=num; ++i){
		samplelist_set(samplelist,i,val);
		val += inc;
	}
	integrator_set_samples(integ,samplelist);

	CU_ASSERT_FATAL(0 == integrator_solve(integ, 0, samplelist_length(samplelist)-1));

	integrator_free(integ);
	samplelist_free(samplelist);

	CU_ASSERT_FATAL(NULL != sys);
	system_destroy(sys);
	system_free_reused_mem();

	struct Instance *simroot = GetSimulationRoot(siminst);
	CU_TEST(simroot != NULL);
	struct Instance *ix = ChildByChar(simroot,AddSymbol("y"));
	CU_TEST(ix != NULL);

	CONSOLE_DEBUG("Final y = %e",RealAtomValue(ix));

	CU_TEST(fabs(RealAtomValue(ix) - 210) < 1e-4);

	/* destroy all that stuff */
	CU_ASSERT(siminst != NULL);

	solver_destroy_engines();
	integrator_free_engines();
	sim_destroy(siminst);
	Asc_CompilerDestroy();
}

static void test_bball(){

	Asc_CompilerInit(1);

	/* set paths relative to test executable */
	Asc_PutEnv(ASC_ENV_LIBRARY "=models");
	Asc_PutEnv(ASC_ENV_SOLVERS "=solvers/ida" OSPATH_DIV "solvers/lrslv:solvers/qrslv");
	CU_TEST_FATAL(0 == package_load("lrslv",NULL));
	CU_TEST_FATAL(0 == package_load("qrslv",NULL));

	/* load the file */
#define FILESTEM "bball_event3"
	char path[PATH_MAX] = "ksenija/" FILESTEM ".a4c";
	{
		int status;
		Asc_OpenModule(path, &status);
		CU_ASSERT_FATAL(status == 0);
	}

	/* parse it */
	CU_ASSERT(0 == zz_parse());

	/* find the model */
	CU_ASSERT(FindType(AddSymbol(FILESTEM))!=NULL);

	/* instantiate it */
	struct Instance *siminst = SimsCreateInstance(AddSymbol(FILESTEM), AddSymbol("sim1"), e_normal, NULL);
	CU_ASSERT_FATAL(siminst!=NULL);
#undef FILESTEM

	CONSOLE_DEBUG("RUNNING ON_LOAD");

	/** Call on_load */
	struct Name *name = CreateIdName(AddSymbol("on_load"));
	enum Proc_enum pe = Initialize(GetSimulationRoot(siminst),name,"sim1", ASCERR, WP_STOPONERR, NULL, NULL);
	CU_ASSERT(pe==Proc_all_ok);

	/* create the integrator */

	g_use_dersyntax = 1;
	slv_system_t sys = system_build(GetSimulationRoot(siminst));
	CU_ASSERT_FATAL(sys != NULL);

	IntegratorSystem *integ = integrator_new(sys,siminst);

	CU_ASSERT_FATAL(0 == integrator_set_engine(integ,"IDA"));
	CONSOLE_DEBUG("Assigned integrator '%s'...",integ->internals->name);

	slv_parameters_t p;
	CU_ASSERT(0 == integrator_params_get(integ,&p));
	/* TODO set some parameters? */

	/* perform problem analysis */
	CU_ASSERT_FATAL(0 == integrator_analyse(integ));

	/* TODO assign an integrator reporter */
	integrator_set_reporter(integ, &test_ida_reporter);

	integrator_set_minstep(integ,0.00001);
	integrator_set_maxstep(integ,0.01);
	integrator_set_stepzero(integ,0.0001);
	integrator_set_maxsubsteps(integ,200);

	/* set a linearly-distributed samplelist */
	double start = 0, end = 60;
	int num = 100;
	dim_type d;
	SetDimFraction(d,D_TIME,CreateFraction(1,1));
	SampleList *samplelist = samplelist_new(num+1, &d);
	double val = start;
	double inc = (end-start)/(num);
	unsigned long i;
	for(i=0; i<=num; ++i){
		samplelist_set(samplelist,i,val);
		val += inc;
	}
	integrator_set_samples(integ,samplelist);

	CU_ASSERT_FATAL(0 == integrator_solve(integ, 0, samplelist_length(samplelist)-1));

	integrator_free(integ);
	samplelist_free(samplelist);

	CU_ASSERT_FATAL(NULL != sys);
	system_destroy(sys);
	system_free_reused_mem();

	struct Instance *simroot = GetSimulationRoot(siminst);
	CU_TEST(simroot != NULL);
	struct Instance *ix = ChildByChar(simroot,AddSymbol("y"));
	CU_TEST(ix != NULL);
	struct Instance *ir = ChildByChar(simroot,AddSymbol("r"));
	CU_TEST(ix != NULL);

	CONSOLE_DEBUG("Final y = %e",RealAtomValue(ix));

	CU_TEST(fabs(RealAtomValue(ix) - RealAtomValue(ir)) < 1e-4);

	/* destroy all that stuff */
	CU_ASSERT(siminst != NULL);

	solver_destroy_engines();
	integrator_free_engines();
	sim_destroy(siminst);
	Asc_CompilerDestroy();
}



/*===========================================================================*/
/* Registration information */

#define TESTS(T) \
	T(hysteron) \
	T(test1) \
	T(test2) \
	T(successive) \
	T(bball)


REGISTER_TESTS_SIMPLE(integrator_idaevent, TESTS)
