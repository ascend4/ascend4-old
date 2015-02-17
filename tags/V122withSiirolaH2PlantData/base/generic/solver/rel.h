/* 
 *  SLV: Ascend Numeric Solver
 *  by Karl Michael Westerberg
 *  Created: 2/6/90
 *  Version: $Revision: 1.41 $
 *  Version control file: $RCSfile: rel.h,v $
 *  Date last modified: $Date: 1998/02/05 15:59:24 $
 *  Last modified by: $Author: ballan $
 *
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

/** @file
 *  Relation module for the SLV solver.
 *  <pre>
 *  Contents:     Relation module (ascend)
 *
 *  Authors:      Karl Westerberg
 *                Joseph Zaher
 *
 *  Dates:        06/90 - original version
 *                04/94 - added rel_apply_filter() which uses the new
 *                        rel_filter_t data structure for perfoming all
 *                        relation filtering needs (eliminating the
 *                        filter module)
 *
 *  Description:  This is the ascend version of the rel module.  This
 *                version should be used by any user who receives his/her
 *                equations directly from an instance tree created by the
 *                ASCEND compiler.
 *
 *  Requires:     #include "utilities/ascConfig.h"
 *                #include "var.h"
 *
 *  SERVER ONLY:  #include "expr.h"
 *                #include "types.h"
 *                #include "extfunc.h"
 *                #include "relation.h"
 *                #include "packages.h"
 *                #include "extcall.h"
 *                #include "mtx.h"
 *  </pre>
 */

#ifndef ASC_REL_H
#define ASC_REL_H

/*
 * rel_relation data type and basic type operators.
 */

#define rel_TOK_less    0x1
#define rel_TOK_equal   0x2
#define rel_TOK_greater 0x4

struct rel_extnode {
  int32 whichvar;
  struct ExtRelCache *cache;  /**< pointer to private structure */
};
  
/** The exact operator of a relation. */
enum rel_enum {
  e_rel_minimize,
  e_rel_maximize,
  e_rel_equal,
  e_rel_not_equal,
  e_rel_less,
  e_rel_greater,
  e_rel_lesseq,
  e_rel_greatereq
};

/** Backend relation types. */
enum backend_enum {
  e_rel_token,
  e_rel_glassbox,
  e_rel_blackbox
};

/**
 *  Relation data structure.
 *  If you mess with this struct, change the defaults for it in .c file.
 */
struct rel_relation {
   SlvBackendToken instance;
   struct rel_extnode *nodeinfo;  /**< Not NULL if blackbox relation */
   struct var_variable **incidence;
                        /**< array of atomic vars and nonvars in rel */
/*
 * For future use:
 *
 * struct var_variable **incidentals; array of nonvars,
 * once solveratoms arrive.
 *
 * int32 n_incidentals;
 */
   enum backend_enum type;  /**< tokens, glassbox or blackbox */
   int32 n_incidences;      /**< length of incidence. */
   int32 mindex;            /**< index in the slv_system_t master list */
   int32 sindex;            /**< index in the slv_system_t solver list */
   int32 model;             /**< index of a hypothetical MODEL rel is from */
   uint32 flags;            /**< flags */
};

extern struct rel_relation *rel_create(SlvBackendToken instance,
                                       struct rel_relation *rel);
/**<
 *  <!--  rel_create(instance,rel)                                     -->
 *  <!--  rel = rel_create(instance,NULL)                              -->
 *  <!--  struct rel_relation *rel;                                    -->
 *  <!--  SlvBackendToken instance;                                    -->
 *
 *  Creates a relation given the relation instance.
 *  If the rel supplied is NULL, we allocate the memory for the
 *  rel we return, else we just init the memory you hand us and
 *  return it to you.<br><br>
 *  We set the fields instance, nodeinfo, type following
 *  the instance. Setting the rest of the information is the job
 *  of the bridge building function between the ascend instance
 *  tree (or other relation back end) and the slv_system_t.
 *  In particular, the incidence list and indexing info is not
 *  handled here.
 */

extern void rel_destroy(struct rel_relation *rel);
/**<
 *  <!--  rel_destroy(rel)                                             -->
 *  <!--  struct rel_relation *rel;                                    -->
 *
 *  Destroys a relation.
 */

extern void rel_write_name(slv_system_t sys, struct rel_relation *rel, FILE *file);
/**<
 *  <!--  rel_write_name(sys,rel,file);                                -->
 *  Writes a name to the file given. Handles NULL inputs gracefully.
 *  Does not print any whitespace, including carriage returns.
 *  Is faster than slv_print_var_name.
 *  If sys is NULL, writes full ascend name. If file or rel is NULL
 *  does not write.
 */

