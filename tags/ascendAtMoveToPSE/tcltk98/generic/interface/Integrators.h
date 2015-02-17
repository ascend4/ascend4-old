/*                              
 *  Integrators.h
 *  by Kirk Abbott and Ben Allan
 *  Created: 1/94
 *  Version: $Revision: 1.16 $
 *  Version control file: $RCSfile: Integrators.h,v $
 *  Date last modified: $Date: 2003/08/23 18:43:06 $
 *  Last modified by: $Author: ballan $
 *
 *  This file is part of the ASCEND Tcl/Tk interface
 *
 *  Copyright 1997, Carnegie Mellon University
 *
 *  The ASCEND Tcl/Tk interface is free software; you can redistribute
 *  it and/or modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  The ASCEND Tcl/Tk interface is distributed in hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with the program; if not, write to the Free Software Foundation,
 *  Inc., 675 Mass Ave, Cambridge, MA 02139 USA.  Check the file named
 *  COPYING.  COPYING is found in ../compiler.
 */

/** @file
 *  Implementation of the Integration Interface.
 *  <pre>
 *  Author:       Boyd T. Safrit
 *
 *  Changes:  8/95  Added support code for blsode, an
 *                  experimental user interface
 *                  which identifies states, derivatives,
 *                  observations, and tolerances by flags.
 *                  Interface redesign (ASCEND side) by
 *                  Jennifer Stokes and (C side) Ben Allan.
 *                  The only required vector left is that
 *                  defining the time steps to calculate
 *                  results at. All state and observation
 *                  data are (optionally) logged to files.
 *            5/96  Removed old lsode interface routines.
 *                  The Piela version of lsode is dead.
 *            8/96  Removed the var == instance assumption from
 *                  the implementation.
 *
 *  Description:  This module defines the general integration
 *                auxillaries for Ascend.
 *
 *  Requires      #include "utilities/ascConfig.h
 *                #include "general/list.h"
 *                #include "compiler/instance_enum.h"
 *                #include "compiler/dimen.h"
 *                #include "solver/slv_client.h"
 *                #include "compiler/interface.h"
 *                #include <stdio.h>   (for current INTEG_DEBUG functions)
 *  </pre>
 */

#ifndef integration_already_included
#define integration_already_included

/** 
 *  Define as TRUE to enable debug message printing.
 *  @todo this needs to go away. 
 */
#define INTEG_DEBUG FALSE
#define print_debug(msg, value) if (INTEG_DEBUG) FPRINTF(stdout, msg, value)
/**<  Print a debug message with value if INTEG_DEBUG is TRUE. */
#define print_debugstring(msg) if (INTEG_DEBUG) FPRINTF(stdout, msg)
/**<  Print a debug message string if INTEG_DEBUG is TRUE. */

/** The integrators that are supported */
enum Integrator_type {
  BLSODE,
  /* SDASSL, */
  UNKNOWN
};

struct Integ_DiffData {
  long n_eqns;                     /**< dimension of state vector */
  int *input_indices;              /**< vector of state vars indexes */
  int *output_indices;             /**< vector of derivative var indexes */
  struct var_variable **y_vars;    /**< NULL terminated list of states vars */
  struct var_variable **ydot_vars; /**< NULL terminated list of derivative vars*/
  struct rel_relation **rlist;     /**< NULL terminated list of relevant rels
                                        to be differentiated */
  double **dydx_dx;                /**< change in derivatives wrt states
                                        I prefer to call this: d(ydot)/dy */
};

extern struct Integ_DiffData g_intg_diff;
/**<
 * global creature for C's FEX and JEX like functions to access data
 * when called from Fortran.
 */

/**
 * Experimental ivp problem description token. anyone making a copy of
 * the y, ydot, or obs pointers who plans to free that pointer later
 * should increment the reference count for that pointer so if blsys
 * is destroyed later we don't end up double freeing it. Note this
 * scheme will only work for the first copy and could be eliminated
 * if we decoupled blsode from lsode as we will once things restabilize.
 */
struct Integ_system_t {
  /* temporaries for build. these elements will be NULL to clients */
  struct gl_list_t *indepvars;  /**< all apparent independent vars */
  struct gl_list_t *dynvars;  /**< all state and deriv instances plus indices */
  struct gl_list_t *obslist;  /**< observation instance plus indices */
  struct gl_list_t *states;   /**< ordered list of state variables and indices*/
  struct gl_list_t *derivs;   /**< ordered list of derivative (ydot) insts */
  /* persistent, these elements are valid to C clients. */
  struct var_variable *x;     /**< independent variable */
  struct var_variable **y;    /**< array form of states */
  struct var_variable **ydot; /**< array form of derivatives */
  struct var_variable **obs;  /**< array form of observed variables */
  long *y_id;                 /**< array form of y/ydot user indices */
  long *obs_id;               /**< array form of obs user indices */
  long n_y;
  long n_obs;
  int ycount;                 /**< number of external references to  y */
  int ydotcount;              /**< number of external references to  ydot */
  int obscount;               /**< number of external references to  obs */
  int maxsteps;               /**< most steps between mesh poins */
  double stepzero;            /**< initial step length, SI units. */
  double stepmin;             /**< shortest step length, SI units. */
  double stepmax;             /**< longest step length, SI units. */
};

