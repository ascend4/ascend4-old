/*
 *  General routines for Slv interface.
 *  by Ben Allan
 *  Created 1/95
 *  Version: $Revision: 1.25 $
 *  Version control file: $RCSfile: slv_common.c,v $
 *  Date last modified: $Date: 1998/02/02 23:16:22 $
 *  Last modified by: $Author: ballan $
 *  This file is part of the SLV solver.
 *
 *  Copyright (C) 1990 Karl Michael Westerberg
 *  Copyright (C) 1993 Joseph Zaher
 *  Copyright (C) 1994 Joseph Zaher, Benjamin Andrew Allan
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

/*
 *  general C  utility routines for slv class interfaces. Abstracted from
 *  slvX.c January 1995. Ben Allan.
 */

#include <math.h>
#include "utilities/ascConfig.h"
#include "utilities/ascSignal.h"
#include "compiler/compiler.h"
#include "utilities/ascMalloc.h"
#include "solver/mtx.h"
#include "solver/slv_types.h"
#include "solver/rel.h"
#include "solver/var.h"
#include "solver/discrete.h"
#include "solver/logrel.h"
#include "solver/slv_common.h"
#include "utilities/mem.h"
/* if libasc.a running around, the following: */
#if SLV_INSTANCES
#include "compiler/fractions.h"
#include "compiler/dimen.h"
#include "compiler/functype.h"
#include "compiler/func.h"
#include "solver/relman.h"
#include "solver/logrelman.h"
#else
#ifdef NDEBUG
#define ascnint(a) (((int) (a)>=0.0 ? floor((a) + 0.5) : -floor(0.5 - (a))))
#else
#define ascnint(a) ascnintF(a)
#endif /* NDEBUG */
#endif /* instances */

#define SAFE_FIX_ME 0

/**
 ***  Array/vector operations
 ***  ----------------------------
 ***     slv_zero_vector(vec)
 ***     slv_copy_vector(vec1,vec2)
 ***     prod = slv_inner_product(vec1,vec2)
 ***     norm2 = slv_square_norm(vec)
 ***     slv_matrix_product(mtx,vec,prod,scale,transpose)
 ***     slv_write_vector(file,vec)
 **/

void slv_zero_vector( struct vector_data *vec)
{
  real64 *p;
  int32 len;

  p = vec->vec + vec->rng->low;
  len = vec->rng->high - vec->rng->low + 1;
  mtx_zero_real64(p,len);
}

void slv_copy_vector( struct vector_data *vec1,struct vector_data *vec2)
{
  real64 *p1,*p2;
  int32 len;

  p1 = vec1->vec + vec1->rng->low;
  p2 = vec2->vec + vec2->rng->low;
  len = vec1->rng->high - vec1->rng->low + 1;
  /*mem_copy_cast not in order here, probably */
  mem_move_cast(p1,p2,len*sizeof(real64));
}

#define USEDOT TRUE
/* USEDOT = TRUE is a winner on alphas, hps, and sparc20 */
real64 slv_inner_product(struct vector_data *vec1 ,
                               struct vector_data *vec2)
/**
 ***  Computes inner product between vec1 and vec2, returning result.
 ***  vec1 and vec2 may overlap or even be identical.
 **/
{
  real64 *p1,*p2;
#if !USEDOT
  real64 sum;
#endif
  int32 len;

  p1 = vec1->vec + vec1->rng->low;
  p2 = vec2->vec + vec2->rng->low;
  len = vec1->rng->high - vec1->rng->low + 1;
#if !USEDOT
  if (p1 != p2) {
    for( sum=0.0 ; --len >= 0 ; ++p1,++p2 ) {
      sum += (*p1) * (*p2);
    }
    return(sum);
  } else {
    for( sum=0.0 ; --len >= 0 ; ++p1 ) {
      sum += (*p1) * (*p1);
    }
    return(sum);
  }
#else
  return slv_dot(len,p1,p2);
#endif
}

real64 slv_square_norm(struct vector_data *vec)
/**
 ***  Computes norm^2 of vector, assigning the result to vec->norm2
 ***  and returning the result as well.
 **/
{
  vec->norm2 = slv_inner_product(vec,vec);
  return vec->norm2;
}

void slv_matrix_product(mtx_matrix_t mtx, struct vector_data *vec,
                        struct vector_data *prod, real64 scale,
                        boolean transpose)
