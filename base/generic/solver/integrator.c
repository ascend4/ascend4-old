/*	ASCEND modelling environment
	Copyright (C) 2006 Carnegie Mellon University

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2, or (at your option)
	any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330,
	Boston, MA 02111-1307, USA.
*//**
	@file
	Integrator API for ASCEND, for solving systems of ODEs and/or DAEs.
*//*
	by John Pye, May 2006
	based on parts of Integrators.c in the Tcl/Tk interface directory, heavily
	modified to provide a non-GUI-specific API and modularised for multiple
	integration engines.
*/
#include <time.h>
#include "integrator.h"
#include "lsode.h"
#include "ida.h"

#include "samplelist.h"

/**
	Define as TRUE to enable debug message printing.
	@TODO this needs to go away.
*/
#define INTEG_DEBUG TRUE

/**
	Print a debug message with value if INTEG_DEBUG is TRUE.
*/
#define print_debug(msg, value) \
	if(INTEG_DEBUG){ CONSOLE_DEBUG(msg, value); }

/**
	Print a debug message string if INTEG_DEBUG is TRUE.
*/
#define print_debugstring(msg) \
	if(INTEG_DEBUG){ CONSOLE_DEBUG(msg); }

/*------------------------------------------------------------------------------
   The following names are of solver_var children or attributes
 * we support (at least temporarily) to determine who is a state and
 * who matching derivative.
 * These should be supported directly in a future solveratominst.
 */

static symchar *g_symbols[4];

#define STATEFLAG g_symbols[0]
/*
	Integer child. 0= algebraic, 1 = state, 2 = derivative, 3 = 2nd deriv etc
	independent variable is -1.
*/
#define INTEG_OTHER_VAR -1L
#define INTEG_ALGEBRAIC_VAR 0L
#define INTEG_STATE_VAR 1L

#define STATEINDEX g_symbols[1]
/* Integer child. all variables with the same STATEINDEX value are taken to
 * be derivatives of the same state variable. We really need a compiler
 * that maintains this info by backpointers, but oh well until then.
 */
#define OBSINDEX g_symbols[2]
/* Integer child. All variables with OBSINDEX !=0 will be recorded in
 * the blsode output file. Tis someone else's job to grok this output.
 */

#define FIXEDSYMBOL g_symbols[3]

/** Temporary catcher of dynamic variable and observation variable data */
struct Integ_var_t {
  long index;
  long type;
  struct Integ_var_t *derivative;
  struct Integ_var_t *derivative_of;
  struct var_variable *i;
  int varindx; /**< index into slv_get_master_vars_list, or -1 if not there */
  int isstate;
};

/*------------------------------------------------------------------------------
  forward declarations
*/

/* abstractions of setup/teardown procedures for the specific solvers */
void integrator_create_engine(IntegratorSystem *sys);
void integrator_free_engine(IntegratorSystem *sys);

static int integrator_analyse_ode(IntegratorSystem *sys);
static int integrator_analyse_dae(IntegratorSystem *sys);

typedef void (IntegratorVarVisitorFn)(IntegratorSystem *sys, struct var_variable *var, const int *varindx);
static void integrator_visit_system_vars(IntegratorSystem *sys,IntegratorVarVisitorFn *visitor);
IntegratorVarVisitorFn integrator_ode_classify_var;
IntegratorVarVisitorFn integrator_dae_classify_var;
IntegratorVarVisitorFn integrator_classify_indep_var;

static int integrator_sort_obs_vars(IntegratorSystem *sys);
static void integrator_print_var_stats(IntegratorSystem *sys);
static int integrator_check_indep_var(IntegratorSystem *sys);

static int Integ_CmpDynVars(struct Integ_var_t *v1, struct Integ_var_t *v2);
static int Integ_CmpObs(struct Integ_var_t *v1, struct Integ_var_t *v2);
static void Integ_SetObsId(struct var_variable *v, long oindex);

static long DynamicVarInfo(struct var_variable *v,long *vindex);
static struct var_variable *ObservationVar(struct var_variable *v, long *oindex);
static void IntegInitSymbols(void);

/*------------------------------------------------------------------------------
  INSTANTIATION AND DESTRUCTION
*/

/**
	Create a new IntegratorSystem and assign a slv_system_t to it.
*/
IntegratorSystem *integrator_new(slv_system_t slvsys, struct Instance *inst){
	IntegratorSystem *sys;

	if (slvsys == NULL) {
		ERROR_REPORTER_HERE(ASC_PROG_ERR,"slvsys is NULL!");
		return NULL;
	}

	sys = ASC_NEW_CLEAR(IntegratorSystem);
	sys->system = slvsys;
	sys->instance = inst;

	sys->states = NULL; sys->derivs = NULL;
	sys->dynvars = NULL; sys->obslist = NULL; sys->indepvars = NULL;

	sys->y_id = NULL;
	sys->obs_id = NULL;
	sys->y = NULL;
	sys->ydot = NULL;
	sys->obs = NULL;
	return sys;
}

/**
	Carefully trash any data in the IntegratorSystem that we own,
	then destroy the IntegratorSystem struct.

	Note that the integrator doesn't own the samplelist.

	@param sys will be destroyed and set to NULL.
*/
void integrator_free(IntegratorSystem *sys){
	if(sys==NULL)return;

	integrator_free_engine(sys);

	if(sys->states != NULL)gl_destroy(sys->states);
	if(sys->derivs != NULL)gl_destroy(sys->derivs);

	if(sys->dynvars != NULL)gl_free_and_destroy(sys->dynvars);    /* we own the objects in dynvars */
	if(sys->obslist != NULL)gl_free_and_destroy(sys->obslist);    /* and obslist */
	if (sys->indepvars != NULL)gl_free_and_destroy(sys->indepvars);  /* and indepvars */

	/* if(sys->y_id != NULL)ASC_FREE(sys->y_id); */
	if(sys->obs_id != NULL)ASC_FREE(sys->obs_id);

	if(sys->y != NULL && !sys->ycount)ASC_FREE(sys->y);
	if(sys->ydot != NULL && !sys->ydotcount)ASC_FREE(sys->ydot);
	if(sys->obs != NULL && !sys->obscount)ASC_FREE(sys->obs);

	ascfree(sys);
	sys=NULL;
}

