/*
 *  Relation Output Routines
 *  by Tom Epperly
 *  Created: 2/8/90
 *  Version: $Revision: 1.9 $
 *  Version control file: $RCSfile: relation_io.h,v $
 *  Date last modified: $Date: 1998/01/27 11:00:18 $
 *  Last modified by: $Author: ballan $
 *
 *  This file is part of the Ascend Language Interpreter.
 *
 *  Copyright (C) 1990, 1993, 1994 Thomas Guthrie Epperly
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
 *  You should have received a copy of the GNU General Public License along
 *  with the program; if not, write to the Free Software Foundation, Inc., 675
 *  Mass Ave, Cambridge, MA 02139 USA.  Check the file named COPYING.
 */

/** @file
 *  Relation Output Routines.
 *  <pre>
 *  When #including relation_io.h, make sure these files are #included first:
 *         #include <stdio.h>
 *         #include "utilities/ascConfig.h"
 *         #include "instance_enum.h"
 *         #include "fractions.h"
 *         #include "compiler.h"
 *         #include "dimen.h"
 *         #include "types.h"
 *         #include "relation_type.h"
 *         #include "exprs.h"
 *         #include "relation.h"
 *  </pre>
 */

#ifndef __RELATION_IO_H_SEEN__
#define __RELATION_IO_H_SEEN__

/**< 
 * This enum controls the format of function names and other
 * minor operator variations in the supported languages.
 *
 * Add others here as desired and then update RelationWriteString.
 */
enum rel_lang_format {
  relio_ascend,
  relio_C
};

typedef char * (*WRSNameFunc)(CONST struct relation *,unsigned long,void *);
/**<
 * <!--  name = func(tokenrel,varnum,userdata);                        -->
 * For use with WriteRelationString() and token relations.
 * Func should return a name for varnum in tokenrel.
 * Userdata will be that which the caller of WriteRelationString provides.
 * We do not keep the name returned or free it -- it is the func's
 * responsibility to track this memory.
 */

struct RXNameData{
  char *prefix;
  int *indices;
  char *suffix;
};

extern char *RelationVarXName(CONST struct relation *r,
                              unsigned long varnum,
                              struct RXNameData *rxndata);
/**<
 * <!--  ns = RelationVarXName(r,varnum,rxndata);                      -->
 * Returns names like x23.
 * Returns a character string (which we own, not the caller, but it won't
 * change until we're called again) with the name of the
 * the variable in "Prefix%luSuffix" format. prefix  and suffix are limited
 * to 110 characters and are taken from rxndata.
 * If indices is not NULL, then the number printed will be
 * (unsigned long)indices[varnum] (remember: varnums go from 1 and
 * C arrays from 0!) else number printed will be varnum.
 * If prefix is NULL, "x" will be assumed and suffix will be assumed "".
 * If prefix is not NULL, suffix must not be NULL.
 * r is ignored entirely.<br><br>
 *
 * This function is compatible with WriteRelationString().
 */

extern int NeedParen(enum Expr_enum parent, enum Expr_enum child, int rhs);
/**<
 *  <!--  int NeedParen(parent,child,rhs);                             -->
 *  <!--  enum Expr_enum parent_op;                                    -->
 *  <!--  enum Expr_enum child_op;                                     -->
 *  <!--  int rhs;                                                     -->
 *  Given a unary or binary expression, will determine whether, the
 *  child expression needs parentheses. "rhs" tells if we are looking
 *  at the left or right side of a binary token.
 *  E.g x ^ (2/y), would need parens exactly as shown to give the
 *  correct precedence.
 *  This would be called as : NeedParen(e_power,e_divide,right);
 *  and would return nonzero.
 */

extern void WriteRelation(FILE *f,
                          CONST struct Instance *relinst,
                          CONST struct Instance *ref);
/**<
 *  <!--  WriteRelation(f,relinst,ref);                                -->
 *  <!--  FILE *f;                                                     -->
 *  <!--  const struct Instance *relinst;                              -->
 *  <!--  const struct Instance *ref;                                  -->
 *  Write the relation in infix to the file indicated.
 *  This function comfortably handles all the relation types.
 *  Variables are written with names relative to ref.
 */

extern char *WriteRelationString(CONST struct Instance *relinst,
                                 CONST struct Instance *ref,
                                 WRSNameFunc func,
                                 void *userdata,
                                 enum rel_lang_format lang,
                                 int *lenptr);
