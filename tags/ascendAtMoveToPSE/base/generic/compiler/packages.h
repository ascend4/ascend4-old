/*
 *  User Packages
 *  by Ben Allan & Kirk Abbott
 *  Created: July 4, 1994
 *  Version: $Revision: 1.12 $
 *  Version control file: $RCSfile: packages.h,v $
 *  Date last modified: $Date: 1998/05/18 16:36:45 $
 *  Last modified by: $Author: ballan $
 *  This file is part of the Ascend Language Interpreter.
 *
 *  Copyright (C) 1990, 1993, 1994 Thomas Guthrie Epperly, Kirk Abbott.
 *  Copyright (C) 1997 Benjamin Allan
 *
 *  The Ascend Language Interpreter is free software; you can redistribute
 *  it and/or modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  The Ascend Language Interpreter is distributed in hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with the program; if not, write to the Free Software Foundation,
 *  Inc., 675 Mass Ave, Cambridge, MA 02139 USA.  Check the file named
 *  COPYING.
 *
 */

/** @file
 *  User package management.
 *  <pre>
 *  When #including packages.h, make sure these files are #included first:
 *         #include <stdio.h>
 *         #include "utilities/ascConfig.h"
 *         #include "compiler.h"
 *         #include "instance_enum.h"
 *         #include "list.h"
 *         #include "ascMalloc.h"
 *         #include "extfunc.h" (going away soon)
 *         #include "type_desc.h" (going away soon)
 *         #include "module.h" (going away soon)
 *         #include "library.h" (going away soon)
 *         #include "relation.h" (going away soon)
 *  </pre>
 *  Users of this header should not have to include anything
 *  except instance_enum.h and list.h and whatever headers those
 *  may require.
 *
 *  @todo Comments about header dependencies are contradictory.
 */

#ifndef __PACKAGES_H_SEEN__
#define __PACKAGES_H_SEEN__

#ifndef _ASCCLIENTDATA
#   ifdef __STDC__
    typedef void *ascClientData;
#   else
    typedef int *ascClientData;
#   endif /* __STDC__ */
#define _ASCCLIENTDATA
#endif

/** these are for external clients to return to us when they have
 * an error or other procedural message to report about flow of
 * control.
 */
#define ASC_OK          0
#define ASC_ERROR       1
#define ASC_RETURN      2
#define ASC_BREAK       3
#define ASC_CONTINUE    4

typedef int (Asc_MethodProc) (ascClientData, CONST char *,
                              struct Instance *, struct gl_list_t *);

typedef int (Asc_MethodDelete) (ascClientData, CONST char *, Asc_MethodProc);

/** Clients should use the operators below to access this enum and
 * associated structure.
 */