/**
 ***  Stores prod := (scale)*(mtx)(vec) or (scale)*(mtx-transpose)(vec).
 ***  vec and prod must be completely different.
 **/
{
  mtx_coord_t nz;
  real64 value, *vvec, *pvec;
  int32 lim;

  lim=prod->rng->high;
  pvec=prod->vec;
  vvec=vec->vec;
  if( transpose ) {
    for(nz.col = prod->rng->low ; nz.col <= lim ; ++(nz.col) ) {
      pvec[nz.col] = 0.0;
      nz.row = mtx_FIRST;
      while( value = mtx_next_in_col(mtx,&nz,vec->rng),
             nz.row != mtx_LAST )
        pvec[nz.col] += value*vvec[nz.row];
      pvec[nz.col] *= scale;
     }
  } else {
    for(nz.row = prod->rng->low ; nz.row <= lim ; ++(nz.row) ) {
      pvec[nz.row] = 0.0;
      nz.col = mtx_FIRST;
      while( value = mtx_next_in_row(mtx,&nz,vec->rng),
             nz.col != mtx_LAST )
        pvec[nz.row] += value*vvec[nz.col];
      pvec[nz.row] *= scale;
    }
  }
}

void slv_write_vector(FILE *fp,struct vector_data *vec)
/**
 ***  Outputs a vector.
 **/
{
  int32 ndx,hi;
  real64 *vvec;
  vvec = vec->vec;
  hi = vec->rng->high;
  FPRINTF(fp,"Norm = %g, Accurate = %s, Vector range = %d to %d\n",
    sqrt(fabs(vec->norm2)), vec->accurate?"TRUE":"FALSE",
    vec->rng->low,vec->rng->high);
  FPRINTF(fp,"Vector --> ");
  for( ndx=vec->rng->low ; ndx<=hi ; ++ndx )
    FPRINTF(fp, "%g ", vvec[ndx]);
  PUTC('\n',fp);
}

/* Dot product for loop unrolled vector norms */
real64 slv_dot(int32 len, const real64 *p1, const real64 *p2)
{
  register double sum,lsum;
  int m,n;

/* AVMAGIC in fact isn't magic.
 * only goes to 2-10. change the code below if you mess with it.
 * Default AVMAGIC is 4, which works well on alphas, tika.
 */
#define AVMAGIC 4
#ifdef sun
#undef AVMAGIC
#define AVMAGIC 6
#endif
#ifdef __hpux
#undef AVMAGIC
#define AVMAGIC 4
/* 2 was best value on ranier(9000/720) but tika (9000/715) likes 4
 * there are no recognizable (defines) compiler differences, so tika
 * will set the hp default
 */
#endif
/* hands down best avmagic on atlas is 4, ranier 2, unxi21 6
 * under native compilers. no bets on the billions of gcc variants.
 * needs to be tried on sgi.
 * Note, these are tuned for speed, not for accuracy. on very large
 * vectors something like dnrm2 may be more appropriate, though
 * it is very slow. upping avmagic to 10 will help accuracy some.
 */

#if (AVMAGIC>10)
#undef AVMAGIC
#define AVMAGIC 10
#endif
  m = len / AVMAGIC;
  n = len % AVMAGIC;
  if (p1!=p2) {
    /* get leading leftovers */
    for( sum=0.0 ; --n >= 0 ;  ) {
      sum += (*p1) * (*p2);
      ++p1; ++p2;
    }
    /* p1,p2 now point at first unadded location, or just after the end (m=0)*/
    /* eat m chunks */
    for( n=0; n <m; n++) {
      /* now, as i am too lazy to figure out a macro that expands itself */
      lsum = (*p1) * (*p2); /* zeroth term is assigned */
      p1++; p2++;
      lsum += (*p1) * (*p2); /* add 1th */
#if (AVMAGIC>2)
      p1++; p2++;
      lsum += (*p1) * (*p2); /* add 2th */
#if (AVMAGIC>3)
      p1++; p2++;
      lsum += (*p1) * (*p2); /* add 3th */
#if (AVMAGIC>4)
      p1++; p2++;
      lsum += (*p1) * (*p2); /* add 4th */
#if (AVMAGIC>5)
      p1++; p2++;
      lsum += (*p1) * (*p2); /* add 5th */
#if (AVMAGIC>6)
      p1++; p2++;
      lsum += (*p1) * (*p2); /* add 6th */
#if (AVMAGIC>7)
      p1++; p2++;
      lsum += (*p1) * (*p2); /* add 7th */
#if (AVMAGIC>8)
      p1++; p2++;
      lsum += (*p1) * (*p2); /* add 8th */
#if (AVMAGIC>9)
      p1++; p2++;
      lsum += (*p1) * (*p2); /* add 9th */
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
      p1++; p2++;             /* leave p1,p2 pointing at next zeroth */
      sum += lsum;
    }
  } else {
    /* get leading leftovers */
    for( sum=0.0 ; --n >= 0 ; ++p1 ) {
      sum += (*p1) * (*p1);
    }
    /* p1 now points at first unadded location, or just after the end (m=0)*/
    /* eat m chunks */
    for( n=0; n <m; n++) {
      /* now, as i am too lazy to figure out a macro that expands itself */
      lsum = (*p1) * (*p1); /* zeroth term is assigned */
      p1++;
      lsum += (*p1) * (*p1); /* add 1th */
#if (AVMAGIC>2)
      p1++;
      lsum += (*p1) * (*p1); /* add 2th */
#if (AVMAGIC>3)
      p1++;
      lsum += (*p1) * (*p1); /* add 3th */
#if (AVMAGIC>4)
      p1++;
      lsum += (*p1) * (*p1); /* add 4th */
#if (AVMAGIC>5)
      p1++;
      lsum += (*p1) * (*p1); /* add 5th */
#if (AVMAGIC>6)
      p1++;
      lsum += (*p1) * (*p1); /* add 6th */
#if (AVMAGIC>7)
      p1++;
      lsum += (*p1) * (*p1); /* add 7th */
#if (AVMAGIC>8)
      p1++;
      lsum += (*p1) * (*p1); /* add 8th */
#if (AVMAGIC>9)
      p1++;
      lsum += (*p1) * (*p1); /* add 9th */
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
      p1++;                  /* leave p1 pointing at next zeroth */
      sum += lsum;
    }
  }
  return(sum);
#undef AVMAGIC
}