extern int Asc_IntegInstIntegrable(struct Instance *inst,
                                   enum Integrator_type integrator);
/**<
 *  Takes an instance and a type of integrator and determines
 *  if the instance can be integrated using this integrator.
 */

extern int Asc_IntegCheckStatus(slv_status_t status);
/**<
 *  Takes a status and checks for convergence.  If converged then return 1
 *  else return 0 and print error message if appropriate.
 */

/*  functions affecting the logging of data during integration */
extern FILE *Asc_IntegOpenYFile(void);
/**<
 *  Returns the pointer to a file to which state
 *  variables should be written at each recorded time interval. Other
 *  info may be written here as well, but that is up to the designer
 *  of the specific integrator interface. The filename used will be
 *  the one most recently set via Asc_IntegSetYFileCmd().  If no 
 *  name (or an empty name) has been set via Asc_IntegSetYFileCmd(),
 *  the file returned will be NULL.<br><br>
 *
 *  This function may return NULL, in which case do not write to
 *  them as there was some problem opening the file.
 *  If opened successfully, the file returned will have DATASET and
 *  the open time stamped at the bottom. This will separate multiple
 *  runs or changes in the same run.<br><br>
 *
 *  Closing the files is your job.  When finished with the file, you 
 *  should call Asc_IntegReleaseYFile() before closing it.
 */
extern FILE *Asc_IntegOpenObsFile(void);
/**<
 *  Returns the pointer to a file to which observation
 *  variables should be written at each recorded time interval. Other
 *  info may be written here as well, but that is up to the designer
 *  of the specific integrator interface. The filename used will be
 *  the one most recently set via Asc_IntegSetObsFileCmd().  If no 
 *  name (or an empty name) has been set via Asc_IntegSetObsFileCmd(),
 *  the file returned will be NULL.<br><br>
 *
 *  This function may return NULL, in which case do not write to
 *  them as there was some problem opening the file.
 *  If opened successfully, the file returned will have DATASET and
 *  the open time stamped at the bottom. This will separate multiple
 *  runs or changes in the same run.<br><br>
 *
 *  Closing the files is your job.  When finished with the file, you 
 *  should call Asc_IntegReleaseObsFile() before closing it.
 */
extern FILE *Asc_IntegGetYFile(void);
/**<
 *  Returns the current FILE * for writing state variables,
 *  or NULL if none.
 */
extern FILE *Asc_IntegGetObsFile(void);
/**<
 *  Returns the current FILE * for writing observation variables,
 *  or NULL if none.
 */
extern void Asc_IntegReleaseYFile(void);
extern void Asc_IntegReleaseObsFile(void);
/**<
 *  Releases the internally-stored FILE * for observation variables.
 *  This does not close the file (which you still need to do).
 *  This function should be called just before closing the files,
 *  though it may be called sooner if you want to keep the file pointer
 *  returned all to yourself.
 */

extern void Asc_IntegPrintYHeader(FILE *fp, struct Integ_system_t *blsys);
/**<
 *  Prints Y header info to the file given.
 *  If FILE is NULL, returns immediately.
 *  If FILE ok, but blsys NULL prints warning to stderr and returns.
 *  At the moment, the written header info is:
 *      A vertical listing of
 *      ode_id/obs_id instance_name_as_seen_in_solver.d UNITS
 *      then a row (which will line up with printed values)
 *      of the ode_id/obs_id.
 */
extern void Asc_IntegPrintObsHeader(FILE *fp, struct Integ_system_t *blsys);
/**<
 *  Prints observation header info to the file given.
 *  If FILE is NULL, returns immediately.
 *  If FILE ok, but blsys NULL prints warning to stderr and returns.
 *  At the moment header info is:
 *      A vertical listing of
 *      ode_id/obs_id instance_name_as_seen_in_solver.d UNITS
 *      then a row (which will line up with printed values)
 *      of the ode_id/obs_id.
 */

extern void Asc_IntegPrintYLine(FILE *fp, struct Integ_system_t *blsys);
/**<
 *  Prints a Y line to the file given.
 *  If FILE is NULL, returns immediately.
 *  If FILE ok, but blsys NULL prints warning to stderr and returns.
 *  Each line is a set of values, identified by the header, following
 *  the value of the independent variable. These lines are of arbitrary
 *  length since it is expected that some other program will be
 *  reading the output, not a human.
 */