enum argItemEnum {
  argError,         /**< should never be seen by client */
  /* the first 3 characters of the comment lines are the
   * equivalent of C conversion characters in printf when
   * specifying the arguments desired in the parsestring.
   */
  argOldFileId,     /**< OF  name of an existing file (char *) */
  argNewFileId,     /**< NF  name of new file (char *) */
  argUnkFileId,     /**< UF  name of file, unknown status (char *) */
  argFloat,         /**<  f  maps ASCEND integer/real types to a float value */
  argDouble,        /**<  g  maps ASCEND integer/real types to a double value */
  argLong,          /**<  ld maps ASCEND integer/boolean to long value. */
  argDoublePtr,     /**< pg  maps ASCEND reals to a double * */
  argLongPtr,       /**< pld maps ASCEND integers to long * */
  argInt,           /**<  d  maps ASCEND integer/boolean to int value. */
  argBoolean,       /**<  b  maps ASCEND boolean or integer to 0,1 value */
  argString,        /**<  s  maps ASCEND symbol types to a C const char * */
  argSetString,     /**<  ss maps ASCEND set types into a C const char * */
  argNameString,    /**<  n  maps ASCEND Name given into a C const char * */
  argNameList,      /**<  N  expand ASCEND Name given into a C const char * */
  argTypeString,    /**<  T  maps ASCEND type name given into a C const char * */
  argInst,          /**<  i  instance pointer may be any kind */
  argInstReal,      /**<  R  instance pointer must be REAL*INST */
  argInstRealList,  /**< LR  flattens array of real insts into a gl_list */
  argInstRealArray, /**< AR  instance pointer must be an array of reals */
  argDoubleConst,   /**< CR  pointer to new double value for unassigned const */
  argInstInt,       /**<  I  instance pointer must be INTEGER*INST */
  argInstIntList,   /**< LI  flattens array of integer insts into a gl_list */
  argInstIntArray,  /**< AI  instance pointer must be an array of integers */
  argIntConst,      /**< CI  pointer to new long value for unassigned const */
  argInstBool,      /**<  B  instance must be BOOLEAN_[ATOM/CONSTANT]_INST */
  argInstBoolList,  /**< LB  flattens array of boolean insts into a gl_list */
  argInstBoolArray, /**< AB  instance pointer must be an array of boolean */
  argBoolConst,     /**< CB  pointer to new 0/1 value for unassigned const */
  argInstSym,       /**<  S  instance must be SYMBOL_[ATOM/CONSTANT]_INST */
  argInstSymList,   /**< LS  flattens array of symbol insts into a gl_list */
  argInstSymArray,  /**< AS  instance pointer must be an array of symbols */
  argSymConst,      /**< CS  pointer to new char * value for unassigned const */
  argInstSet,       /**<  SS instance must be SET_[ATOM/CONSTANT]_INST */
  argInstSetList,   /**< LSS flattens array of set insts into a gl_list */
  argInstSetArray,  /**< ASS instance pointer must be an array of sets */
  argISetConst,     /**< CSS pointer to gl_list of longs for unassigned set */
  argESetConst,     /**< DSS pointer to gl_list of char * for unassigned set */
  argInstRel,       /**<  r  instance must be RELATION_INST */
  argInstRelList,   /**< Lr  flattens array of relation insts into a gl_list */
  argInstRelArray,  /**< Ar  instance pointer must be an array of relations */
  argInstLRel,      /**<  lr instance must be LOGREL_INST */
  argInstLRelList,  /**< Llr flattens array of logrelations into a gl_list */
  argInstLRelArray, /**< Alr instance pointer must be an array of logrel */
  argInstModel,     /**<  M  MODEL instance pointer */
  argInstModelList, /**< LM  flattens array of MODEL insts into a gl_list */
  argInstModelArray,/**< AM  flattens array of MODEL insts into a gl_list */
  argProcName,      /**<  P  maps name of a method/call to a CONST char * */
  argExpr,          /**<  V  maps a reference to an ASCEND Expr into lval */
  argSLit           /**<  Q  a C string literal is expected. */
  /* probably need to add type Q for double quoted text */
};

struct argItem {
  enum argItemEnum kind;
  int depth;    /**< For Array and list arguments, sets the number of
                 * subscripts expected. If 0, any number is allowed.
                 * Internal use only.
                 */
  union {
    double dval;        /**< double value */
    long lival;         /**< long int value */
    double dptr;        /**< double pointer */
    long liptr;         /**< long int pointer */
    int ival;           /**< int value */
    int bval;           /**< boolean value */
    CONST char *cval;   /**< const char * from an ascend set/symbol */
    CONST char *nval;   /**< string form of the ascend name given */
    CONST char *fval;   /**< file id string */
    CONST struct gl_list_t *lval; /**< list according to enum */
    struct Instance *i; /**< instance according to enum */
  } u;                  /**< union of possible values */
  short args;           /**< 0 usually. 1 if item is ... at the end. Internal use. */
  short exact;          /**< 0 usually. 1 if type specifier to be match. 2 if exactly */
  symchar *typename;    /**< typeidentifier from the symbol table */
};