/**
	Utility function to retreive pointers to the symbols we'll be looking for
	in the instance hierarchy.
*/
static void IntegInitSymbols(void){
  STATEFLAG = AddSymbol("ode_type");
  STATEINDEX = AddSymbol("ode_id");
  OBSINDEX = AddSymbol("obs_id");
  FIXEDSYMBOL = AddSymbol("fixed");
}

/*------------------------------------------------------------------------------
  INTEGRATOR ENGINE
*/

/* return 1 on success */
int integrator_set_engine(IntegratorSystem *sys, IntegratorEngine engine){

	/* verify integrator type ok. always passes for nonNULL inst. */
	if(engine==INTEG_UNKNOWN){
		ERROR_REPORTER_NOLINE(ASC_USER_ERROR
			,"Integrator has not been specified (or is unknown)."
		);
		return 0;
	}

	if(engine==sys->engine){
		return 1;
	}
	if(sys->engine!=INTEG_UNKNOWN){
		integrator_free_engine(sys);
	}
	sys->engine = engine;
	integrator_create_engine(sys);

	return 1;
}

IntegratorEngine integrator_get_engine(const IntegratorSystem *sys){
	return sys->engine;
}

/**
	Free any engine-specific  data that was required for the solution of
	this system. Note that this data is pointed to by sys->enginedata.
*/
void integrator_free_engine(IntegratorSystem *sys){
	switch(sys->engine){
		case INTEG_LSODE: integrator_lsode_free(sys->enginedata); break;
#ifdef ASC_WITH_IDA
		case INTEG_IDA: integrator_ida_free(sys->enginedata); break;
#endif
		default: break;
	}
	sys->enginedata=NULL;
}

/**
	Create enginedata memory if required for this solver. This doesn't include
	allocating computation space, since we assume that this stage all we know
	is that we want to use a specified integrator engine, not the full details
	of the problem at hand. Allocating space inside enginedata should be done
	during the solve stage (and freed inside integrator_free_engine)
*/
void integrator_create_engine(IntegratorSystem *sys){
	if(sys->enginedata!=NULL)return;
	switch(sys->engine){
		case INTEG_LSODE: integrator_lsode_create(sys); break;
#ifdef ASC_WITH_IDA
		case INTEG_IDA: integrator_ida_create(sys); break;
#endif
		default: break;
	}
}

/*------------------------------------------------------------------------------
  ANALYSIS

  Provide two modes in order to provide analysis suitable for solution of both
  ODEs (the previous technique) and DAEs (new code). These share a common
  "visit" method that needs to eventually be integrated with the code in
  <solver/analyze.c>. For the moment, we're just hacking in to the compiler.
*/

/**
	Locate the independent variable. For the purpose of GUI design, this needs
	to work independent of the integration engine being used.
*/
int integrator_find_indep_var(IntegratorSystem *sys){
	int result = 0;

	if(sys->x != NULL){
		CONSOLE_DEBUG("sys->x already set");
		return 1;
	}
	assert(sys->indepvars==NULL);
	sys->indepvars = gl_create(10L);

	IntegInitSymbols();

	/* CONSOLE_DEBUG("About to visit..."); */
	integrator_visit_system_vars(sys,&integrator_classify_indep_var);

	/* CONSOLE_DEBUG("..."); */

	result = integrator_check_indep_var(sys);
	gl_free_and_destroy(sys->indepvars);
	sys->indepvars = NULL;

	/* ERROR_REPORTER_HERE(ASC_PROG_NOTE,"Returning result %d",result); */

	return result;
}

/**
	Analyse the system, either as DAE or as an ODE system, depending on the
	solver engine selected.

	@return 1 on success
*/
int integrator_analyse(IntegratorSystem *sys){
	switch(sys->engine){
		case INTEG_LSODE: return integrator_analyse_ode(sys);
#ifdef ASC_WITH_IDA
		case INTEG_IDA: return integrator_analyse_dae(sys);
#endif
		case INTEG_UNKNOWN:
			ERROR_REPORTER_HERE(ASC_PROG_ERR,"No engine selected: can't analyse");
		default:
			ERROR_REPORTER_HERE(ASC_PROG_ERR
				,"The selected integration engine (%d) is not available"
				,sys->engine
			);
	}
	return 0;
}

