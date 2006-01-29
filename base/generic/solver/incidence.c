/*
	ASCEND Solver Interface
	Copyright (C) 2006 Carnegie Mellon University

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#include <solver/incidence.h>

#include <utilities/ascSignal.h>
#include <utilities/ascMalloc.h>
#include <utilities/error.h>
#include <utilities/mem.h>
#include <utilities/set.h>
#include <general/list.h>
#include <general/mathmacros.h>

#include <compiler/instance_enum.h>

#include "discrete.h"
#include "conditional.h"
#include "logrel.h"
#include "bnd.h"
#include "mtx.h"
#include "linsol.h"
#include "linsolqr.h"

#include "slv_common.h"

#include "slv_client.h"

/*
	--removed some old notes from 1996-1997, please see code repository--
*/
int
build_incidence_data(CONST slv_system_t sys, incidence_vars_t *pd){

  int32 mord=-1, row,col,var,rel,plrow,plcol,uninclow;
  var_filter_t vincident;
  rel_filter_t ractive;

  vincident.matchbits = (VAR_INCIDENT | VAR_SVAR | VAR_ACTIVE);
  vincident.matchvalue = (VAR_INCIDENT | VAR_SVAR | VAR_ACTIVE );

  ractive.matchbits = (REL_ACTIVE);
  ractive.matchvalue = (REL_ACTIVE );

  if (ISNULL(sys) || ISNULL(pd)) {
    ERROR_REPORTER_HERE(ASC_PROG_ERR,"called with NULL!");
    return 1; /* ERROR */
  }

  pd->vlist = slv_get_solvers_var_list(sys);
  pd->rlist = slv_get_solvers_rel_list(sys);
  if (pd->vlist==NULL || pd->rlist==NULL) {
    ERROR_REPORTER_HERE(ASC_PROG_ERROR,"Nothing to plot!");
    return 1;
  }
  pd->neqn = slv_count_solvers_rels(sys,&ractive);
  pd->nprow = pd->neqn;
  pd->nvar = slv_get_num_solvers_vars(sys);
  pd->npcol = slv_count_solvers_vars(sys,&vincident);
  pd->nfakevar = pd->npcol; /* this could change with autoslack solvers */
  pd->pr2e = (int *)ascmalloc(sizeof(int)*(pd->nprow +1));
  pd->e2pr = (int *)ascmalloc(sizeof(int)*(pd->neqn +1));
  pd->pc2v = (int *)ascmalloc(sizeof(int)*(pd->npcol +1));
  pd->v2pc = (int *)ascmalloc(sizeof(int)*(pd->nvar +1));
  pd->vfixed = (char *)asccalloc((pd->nvar +1),sizeof(char));
  if ( ISNULL(pd->pr2e) || ISNULL(pd->e2pr) ||
       ISNULL(pd->pc2v) || ISNULL(pd->v2pc) ||
       ISNULL(pd->vfixed) ) {
    ERROR_REPORTER_HERE(ASC_PROG_ERROR,"Insufficient memory!");
    return 1;
  }
  mord =  MAX(pd->neqn,pd->nvar);

  /* fix up row permutations */
  plrow=plcol=-1;
  uninclow=pd->neqn; /* set lowwater mark for unincluded eqns */
  for (row=0;row<mord;row++) {
    rel = row;
    if (rel < pd->neqn) {
      if (rel_included(pd->rlist[rel]) && rel_active(pd->rlist[rel])) {
        plrow++;
        pd->pr2e[plrow] = rel;
        pd->e2pr[rel] = plrow;
      } else {
        uninclow--;
        pd->pr2e[uninclow] = rel;
        pd->e2pr[rel] = uninclow;
      }
    } /* else skip this row: it is nothing */
  }

  for (col = 0; col < mord; col++) {
    var = col;
    if (var < pd->nvar) {
      /* set fixed flag vector whether incident or not */
      if (var_fixed(pd->vlist[var])) {
        pd->vfixed[var]=1;
      }
      if (var_incident(pd->vlist[var]) && var_active(pd->vlist[var])) {
        plcol++;
        pd->pc2v[plcol] = var;
        pd->v2pc[var] = plcol;
      } else {
        /* nonincident vars dont get plot cols */
        pd->v2pc[var] = -1;  /* be safe if someone asks */
      }
    } /* else skip this col: it is nothing */
  }
  return 0; /* OK */
}

/*
	Note, simplified this code since there was no case where 'non-owned' data was being used in incidence_vars_t.
*/
void 
free_incidence_data(incidence_vars_t *pd) {

  if (pd != NULL) {
    ascfree(pd->pr2e); pd->pr2e=NULL;
    ascfree(pd->e2pr); pd->e2pr=NULL;
    ascfree(pd->pc2v); pd->pc2v=NULL;
    ascfree(pd->v2pc); pd->v2pc=NULL;
    ascfree(pd->vfixed); pd->vfixed=NULL;
  }
}