/** Relation filter structure */
typedef struct rel_filter_structure {
  uint32 matchbits;   /**< Bits to match. */
  uint32 matchvalue;  /**< Value to match. */
} rel_filter_t;
/**< Relation filter type */

extern SlvBackendToken rel_instance(struct rel_relation *rel);
/**<
 *  <!--  rel_instance(rel)                                            -->
 *  <!--  struct rel_relation *rel;                                    -->
 *
 *  Returns the instance pointer from a rel.
 */

extern void rel_set_extnodeinfo(struct rel_relation *rel, 
                                struct rel_extnode *nodeinfo);
/**< Sets the external node information structure for a relation. */
extern struct rel_extnode *rel_extnodeinfo(struct rel_relation *rel);
/**<
 *  <!--  struct rel_relation *rel;                                    -->
 *  <!--  struct rel_extnode *nodeinfo;                                -->
 *
 *  <!--  struct rel_extnode *nodeinfo = rel_extnodeinfo(rel);         -->
 *  <!--  rel_set_extnodeinfo(rel,nodeinfo);                           -->
 *  Fetches the pointer to the external node information structure for
 *  a relation. If this is NULL, which will be the case for most
 *  relations, then there are no external call nodes present.
 */

extern void rel_set_extwhichvar(struct rel_relation *rel, int whichvar);
/**< Sets the argument list index for a relation having external nodes. */
extern int32 rel_extwhichvar(struct rel_relation *rel);
/**<
 *  <!--  struct rel_relation *rel;                                    -->
 *  <!--  int32 whichvar;                                              -->
 *
 *  Returns the index into the argument list from which rel was
 *  constructed.  This applies ONLY to rels that have external nodes !
 *  Relations that have external nodes have associated with them an
 *  index into the argument list from which the relation was constructed.
 *  rel_whichvar returns that index. e.g. if when this relation was
 *  constructed from an external procedure call, the number of output
 *  variables was 4, and the number of inputs was 6, valid results from
 *  this rel_whichvar would be 7, 8, 9, 10.
 *  A return value <= 0 is an error.
 */

extern boolean rel_less(struct rel_relation *rel);
/**<
 *  Returns true if the given relation is satisfied if "less than"
 *  is among those that make up the comparator of the relation
 *  (i.e. <>, <, or <=).<br><br>
 *  le==TRUE implies rel would be satisfied if lhs < rhs
 */
extern boolean rel_equal(struct rel_relation *rel);
/**<
 *  Returns true if the given relation is satisfied if "equals"
 *  is among those that make up the comparator of the relation
 *  (i.e. =, >=, or <=).<br><br>
 *  eq==TRUE implies rel would be satisfied if lhs ~ rhs
 */
extern boolean rel_greater(struct rel_relation *rel);
/**<
 *  Returns true if the given relation is satisfied if "greater than"
 *  is among those that make up the comparator of the relation
 *  (i.e. <>, >, or >=).<br><br>
 *  gr==TRUE implies rel would be satisfied if lhs > rhs
 */
/* OLD GROUP COMMENT */
/*
 *  le = rel_less(rel)
 *  eq = rel_equal(rel)
 *  gr = rel_greater(rel)
 *  boolean le,eq,gr;
 *  struct rel_relation *rel;
 *
 *  Returns true if the given relation is satisfied if the
 *  operator in question is among those that make up the
 *  comparator of the relation. (e.g. rel_less is satisfied
 *  by (<>,<.<=).
 *  le==TRUE implies rel would be satisfied if lhs < rhs
 *  gr==TRUE implies rel would be satisfied if lhs > rhs
 *  eq==TRUE implies rel would be satisfied if lhs ~ rhs
 */

extern enum rel_enum rel_relop(struct rel_relation *rel);
/**<
 * <!--  relop = rel_relop(rel);                                       -->
 * <!--  rel_enum relop;                                               -->
 * <!--  struct rel_relation *rel;                                     -->
 * Returns the type of the relational operator of a given relation.
 */

extern char *rel_make_name(slv_system_t sys, struct rel_relation *rel);
/**<
 *  <!--  name = rel_make_name(sys,rel)                                -->
 *  <!--  slv_system_t sys;                                            -->
 *  <!--  struct rel_relation *rel;                                    -->
 *  <!--  char *name;                                                  -->
 *
 *  Copies of the relation instance name can be made and returned.
 *  The string returned should be freed when no longer in use.
 */