/**<
 *  <!--  s = WriteRelationString(relinst,ref,func,userdata,lang,lenptr); -->
 *  <!--  char *f;                                                     -->
 *  <!--  const struct Instance *relinst;                              -->
 *  <!--  const struct Instance *ref;                                  -->
 *  <!--  WRSNameFunc func;                                            -->
 *  <!--  void *userdata;                                              -->
 *  <!--  enum rel_lang_format lang;                                   -->
 *  <!--  int *lenptr;                                                 -->
 *  Write the token relation in infix to a char using the
 *  variable names as generated by func.
 *  If func is NULL, name will be written in ascend form
 *  from ref to the variable.
 *  This function comfortably handles all the relation types,
 *  but glassbox and blackbox do not use func or userdata.
 *  lenptr contains the length of the string returned, or -1
 *  if string is NULL. If lenptr is NULL, it is ignored.
 */

extern void WriteRelationPostfix(FILE *f,
                                 CONST struct Instance *relinst,
                                 CONST struct Instance *ref);
/**<
 *  <!--  void WriteRelationPostfix(f,relinst,ref)                     -->
 *  <!--  FILE *f;                                                     -->
 *  <!--  const struct Instance *relinst;                              -->
 *  <!--  const struct Instance *ref;                                  -->
 *  Write the relation in postfix to the file indicated.
 *  This function at the moment is only applicable to token relations.
 */

extern char *WriteRelationPostfixString(CONST struct Instance *relinst,
                                        CONST struct Instance *ref);
/**<
 *  <!--  void WriteRelationPostfixString(relinst,ref)                 -->
 *  <!--  char *f;                                                     -->
 *  <!--  const struct Instance *relinst;                              -->
 *  <!--  const struct Instance *ref;                                  -->
 *  Write the relation in postfix to the file indicated.
 *  This function at the moment is only applicable to token relations.
 */

extern void Infix_WriteRelation(FILE *f,
                                CONST struct Instance *r,
                                CONST struct Instance *ref);
/**<
 *  <!--  void Infix_WriteRelation(f,r,ref)                            -->
 *  <!--  FILE *f;                                                     -->
 *  <!--  const struct Instance *relinst;                              -->
 *  <!--  const struct Instance *ref;                                  -->
 *  Write the relation in infixe to the file indicated.
 *  This function uses the "new" infix representation of relations.
 *  This function at the moment is only applicable to token relations.
 */

extern void WriteRelationsInTree(FILE *f, struct Instance *relinst);
/**<
 *  Search for relations in an instance and write them to the
 *  file given in various formats.
 */

extern void WriteRelationsInList(FILE *f, struct gl_list_t *l);
/**<
 *  Search for relations in an instance list and write them to the
 *  file given in various formats.
 */

extern void SaveRelationVariables(FILE *fp, CONST struct relation *r);
/**<
 *  Given a relation will save its variable list in the ASCEND condensed
 *  format.
 */

extern void SaveGlassBoxRelation(FILE *fp, CONST struct Instance *relinst);
/**<
 *  Given a glassbox relation will save it in the ASCEND condensed
 *  format.
 */

extern void SaveTokenRelation(FILE *fp, CONST struct Instance *relinst);
/**<
 *  Given a token relation will save it in the ASCEND condensed
 *  format.
 */

extern void SaveReln2GlassBox(FILE *fp,
                              CONST struct Instance *relinst,
                              char *prefix,
                              unsigned long index_);
/**<
 *  Given a relation will save it and perform conversion to the glassbox
 *  format.
 */

extern int ConversionIsValid(enum Expr_enum old, enum Expr_enum new);
/**<
 *  Given a relation format will return TRUE if conversion to another
 *  format is valid. It assumes that new is a valid relation type.
 */

extern void RelationIO_init_pool(void);
/**<
 * starts memory recycle. Do not call twice before stopping recycle.
 */

extern void RelationIO_destroy_pool(void);
/**<
 * Stops memory recycle. Do not call while ANY Expr are outstanding.
 */

extern void RelationIO_report_pool(void);
/**<
 * Write the pool report to ASCERR for the ppe pool.
 */

#endif /* __RELATION_IO_H_SEEN__ */