#define Asc_argItemKind(aip)        ((aip)->kind)
#define Asc_argItemDepth(aip)       ((aip)->depth)
#define Asc_argItemInstance(aip)    ((aip)->u.i)
#define Asc_argItemDoublePtr(aip)   ((aip)->u.dptr)
#define Asc_argItemLongPtr(aip)     ((aip)->u.liptr)
#define Asc_argItemDouble(aip)      ((aip)->u.dval)
#define Asc_argItemLongVal(aip)     ((aip)->u.lival)
#define Asc_argItemIntVal(aip)      ((aip)->u.ival)
#define Asc_argItemBoolVal(aip)     ((aip)->u.bval)
#define Asc_argItemStringVal(aip)   ((aip)->u.cval)
#define Asc_argItemNameString(aip)  ((aip)->u.nval)
#define Asc_argItemTypeName(aip)    ((aip)->u.nval)
#define Asc_argItemFileString(aip)  ((aip)->u.fval)
#define Asc_argItemListValue(aip)   ((aip)->u.lval)
/**< 
 * All of the data in or referenced directly by an argItem
 * should not be changed or destroyed by the user proc.
 * The only exception is that the value pointed to by argItemDoublePtr
 * or argItemLongPtr items may be changed.
 */

extern int Asc_AddUserMethod(CONST char *methodname,
                             Asc_MethodProc proc,
                             enum inst_t selfKind,
                             CONST char *parseString,
                             ascClientData cData,
                             Asc_MethodDelete deleteProc,
                             CONST char *description,
                             CONST char *details);
