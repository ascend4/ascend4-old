/*
 *  Problem Analysis Routines
 *  by Benjamin Andrew Allan
 *  5/19/96
 *  Version: $Revision: 1.10 $
 *  Version control file: $RCSfile: analyze.h,v $
 *  Date last modified: $Date: 1997/07/18 12:13:50 $
 *  Last modified by: $Author: mthomas $
 *  Copyright(C) 1996 Benjamin Andrew Allan
 *
 *  This file is part of the ASCEND IV math programming system.
 *
 *  The SLV solver is free software; you can redistribute
 *  it and/or modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  The SLV solver is distributed in hope that it will be
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
 *  Problem Analysis Routines.
 *  <pre>
 *  These functions are the start of a new design for feeding
 *  solvers from the ASCEND compiler or any arbitrary backend.
 *
 *  The intention is that eventually the other code in the solver
 *  directory will really be for *solution algorithms* and the
 *  definition of a *problem* will come from here. In essence, most
 *  of what was solver/system.c will be here. Negotiating directly
 *  with the ASCEND instance hierarchy should not be a solver's
 *  job.
 *  The goal of this module is to CREATE a slv_system_t data structure
 *  capable of supporting code generation, an interactive interface, and
 *  in-core solvers, while being expandable in the future to out of core
 *  solvers/external-process solvers.
 *
 *  A secondary goal is to have nonlinear solver files be independent of
 *  all the compiler directory files except ascmalloc.h.
 *  The present fly in the ointment is expr.h because of the objective fcns.
 *  The relman and exprman modules go away because they are indicative of
 *  functionality that belongs either in the compiler or rel.c.
 *  If we meet this goal, then it is a simple matter to connect any
 *  arbitrary compiler backend to the solver API by replacing the rel
 *  and var and analyze modules.
 *
 *  Requires:
 *         #include "utilities/ascConfig.h"
 *         #include "system.h"
 *         #include "instance_enum.h"
 *  </pre>
 */

#ifndef _analyze_h_seen_
#define _analyze_h_seen_

/** Used to give an integer value to each symbol used in a when */
struct SymbolValues {
  char *name;
  int value;
};

extern int analyze_make_problem(slv_system_t sys, struct Instance *inst);
/**<
 *  Takes a system and populates the guts of it from the instance.
 *  this implementation of analyze is specific to the ascend compiler
 *  back end.
 */

extern int analyze_remake_problem(slv_system_t sys);
/**<
 * Reanalyzes the system, reordering solvers var and relation lists
 * if needed.
 * @todo Not implemented.  Remove from header if not needed.
 */

extern void analyze_free_reused_mem(void);
/**< 
 * Resets all internal memory recycles.
 */

extern int varinst_found_in_whenlist(slv_system_t sys, struct Instance *inst);
/**<
 * Determine if the conditional variable inst is part of the
 * variable list of some when in the when list of slv_system_t
 */

extern int dis_var_in_a_when(struct Instance *var, struct w_when *when);
/**<
 * Return 1 if the discrete var is a member of the when var list, else
 * return 0
 */

extern int GetIntFromSymbol(CONST char *symval, struct gl_list_t *symbol_list);
/**<
 * Creates the gl_list of SymboValues struct to asign an integer
 * value to a symbol value
 */

extern void DestroySymbolValuesList(struct gl_list_t *symbol_list);
/**<
 * Destroy the gl_list of SymbolValues struct created to asign an integer
 * value to symbol value
 */

#endif /*  _analyze_h_seen_ */