/**
	To analyse a DAE we need to identify *ALL* variables in the system
	Except for the highest-level derivatives of any present?
	We also need to identify the independent variable (just one).

	@TODO implement Pantelides algorithm in here?
	@TODO prevent re-analysis without clearing out the data structures?
	@return 1 on success
*/
int integrator_analyse_dae(IntegratorSystem *sys){
	struct Integ_var_t *info, *prev;
	char *varname, *derivname;
	struct var_variable **varlist;
	int nvarlist;
	int i, j;
	int numstates;
	int numy, nrels;
	int yindex;
	int maxderiv;

	CONSOLE_DEBUG("Starting DAE analysis");
	IntegInitSymbols();

	assert(sys->indepvars==NULL);

	sys->indepvars = gl_create(10L);  /* t var info */
	sys->dynvars = gl_create(200L);  /* y and ydot var info */
	sys->obslist = gl_create(100L);  /* obs info */

	if(sys->indepvars==NULL
		|| sys->dynvars==NULL
		|| sys->obslist==NULL
	){
		ERROR_REPORTER_HERE(ASC_PROG_ERR,"Insufficient memory");
		return 0;
	}

	integrator_visit_system_vars(sys,&integrator_dae_classify_var);

	CONSOLE_DEBUG("Found %lu observation variables:",gl_length(sys->obslist));
	for(i=1; i<=gl_length(sys->obslist); ++i){
		info = (struct Integ_var_t *)gl_fetch(sys->obslist, i);
		varname = var_make_name(sys->system,info->i);
		CONSOLE_DEBUG("observation[%d] = \"%s\"",i,varname);
		ASC_FREE(varname);
	}

	/* CONSOLE_DEBUG("Checking found vars..."); */
	if(gl_length(sys->dynvars)==0){
		ERROR_REPORTER_HERE(ASC_USER_ERROR,"No solver_var vars found to integrate (check 'ode_type'?).");
		return 0;
	}

	CONSOLE_DEBUG("Found %lu vars.", gl_length(sys->dynvars));

	maxderiv = 0;
	numstates = 0;
	for(i=1; i<=gl_length(sys->dynvars); ++i){
		info = (struct Integ_var_t *)gl_fetch(sys->dynvars, i);
		if(info->type==1 || info->type==0)numstates++;
		if(maxderiv < info->type - 1)maxderiv = info->type - 1;
		/* varname = var_make_name(sys->system,info->i);
		CONSOLE_DEBUG("var[%d] = \"%s\": ode_index = %ld",i,varname,info->type);
		ASC_FREE(varname); */
	}
	if(maxderiv == 0){
		ERROR_REPORTER_HERE(ASC_USER_ERROR,"No derivatives found (check 'ode_type' values for your vars).");
		return 0;
	}
	if(maxderiv > 1){
		ERROR_REPORTER_HERE(ASC_USER_ERROR,"Higher-order derivatives found. You must provide a reduced order formulation for your model.");
		return 0;
	}

	if(!integrator_check_indep_var(sys))return 0;

	gl_sort(sys->dynvars,(CmpFunc)Integ_CmpDynVars);

	fprintf(stderr,"\n\n\nSORTED VARS\n");
	for(i=1; i<=gl_length(sys->dynvars); ++i){
		info = (struct Integ_var_t *)gl_fetch(sys->dynvars, i);
		varname = var_make_name(sys->system,info->i);
		CONSOLE_DEBUG("var[%d] = \"%s\": ode_type = %ld",i,varname,info->type); 
		ASC_FREE(varname);
	}

	/* link up variables with their derivatives */
	prev = NULL;
	for(i=1; i<=gl_length(sys->dynvars); ++i){ /* why does gl_list index with base 1??? */
		info = (struct Integ_var_t *)gl_fetch(sys->dynvars, i);
		
		if(info->type == INTEG_STATE_VAR || info->type == INTEG_ALGEBRAIC_VAR){
			varname = var_make_name(sys->system,info->i);
			CONSOLE_DEBUG("Var \"%s\" is an algebraic variable",varname);
			ASC_FREE(varname);
			info->type = INTEG_STATE_VAR;
			info->derivative_of = NULL;
		}else{
			if(prev==NULL || info->index != prev->index){
				/* derivative, but without undifferentiated var present in model */
				varname = var_make_name(sys->system,info->i);
				ERROR_REPORTER_HERE(ASC_USER_ERROR,"Derivative %d of \"%s\" is present without its un-differentiated equivalent"
					, info->type-1
					, varname
				);
				ASC_FREE(varname);
				return 0;
			}else if(info->type != prev->type + 1){
				/* derivative, but missing the next-lower-order derivative */
				derivname = var_make_name(sys->system,info->i);
				varname = var_make_name(sys->system,prev->i);
				ERROR_REPORTER_HERE(ASC_USER_ERROR
					,"Looking at \"%s\", expected a derivative (order %d) of \"%s\"."
					,varname
					,prev->type+1
					,derivname
				);
				ASC_FREE(varname);
				ASC_FREE(derivname);
				return 0;
			}else{
				/* variable with derivative */
				varname = var_make_name(sys->system,prev->i);
				derivname = var_make_name(sys->system,info->i);
				CONSOLE_DEBUG("Var \"%s\" is the derivative of \"%s\"",derivname,varname);
				ASC_FREE(varname);
				ASC_FREE(derivname);
				info->derivative_of = prev;
			}
		}
		prev = info;
	}

	/* record which vars have derivatives and which don't, and count 'states' */
	numy = 0;
	for(i=1; i<=gl_length(sys->dynvars); ++i){
		info = (struct Integ_var_t *)gl_fetch(sys->dynvars, i);
		if(info->derivative_of){
			info->derivative_of->derivative = info;
		}else{
			numy++;
		}
	}

	/* allocate storage for the 'y' and 'ydot' arrays */
	sys->y = ASC_NEW_ARRAY(struct var_variable *,numy);
	sys->ydot = ASC_NEW_ARRAY(struct var_variable *,numy);
	sys->y_id = ASC_NEW_ARRAY(int, slv_get_num_master_vars(sys->system));

	/* now add variables and their derivatives to 'ydot' and 'y' */
	yindex = 0;
	
	for(i=1; i<=gl_length(sys->dynvars); ++i){
		info = (struct Integ_var_t *)gl_fetch(sys->dynvars, i);
		if(info->derivative_of)continue;
		if(info->derivative){
			sys->y[yindex] = info->i;
			sys->ydot[yindex] = info->derivative->i;
			if(info->varindx >= 0){
				sys->y_id[info->varindx] = yindex;
				CONSOLE_DEBUG("y_id[%d] = %d",info->varindx,yindex);
			}
			if(info->derivative->varindx >= 0){
				sys->y_id[info->derivative->varindx] = -1-yindex;
				CONSOLE_DEBUG("y_id[%d] = %d",info->derivative->varindx,-1-yindex);
			}
		}else{
			sys->y[yindex] = info ->i;
			sys->ydot[yindex] = NULL;
			if(info->varindx >= 0){
				sys->y_id[info->varindx] = yindex;
				CONSOLE_DEBUG("y_id[%d] = %d",info->varindx,yindex);
			}
		}
		yindex++;
	}

	nrels = slv_get_num_solvers_rels(sys->system);
	if(numy != nrels){
		ERROR_REPORTER_HERE(ASC_USER_ERROR
			,"System is not square: solver has %d rels, found %d system states"
			,nrels, numy
		);
		return 0;
	}

	CONSOLE_DEBUG("THERE ARE %d VARIABLES IN THE INTEGRATION SYSTEM",numy);
	
	sys->n_y = numy;

	if(!integrator_sort_obs_vars(sys))return 0;

	return 1;
}