extern int32 rel_mindex(struct rel_relation *rel);
/**<
 *  Retrieves the index number of the given relation as it
 *  appears in a slv_system_t master relation list.
 */
extern void rel_set_mindex(struct rel_relation *rel, int32 index);
/**<
 *  <!--  index = rel_mindex(rel)                                      -->
 *  <!--  rel_set_mindex(rel,index)                                    -->
 *  <!--  int32 index;                                                 -->
 *  <!--  struct rel_relation *rel;                                    -->
 *
 *  Sets the index number of the given relation as it
 *  appears in a slv_system_t master relation list.
 */

extern int32 rel_sindex(const struct rel_relation *rel);
/**<
 *  Retrieves the index number of the given relation as it
 *  appears in a solvers relation list. The index is most often used
 *  to assign the relation to a specific original row of a matrix.
 */
extern void rel_set_sindex(struct rel_relation *rel, int32 index);
/**<
 *  <!--  index = rel_sindex(rel)                                      -->
 *  <!--  rel_set_sindex(rel,index)                                    -->
 *  <!--  int32 index;                                                 -->
 *  <!--  struct rel_relation *rel;                                    -->
 *
 *  Sets the index number of the given relation as it
 *  appears in a solvers relation list. The index is most often used
 *  to assign the relation to a specific original row of a matrix.
 */

extern int32 rel_model(const struct rel_relation *rel);
/**<
 *  Retrieves the model number of the given relation.
 *  In a hierarchy, relations come in groups associated with
 *  models. Models are numbered from 1 to some upper limit.
 */
extern void rel_set_model(struct rel_relation *rel, int32 index);
/**<
 *  <!--  index = rel_model(rel)                                       -->
 *  <!--  rel_set_model(rel,index) //SERVER ONLY                       -->
 *  <!--  int32 index;                                                 -->
 *  <!--  struct rel_relation *rel;                                    -->
 *
 *  Sets the model number of the given relation.
 *  In a hierarchy, relations come in groups associated with
 *  models. Models are numbered from 1 to some upper limit.
 */

extern real64 rel_residual(struct rel_relation *rel);
/**<
 *  Retrieves the residual field of the given relation.
 *  Note that the residual is not actually computed by rel_residual:
 *  there is no guarantee (from this function) that the residual is
 *  actually correct.
 */
extern void rel_set_residual(struct rel_relation *rel, real64 residual);
/**<
 *  <!--  residual = rel_residual(rel)                                 -->
 *  <!--  rel_set_residual(rel,residual)                               -->
 *  <!--  real64 residual;                                             -->
 *  <!--  struct rel_relation *rel;                                    -->
 *
 *  Sets the residual field of the given relation.
 */

extern real64 rel_nominal(struct rel_relation *rel);
/**<
 *  <!--  nominal = rel_nominal(rel)                                   -->
 *  <!--  real64 nominal;                                              -->
 *  <!--  struct rel_relation *rel;                                    -->
 *
 *  Retrieves the nominal field of the given relation.
 *  No slv client has any business being able to set the nominal,
 *  so no such operator is provided.
 */

extern void rel_set_nominal(struct rel_relation *rel, real64 nominal);
/**<
 *  Breaking the 'rule' of rel_nominal() for the time being.
 *  @todo Remove rel_set_nominal() or change "rule" stated by rel_nominal().
 */

#ifdef NDEBUG
#define rel_n_incidences(rel) ((rel)->n_incidences)
#else
#define rel_n_incidences(rel) rel_n_incidencesF(rel)
#endif 
/**<
 *  Returns the length of the incidence_list for a relation.
 *  Not everything in the incidence list is necessarily a
 *  variable for your particular solver -- check the flags.
 *  @param rel struct rel_relation *, the relation to query.
 *  @return Returns the length as an int32.
 *  @see rel_n_incidencesF()
 */

#ifdef NDEBUG
#define rel_set_incidences(rel,n,ilist) \
  (rel)->n_incidences=(n); (rel)->incidence = (ilist)
#else
#define rel_set_incidences(rel,n,ilist) rel_set_incidencesF((rel),(n),(ilist))
#endif 
/**<
 *  Sets the length and incidence_list for a relation.
 *  Not everything in the incidence list is necessarily a
 *  variable for your particular solver -- check the flags.
 *  Solver clients should not call rel_set_incidences,
 *  it is only for use by constructors of bridges to relation
 *  back ends.
 *  @param rel   struct rel_relation *, the relation to modify.
 *  @param n     int32
 *  @param ilist struct var_variable **, the incidence list
 *  @return No return value.
 *  @see rel_set_incidencesF()
 */