/**<
 * <!--  err = Asc_AddUserMethod(methodname,                           -->
 * <!--                          proc,                                 -->
 * <!--                          selfKind,                             -->
 * <!--                          parseString,                          -->
 * <!--                          cData,                                -->
 * <!--                          deleteProc,                           -->
 * <!--                          description,                          -->
 * <!--                          details);                             -->
 *
 * Add a user method.
 * <pre>
 * methodname is the name to be used in the ASCEND method.
 * proc is the function pointer that will be called.
 * selfKind is the kind of instance that this method must be
 *   called in the scope of. If any is acceptable, or proc will check
 *   the kind, then selfKind should be DUMMY_INST.
 *   If NULL is an acceptable value of self, ERROR_INST should be used.
 * parseString is a string of arg type directives separated by semicolons.
 *   For example, "g, ...M" has directives "g" and "...M".
 *   Whitespace in the parseString is ignored.
 * ascClientData is a void pointer that will be included in every
 *   call to proc. It may be NULL.
 * deleteProc is called by Asc_DeleteUserMethod with cData, methodname.
 *   and proc. deleteProc may be NULL if no special delete action is wanted.
 * description is a one-line description of the function. Cannot be NULL.
 * details is a longer piece of useful information or directions how
 *   and where to obtain more information. Cannot be NULL.
 *
 * We guarantee to not call proc until the arguments supplied in the
 * call satisfy the conditions implied by the parseString. This frees
 * the user method from a vast chunk of condition checking.
 * In no case will the user method be called with the actual UNIVERSAL
 * DummyInstance. Dummys are considered to always be erroneous input.
 * The user method is responsible for handling all possible inputs which
 * conform to the parse string.
 *
 * Each directive is of the form WXYZ where:
 * W is either the ... or empty
 * X is A,L,p or a positive integer decimal number or empty for Instance Y,
 *   or X is O, N, or U for file Y.
 * Y is F, s,ss,d,ld,b,f,g,n,N,T,i,R,B,S,SS,I, or M.
 * Z is .<identifier> or +<identifier> for Y that are instances
 * W:  ...XYZ indicates that any number of additional arguments
 *     conforming to the directive remaining XYZ are permitted.
 *     If XYZ calls for scalars and arrays are found, arrays will be
 *     expanded into scalar instances.
 *     ... can be used in the any directive, but "...XYZ;...XYZ"
 *     is impossible to satisfy because the second slot is always empty
 *     as the first slot eats up all XYZ arguments.
 * X:  A indicates to expect an array of the kind required by the remaining
 *      directive YZ. Applies only to Instance flavors of Y.
 *     L indicates to expect a list cooked up from the elements of an array.
 *       The elements of the array must conform to YZ.
 *     O,N,U apply only when Y = "F"
 *       O indicates an old file, N a new one, and U an unknown.
 *     p indicates that the pointer to a double or long is desired.
 *       callers can read/set values this way without knowing about
 *       the details of real/integer instances. These pointers are
 *       not stable, and the user should not cache them anywhere.
 *       This modifier does not work for constant or other types:
 *       anyone wanting to mess with other types must use our detailed headers.
 *       All double values are stored in SI units.
 *     <decimal number> indicates both the minium & maximum string length
 *       allowed for arguments that are normally C strings (s,ss,n,N,Q,T).
 *       C strings will be padded to this length with trailing blanks
 *       This is for FORTRAN compatibility. Strings too long are invalid
 *       and will not be passed. If the number is -1, then the string
 *       argument will be expanded into two arguments: an int that specifies
 *       the length and the string following.
 * Y:  F indicates filename expected.
 *     s indicates string value expected.
 *     ss indicates string value of a set expected.
 *     d indicates int value expected.
 *     ld indicates long int value expected.
 *     b indicates boolean int value expected.
 *     f indicates float value expected.
 *     g indicates double value expected.
 *     n indicates string form of a name expected.
 *     N indicates set expanded, comma separated string form of name.
 *     T indicates the name of a known type is expected.
 *     i indicates any single instance is acceptable.
 *     S indicates a symbol instance is expected.
 *     SS indicates a set instance is expected.
 *     R indicates a real instance is expected.
 *     B indicates a boolean instance is expected.
 *     I indicates an integer instance is expected.
 *     M indicates a MODEL instance is expected.
 *     V indicates an ASCEND Expr is expected.
 *     r indicates a relation instance is expected.
 *     lr indicates a logical relation instance is expected.
 * Z:  -typeidentifier further restricts any instances passing WXY
 *       to be of exactly the type 'typeidentifier'.
 *     +typeidentifier further restricts any instances passing WXY
 *       to be of the type 'typeidentifier' or a refinement of that type.
 *     .intdepth restricts array instances to have intdepth subscripts.
 *
 * Example:
 * int ClearWrapper(ascClientData cdata, CONST char *calledAs,
 *                  struct Instance *root, struct gl_list_t *argList)
 * {
 *   unsigned long c,len;
 *   len = gl_length(argList);
 *   if (len == 0) {
 *     Asc_ClearVarsInTree(root);
 *   } else {
 *     for (c = 1; c <= len; c++) {
 *       item = (struct argItem *)gl_fetch(argList,c);
 *       Asc_ClearVarsInTree(Asc_argItemInstance(item));
 *          note a simpler version with gl_iterate is possible.
 *     }
 *   }
 *   return ASC_OK;
 * }
 * argList is a gl_list derived from the list of arguments the
 * function caller supplies in the method.
 *
 * argList may be empty. It will never be NULL.
 * argItem = (struct argItem *)gl_fetch(argList,k);
 * returns the kth argument.
 * The argItem is described by the enum above.
 *
 * AddUserMethod("ClearAllSolverVar",ClearWrapper,DUMMY_INST,"...M",NULL,NULL);
 * leaves us able to call a method:
 *   CALL ClearAllSolverVar(); (* clears self, we assume *)
 *   CALL ClearAllSolverVar(SELF); (* redundant, but legal *)
 *   CALL ClearAllSolverVar(foo,bar); (* ClearWrapper can clear several insts*)
 * ClearWrapper requires no state information saved between calls, so
 * cData and deleteProc are NULL.
 * </pre>
 */

extern int Asc_DeleteUserMethod(char *mname);
/**<
 * <!--  err = Asc_DeleteUserMethod(mname);                            -->
 * Finds mname among the user defined methods and undefines it.
 * normally returns 0.
 * Call the Asc_MethodDelete function specifed when adding the method.
 * Redefining user methods requires deleting the old and
 * calling Add with the new.
 * If the same proc is registered under different names, only
 * the named reference is deleted.
 * This does not cause the unloading of dynamically loaded libraries,
 * but it should be done to eliminate references to a library about to
 * be unloaded.
 */