void integrator_visit_system_vars(IntegratorSystem *sys,IntegratorVarVisitorFn *visitfn){
  struct var_variable **vlist;
  int i, vlen;

  /* visit all the slv_system_t master var lists to collect vars */
  /* find the vars mostly in this one */
  vlist = slv_get_master_var_list(sys->system);
  vlen = slv_get_num_master_vars(sys->system);
  for (i=0;i<vlen;i++) {
    (*visitfn)(sys, vlist[i], &i);
  }

  /*
  CONSOLE_DEBUG("Checked %d vars",vlen);
  integrator_print_var_stats(sys);
  */

  /* probably nothing here, but gotta check. */
  vlist = slv_get_master_par_list(sys->system);
  vlen = slv_get_num_master_pars(sys->system);
  for (i=0;i<vlen;i++) {
    (*visitfn)(sys, vlist[i], NULL);
  }

  /*
  CONSOLE_DEBUG("Checked %d pars",vlen);
  integrator_print_var_stats(sys);
  */

  /* might find t here */
  vlist = slv_get_master_unattached_list(sys->system);
  vlen = slv_get_num_master_unattached(sys->system);
  for (i=0;i<vlen;i++) {
    (*visitfn)(sys, vlist[i], NULL);
  }

  /* CONSOLE_DEBUG("Checked %d unattached",vlen); */
}
/**
	@return 1 on success
*/
int integrator_analyse_ode(IntegratorSystem *sys){
  struct Integ_var_t *v1,*v2;
  long half,i,len;
  int happy=1;
  char *varname1, *varname2;

  CONSOLE_DEBUG("Starting ODE analysis");
  IntegInitSymbols();

  /* collect potential states and derivatives */
  sys->indepvars = gl_create(10L);  /* t var info */
  sys->dynvars = gl_create(200L);  /* y ydot var info */
  sys->obslist = gl_create(100L);  /* obs info */
  if (sys->dynvars == NULL
    || sys->obslist == NULL
    || sys->indepvars == NULL
  ){
    ERROR_REPORTER_HERE(ASC_PROG_ERR,"Insufficient memory.");
    return 0;
  }

  sys->nstates = sys->nderivs = 0;

  integrator_visit_system_vars(sys,&integrator_ode_classify_var);

  integrator_print_var_stats(sys);

  if(!integrator_check_indep_var(sys))return 0;

  /* check sanity of state and var lists */

  len = gl_length(sys->dynvars);
  half = len/2;
  CONSOLE_DEBUG("NUMBER OF DYNAMIC VARIABLES = %ld",half);

  if (len % 2 || len == 0L || sys->nstates != sys->nderivs ) {
    /* list length must be even for vars to pair off */
    ERROR_REPORTER_NOLINE(ASC_USER_ERROR,"n_y != n_ydot, or no dynamic vars found. Fix your indexing.");
    return 0;
  }
  gl_sort(sys->dynvars,(CmpFunc)Integ_CmpDynVars);
  if (gl_fetch(sys->dynvars,len)==NULL) {
    ERROR_REPORTER_NOLINE(ASC_PROG_ERR,"Mysterious NULL found!");
    return 0;
  }
  sys->states = gl_create(half);   /* state vars Integ_var_t references */
  sys->derivs = gl_create(half);   /* derivative var atoms */
  for (i=1;i < len; i+=2) {
    v1 = (struct Integ_var_t *)gl_fetch(sys->dynvars,i);
    v2 = (struct Integ_var_t *)gl_fetch(sys->dynvars,i+1);
    if (v1->type!=1  || v2 ->type !=2 || v1->index != v2->index) {
      varname1 = var_make_name(sys->system,v1->i);
	  varname2 = var_make_name(sys->system,v2->i);

      ERROR_REPORTER_HERE(ASC_USER_ERROR,"Mistyped or misindexed dynamic variables: %s (%s = %ld,%s = %ld) and %s (%s = %ld,%s = %ld).",
             varname1, SCP(STATEFLAG),v1->type,SCP(STATEINDEX),v1->index,
             varname2, SCP(STATEFLAG),v2->type,SCP(STATEINDEX),v2->index
		);
      ASC_FREE(varname1);
      ASC_FREE(varname2);
      happy=0;
      break;
    } else {
      gl_append_ptr(sys->states,(POINTER)v1);
      gl_append_ptr(sys->derivs,(POINTER)v2->i);
    }
  }
  if (!happy) {
    return 0;
  }
  sys->n_y = half;
  sys->y = ASC_NEW_ARRAY(struct var_variable *, half);
  sys->y_id = ASC_NEW_ARRAY(long, half);
  sys->ydot = ASC_NEW_ARRAY(struct var_variable *, half);
  if (sys->y==NULL || sys->ydot==NULL || sys->y_id==NULL) {
    ERROR_REPORTER_HERE(ASC_PROG_ERR,"Insufficient memory.");
    return 0;
  }
  for (i = 1; i <= half; i++) {
    v1 = (struct Integ_var_t *)gl_fetch(sys->states,i);
    sys->y[i-1] = v1->i;
    sys->y_id[i-1] = v1->index;
    sys->ydot[i-1] = (struct var_variable *)gl_fetch(sys->derivs,i);
  }

  if(!integrator_sort_obs_vars(sys))return 0;

  /* don't need the gl_lists now that we have arrays for everyone */
  gl_destroy(sys->states);
  gl_destroy(sys->derivs);
  gl_free_and_destroy(sys->indepvars);  /* we own the objects in indepvars */
  gl_free_and_destroy(sys->dynvars);    /* we own the objects in dynvars */
  gl_free_and_destroy(sys->obslist);    /* and obslist */
  sys->states = NULL;
  sys->derivs = NULL;
  sys->indepvars = NULL;
  sys->dynvars = NULL;
  sys->obslist = NULL;

  /* analysis completed OK */
  return 1;
}