extern int32 rel_n_incidencesF(struct rel_relation *rel);
/**<
 *  Implementation function for rel_n_incidences().  Do not call
 *  this function directly - use rel_n_incidences() instead.
 */
extern void rel_set_incidencesF(struct rel_relation *rel,
                                int32 n,
                                struct var_variable **ilist);
/**<
 *  Implementation function for rel_set_incidences().  Do not call
 *  this function directly - use rel_set_incidences() instead.
 */
/* OLD GROUP COMMENTS */
/*
 *  rel_n_incidences(rel)                                        
 *  rel_set_incidences(rel,n,ilist) //SERVER ONLY                
 *  struct rel_relation *rel;                                    
 *  struct var_variable **ilist;                                 
 *  int32 n;
 *
 *  rel_n_incidences returns the length of the incidence_list.
 *  Not everything in the incidence list is necessarily a
 *  variable for your particular solver -- check the flags.
 *  Solver clients should not call rel_set_incidences,
 *  it is only for use by constructors of bridges to relation
 *  back ends.
 */

extern struct var_variable
**rel_incidence_list_to_modify(struct rel_relation *rel);
/**<
 *  Returns a non-const pointer to an array rel_n_incidences(rel) 
 *  long of vars.
 *  @see rel_incidence_list().
 */
extern const struct var_variable **rel_incidence_list(struct rel_relation *rel);
/**<
 *  <!--  va = rel_incidence_list(rel)                                 -->
 *  <!--  struct rel_relation *rel;                                    -->
 *  <!--  struct var_variable **va;                                    -->
 *
 *  Returns a pointer to an array rel_n_incidences(rel) long of vars.
 *  Each element of the array is a struct var_variable *.
 *  Check the var sindex to see where each might go in a jacobian.
 *  If there is no incidence, NULL is returned.
 *  Pointers in this array will be unique.<br><br>
 *  The list belongs to the relation. Do not destroy it. Do not change it.<br><br>
 *
 *  The return value IS NOT A NULL-TERMINATED LIST.
 */

/*
 * relation filtration functions.
 * We have a lot (32) of binary (one bit) flags a client may want to query
 * in arbitrary combinations and paying attention to only certain of
 * the bits. We will provide a set of macros and functions for each of
 * these bits and for operations on the whole set.
 */

extern int32 rel_apply_filter(const struct rel_relation *rel,
                              rel_filter_t *filter);
/**< 
 *  <!--  value = rel_apply_filter(rel,filter)                         -->
 *  <!--  int32 value;                                                 -->
 *  <!--  struct rel_relation *rel;                                    -->
 *  <!--  rel_filter_t *filter;                                        -->
 *
 *  Returns 1 only if all of the positions specified in
 *  filter->matchbits have the same values in
 *  filter->matchvalue and the relation's flags value.
 *  Bits set to 0 in filter->matchbits are ignored for the test.<br><br>
 *
 *  EXAMPLE:
 *  To find out if a relation is an unsatisfied equality:
 *    We don't care about anything except satisfaction and equality,
 *    so set filter->matchbits = (REL_SATISFIED | REL_EQUALITY);
 *    We want relation NOT satisfied, so leave REL_SATISFIED = 0
 *    in filter->matchvalue, i.e. filter->matchvalue = REL_EQUALITY;
 *  Usually it is most convenient to just declare a rel_filter_t filt;
 *  locally in a function and then take the address of it (&filt) after
 *  setting filt.matchbits = (| of all your interesting bits) and
 *  setting filt.matchvalue = (| of the bits you want to be TRUE).
 */
  
extern uint32 rel_flags(struct rel_relation *rel);
/**<
 * Returns the flags field of the relation.
 */
extern void rel_set_flags(struct rel_relation *rel, uint32 flags);
/**<
 *  <!--  struct rel_relation *rel;                                    -->
 *  <!--  uint32 flags;                                                -->
 *
 *  <!--  rel_flags(rel) returns the flags field of the relation.      -->
 *  Sets the entire flag field to the value of flags given.
 */

extern uint32 rel_flagbit(struct rel_relation *rel, uint32 name);
/**<
 *  <!--  rel_flagbit(rel,name);                                       -->
 *  <!--  struct rel_relation *rel;                                    -->
 *  <!--  uint32 name;		                                               -->
 *  Returns the value of the bit specified from the relation flags.
 *  name should be a REL_xx flag defined above)
 */

extern void rel_set_flagbit(struct rel_relation *rel,
                            uint32 NAME, uint32 oneorzero);