/**
 ***  General input/output routines
 ***  -----------------------------
 ***     fp = MIF(sys)
 ***     fp = LIF(sys)
 ***     slv_print_var_name(out,sys,var)
 ***     slv_print_rel_name(out,sys,rel)
 ***     slv_print_dis_name(out,sys,dvar)
 ***     slv_print_logrel_name(out,sys,lrel)
 ***  NOt yet implemented correctly:
 ***     slv_print_obj_name(out,obj)
 ***     slv_print_var_sindex(out,var)
 ***     slv_print_rel_sindex(out,rel)
 ***     slv_print_dis_sindex(out,dvar)
 ***     slv_print_logrel_sindex(out,lrel)
 ***     slv_print_obj_index(out,obj)
 **/

FILE *slv_get_output_file(fp)
FILE *fp;
/**
 ***  Returns fp if fp!=NULL, or a file pointer
 ***  open to nul device if fp == NULL.
 **/
{
   static FILE *nuldev = NULL;
   static char fname[] = "/dev/null";

   if( fp==NULL ) {
      if(nuldev==NULL)
         if( (nuldev=fopen(fname,"w")) == NULL ) {
            FPRINTF(stderr,"ERROR:  (slv) get_output_file\n");
            FPRINTF(stderr,"        Unable to open %s.\n",fname);
         }
      fp=nuldev;
   }
   return(fp);
}
#define MIF(sys) slv_get_output_file( (sys)->p.output.more_important )
#define LIF(sys) slv_get_output_file( (sys)->p.output.less_important )

#if SLV_INSTANCES

void slv_print_var_name( FILE *out,slv_system_t sys, struct var_variable *var)
{
   char *name=NULL;
   if (out == NULL || sys == NULL || var == NULL) return;
   name = var_make_name(sys,var);
   if( *name == '?' ) FPRINTF(out,"%d",var_sindex(var));
   else FPRINTF(out,"%s",name);
   if (name) ascfree(name);
}

void slv_print_rel_name( FILE *out, slv_system_t sys, struct rel_relation *rel)
{
   char *name;
   name = rel_make_name(sys,rel);
   if( *name == '?' ) FPRINTF(out,"%d",rel_sindex(rel));
   else FPRINTF(out,"%s",name);
   ascfree(name);
}

void slv_print_dis_name( FILE *out,slv_system_t sys, struct dis_discrete *dvar)
{
   char *name=NULL;
   if (out == NULL || sys == NULL || dvar == NULL) return;
   name = dis_make_name(sys,dvar);
   if( *name == '?' ) FPRINTF(out,"%d",dis_sindex(dvar));
   else FPRINTF(out,"%s",name);
   if (name) ascfree(name);
}