/**
	Reindex observations. Sort if the user mostly numbered. Take natural order
	if user just booleaned.

	@return 1 on success
*/
static int integrator_sort_obs_vars(IntegratorSystem *sys){
  int half, i, len = 0;
  struct Integ_var_t *v2;

  half = sys->n_y;
  len = gl_length(sys->obslist);
  /* we shouldn't be seeing NULL here ever except if malloc fail. */
  if (len > 1L) {
    half = ((struct Integ_var_t *)gl_fetch(sys->obslist,1))->index;
    /* half != 0 now because we didn't collect 0 indexed vars */
    for (i=2; i <= len; i++) {
      if (half != ((struct Integ_var_t *)gl_fetch(sys->obslist,i))->index) {
        /* change seen. sort and go on */
        gl_sort(sys->obslist,(CmpFunc)Integ_CmpObs);
        break;
      }
    }
  }
  for (i = half = 1; i <= len; i++) {
    v2 = (struct Integ_var_t *)gl_fetch(sys->obslist,i);
    if (v2==NULL) {
      /* we shouldn't be seeing NULL here ever except if malloc fail. */
      gl_delete(sys->obslist,i,0); /* should not be gl_delete(so,i,1) */
    } else {
      Integ_SetObsId(v2->i,half);
      v2->index = half++;
    }
  }

  /* obslist now uniquely indexed, no nulls */
  /* make into arrays */
  half = gl_length(sys->obslist);
  sys->obs = ASC_NEW_ARRAY(struct var_variable *,half);
  sys->obs_id = ASC_NEW_ARRAY(long, half);
  if ( sys->obs==NULL || sys->obs_id==NULL) {
    ERROR_REPORTER_HERE(ASC_PROG_ERR,"Insufficient memory.");
    return 0;
  }
  sys->n_obs = half;
  for (i = 1; i <= half; i++) {
    v2 = (struct Integ_var_t *)gl_fetch(sys->obslist,i);
    sys->obs[i-1] = v2->i;
    sys->obs_id[i-1] = v2->index;
  }

  return 1;
}

static void integrator_print_var_stats(IntegratorSystem *sys){
	int v = gl_length(sys->dynvars);
	int i = gl_length(sys->indepvars);
	CONSOLE_DEBUG("Currently %d vars, %d indep",v,i);
}

/**
	Check sanity of the independent variable.

	@return 1 on success
*/
static int integrator_check_indep_var(IntegratorSystem *sys){
  int len, i;
  struct Integ_var_t *info;
  char *varname;

  /* check the sanity of the independent variable */
  len = gl_length(sys->indepvars);
  if (!len) {
    ERROR_REPORTER_HERE(ASC_PROG_ERR,"No independent variable found.");
    return 0;
  }
  if (len > 1) {
	ERROR_REPORTER_START_HERE(ASC_USER_ERROR);
    FPRINTF(ASCERR,"Excess %ld independent variables found:",
      len);
    for(i=1; i <=len;i++) {
      info = (struct Integ_var_t *)gl_fetch(sys->indepvars,i);
      if(info==NULL)continue;

      varname = var_make_name(sys->system,info->i);
      FPRINTF(ASCERR," %s",varname);
      ASC_FREE(varname);
    }
    FPRINTF(ASCERR , "\nSet the \"%s\" flag on all but one of these to %s >= 0.\n"
        , SCP(STATEFLAG),SCP(STATEFLAG)
	);
	error_reporter_end_flush();
    return 0;
  }else{
    info = (struct Integ_var_t *)gl_fetch(sys->indepvars,1);
    sys->x = info->i;
  }
  return 1;
}

/*------------------------------------------------------------------------------
  CLASSIFICATION OF VARIABLES (for ANALYSIS step)
*/

#define INTEG_ADD_TO_LIST(info,TYPE,INDEX,VAR,VARINDX,LIST) \
	info = ASC_NEW(struct Integ_var_t); \
	if(info==NULL){ \
		ERROR_REPORTER_HERE(ASC_PROG_ERR,"Insufficient memory (INTEG_VAR_NEW)"); \
		return; \
	} \
	info->type=TYPE; \
	info->index=INDEX; \
	info->i=VAR; \
    if(VARINDX==NULL){ \
		info->varindx = -1; \
	}else{ \
		info->varindx = *VARINDX; \
	} \
	gl_append_ptr(LIST,(void *)info); \
	info = NULL

/**
	In a DAE, it's either the (single) independent variable, or it's a
	variable in the model.

	I'm not sure what we should be doing with variables that are already
	present as derivatives of other variables, I guess those ones need to be
	removed from the list in a second pass?
*/
void integrator_dae_classify_var(IntegratorSystem *sys
		, struct var_variable *var, const int *varindx
){
	struct Integ_var_t *info;
	long type,index;

	/* filter for recognition of solver_vars */
	var_filter_t vfilt;
	vfilt.matchbits = VAR_SVAR;
	vfilt.matchvalue = VAR_SVAR;

	assert(var != NULL && var_instance(var)!=NULL );

	if( var_apply_filter(var,&vfilt) ) {
		if(!var_active(var)){
			CONSOLE_DEBUG("VARIABLE IS NOT ACTIVE");
			return;
		}
		
		/* only non-fixed variables are accepted */
		if(!var_fixed(var)){
			/* get the ode_type and ode_id of this solver_var */
			type = DynamicVarInfo(var,&index);

			if(type==INTEG_OTHER_VAR){
				/* if the var's type is -1, it's independent */
				INTEG_ADD_TO_LIST(info,INTEG_OTHER_VAR,0,var,varindx,sys->indepvars);
			}else{
				if(type < 0)type=0;
				/* any other type of var is in the DAE system, at least for now */
				INTEG_ADD_TO_LIST(info,type,index,var,varindx,sys->dynvars);
			}
		}
#if 0
		else{
			/* fixed variable, only include it if ode_type == 1 */
			type = DynamicVarInfo(var,&index);
			if(type==INTEG_STATE_VAR){
				INTEG_ADD_TO_LIST(info,type,index,var,varindx,sys->dynvars);
			}
		}
#endif

		/* if the var's obs_id > 0, add it to the observation list */
		if(ObservationVar(var,&index) != NULL && index > 0L) {
			INTEG_ADD_TO_LIST(info,type,index,var,varindx,sys->obslist);
		}
	}
}