extern void Asc_IntegPrintObsLine(FILE *fp, struct Integ_system_t *blsys);
/**<
 *  Prints an observation line to the file given.
 *  If FILE is NULL, returns immediately.
 *  If FILE ok, but blsys NULL prints warning to stderr and returns.
 *  Each line is a set of values, identified by the header, following
 *  the value of the independent variable. These lines are of arbitrary
 *  length since it is expected that some other program will be
 *  reading the output, not a human.
 */

extern int Asc_IntegSetYFileCmd(ClientData cdata, Tcl_Interp *interp,
                                int argc, CONST84 char *argv[]);
/**<
 *  Set the next filename to be used for integration state variable
 *  logging. Does not check the filesystem sanity of the given name.<br><br>
 *
 *  Registered as: integrate_set_y_file filename
 */

extern int Asc_IntegSetObsFileCmd(ClientData cdata, Tcl_Interp *interp,
                                  int argc, CONST84 char *argv[]);
/**<
 *  Set the next filename to be used for integration observation variable
 *  logging. Does not check the filesystem sanity of the given name.<br><br>
 *
 *  Registered as: integrate_set_obs_file filename
 */

extern int Asc_IntegSetFileUnitsCmd(ClientData cdata, Tcl_Interp *interp,
                                    int argc, CONST84 char *argv[]);
/**<
 *  Sets output to be in SI (internal) units or in the user set display
 *  units. If display is selected and the variable to be printed cannot
 *  be displayed in the display units because of floating point errors
 *  in conversion, the value will be printed in SI units and a warning
 *  issued to stderr. The user is then expected to deal with the problem
 *  of inconsistency in his output.
 *  Only the first letter of the argument is significant.<br><br>
 *
 *  Registered as: integrate_logunits <display,si>
 */

extern int Asc_IntegSetFileFormatCmd(ClientData cdata, Tcl_Interp *interp,
                                     int argc, CONST84 char *argv[]);
/**<
 *  Sets output data to be in fixed column width (with extra white) or in
 *  space separated columns whose width is variable from line to line.
 *  Only the first letter of the argument is significant.<br><br>
 *
 *  Registered as: integrate_logformat <fixed,variable>
 */

extern int Asc_IntegSetXSamples(long ns, double *values, dim_type *dimp);
/**<
 *  <!--  Asc_IntegSetXSamples(ns, values, dimp);                      -->
 *  Sets values of time samples to the values given (ns of them) and
 *  keeps both the dim pointer and vector given. The vector and dimp
 *  given may be freed by calling this again, but xsamples owns
 *  them until then. If called with ns<1 or values==NULL, will free any
 *  previously captured values/dimp. If called with dimp==NULL, will
 *  assume WildDimension. Don't call this with a dimp we can't free later.
 *  Return is 1 if for some reason we can't set as expected, 0 otherwise.
 */

extern double Asc_IntegGetStepZero(struct Integ_system_t *sys);
/**<
 *  Returns the length of the initial step user specified,
 *  or 0.0 if none was set.
 */

extern double Asc_IntegGetStepMin(struct Integ_system_t *sys);
/**<
 *  Returns the length of the longest allowable step.
 *  or 0.0 if none was set by user.
 */

extern double Asc_IntegGetStepMax(struct Integ_system_t *sys);
/**<
 *  Returns the length of the shortest allowable step.
 *  or 0.0 if none was set by user.
 */

extern int Asc_IntegGetMaxSteps(struct Integ_system_t *sys);
/**<
 *  Returns the most internal steps allowed between
 *  two time samples,  or 0 if none was set by user.
 */

extern long Asc_IntegGetNSamples(void);
/**<
 *  Returns the number of values currently stored in xsamples.
 */

extern double Asc_IntegGetXSamplei(long i);
/**<
 *  Returns the value stored in xsamples[i].  Will whine if
 *  if xsample[i] does not exist.   No, there is no way to get
 *  back the pointer to the xsamples vector.
 */
extern void Asc_IntegSetXSamplei(long i, double val);
/**<
 *  Sets the value stored in xsamples[i] to val.  Will whine if
 *  if xsample[i] does not exist.   No, there is no way to get
 *  back the pointer to the xsamples vector.
 */

extern dim_type *Asc_IntegGetXSamplesDim(void);
/**<
 *  Returns a pointer to the dimensionality of the samples currently
 *  stored, or NULL if none are stored. Do not free this pointer: we
 *  own it.
 */

extern int Asc_IntegGetXSamplesCmd(ClientData cdata, Tcl_Interp *interp,
                                   int argc, CONST84 char *argv[]);