void slv_print_logrel_name( FILE *out, slv_system_t sys,
                            struct logrel_relation *lrel)
{
   char *name;
   name = logrel_make_name(sys,lrel);
   if( *name == '?' ) FPRINTF(out,"%d",logrel_sindex(lrel));
   else FPRINTF(out,"%s",name);
   ascfree(name);
}

#ifdef NEWSTUFF
void slv_print_obj_name(out,obj)
FILE *out;
obj_objective_t obj;
{
   char *name;
   name = obj_make_name(obj);
   if( *name == '?' ) FPRINTF(out,"%d",obj_index(obj));
   else FPRINTF(out,"%s",name);
   ascfree(name);
}
#endif

void slv_print_var_sindex(out,var)
FILE *out;
struct var_variable *var;
{
   FPRINTF(out,"%d",var_sindex(var));
}

void slv_print_rel_sindex(out,rel)
FILE *out;
struct rel_relation *rel;
{
   FPRINTF(out,"%d",rel_sindex(rel));
}

void slv_print_dis_sindex(out,dvar)
FILE *out;
struct dis_discrete *dvar;
{
   FPRINTF(out,"%d",dis_sindex(dvar));
}

void slv_print_logrel_sindex(out,lrel)
FILE *out;
struct logrel_relation *lrel;
{
   FPRINTF(out,"%d",logrel_sindex(lrel));
}

#ifdef NEWSTUFF
void slv_print_obj_index(out,obj)
FILE *out;
struct rel_relation *rel;
{
   FPRINTF(out,"%d",obj_index(obj));
}
#endif

#define destroy_array(p)  \
   if( (p) != NULL ) ascfree((p))

int slv_direct_solve(slv_system_t server, struct rel_relation *rel,
                     struct var_variable *var, FILE *fp,
                     real64 epsilon, int ignore_bounds, int scaled)
/**
 ***  Attempt to directly solve the given relation (equality constraint) for
 ***  the given variable, leaving the others fixed.  Returns an integer
 ***  signifying the status as one of the following three:
 ***
 ***     0  ==>  Unable to determine anything, possibly due to fp exception.
 ***     1  ==>  Solution found, variable value set to this solution.
 ***    -1  ==>  No solution found.
 ***
 ***  The variable bounds will be upheld, unless they are to be ignored.
 **/
{
  int32 able,status;
  int nsolns,allsolns;
  real64 *slist,save;

  slist = relman_directly_solve_new(rel,var,&able,&nsolns,epsilon);
  if( !able ) {
    return(0);
  }
  allsolns = nsolns;
  while( --nsolns >= 0 ) {
    if( ignore_bounds ) {
      var_set_value(var,slist[nsolns]);
      break;
    }
    if( var_lower_bound(var) > slist[nsolns] ) {
      save = var_value(var);
      var_set_value(var,var_lower_bound(var));
      Asc_SignalHandlerPush(SIGFPE,SIG_IGN);
      relman_eval(rel,&status,SAFE_FIX_ME);
      Asc_SignalHandlerPop(SIGFPE,SIG_IGN);
      if (scaled) {
        if( relman_calc_satisfied_scaled(rel,epsilon) ) break;
      } else {
        if( relman_calc_satisfied(rel,epsilon) ) break;
      }
      var_set_value(var,save);
    } else if( var_upper_bound(var) < slist[nsolns] ) {
      save = var_value(var);
      var_set_value(var,var_upper_bound(var));
      Asc_SignalHandlerPush(SIGFPE,SIG_IGN);
      relman_eval(rel,&status,SAFE_FIX_ME);
      Asc_SignalHandlerPop(SIGFPE,SIG_IGN);
      if (scaled) {
        if( relman_calc_satisfied_scaled(rel,epsilon) ) break;
      } else {
        if( relman_calc_satisfied(rel,epsilon) ) break;
      }
      var_set_value(var,save);
    } else {
      var_set_value(var,slist[nsolns]);
      Asc_SignalHandlerPush(SIGFPE,SIG_IGN);
      relman_eval(rel,&status,SAFE_FIX_ME);
      Asc_SignalHandlerPop(SIGFPE,SIG_IGN);
      break;
    }
  }
  if (nsolns<0 && allsolns>0 && fp !=NULL) {
   /* dump the rejected solutions to give the user a clue */
    FPRINTF(fp,"Ignoring potential solutions for variable\n");
    var_write_name(server,var,fp);
    FPRINTF(fp,"\nin equation\n");
    rel_write_name(server,rel,fp);
    FPRINTF(fp,"\n");
    for (--allsolns; allsolns >= 0; allsolns--)  {
      FPRINTF(fp,"Rejected solution: %.18g\n",slist[allsolns]);
    }
  }
  /* destroy_array(slist); do not do this */
  return( nsolns >= 0 ? 1 : -1 );
}