/**
	Inspect a specific variable and work out what type it is (what 'ode_type' it
	has) and what other variable(s) it corresponds to (ie dydt corresponds to
	y as a derivative).

	@TODO add ability to create new variables for 'missing' derivative vars?
*/
void integrator_ode_classify_var(IntegratorSystem *sys, struct var_variable *var
		, const int *varindx
){
  struct Integ_var_t *info;
  long type,index;

  var_filter_t vfilt;
  vfilt.matchbits = VAR_SVAR;
  vfilt.matchvalue = VAR_SVAR;

  assert(var != NULL && var_instance(var)!=NULL );

  if( var_apply_filter(var,&vfilt) ) {
	/* it's a solver var: what type of variable? */
    type = DynamicVarInfo(var,&index);

    if(type==INTEG_ALGEBRAIC_VAR){
		/* no action required */
	}else if(type==INTEG_OTHER_VAR){
		/* i.e. independent var */
        INTEG_ADD_TO_LIST(info,type,index,var,varindx,sys->indepvars);
	}else if(type>=INTEG_STATE_VAR){
        INTEG_ADD_TO_LIST(info,type,index,var,varindx,sys->dynvars);
        if(type == 1){
          sys->nstates++;
        }else if(type == 2){ /* what about higher-order derivatives? -- JP */
          sys->nderivs++;
        }else{
		  ERROR_REPORTER_HERE(ASC_USER_WARNING,"Higher-order (>=2) derivatives are not supported in ODEs.");
		}	}

    if(ObservationVar(var,&index) != NULL && index > 0L) {
		INTEG_ADD_TO_LIST(info,0L,index,var,varindx,sys->obslist);
    }
  }
}

/**
	Look at a variable and determine if it's the independent variable or not.
	This is just for the purpose of the integrator_find_indep_var function,
	which is a utility function provided for use by the GUI.
*/
void integrator_classify_indep_var(IntegratorSystem *sys
		, struct var_variable *var, const int *varindx
){
	struct Integ_var_t *info;
	long type,index;

	var_filter_t vfilt;
	vfilt.matchbits = VAR_SVAR;
	vfilt.matchvalue = VAR_SVAR;

	/* CONSOLE_DEBUG("..."); */

	assert(var != NULL && var_instance(var)!=NULL );

	if( var_apply_filter(var,&vfilt) ) {
		type = DynamicVarInfo(var,&index);

		if(type==INTEG_OTHER_VAR){
			/* i.e. independent var */
			INTEG_ADD_TO_LIST(info,type,index,var,varindx,sys->indepvars);
		}
	}
}

/**
	Look at a variable, and if it is an 'ODE variable' (it has a child instance
	named 'ode_type') return its type, which will be either:
		- INTEG_OTHER_VAR (if 'ode_type' is -1)
		- INTEG_ALGEBRAIC_VAR (if 'ode_type' is zero or any negative value < -1)
		- INTEG_STATE_VAR (if 'ode_type' is 1)
		- values 2, 3 or up, indicating derivatives (1st deriv=2, 2nd deriv=3, etc)

	If the parameter 'index' is not null, the value of 'ode_id' will be stuffed
	there.
*/
static long DynamicVarInfo(struct var_variable *v,long *index){
  struct Instance *c, *d, *i;
  i = var_instance(v);
  assert(i!=NULL);
  assert(STATEFLAG!=NULL);
  assert(STATEINDEX!=NULL);
  c = ChildByChar(i,STATEFLAG);
  d = ChildByChar(i,STATEINDEX);
  /* lazy evaluation is important in the following if */
  if(c == NULL
      || d == NULL
      || InstanceKind(c) != INTEGER_INST
      || InstanceKind(d) != INTEGER_INST
      || !AtomAssigned(c)
      || (!AtomAssigned(d) && GetIntegerAtomValue(c) != INTEG_OTHER_VAR)
  ){
    return INTEG_ALGEBRAIC_VAR;
  }
  if (index != NULL) {
    *index = GetIntegerAtomValue(d);
  }
  return GetIntegerAtomValue(c);
}

/**
	Looks at the given variable checks if it is an 'observation variable'. This
	means that it has its 'obs_id' child instance set to a non-zero value.

	If the variable is an observation variable, its index value ('obs_id') is
	stuff into *index (provided index!=NULL), and the pointer to the original
	instance is rtruend.

	If it's not an observation variable, we return NULL and *index is untouched.
 */
static struct var_variable *ObservationVar(struct var_variable *v, long *index){
  struct Instance *c,*i;
  i = var_instance(v);
  assert(i!=NULL);
  c = ChildByChar(i,OBSINDEX);
  if( c == NULL || InstanceKind(c) != INTEGER_INST || !AtomAssigned(c)) {
    return NULL;
  }
  if (index != NULL) {
    *index = GetIntegerAtomValue(c);
  }
  return v;
}

/*------------------------------------------------------------------------------
  RUNNING THE SOLVER
*/

/*
	Make the call to the actual integrator we've selected, for the range of
	time values specified. The sys contains all the specifics.

	Return 1 on success
*/
int integrator_solve(IntegratorSystem *sys, long i0, long i1){

	long nstep;
	unsigned long start_index=0, finish_index=0;
	assert(sys!=NULL);

	nstep = integrator_getnsamples(sys)-1;
	/* check for at least 2 steps and dimensionality of x vs steps here */

	if (i0<0 || i1 <0) {
		/* dude, there's no way we're writing interactive stuff here... */
		ERROR_REPORTER_HERE(ASC_PROG_ERROR,"Console input of integration limits has been disabled!");
		return 0;
	} else {
		start_index=i0;
		finish_index =i1;
		if (start_index >= (unsigned long)nstep) {
			ERROR_REPORTER_NOLINE(ASC_USER_ERROR,"Start point (=%lu) must be an index in the range [0,%li]."
				,start_index,nstep
			);
			return 0;
		}
		if (finish_index > (unsigned long)nstep) {
			ERROR_REPORTER_NOLINE(ASC_USER_ERROR,"End point (=%lu) must be an index in the range [0,%li]."
				,finish_index,nstep
			);
			return 0;
		}
    }

	if(finish_index <= start_index) {
		ERROR_REPORTER_NOLINE(ASC_USER_ERROR,"End point comes before start point! (start=%lu, end=%lu)"
			,start_index,finish_index
		);
		return 0;
	}

	CONSOLE_DEBUG("RUNNING INTEGRATION...");

	/* now go and run the integrator */
	switch (sys->engine) {
		case INTEG_LSODE:
		return integrator_lsode_solve(sys, start_index, finish_index);
		break;
#ifdef ASC_WITH_IDA
		case INTEG_IDA:
		return integrator_ida_solve(sys,start_index, finish_index);
		break;
#endif
		default:
			ERROR_REPORTER_HERE(ASC_PROG_ERR,"Unknown integrator (invalid, or not implemented yet)");
			return 0;
	}
}