/**<
 *  Returns the current vector of independent variable samples as fed to
 *  smarter integrators such as lsode. Returns 2 element list:
 *  {units of x} {list of doubles in the units indicated in the first
 *  element}. In the event that display is given but the sample values
 *  (which are stored in si) cannot be converted to display units, the
 *  units of x in the first element will change and the SI values will
 *  be returned in the second element. If display is not given, samples
 *  will be returned in SI units.
 *  The units used will be dimensionally consistent with those specified
 *  when integrate_set_samples was called.
 *  Only the first letter of the optional argument display is significant.<br><br>
 *
 *  Registered as: integrate_get_samples [display]
 */

extern int Asc_IntegSetXSamplesCmd(ClientData data, Tcl_Interp *interp,
                                   int argc, CONST84 char *argv[]);
/**<
 *  Takes the values given and sets the intervals for smarter
 *  integrators accordingly. Values will be converted to SI values
 *  using the units given. ?,* is a legal unit, in which case the
 *  values will be assumed to be SI requiring no conversion.
 *  If no arguments are given, we will free memory allocated for storage
 *  of the list, otherwise at least 2 values are required.<br><br>
 *
 *  Note: It is the users responsibility to make sure that the dimens/
 *  units this is called with match the independent variable in the ivp.
 *  C clients are given the power to verify this.
 *  Also, the units given must already exist: we do not create them.<br><br>
 *
 *  Registered as:  integrate_set_samples <units> <value [value...] value>
 */

extern int Asc_IntegInstIntegrableCmd(ClientData cdata, Tcl_Interp *interp,
                                      int argc, CONST84 char *argv[]);
/**<
 *  Returns "1" if instance is integrable by the named integrator, "0"
 *  otherwise. Currently valid names are: lsode
 *  The instance examined is "current", "search", or "solver".<br><br>
 *
 *  Registered as:  integrate_able <instance> <NAME>
 */

/*
 *  The following functions fetch and set parts with names specific to
 *  the type definitions in ivp.lib, the ASCEND initial value problem
 *  model file. They are for use by any ivp solver interface.
 *  All names are given at the scope of ivp.
 */

/*
 * Problem size parameters.
 */

/*
 * Parts of type definition derivatives refinements.
 */

extern double Asc_IntegGetDX(void);
/**<
 *  <!--  IntegGet_d_x(value)                                          -->
 *  Gets d.x  value.
 */

extern void Asc_IntegSetDX(double value);
/**<
 *  <!--  IntegSet_d_x(value)                                          -->
 *  Sets d.x to value.
 */

extern double *Asc_IntegGetDY(double *vector);
/**<
 *  <!--  vector=IntegGetDY(vector)                                    -->
 *  Return the vector d.y.
 *  If vector given is NULL, allocates vector, which the caller then owns.
 *  Vector, if given, should be IntegGet_d_neq()+1 long.
 */

extern void Asc_IntegSetDY(double *vector);
/**<
 *  <!--  IntegSet_d_y(vector)                                         -->
 *  Sets d.y[] to values in vector.
 */

extern double *Asc_IntegGetDDydx(double *vector);
/**<
 *  <!--  IntegGet_d_dydx(vector)                                      -->
 *  Returns the vector d.dydx.
 *  If vector given is NULL, allocates vector, which the caller then owns.
 *  Vector, if given, should be IntegGet_d_neq()+1 long.
 */

extern void Asc_IntegSetDDydx(double *vector);
/**<
 *  <!--  IntegSet_d_dydx(vector)                                      -->
 *  Sets d.dydx[] to values in vector.
 */

extern double *Asc_IntegGetDObs(double *vector);
/**<
 *  <!--  IntegGet_d_obs(vector)                                       -->
 *  Returns the vector d.obs.
 *  Vector should be of sufficient length (g_intginst_d_n_obs+1).
 *  If NULL is given a vector is allocated and is the caller's
 *  responsibility to deallocate.
 */

/*
 *  END of the general ivp type utilities.
 */

extern int Asc_IntegSetupCmd(ClientData cdata,Tcl_Interp *interp,
                             int argc, CONST84 char *argv[]);
/**<
 *  Set up the integrator.
 *  itype is one of the supported integrator types (currently:
 *  BLSODE)
 *  n1 is the start index to run the integrator from.
 *  n2 is the end index to stop at.
 *
 *  Registered as:  integrateSetup itype n1 n2
 */

extern int Asc_IntegCleanupCmd(ClientData cdata, Tcl_Interp *interp,
                               int argc, CONST84 char *argv[]);
/**<
 *  Do any C level housekeeping need after calling integration.
 *  Always use integrate_setup/integrate_cleanup in pairs.<br><br>
 *
 *  Registered as:  integrate_cleanup
 */

#endif  /* integration_already_included */