/**<
 *  <!--  struct rel_relation *rel;                                    -->
 *  <!--  uint32 NAME,oneorzero;                                       -->
 *  <!--  rel_set_flagbit(rel,NAME,oneorzero)                          -->
 *
 *  Sets the bit, which should be referred to by its macro name,
 *  on if oneorzero is >0 and off is oneorzero is 0.
 *  The macro names are the defined up at the top of this file.
 *  <pre>
 *  Example:
 *    rel_set_flags(rel,REL_PARTITION,1) turns on the REL_PARTITION bit.
 *
 *    What it really does is:
 *      if (oneorzero) {
 *        rel->flags |= field;
 *      } else {
 *        rel->flags &= ~field;
 *      }
 *  </pre>
 *  In unix, see also man 3f bit or man not.
 */

/*
 *  the bit flags. explained afterward. several are for use of
 *  transient clients and should be ignored by solver engines
 */
#define REL_PARTITION 0x1
/**< Is it in the interesting region? Reordering clients. */
#define REL_TORN      0x2
/**<  Is it a tear? Reordering clients output. */
#define REL_INTERFACE 0x4
/**< User suggests it's a tear eqn. Solvers, ui clients. */
#define REL_INCLUDED  0x8
/**<
 *  User wants eqn in problem.  Solvers, ui clients.
 *  Use rel_set_* to change.
 *  INCLUDED is as yet a funny one. Treat it as readonly because
 *  you can only change it using a real function and not the
 *  bit manipulation functions. It is here in the bits because
 *  it is useful in filters sometimes.
 */
#define REL_OBJNEGATE 0x10
/**<  Read_only for clients.  Rel module. */
#define REL_BLACKBOX  0x20
/**<  Read_only for clients.  Rel module. */
#define REL_SATISFIED 0x40
/**< 
 *  Has rel been pronounced satisfied by someone?
 *  Bit should be treated as readonly. use rel_set_* to change.
 */
#define REL_EQUALITY  0x80
/**< Is relation an equality?  Readonly for clients. */
#define REL_INBLOCK   0x100
/**< Is the relation in the current block of registered client?  For clients. */
/* Conditional Modeling */
#define REL_INWHEN      0x200
/**< Is relation in a when?  Readonly for clients. */
#define REL_ACTIVE      0x400
/**< Is this relation currently a part of my problem? */
#define REL_INVARIANT   0x800
/**< Is this relation an invariant in the conditional modeling analysis? */
/* Conditional Relations (Boundaries) */
#define REL_CONDITIONAL       0x1000
/**< Is relation conditional?  Readonly for clients. */
#define REL_IN_CUR_SUBREGION  0x2000
/**< Is the relation in the subregion currently analyzed?  */
#define REL_GENERATED 0x10000
/**<
 *  Temporary relation that doesn't exist independently in the backend,
 *  but is made by some process of the backend or the solver client.
 *  Is rel fake and cooked up for this system only? */

/* OLD GROUP COMMENTS */
/*
 * REL_PARTITION	reordering clients. is it in the interesting region
 * REL_TORN	        reordering clients output. is it a tear.
 * REL_INTERFACE	solvers, ui clients. user suggests it's a tear eqn.
 * REL_INCLUDED 	solvers, ui clients. user wants eqn in problem.
 *	        	bit should be treated as readonly. use rel_set_*
 *	        	to change.
 * REL_OBJNEGATE	rel module. read_only for clients.
 * REL_BLACKBOX	        rel module. read_only for clients.
 * REL_SATISFIED	has rel been pronounced satisfied by someone?
 *       		bit should be treated as readonly. use rel_set_*
 *        		to change.
 * REL_EQUALITY 	is relation an equality? readonly for clients.
 * REL_INBLOCK  	is the relation in the current block of registered
 *	        	client? for clients.
 * REL_INWHEN   	is relation in a when? readonly for clients.
 * REL_ACTIVE           is this relation currently a part of my problem ?
 * REL_INVARIANT        is this relation an invariant in the conditional
 *                       modeling analysis
 * REL_CONDITIONAL	is relation conditional? readonly for clients
 * REL_IN_CUR_SUBREGION is the relation in the subregion currently
 *                      analyzed ?
 * REL_GENERATED	is rel fake and cooked up for this system only?
 */

/*
 * the bit flag lookups
 */