/*---------------------------------------------------------------
  HANDLING THE LIST OF TIMESTEMPS
*/

#define GETTER_AND_SETTER(TYPE,NAME) \
	void integrator_set_##NAME(IntegratorSystem *sys, TYPE val){ \
		sys->NAME=val; \
	} \
	TYPE integrator_get_##NAME(IntegratorSystem *sys){ \
		return sys->NAME; \
	}

GETTER_AND_SETTER(SampleList *,samples) /*;*/
GETTER_AND_SETTER(double,maxstep) /*;*/
GETTER_AND_SETTER(double,minstep) /*;*/
GETTER_AND_SETTER(double,stepzero) /*;*/
GETTER_AND_SETTER(int,maxsubsteps) /*;*/
#undef GETTER_AND_SETTER

long integrator_getnsamples(IntegratorSystem *sys){
	assert(sys!=NULL);
	assert(sys->samples!=NULL);
	return samplelist_length(sys->samples);
}

double integrator_getsample(IntegratorSystem *sys, long i){
	assert(sys!=NULL);
	assert(sys->samples!=NULL);
	return samplelist_get(sys->samples,i);
}

void integrator_setsample(IntegratorSystem *sys, long i,double xi){
	assert(sys!=NULL);
	assert(sys->samples!=NULL);
	samplelist_set(sys->samples,i,xi);
}

const dim_type *integrator_getsampledim(IntegratorSystem *sys){
	assert(sys!=NULL);
	assert(sys->samples!=NULL);
	return samplelist_dim(sys->samples);
}

ASC_DLLSPEC(long) integrator_getcurrentstep(IntegratorSystem *sys){
	return sys->currentstep;
}

/*------------------------------------------------------------------------------
  GET/SET VALUE OF THE INDEP VARIABLE
*/

/**
	Retrieve the value of the independent variable (time) from ASCEND
	and return it as a double.
*/
double integrator_get_t(IntegratorSystem *sys){
	assert(sys->x!=NULL);
	return var_value(sys->x);
}

/**
	Set the value of the independent variable (time) in ASCEND.
*/
void integrator_set_t(IntegratorSystem *sys, double value){
  var_set_value(sys->x, value);
  /* CONSOLE_DEBUG("set_t = %g", value); */
}

/*------------------------------------------------------------------------------
  PASSING DIFFERENTIAL VARIABLES AND THEIR DERIVATIVES TO/FROM THE SOLVER
*/
/**
	Retrieve the current values of the derivatives of the y-variables
	and stick them in the/an array that the integrator will use.

	If the pointer 'y' is NULL, the necessary space is allocated (and
	must be freed somewhere else).
*/
double *integrator_get_y(IntegratorSystem *sys, double *y) {
  long i;

  if (y==NULL) {
    y = ASC_NEW_ARRAY_CLEAR(double, sys->n_y+1);
    /* C y[0]  <==> ascend d.y[1]  <==>  f77 y(1) */
  }

  for (i=0; i< sys->n_y; i++) {
	assert(sys->y[i]!=NULL);
    y[i] = var_value(sys->y[i]);
    /* CONSOLE_DEBUG("ASCEND --> y[%ld] = %g", i+1, y[i]); */
  }
  return y;
}

/**
	Take the values of the derivatives from the array that the integrator
	uses, and use them to update the values of the corresponding variables
	in ASCEND.
*/
void integrator_set_y(IntegratorSystem *sys, double *y) {
  long i;
#ifndef NDEBUG
  char *varname;
#endif

  for (i=0; i < sys->n_y; i++) {
	assert(sys->y[i]!=NULL);
    var_set_value(sys->y[i],y[i]);
#ifndef NDEBUG
	varname = var_make_name(sys->system, sys->y[i]);
    /* CONSOLE_DEBUG("y[%ld] = \"%s\" = %g --> ASCEND", i+1, varname, y[i]); */
	ASC_FREE(varname);
#endif
  }
}

/**
	Send the values of the derivatives of the 'y' variables to the solver.
	Allocate space for an array if necessary.

	Any element in sys->ydot that is NULL will be passed over (the value
	won't be modified in dydx).
*/
double *integrator_get_ydot(IntegratorSystem *sys, double *dydx) {
  long i;

  if (dydx==NULL) {
    dydx = ASC_NEW_ARRAY_CLEAR(double, sys->n_y+1);
    /* C dydx[0]  <==> ascend d.dydx[1]  <==>  f77 ydot(1) */
  }

  for (i=0; i < sys->n_y; i++) {
    if(sys->ydot[i]!=NULL){
		dydx[i] = var_value(sys->ydot[i]);
	}
    /* CONSOLE_DEBUG("ASCEND --> ydot[%ld] = %g", i+1, dydx[i]); */
  }
  return dydx;
}

void integrator_set_ydot(IntegratorSystem *sys, double *dydx) {
	long i;
#ifndef NDEBUG
	/* char *varname; */
#endif
	for (i=0; i < sys->n_y; i++) {
		if(sys->ydot[i]!=NULL){
    		var_set_value(sys->ydot[i],dydx[i]);
#ifndef NDEBUG
			/* varname = var_make_name(sys->system, sys->ydot[i]);
			CONSOLE_DEBUG("ydot[%ld] = \"%s\" = %g --> ASCEND", i+1, varname, dydx[i]);
			ASC_FREE(varname); */
#endif
		}
#ifndef NDEBUG
		/*else{
			CONSOLE_DEBUG("ydot[%ld] = %g (internal)", i+1, dydx[i]);
		}*/
#endif
	}
}