extern CONST char *Asc_UserMethodDescription(char *mname);
/**<
 * <!--  string = Asc_UserMethodDescription(mname);                    -->
 * Looks up the one-liner for mname.
 * Returns NULL if mname is unknown.
 */

extern CONST char *Asc_UserMethodDetails(char *mname);
/**< 
 * <!--  string = Asc_UserMethodDetails(mname);                        -->
 * Looks up the detail information for mname.
 * Returns NULL if mname is unknown.
 */

extern struct gl_list_t *Asc_UserMethodsDefined(void);
/**< 
 * <!--  mlist = Asc_UserMethodsDefined();                             -->
 * Returns a list of CONST char * pointers to names registered.
 * The caller should gl_destroy(mlist) when done with looking at it.
 */

extern int Asc_UserMethodArguments(FILE *fp, char *mname);
/**< 
 * <!--  Asc_UserMethodArguments(fp,mname);                            -->
 * Writes a synopsis of the arguments required, based on information
 * derived from the parse-string to fp.
 */

/* ********************************************************************/
/*
 * This whole header is junk below here. Temporarily, it is functioning
 * extern * junk. We need to reimplement some and scrap the rest.
 */
/* ********************************************************************/
/*
 *  This file implements an interface to external packages. These
 *  external packages may be invoked from the procedural section, say
 *  for doing external calculations, querying a database, and writing/
 *  reading values from the instance tree.
 *  They may also be used in the declarative section for providing relations
 *  to augment those in the declarative section. These may be full models/
 *  solvers capable of solving themselves or smarter models/matrix routines
 *  capable of operating in a single step mode.
 *  [This overloading has caused a protocol that serves neither very well.]
 *  The rest of the discussion here is based on packages in the procedural
 *  section, and packages in the declarative section that will solve
 *  themselves to completion at each major iteration of the solution scheme
 *  in which they are imbedded.
 *
 *  Procedural Section:
 *  The packages provided in the procedural section follow the 'foreign
 *  pointer' concept; i.e., they will be handed a pointer to a reference
 *  instance, and a list of list of arguments. They need to provide only
 *  the number of input arguements, and may freely manipulate the instance
 *  tree and whatever way they feel. The only *mandatory* function must
 *  respect the following function prototype:
 *
 *  int (*proc) (struct Slv_Interp *interp,
 *                struct Instance *reference,
 *                struct gl_list_t *arglist);
 *
 *  And optional help string may be provided. We will *make* a copy of this
 *  string, if non-null. When invoked this package must do whatever arg
 *  checking that is necessary, do its thing, and return a nonzero error
 *  result if there are any errors.
 *
 *  Declarative Section.
 *  More information is need from the packages in the declarative section
 *  that provide relations. In particular a *presolve/init* routine must
 *  be provided, which the compiler/solver will invoke to make sure that
 *  all is ok. This package when registering itself has to provide:
 *  n_input_args -- the number of input args on its arglist.
 *  n_output_args -- the number of output args on its arglist.
 *  In addition to the presolve/init routine, the following functions for
 *  doing function and jacobian evaluations have to be provided. The
 *  deriv (jacobian evaluation routine) and higher order derivative routines
 *  may be left NULL, and finite difference via repeated function calls
 *  will be done. The calling protocal for all functions is as follows:
 *
 *  int (*init) (struct Slv_Interp *interp,
 *                struct Instance *model_data,
 *                struct gl_list_t *arglist);
 *
 *  int (*value) (struct Slv_Interp *interp,
 *                int ninputs, int noutputs,
 *         double *inputs, double *outputs,
 *         double *jacobian);
 *
 *  int (*deriv) (struct Slv_Interp *interp,
 *                int ninputs, int noutputs,
 *         double *inputs, double *outputs,
 *         double *jacobian);
 *
 *  deriv2 and any higher order derivatives that come along will follow
 *  same calling protocol as for value and deriv.
 *  NB: The arguement model_data is provided for external packages that
 *  need to get additional information from the instance tree so as to
 *  do there computations. This information, should ideally be retrieved
 *  during exection of the init function and cached away.
 *
 *  The Interpreter:
 *  The interpreter structure is the means of communication between
 *  external packages and the ASCEND instance tree and/or solvers.
 *  It is particularly necessary in the case of packages in the
 *  declarative section. The state of the bit flags must be monitored
 *  and appropriate action taken. In the case of an error, a nonzero
 *  result must be returned from all functions, and the reason indicated
 *  by writing to the interp->status field.
 *  At the moment 1 wild hook is provided for the convenience of users.
 *  A user may attach an object to this hook and it will be
 *  passed around to each of its function calls. As an example consider the
 *  following code fragment.
 *
 *  int init(struct Slv_Interp *slv_interp, struct Instance *data,
 *            struct gl_list_t *arglist)
 *  {
 *     struct my_solve_system *system;
 *     double *my_inputs;
 *
 *     if (slv_interp->firstcall) {
 *       system = my_presolve(data,arglist);
 *         if (system!=NULL) {
 *           my_inputs = MakeVectorFromList(arglist);
 *  system->inputs = my_inputs;
 *           slv_interp->user_data = (void *)system;
 *  } else {
 *  slv_interp->status = calc__error;
 *  return 1;
 *  }
 *     } else {
 *      destroy_sys((my_solve_system *)slv_interp->user_data);
 *      slv_interp->user_data = NULL;
 *      }
 *      slv_interp->status = calc_all_ok;
 *      return 0;
 *  }
 *
 *  The arglist and all references to instances have been intentionally
 *  removed from the value (function evaluation) and deriv* routines
 *  so as to allow ease of interfacing to existing standalone packages.
 */