#ifdef NDEBUG
#define rel_partition(rel)        ((rel)->flags & REL_PARTITION)
#define rel_torn(rel)             ((rel)->flags & REL_TORN)
#define rel_interface(rel)        ((rel)->flags & REL_INTERFACE)
#define rel_obj_negate(rel)       ((rel)->flags & REL_OBJNEGATE)
/**<
 *  Returns TRUE if relation is of type e_maximize.
 *  Returns FALSE if relation is of type e_minimize.
 *  Note: should only be used on objectives. other relations
 *  will give a meaningless result (probably FALSE).
 */
#define rel_blackbox(rel)         ((rel)->flags & REL_BLACKBOX)
#define rel_satisfied(rel)        ((rel)->flags & REL_SATISFIED)
/**< Retrieves the satisfied field of the given relation.
 * See rel_residual() for disclaimer. */
#define rel_equality(rel)         ((rel)->flags & REL_EQUALITY)
/**<  TRUE if relation is of form lhs = rhs. */
#define rel_in_block(rel)         ((rel)->flags & REL_INBLOCK)
/**< Retrieves the INBLOCK member of the given relation flags
 *  which determines if the relation is within the current block. */
#define rel_in_when(rel)          ((rel)->flags & REL_INWHEN)
#define rel_active(rel)           ((rel)->flags & REL_ACTIVE)
#define rel_invariant(rel)        ((rel)->flags & REL_INVARIANT)
#define rel_conditional(rel)      ((rel)->flags & REL_CONDITIONAL)
#define rel_in_cur_subregion(rel) ((rel)->flags & REL_IN_CUR_SUBREGION)
#define rel_generated(rel)        ((rel)->flags & REL_GENERATED)
#else
#define rel_partition(rel)        rel_flagbit((rel),REL_PARTITION)
#define rel_torn(rel)             rel_flagbit((rel),REL_TORN)
#define rel_interface(rel)        rel_flagbit((rel),REL_INTERFACE)
#define rel_obj_negate(rel)       rel_flagbit((rel),REL_OBJNEGATE)
/**<
 *  Returns TRUE if relation is of type e_maximize.
 *  Returns FALSE if relation is of type e_minimize.
 *  Note: should only be used on objectives. other relations
 *  will give a meaningless result (probably FALSE).
 */
#define rel_blackbox(rel)         rel_flagbit((rel),REL_BLACKBOX)
#define rel_satisfied(rel)        rel_flagbit((rel),REL_SATISFIED)
/**< Retrieves the satisfied field of the given relation.
 * See rel_residual() for disclaimer. */
#define rel_equality(rel)         rel_flagbit((rel),REL_EQUALITY)
/**<  TRUE if relation is of form lhs = rhs. */
#define rel_in_block(rel)         rel_flagbit((rel),REL_INBLOCK)
/**< Retrieves the INBLOCK member of the given relation flags
 *  which determines if the relation is within the current block. */
#define rel_in_when(rel)          rel_flagbit((rel),REL_INWHEN)
#define rel_active(rel)           rel_flagbit((rel),REL_ACTIVE)
#define rel_invariant(rel)        rel_flagbit((rel),REL_INVARIANT)
#define rel_conditional(rel)      rel_flagbit((rel),REL_CONDITIONAL)
#define rel_in_cur_subregion(rel) rel_flagbit((rel),REL_IN_CUR_SUBREGION)
#define rel_generated(rel)        rel_flagbit((rel),REL_GENERATED)
#endif /* NDEBUG */

/*
 * bit flag assignments. any value other than 0 for bv turns the
 * named flag to 1. 0 sets it to 0.
 * Many of these bits have read-only semantics for clients.
 * Take care when setting them.
 */
#define rel_set_partition(rel,bitval)   rel_set_flagbit((rel),REL_PARTITION,(bitval))
#define rel_set_torn(rel,bitval)        rel_set_flagbit((rel),REL_TORN,(bitval))
#define rel_set_interface(rel,bitval)   rel_set_flagbit((rel),REL_INTERFACE,(bitval))
#define rel_set_obj_negate(rel,bitval)  rel_set_flagbit((rel),REL_OBJNEGATE,(bitval))
#define rel_set_blackbox(rel,bitval)    rel_set_flagbit((rel),REL_BLACKBOX,(bitval))
#define rel_set_satisfied(rel,bitval)   rel_set_flagbit((rel),REL_SATISFIED,(bitval))
#define rel_set_equality(rel,bitval)    rel_set_flagbit((rel),REL_EQUALITY,(bitval))
#define rel_set_in_block(rel,bitval)    rel_set_flagbit((rel),REL_INBLOCK,(bitval))
#define rel_set_in_when(rel,bitval)     rel_set_flagbit((rel),REL_INWHEN,(bitval))
#define rel_set_active(rel,bitval)      rel_set_flagbit((rel),REL_ACTIVE,(bitval))
#define rel_set_invariant(rel,bitval)   rel_set_flagbit((rel),REL_INVARIANT,(bitval))
#define rel_set_conditional(rel,bitval)  \
        rel_set_flagbit((rel),REL_CONDITIONAL,(bitval))