/*-------------------------------------------------------------
  RETRIEVING OBSERVATION DATA
*/

/**
   This function takes the inst in the solver and returns the vector of
   observation variables that are located in the submodel d.obs array.
*/
double *integrator_get_observations(IntegratorSystem *sys, double *obsi) {
  long i;

  if (obsi==NULL) {
    obsi = ASC_NEW_ARRAY_CLEAR(double, sys->n_obs+1);
  }

  /* C obsi[0]  <==> ascend d.obs[1] */

  for (i=0; i < sys->n_obs; i++) {
    obsi[i] = var_value(sys->obs[i]);
    /* CONSOLE_DEBUG("*get_d_obs[%ld] = %g\n", i+1, obsi[i]); */
  }
  return obsi;
}

struct var_variable *integrator_get_observed_var(IntegratorSystem *sys, const long i){
	assert(i>=0);
	assert(i<sys->n_obs);
	return sys->obs[i];
}

/**
	@NOTE Although this shouldn't be required for implementation of solver
	engines, this is useful for GUI reporting of integration results.
*/
struct var_variable *integrator_get_independent_var(IntegratorSystem *sys){
	return sys->x;
}


/*----------------------------------------------------
	Build an analytic jacobian for solving the state system

	This necessarily ugly piece of code attempts to create a unique
	list of relations that explicitly contain the variables in the
	given input list. The utility of this information is that we know
	exactly which relations must be differentiated, to fill in the
	df/dy matrix. If the problem has very few derivative terms, this will
	be of great savings. If the problem arose from the discretization of
	a pde, then this will be not so useful. The decision wether to use
	this function or to simply differentiate the entire relations list
	must be done before calling this function.

	Final Note: the callee owns the array, but not the array elements.
 */
#define AVG_NUM_INCIDENT 4


/**
	This function helps to arrange the observation variables in a sensible order.
	The 'obs_id' child instance of v, if present, is assigned the value of the
	given parameter 'index'.
*/
static void Integ_SetObsId(struct var_variable *v, long index){
  struct Instance *c, *i;
  i = var_instance(v);
  assert(i!=NULL);
  c = ChildByChar(i,OBSINDEX);
  if( c == NULL || InstanceKind(c) != INTEGER_INST || !AtomAssigned(c)) {
    return;
  }
  SetIntegerAtomValue(c,index,0);
}

/**
	Compares observation structs. NULLs should end up at far end.
*/
static int Integ_CmpObs(struct Integ_var_t *v1, struct Integ_var_t *v2){
  if(v1 == NULL)return 1;
  if(v2 == NULL)return -1;
  if(v1->index > v2->index)return 1;
  if(v1->index == v2->index)return 0;
  return -1;
}

/**
	Compares dynamic vars structs. NULLs should end up at far end.
	List should be sorted primarily by index and then by type, in order
	of increasing value of both.
*/
static int Integ_CmpDynVars(struct Integ_var_t *v1, struct Integ_var_t *v2){
  if(v1 == NULL)return 1;
  if(v2 == NULL)return -1;
  if(v1->index > v2->index)return 1;
  if(v1->index != v2->index)return -1;
  if(v1->type > v2->type)return 1;
  return -1;
}
/*----------------------------
  Output handling to the GUI/interface.
*/

int integrator_set_reporter(IntegratorSystem *sys
	, IntegratorReporter *reporter
){
	assert(sys!=NULL);
	sys->reporter = reporter;
	/* ERROR_REPORTER_HERE(ASC_PROG_NOTE,"INTEGRATOR REPORTER HOOKS HAVE BEEN SET\n"); */
	return 1;
}

int integrator_output_init(IntegratorSystem *sys){
	assert(sys!=NULL);
	assert(sys->reporter!=NULL);
	if(sys->reporter->init!=NULL){
		/* call the specified output function */
		return (*(sys->reporter->init))(sys);
	}
	ERROR_REPORTER_HERE(ASC_PROG_ERR,"No integrator reporter init method");
	return 1;
}

int integrator_output_write(IntegratorSystem *sys){
	static int reported_already=0;
	assert(sys!=NULL);
	if(sys->reporter->write!=NULL){
		return (*(sys->reporter->write))(sys);
	}
	if(!reported_already){
		ERROR_REPORTER_HERE(ASC_PROG_ERR,"No integrator reporter write method (this message only shown once)");
		reported_already=1;
	}
	return 1;
}

int integrator_output_write_obs(IntegratorSystem *sys){
	static int reported_already=0;
	assert(sys!=NULL);
	if(sys->reporter->write_obs!=NULL){
		return (*(sys->reporter->write_obs))(sys);
	}
	if(!reported_already){
		ERROR_REPORTER_HERE(ASC_PROG_ERR,"No integrator reporter write_obs method (this message only shown once)");
		reported_already=1;
	}
	return 1;
}

int integrator_output_close(IntegratorSystem *sys){
	assert(sys!=NULL);
	if(sys->reporter->close!=NULL){
		return (*(sys->reporter->close))(sys);
	}
	ERROR_REPORTER_HERE(ASC_PROG_ERR,"No integrator reporter close method");
	return 1;
}

/**
	Decode status codes from the integrator, and output them via FPRINTF.
*/
int integrator_checkstatus(slv_status_t status) {
  if (status.converged) {
    return 1;
  }
  if (status.diverged) {
    FPRINTF(stderr, "The derivative system did not converge. Integration will terminate.");
    return 0;
  }
  if (status.inconsistent) {
    FPRINTF(stderr, "A numerically inconsistent state was discovered while "
		"calculating derivatives. Integration will terminate.");
    return 0;
  }
  if (status.time_limit_exceeded) {
    FPRINTF(stderr, "The time limit was exceeded while calculating "
		"derivatives. Integration will terminate.");
    return 0;
  }
  if (status.iteration_limit_exceeded) {
    FPRINTF(stderr, "The iteration limit was exceeded while calculating "
		"derivatives. Integration will terminate.");
    return 0;
  }
  if (status.panic) {
    FPRINTF(stderr, "The user patience limit was exceeded while "
		"calculating derivatives. Integration will terminate.");
    return 0;
  }
  return 0;
}