extern void Init_Slv_Interp(struct Slv_Interp *slv_interp);
/**<
 *  Gets the interpreter back to a 'clean' state.
 *  The default settings are guaranteed to be as follows:
 *    - nodestamp = 0;
 *    - status = calc_all_ok;
 *    - first_call  = (unsigned)0;
 *    - last_call   = (unsigned)0;
 *    - check_args = (unsigned)0;
 *    - recalculate = (unsigned)0;
 *    - deriv_eval = (unsigned)0;
 *    - func_eval = (unsigned)0;
 */

extern void Reset_Slv_Interp(struct Slv_Interp *slv_interp);
/**< Resets the interpreter to its initial state. */

extern void AddUserFunctions(void);
/**< 
 *  This function registers user packages, *if* they are statically
 *  linked, otherwise it does nothing.
 */

extern int CallExternalProcs(struct Instance *i);
/**<
 *  This function given a handle to a relation instance which represents
 *  an external relation, will attempt to invoke it and write the results
 *  to stdout.
 */

extern symchar *MakeArchiveLibraryName(CONST char *prefix);
/**< 
 *  Given a file prefix, this function will try to construct a platform
 *  dependent name for a dynamically loadable archive. In the case of
 *  - sun, alphas, solaris -- lib<prefix>.so.1.0
 *  - hpux -- lib<prefix>.sl
 *  - mips ???
 *  - aix ???
 *  - linux ???
 *  Returns symbol table entry.<br><br>
 *
 *  This function is obsolete and should not be used in new code.
 *  See ascDynaLoad.h for correct replacement.
 */

extern int LoadArchiveLibrary(CONST char *name, CONST char *initfunc);
/**< 
 *  Given the name of a dynamically loadable library/archive (dll),
 *  and the name of an initialization routine to be found in this
 *  archive, this function will attempt to load the archive and to run
 *  the initialization routine. Will return a nonzero code in the event
 *  of failure. The calling protocol for the init routine is:
 *  void Routine_init(void);
 *  This is a rather pointless wrapper to the ascDynaLoad functions.
 *  @see ascDynaLoad.h for correct replacement.
 */

#endif /* __PACKAGES_H_SEEN__ */