#define rel_set_in_cur_subregion(rel,bitval)  \
        rel_set_flagbit((rel),REL_IN_CUR_SUBREGION,(bitval))
#define rel_set_generated(rel,bitval)		rel_set_flagbit((rel),REL_GENERATED,(bitval))

extern uint32 rel_included(struct rel_relation *rel);
/**<
 *  Retrieves the included field of the given relation.
 *  @todo This has side effects on the ascend instance, so it isn't
 *        implemented with the rest of the macros above. This needs to
 *        change.
 */
extern void rel_set_included(struct rel_relation *rel, uint32 included);
/**<
 *  <!--  included = rel_included(rel)                                 -->
 *  <!--  rel_set_included(rel,included)                               -->
 *  <!--  uint32 included;                                             -->
 *  <!--  struct rel_relation *rel;                                    -->
 *
 *  Sets the included field of the given relation.
 *  <!--  This has side effect on the ascend instance, so it isn't     -->
 *  <!--  implemented with the rest of the macros above. This needs to -->
 *  <!--  change.                                                      -->
 */

/*
 *  in_block = rel_in_block(rel)
 *  rel_set_in_block(rel,in_block)
 *  uint32 in_block;
 *  struct rel_relation *rel;
 *
 *  Sets or retrieves the INBLOCK member of the given relation flags
 *  which determines if the relation is within the current block.
 */

/*
 *   rel_obj_negate(rel)
 *   struct rel_relation *rel;
 *
 *   Returns TRUE if relation is of type e_maximize.
 *   Returns FALSE if relation is of type e_minimize.
 *   Note: should only be used on objectives. other relations
 *   will give a meaningless result (probably FALSE).
 */

/*
 *  satisfied = rel_satisfied(rel)
 *  rel_set_satisfied(rel,satisfied)
 *  uint32 satisfied;
 *  struct rel_relation *rel;
 *
 *  Sets or retrieves the satisfied field of the given relation (see
 *  rel_residual() for disclaimer).
 */

/*
 *  equality = rel_equality(rel)
 *  rel_set_equality(rel,equality)
 *  uint32 equality;
 *  struct rel_relation *rel;
 *
 *  TRUE if relation is of form lhs = rhs.
 */

extern real64 rel_multiplier(struct rel_relation *rel);
/**<
 *  Retrieves the multiplier field of the given relation.
 *  This is expected to be computed as a lagrange multiplier which will
 *  relate the gradient of the relation rel with that of some objective
 *  function.
 */
extern void rel_set_multiplier(struct rel_relation *rel, real64 multiplier);
/**<
 *  (<!--  won a temporary reprieve. this should be a system property, not rel.) -->
 *  <!--  multiplier = rel_multiplier(rel)                             -->
 *  <!--  rel_set_multiplier(rel,multiplier)                           -->
 *  <!--  real64 multiplier;                                           -->
 *  <!--  struct rel_relation *rel;                                    -->
 *
 *  Sets the multiplier field of the given relation.
 *  This is expected to be computed as a lagrange multiplier which will
 *  relate the gradient of the relation rel with that of some objective
 *  function.
 */

/*
 *  rel_relation utility functions.
 *
 *  Things for the server side only. Not visible to clients.
 *
 *  Ok, really nosy clients courting death can cheat. Don't cry when
 *  they break.
 *  We make absolutely no commitment to being compatible with this portion
 *  of the header at any time in the future.
 */
#ifdef _SLV_SERVER_C_SEEN_
/*
 *  requires #include "expr.h"
 *  requires #include "types.h"
 *  requires #include "extfunc.h"
 *  requires #include "relation.h"
 *  requires #include "packages.h"
 *  requires #include "extcall.h"
 *  requires #include "mtx.h"
 */

extern double g_external_tolerance; /**< DEFAULT 1e-12 */

struct ExtRelCache {
  int32 nodestamp;
  struct ExternalFunc *efunc;       /**< pre_slv, eval and deriv funcs */
  SlvBackendToken data;             /**< only passed on pre_slv */
  struct gl_list_t *arglist;        /**< only passed on pre_slv */
  struct gl_list_t *inputlist;
  void *user_data;                  /**< user data */
  int32 ninputs, noutputs;
  double *inputs;
  double *outputs;
  double *jacobian;
  unsigned newcalc_done     :1;     /**< bits needed to control */
  unsigned first_func_eval  :1;     /**< recalculation. until we can */
  unsigned first_deriv_eval :1;     /**< do proper block coding */
};