int slv_direct_log_solve(slv_system_t server, struct logrel_relation *lrel,
                         struct dis_discrete *dvar, FILE *fp, int perturb,
			 struct gl_list_t *insts)
/**
 ***  Attempt to directly solve the given relation (equality constraint) for
 ***  the given variable, leaving the others fixed.  Returns an integer
 ***  signifying the status as one of the following three:
 ***
 ***     0  ==>  Unable to determine anything. Bad logrelation or dvar
 ***     1  ==>  Solution found, discrete variable value set to this solution.
 ***     2  ==>  More than one solution, Do not modify the current value
 ***             of the variable and display an error message.
 ***    -1  ==>  No solution found.
 **/
{
  int32 able;
  int32 nsolns, c;
  int32 *slist;

  slist = logrelman_directly_solve(lrel,dvar,&able,&nsolns,perturb,insts);
  if( !able ) return(0);
  if(nsolns == -1) return (-1);

  if (nsolns == 1) {
    dis_set_boolean_value(dvar,slist[1]);
    logrel_set_residual(lrel,TRUE);
    destroy_array(slist);
    return 1;
  } else {
    FPRINTF(fp,"Ignoring potential solutions for discrete variable\n");
    dis_write_name(server,dvar,fp);
    FPRINTF(fp,"\nin equation\n");
    logrel_write_name(server,lrel,fp);
    FPRINTF(fp,"\n");
    for (c = nsolns; c >= 1; c--)  {
      FPRINTF(fp,"Rejected solution: %d \n",slist[c]);
    }
    destroy_array(slist); /* should we have to do this? */
    return 2;
  }
}


#endif

int32 **slv_lnkmap_from_mtx(mtx_matrix_t mtx, int32 len, int32 m)
{
  int32 **map,*data,rl;
  real64 val;
  mtx_coord_t coord;

  if (mtx==NULL) {
    FPRINTF(stderr,"Warning: slv_lnkmap_from_mtx called with null mtx\n");
    return NULL;
  }
  data=(int32 *)ascmalloc((m+2*len)*sizeof(int32));
  map=(int **)ascmalloc(m*sizeof(void *));
  for(coord.row=0; coord.row < m; coord.row ++) {
    rl=mtx_nonzeros_in_row(mtx,coord.row,mtx_ALL_COLS);
    map[coord.row]=data;
    data[0]=rl;
    data++;
    coord.col=mtx_FIRST;
    while( val=mtx_next_in_row(mtx,&coord,mtx_ALL_COLS),
           coord.col != mtx_LAST) {
      data[0]=coord.col;
      data[1]= (int32)ascnint(val);
      data+=2;
    }
  }
  return map;
}
int32 **slv_create_lnkmap(int m, int n, int len, int *hi, int *hj) {
  mtx_matrix_t mtx;
  mtx_coord_t coord;
  int32 i, **map;

  mtx=mtx_create();
  mtx_set_order(mtx,MAX(m,n));
  for (i=0;i <len; i++) {
    coord.row=hi[i];
    coord.col=hj[i];
    mtx_fill_value(mtx,&coord,(real64)i);
  }
  map=slv_lnkmap_from_mtx(mtx,len,m);
  mtx_destroy(mtx);
  return map;
}

void slv_destroy_lnkmap(int32 **map) {
  if (NOTNULL(map)) {
    ascfree(map[0]);
    ascfree(map);
  }
}

void slv_write_lnkmap(FILE *fp, int32 m, int32 **map) {
  int32 i,j,nv, *v;
  if (ISNULL(map)) {
    FPRINTF(stderr,"slv_write_lnkmap: Cannot write NULL lnkmap\n");
    return;
  }
  for(i=0; i<m; i++) {
    if (NOTNULL(map[i])) {
      v=map[i];
      nv=v[0];
      v++;
      FPRINTF(fp,"Row %d len = %d\n",i,nv);
      for (j=0;j<nv;j++) {
        FPRINTF(fp,"  Col %d, lnk location %d\n",v[0],v[1]);
        v+=2;
      }
    }
  }
}