extern struct ExtRelCache *rel_extcache(struct rel_relation *rel);
/**<
 *  Retrieve external relation information.
 *  This applies ONLY to rels that have external nodes !
 *  Ensure this by calling rel_extnodeinfo FIRST !
 *  This is the gateway to the external relation information
 *  stashed away to make processing of external relations efficient.
 *  See the file extrel.[ch] for functions to deal with the external
 *  relations cache, unless of course you are a client in which CASE
 *  don't.
 */
extern void rel_set_extcache(struct rel_relation *rel, struct ExtRelCache *cache);
/**<
 *  cache = rel_extcache(rel);
 *  rel_set_extcache(rel,cache);
 *  struct rel_relation *rel;
 *  struct ExtRelCache *cache;
 *
 *  Set external relation information.
 *  This applies ONLY to rels that have external nodes !
 *  Ensure this by calling rel_extnodeinfo FIRST !
 *  This is the gateway to the external relation information
 *  stashed away to make processing of external relations efficient.
 *  See the file extrel.[ch] for functions to deal with the external
 *  relations cache, unless of course you are a client in which CASE
 *  don't.
 */

/*
 * The following aren't commented because Kirk Abbott didn't comment them.
 * It's all server magic anyway? 
 */

extern struct ExtRelCache *CreateExtRelCache(struct ExtCallNode *ext);
struct ExtRelCache *CreateCacheFromInstance(SlvBackendToken relinst);
extern void ExtRel_DestroyCache(struct ExtRelCache *cache);
extern int32 ExtRel_PreSolve(struct ExtRelCache *cache, int32 setup);
extern real64 ExtRel_Evaluate_RHS(struct rel_relation *rel);
extern real64 ExtRel_Evaluate_LHS(struct rel_relation *rel);
extern real64 ExtRel_Diffs_RHS(struct rel_relation *rel, 
                               var_filter_t *filter,
                               int32 row, 
                               mtx_matrix_t matrix);
extern real64 ExtRel_Diffs_LHS(struct rel_relation *rel, 
                               var_filter_t *filter,
                               int32 row, 
                               mtx_matrix_t matrix); /**< not implemented */

#endif /* _SLV_SERVER_C_SEEN_ */

/*
 * Things dead.
 */
/*  DEFUNCT
 *  extern struct rel_relation *rel_objcreate(SlvBackendToken, boolean);
 *  rel = rel_objcreate(instance,negate)
 *  struct rel_relation *rel;
 *  boolean negate;
 *  SlvBackendToken instance;
 *
 *  Creates an objective relation given the relation instance.
 *  The other fields are given default values. The lhs will be NULL.
 *  objrels may not have extrel caches. Use a dummy relation if
 *  necessary. Negate must be TRUE for a maximize relation, FALSE for
 *  a minimize relation.
 */

/*  DEFUNCT
 * If you're smart enough to understand the answer to this question,
 * then you should be asking the instance directly, not asking us.
 * extern enum rel_enum rel_type(struct rel_relation *);
 * rtype = rel_type(rel);
 * rel_enum rtype;
 * struct rel_relation *rel;
 * Returns the type of a given relation. This indicates whether the
 * the relation is a token, opcode, glassbox, or blackbox relation.
 */

/*   DEFUNCT for now.
 * extern boolean rel_in_subregion(struct rel_relation *);
 *  in_subregion = rel_in_subregion(rel)
 *  boolean in_subregion;
 *  struct rel_relation *rel;
 *
 *  Computes if the conditions of the relation are all met by the
 *  current values of the subregion field of each relevant boundary.
 *  It will return TRUE even if there are no conditions.
 */

/* DEFUNCT the solver has no business knowing about sides. this is the
 *  compilers job. temporarily still active.
 * extern expr_t rel_lhs(struct rel_relation *);
 * extern expr_t rel_rhs(struct rel_relation *);
 *  lhs = rel_lhs(rel)
 *  rhs = rel_rhs(rel)
 *  expr_t lhs,rhs;
 *  struct rel_relation *rel;
 *
 *  Fetches the (internal copy of the) rhs and lhs of the relation.
 *  The user should not modify the expression, nor should the user
 *  use the expression after the relation is destroyed or the
 *  lhs/rhs has been re-set.
 */

#endif /* ASC_REL_H  */
