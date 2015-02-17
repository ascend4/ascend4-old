/*
 *  Ascend Instance Tree Type Implementation
 *  by Tom Epperly
 *  9/3/89
 *  Version: $Revision: 1.10 $
 *  Version control file: $RCSfile: mathinst.c,v $
 *  Date last modified: $Date: 1998/05/06 17:33:36 $
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
 *  You should have received a copy of the GNU General Public License
 *  along with the program; if not, write to the Free Software Foundation,
 *  Inc., 675 Mass Ave, Cambridge, MA 02139 USA.  Check the file named
 *  COPYING.
 *
 */
#include <stdarg.h>
#include "utilities/ascConfig.h"
#include "utilities/ascMalloc.h"
#include "utilities/ascPanic.h"
#include "general/pool.h"
#include "general/list.h"
#include "general/dstring.h"
#include "compiler/compiler.h"
#include "compiler/bit.h"
#include "compiler/symtab.h"
#include "compiler/fractions.h"
#include "compiler/dimen.h"
#include "compiler/functype.h"
#include "compiler/types.h"
#include "compiler/child.h"
#include "compiler/type_desc.h"
#include "compiler/instance_enum.h"
#include "compiler/instance_name.h"
#include "compiler/instance_io.h"
#include "compiler/instmacro.h"
#include "compiler/extinst.h"
#include "compiler/instquery.h"
#include "compiler/instance_types.h"
#include "compiler/linkinst.h"
#include "compiler/destroyinst.h"
#include "compiler/createinst.h"
#include "compiler/refineinst.h"
#include "compiler/atomvalue.h"
#include "compiler/atomsize.h"
#include "compiler/check.h"
#include "compiler/dump.h"
#include "compiler/childinfo.h"
#include "compiler/prototype.h"
#include "compiler/pending.h"
#include "compiler/find.h"
#include "compiler/relation_type.h"
#include "compiler/relation.h"
#include "compiler/logical_relation.h"
#include "compiler/logrelation.h"
#include "compiler/relation_util.h"
#include "compiler/logrel_util.h"
#include "compiler/rel_common.h"
#include "compiler/case.h"
#include "compiler/when_util.h"
#include "compiler/universal.h"
#include "compiler/cmpfunc.h"
#include "compiler/tmpnum.h"
#include "compiler/mathinst.h"

#ifndef lint
static CONST char MathInstModuleID[] = "$Id: mathinst.c,v 1.10 1998/05/06 17:33:36 ballan Exp $";
#endif

enum Expr_enum GetInstanceRelationType(CONST struct Instance *i)
{
  AssertMemory(i);
  if (i->t == REL_INST) {
    return RELN_INST(i)->type; /* the implementation kind */
  } else {
    Asc_Panic(2, NULL, "Incorrect type passed to GetInstanceRelationType.\n");
    exit(2);/* Needed to keep gcc from whining */
  }
}

CONST struct relation *GetInstanceRelationOnly(CONST struct Instance *i)
{
  AssertMemory(i);
  if (i->t == REL_INST) {
    return RELN_INST(i)->ptr;
  } else {
    Asc_Panic(2, NULL, "Incorrect type passed to GetInstanceRelationOnly.\n");
    exit(2);/* Needed to keep gcc from whining */
  }
}

CONST struct relation *GetInstanceRelation(CONST struct Instance *i,
					   enum Expr_enum *type)
{
  AssertMemory(i);
  if (i->t == REL_INST) {
    *type = RELN_INST(i)->type;
    return RELN_INST(i)->ptr;
  } else {
    Asc_Panic(2, NULL, "Incorrect type passed to GetInstanceRelation.\n");
    exit(2);/* Needed to keep gcc from whining */
  }
}

struct relation *GetInstanceRelToModify(struct Instance *i,
					enum Expr_enum *type)
{
  AssertMemory(i);
  if (i->t == REL_INST) {
    *type = RELN_INST(i)->type;
    return RELN_INST(i)->ptr;
  } else {
    Asc_Panic(2, NULL, "Incorrect type passed to GetInstanceRelToModify.\n");
    exit(2);/* Needed to keep gcc from whining */
  }
}


CONST struct logrelation *GetInstanceLogRel(CONST struct Instance *i)
{
  AssertMemory(i);
  if (i->t == LREL_INST) {
    return LRELN_INST(i)->ptr;
  } else {
    Asc_Panic(2, NULL, "Incorrect type passed to GetInstanceLogRel.\n");
    exit(2);/* Needed to keep gcc from whining */
  }
}

struct logrelation *GetInstanceLogRelToModify(struct Instance *i)
{
  AssertMemory(i);
  if (i->t == LREL_INST) {
    return LRELN_INST(i)->ptr;
  } else {
    Asc_Panic(2, NULL,"Incorrect type passed to GetInstanceLogRelToModify.\n");
    exit(2);/* Needed to keep gcc from whining */
  }
}
CONST struct logrelation *GetInstanceLogRelOnly(CONST struct Instance *i)
{
  AssertMemory(i);
  if (InstanceKind(i) == LREL_INST) {
    return LRELN_INST(i)->ptr;
  } else {
    Asc_Panic(2, NULL, "Incorrect type passed to GetInstanceLogRelOnly.\n");
    exit(2);/* Needed to keep gcc from whining */
  }
}

/*
 * l = GetInstanceOperands(i);
 * Returns list of vars/models/equations in a mathematical relationship.
 * May want to include the models/relations from the cases of a when.
 */
struct gl_list_t *GetInstanceOperands(CONST struct Instance *i)
{
  CONST struct gl_list_t *list = NULL;
  CONST struct gl_list_t *list2 = NULL;
  struct gl_list_t *result = NULL;
  unsigned long c,len;
  CONST struct relation *rel;
  CONST struct logrelation *lrel;
  void *p;

  if (i == NULL) {
    return NULL;
  }
  switch (InstanceKind(i)) {
  case REL_INST:
    rel = GetInstanceRelationOnly(i);
    if (rel != NULL) {
      list = RelationVarList(rel);
    }
    break; 
  case LREL_INST:
    lrel = GetInstanceLogRel(i);
    if (lrel != NULL) {
      list = LogRelBoolVarList(lrel);
      list2 = LogRelSatRelList(lrel);
    }
    break; 
  case WHEN_INST:
    list = GetInstanceWhenVars(i);
    break; 
  default:
    return NULL;
  }
  len = 0;
  len += (list != NULL) ? gl_length(list) : 0;
  len += (list2 != NULL) ? gl_length(list2) : 0;
  result = gl_create(len);
  if (list != NULL) {
    for (c=1; c <= len; c++) {
      p = gl_fetch(list,c);
      if (p!=NULL) {
        gl_append_ptr(result,p);
      }
    }
  }
  if (list2 != NULL) {
    for (c=1; c <= len; c++) {
      p = gl_fetch(list2,c);
      if (p!=NULL) {
        gl_append_ptr(result,p);
      }
    }
  }
  return result;
}

struct gl_list_t *GetInstanceWhenVars(CONST struct Instance *i)
{
  AssertMemory(i);
  if (i->t == WHEN_INST) {
    return W_INST(i)->bvar;
  } else {
    Asc_Panic(2, NULL, "Incorrect type passed to GetInstanceWhenVars.\n");
    exit(2);/* Needed to keep gcc from whining */
  }
}


struct gl_list_t *GetInstanceWhenCases(CONST struct Instance *i)
{
  AssertMemory(i);
  if (i->t == WHEN_INST) {
    return W_INST(i)->cases;
  } else {
    Asc_Panic(2, NULL,"Incorrect type passed to GetInstanceWhenCases.\n");
    exit(2);/* Needed to keep gcc from whining */
  }
}


struct gl_list_t *GetInstanceWhens(CONST struct Instance *i)
{
  AssertMemory(i);
  switch(i->t) {
    case BOOLEAN_ATOM_INST:
      return BA_INST(i)->whens;
    case INTEGER_ATOM_INST:
      return IA_INST(i)->whens;
    case SYMBOL_ATOM_INST:
      return SYMA_INST(i)->whens;
    case BOOLEAN_CONSTANT_INST:
      return BC_INST(i)->whens;
    case INTEGER_CONSTANT_INST:
      return IC_INST(i)->whens;
    case SYMBOL_CONSTANT_INST:
      return SYMC_INST(i)->whens;
    case MODEL_INST:
      return MOD_INST(i)->whens;
    case REL_INST:
      return RELN_INST(i)->whens;
    case LREL_INST:
      return LRELN_INST(i)->whens;
    case WHEN_INST:
      return W_INST(i)->whens;
    default:
      Asc_Panic(2, NULL, "Incorrect type passed to GetInstanceWhens.\n");
      exit(2);/* Needed to keep gcc from whining */
  }
}

void SetWhenVarList(struct Instance *i,struct gl_list_t *whenvars)
{
  AssertMemory(i);
  if (i->t == WHEN_INST) {
    W_INST(i)->bvar = whenvars;
  } else {
    Asc_Panic(2, NULL, "Incorrect type passed to SetWhenVarList.\n");
  }
}

void SetWhenCases(struct Instance *i,struct gl_list_t *whencases)
{
  AssertMemory(i);
  if (i->t == WHEN_INST) {
    W_INST(i)->cases = whencases;
  } else {
    Asc_Panic(2, NULL, "Incorrect type passed to SetWhenCases.\n");
  }
}


/*
 * This is a tricky function. If we are attempting to set
 * a new relation, and if the i->ptr is not NULL, i.e. a
 * relation structure was already assigned, we must *not* force
 * the assignment, and destroy the old relation. Unfortunately
 * at this stage, we would have to back up todo undo the
 * AddRelations etc. So we punt.
 */
void SetInstanceRelation(struct Instance *i, struct relation *rel,
			 enum Expr_enum type)
{
  AssertMemory(i);
  if (i->t==REL_INST) {
    if (RELN_INST(i)->ptr==NULL) {
      RELN_INST(i)->ptr = rel;
      RELN_INST(i)->type = type;
    } else {
      Asc_Panic(2, NULL, "Attempt to reassign RelationPointer.\n");
    }
  } else {
    Asc_Panic(2, NULL, "Incorrect type passed to SetInstanceRelation.\n");
  }
}


void SetInstanceLogRel(struct Instance *i, struct logrelation *lrel)
{
  AssertMemory(i);
  if (i->t==LREL_INST) {
    if (LRELN_INST(i)->ptr==NULL) {
      LRELN_INST(i)->ptr = lrel;
    } else {
      Asc_Panic(2, NULL, "Attempt to reassign Logical Relation Pointer.\n");
    }
  } else {
    Asc_Panic(2, NULL, "Incorrect type passed to SetInstanceLogRel.\n");
  }
}

/*********************************************************************\
Relation list stuff
\*********************************************************************/

unsigned long RelationsCount(CONST struct Instance *i)
{
  assert(i!=NULL);
  AssertMemory(i);
  switch(i->t) {
  case REAL_ATOM_INST:
    if (RA_INST(i)->relations!=NULL) {
      return gl_length(RA_INST(i)->relations);
    } else {
      return 0;
    }
  default:
    Asc_Panic(2, NULL, "RelationsCount called with inappropriate argument.\n");
    exit(2);/* Needed to keep gcc from whining */
  }
}

struct Instance *RelationsForAtom(CONST struct Instance *i,
				  unsigned long int c)
{
  assert((i!=NULL)&&(c>0)&&(c<=RelationsCount(i)));
  AssertMemory(i);
  switch(i->t){
  case REAL_ATOM_INST:
    if (RA_INST(i)->relations!=NULL) {
      return INST(gl_fetch(RA_INST(i)->relations,c));
    } else {
      Asc_Panic(2, NULL, "c out of bounds in RelationsForAtom.\n");
    }
    break;
  default:
    Asc_Panic(2, NULL,
              "RelationsForAtom called with inappropriate argument.\n");
    break;
  }
  exit(2);/* NOT REACHED.  Needed to keep gcc from whining */
}

void AddRelation(struct Instance *i, struct Instance *reln)
{
  unsigned long len;
  assert(i&&reln&&(reln->t==REL_INST));
  AssertMemory(i);
  switch(i->t){
  case REAL_ATOM_INST:
    if (RA_INST(i)->relations==NULL) {
      RA_INST(i)->relations = gl_create(AVG_RELATIONS);
    }
    len = gl_length(RA_INST(i)->relations);
    if (gl_search(RA_INST(i)->relations,(char*)reln,(CmpFunc)CmpRelations)==0){
      gl_append_ptr(RA_INST(i)->relations,(VOIDPTR)reln);
    }
    break;
  default:
    Asc_Panic(2, NULL, "AddRelation called with inappropriate argument.\n");
  }
}

void RemoveRelation(struct Instance *i, struct Instance *reln)
{
  register unsigned long c;
  assert(i&&reln&&(reln->t==REL_INST));
  AssertMemory(i);
  switch(i->t) {
  case REAL_ATOM_INST:
    if (RA_INST(i)->relations==NULL) {
      return;
    }
    c = gl_search(RA_INST(i)->relations,(char *)reln,(CmpFunc)CmpRelations);
    if (c>0) {
      gl_delete(RA_INST(i)->relations,c,0);
    }
    break;
  default:
    Asc_Panic(2, NULL, "Bad argument to RemoveRelation.\n");
  }
}


/*********************************************************************\
Logical Relation list stuff
\*********************************************************************/

unsigned long LogRelationsCount(CONST struct Instance *i)
{
  assert(i!=NULL);
  AssertMemory(i);
  switch(i->t) {
  case BOOLEAN_ATOM_INST:
    if (BA_INST(i)->logrelations!=NULL) {
      return gl_length(BA_INST(i)->logrelations);
    } else {
      return 0;
    }
  case REL_INST:
    if (RELN_INST(i)->logrels!=NULL) {
      return gl_length(RELN_INST(i)->logrels);
    } else {
      return 0;
    }
  case LREL_INST:
    if (LRELN_INST(i)->logrels!=NULL) {
      return gl_length(LRELN_INST(i)->logrels);
    } else {
      return 0;
    }
  default:
    Asc_Panic(2, "LogRelationsCount",
              "LogRelationsCount called with inappropriate argument.\n");
    exit(2);/* Needed to keep gcc from whining */
  }
}

struct Instance *LogRelationsForInstance(CONST struct Instance *i,
			                 unsigned long int c)
{
  assert((i!=NULL)&&(c>0)&&(c<=LogRelationsCount(i)));
  AssertMemory(i);
  switch(i->t){
  case BOOLEAN_ATOM_INST:
    if (BA_INST(i)->logrelations!=NULL) {
      return INST(gl_fetch(BA_INST(i)->logrelations,c));
    } else {
      Asc_Panic(2, NULL, "c out of bounds in LogRelationsForInstance.\n");
    }
  case REL_INST:
    if (RELN_INST(i)->logrels!=NULL) {
      return INST(gl_fetch(RELN_INST(i)->logrels,c));
    } else {
      Asc_Panic(2, NULL, "c out of bounds in LogRelationsForInstance.\n");
    }
  case LREL_INST:
    if (LRELN_INST(i)->logrels!=NULL) {
      return INST(gl_fetch(LRELN_INST(i)->logrels,c));
    } else {
      Asc_Panic(2, NULL, "c out of bounds in LogRelationsForInstance.\n");
    }
  default:
    Asc_Panic(2, NULL,
              "LogRelationsForInstance called with inappropriate argument.\n");
  }
  exit(2);/* NOT REACHED.  Needed to keep gcc from whining */
}

void AddLogRel(struct Instance *i, struct Instance *lreln)
{
  unsigned long len;
  assert(i&&lreln&&(lreln->t==LREL_INST));
  AssertMemory(i);
  switch(i->t){
  case BOOLEAN_ATOM_INST:
    if (BA_INST(i)->logrelations==NULL) {
      BA_INST(i)->logrelations = gl_create(AVG_LOGRELS);
    }
    len = gl_length(BA_INST(i)->logrelations);
    if (gl_search(BA_INST(i)->logrelations,
                  (char *)lreln,(CmpFunc)CmpLogRelations)==0) {
      gl_append_ptr(BA_INST(i)->logrelations,(VOIDPTR)lreln);
    }
    break;
  case REL_INST:
    if (RELN_INST(i)->logrels==NULL) {
      RELN_INST(i)->logrels = gl_create(AVG_LOGRELS);
    }
    len = gl_length(RELN_INST(i)->logrels);
    if (gl_search(RELN_INST(i)->logrels,
                  (char *)lreln,(CmpFunc)CmpLogRelations)==0) {
      gl_append_ptr(RELN_INST(i)->logrels,(VOIDPTR)lreln);
    }
    break;
  case LREL_INST:
    if (LRELN_INST(i)->logrels==NULL) {
      LRELN_INST(i)->logrels = gl_create(AVG_LOGRELS);
    }
    len = gl_length(LRELN_INST(i)->logrels);
    if (gl_search(LRELN_INST(i)->logrels,
                  (char *)lreln,(CmpFunc)CmpLogRelations)==0) {
      gl_append_ptr(LRELN_INST(i)->logrels,(VOIDPTR)lreln);
    }
    break;
  default:
    Asc_Panic(2, NULL, "AddLogRel called with inappropriate argument.\n");
  }
}

void RemoveLogRel(struct Instance *i, struct Instance *lreln)
{
  register unsigned long c;
  assert(i&&lreln&&(lreln->t==LREL_INST));
  AssertMemory(i);
  switch(i->t) {
  case BOOLEAN_ATOM_INST:
    if (BA_INST(i)->logrelations==NULL) {
      return;
    }
    c = gl_search(BA_INST(i)->logrelations,
                 (char *)lreln,(CmpFunc)CmpLogRelations);
    if (c>0) {
      gl_delete(BA_INST(i)->logrelations,c,0);
    }
    break;
  case REL_INST:
    if (RELN_INST(i)->logrels==NULL) {
      return;
    }
    c = gl_search(RELN_INST(i)->logrels,
                 (char *)lreln,(CmpFunc)CmpLogRelations);
    if (c>0) {
      gl_delete(RELN_INST(i)->logrels,c,0);
    }
    break;
  case LREL_INST:
    if (LRELN_INST(i)->logrels==NULL) {
      return;
    }
    c = gl_search(LRELN_INST(i)->logrels,
                 (char *)lreln,(CmpFunc)CmpLogRelations);
    if (c>0) {
      gl_delete(LRELN_INST(i)->logrels,c,0);
    }
    break;
  default:
    Asc_Panic(2, NULL, "Bad argument to RemoveLogRel.\n");
  }
}

/*********************************************************************\
When list stuff
\*********************************************************************/

unsigned long WhensCount(struct Instance *i)
{
  assert(i!=NULL);
  AssertMemory(i);
  switch(i->t) {
  case BOOLEAN_ATOM_INST:
    if (BA_INST(i)->whens!=NULL) {
      return gl_length(BA_INST(i)->whens);
    } else {
      return 0;
    }
  case INTEGER_ATOM_INST:
    if (IA_INST(i)->whens!=NULL) {
      return gl_length(IA_INST(i)->whens);
    } else {
      return 0;
    }
  case SYMBOL_ATOM_INST:
    if (SYMA_INST(i)->whens!=NULL) {
      return gl_length(SYMA_INST(i)->whens);
    } else {
      return 0;
    }
  case BOOLEAN_CONSTANT_INST:
    if (BC_INST(i)->whens!=NULL) {
      return gl_length(BC_INST(i)->whens);
    } else {
      return 0;
    }
  case INTEGER_CONSTANT_INST:
    if (IC_INST(i)->whens!=NULL) {
      return gl_length(IC_INST(i)->whens);
    } else {
      return 0;
    }
  case SYMBOL_CONSTANT_INST:
    if (SYMC_INST(i)->whens!=NULL) {
      return gl_length(SYMC_INST(i)->whens);
    } else {
      return 0;
    }
  case MODEL_INST:
    if (MOD_INST(i)->whens!=NULL) {
      return gl_length(MOD_INST(i)->whens);
    } else {
      return 0;
    }
  case REL_INST:
    if (RELN_INST(i)->whens!=NULL) {
      return gl_length(RELN_INST(i)->whens);
    } else {
      return 0;
    }
  case LREL_INST:
    if (LRELN_INST(i)->whens!=NULL) {
      return gl_length(LRELN_INST(i)->whens);
    } else {
      return 0;
    }
  case WHEN_INST:
    if (W_INST(i)->whens!=NULL) {
      return gl_length(W_INST(i)->whens);
    } else {
      return 0;
    }
  default:
    Asc_Panic(2, NULL, "WhensCount called with inappropriate argument.\n");
    exit(2);/* Needed to keep gcc from whining */

  }
}


struct Instance *WhensForInstance(struct Instance *i,
			      unsigned long int c)
{
  assert((i!=NULL)&&(c>0)&&(c<=WhensCount(i)));
  AssertMemory(i);
  switch(i->t) {
  case BOOLEAN_ATOM_INST:
    if (BA_INST(i)->whens!=NULL)
      return INST(gl_fetch(BA_INST(i)->whens,c));
    else{
      Asc_Panic(2, NULL, "c out of bounds in WhensForInstance.\n");
    }
  case INTEGER_ATOM_INST:
    if (IA_INST(i)->whens!=NULL) {
      return INST(gl_fetch(IA_INST(i)->whens,c));
    } else {
      Asc_Panic(2, NULL, "c out of bounds in WhensForInstance.\n");
    }
  case SYMBOL_ATOM_INST:
    if (SYMA_INST(i)->whens!=NULL) {
      return INST(gl_fetch(SYMA_INST(i)->whens,c));
    } else {
      Asc_Panic(2, NULL, "c out of bounds in WhensForInstance.\n");
    }
  case BOOLEAN_CONSTANT_INST:
    if (BC_INST(i)->whens!=NULL) {
      return INST(gl_fetch(BC_INST(i)->whens,c));
    } else {
      Asc_Panic(2, NULL, "c out of bounds in WhensForInstance.\n");
    }
  case INTEGER_CONSTANT_INST:
    if (IC_INST(i)->whens!=NULL) {
      return INST(gl_fetch(IC_INST(i)->whens,c));
    } else {
      Asc_Panic(2, NULL, "c out of bounds in WhensForInstance.\n");
    }
  case SYMBOL_CONSTANT_INST:
    if (SYMC_INST(i)->whens!=NULL) {
      return INST(gl_fetch(SYMC_INST(i)->whens,c));
    } else {
      Asc_Panic(2, NULL, "c out of bounds in WhensForInstance.\n");
    }
  case MODEL_INST:
    if (MOD_INST(i)->whens!=NULL) {
      return INST(gl_fetch(MOD_INST(i)->whens,c));
    } else {
      Asc_Panic(2, NULL, "c out of bounds in WhensForInstance.\n");
    }
  case REL_INST:
    if (RELN_INST(i)->whens!=NULL) {
      return INST(gl_fetch(RELN_INST(i)->whens,c));
    } else {
      Asc_Panic(2, NULL, "c out of bounds in WhensForInstance.\n");
    }
  case LREL_INST:
    if (LRELN_INST(i)->whens!=NULL) {
      return INST(gl_fetch(LRELN_INST(i)->whens,c));
    } else {
      Asc_Panic(2, NULL, "c out of bounds in WhensForInstance.\n");
    }
  case WHEN_INST:
    if (W_INST(i)->whens!=NULL) {
      return INST(gl_fetch(W_INST(i)->whens,c));
    } else {
      Asc_Panic(2, NULL, "c out of bounds in WhensForInstance.\n");
    }
  default:
    Asc_Panic(2, "WhensForInstance",
              "WhensForInstance called with inappropriate argument.\n");
  }
  exit(2);/* NOT REACHED.  Needed to keep gcc from whining */
}


void AddWhen(struct Instance *i, struct Instance *when)
{
  unsigned long len;
  assert(i&&when&&(when->t==WHEN_INST));
  AssertMemory(i);
  switch(i->t){
  case BOOLEAN_ATOM_INST:
    if (BA_INST(i)->whens==NULL)
      BA_INST(i)->whens = gl_create(AVG_WHEN);
    len = gl_length(BA_INST(i)->whens);
    if (gl_search(BA_INST(i)->whens,
                 (char *)when,(CmpFunc)CmpWhens)==0)
      gl_append_ptr(BA_INST(i)->whens,(VOIDPTR)when);
    break;
  case INTEGER_ATOM_INST:
    if (IA_INST(i)->whens==NULL)
      IA_INST(i)->whens = gl_create(AVG_WHEN);
    len = gl_length(IA_INST(i)->whens);
    if (gl_search(IA_INST(i)->whens,
                 (char *)when,(CmpFunc)CmpWhens)==0)
      gl_append_ptr(IA_INST(i)->whens,(VOIDPTR)when);
    break;
  case SYMBOL_ATOM_INST:
    if (SYMA_INST(i)->whens==NULL)
      SYMA_INST(i)->whens = gl_create(AVG_WHEN);
    len = gl_length(SYMA_INST(i)->whens);
    if (gl_search(SYMA_INST(i)->whens,
                 (char *)when,(CmpFunc)CmpWhens)==0)
      gl_append_ptr(SYMA_INST(i)->whens,(VOIDPTR)when);
    break;
  case BOOLEAN_CONSTANT_INST:
    if (BC_INST(i)->whens==NULL)
      BC_INST(i)->whens = gl_create(AVG_WHEN);
    len = gl_length(BC_INST(i)->whens);
    if (gl_search(BC_INST(i)->whens,
                 (char *)when,(CmpFunc)CmpWhens)==0)
      gl_append_ptr(BC_INST(i)->whens,(VOIDPTR)when);
    break;
  case INTEGER_CONSTANT_INST:
    if (IC_INST(i)->whens==NULL)
      IC_INST(i)->whens = gl_create(AVG_WHEN);
    len = gl_length(IC_INST(i)->whens);
    if (gl_search(IC_INST(i)->whens,
                 (char *)when,(CmpFunc)CmpWhens)==0)
      gl_append_ptr(IC_INST(i)->whens,(VOIDPTR)when);
    break;
  case SYMBOL_CONSTANT_INST:
    if (SYMC_INST(i)->whens==NULL)
      SYMC_INST(i)->whens = gl_create(AVG_WHEN);
    len = gl_length(SYMC_INST(i)->whens);
    if (gl_search(SYMC_INST(i)->whens,
                 (char *)when,(CmpFunc)CmpWhens)==0)
      gl_append_ptr(SYMC_INST(i)->whens,(VOIDPTR)when);
    break;
  case MODEL_INST:
    if (MOD_INST(i)->whens==NULL)
      MOD_INST(i)->whens = gl_create(AVG_WHEN);
    len = gl_length(MOD_INST(i)->whens);
    if (gl_search(MOD_INST(i)->whens,
                 (char *)when,(CmpFunc)CmpWhens)==0)
      gl_append_ptr(MOD_INST(i)->whens,(VOIDPTR)when);
    break;
  case REL_INST:
    if (RELN_INST(i)->whens==NULL)
      RELN_INST(i)->whens = gl_create(AVG_WHEN);
    len = gl_length(RELN_INST(i)->whens);
    if (gl_search(RELN_INST(i)->whens,
                 (char *)when,(CmpFunc)CmpWhens)==0)
      gl_append_ptr(RELN_INST(i)->whens,(VOIDPTR)when);
    break;
  case LREL_INST:
    if (LRELN_INST(i)->whens==NULL)
      LRELN_INST(i)->whens = gl_create(AVG_WHEN);
    len = gl_length(LRELN_INST(i)->whens);
    if (gl_search(LRELN_INST(i)->whens,
                 (char *)when,(CmpFunc)CmpWhens)==0)
      gl_append_ptr(LRELN_INST(i)->whens,(VOIDPTR)when);
    break;
  case WHEN_INST:
    if (W_INST(i)->whens==NULL)
      W_INST(i)->whens = gl_create(AVG_WHEN);
    len = gl_length(W_INST(i)->whens);
    if (gl_search(W_INST(i)->whens,
                 (char *)when,(CmpFunc)CmpWhens)==0)
      gl_append_ptr(W_INST(i)->whens,(VOIDPTR)when);
    break;
  default:
    Asc_Panic(2, NULL, "AddWhen called with inappropriate argument.\n");
  }
}

void RemoveWhen(struct Instance *i, struct Instance *when)
{
  register unsigned long c;
  assert(i&&when&&(when->t==WHEN_INST));
  AssertMemory(i);
  switch(i->t) {
  case BOOLEAN_ATOM_INST:
    if (BA_INST(i)->whens==NULL) return;
    c = gl_search(BA_INST(i)->whens,
                 (char *)when,(CmpFunc)CmpWhens);
    if (c>0) gl_delete(BA_INST(i)->whens,c,0);
    break;
  case INTEGER_ATOM_INST:
    if (IA_INST(i)->whens==NULL) return;
    c = gl_search(IA_INST(i)->whens,
                 (char *)when,(CmpFunc)CmpWhens);
    if (c>0) gl_delete(IA_INST(i)->whens,c,0);
    break;
  case SYMBOL_ATOM_INST:
    if (SYMA_INST(i)->whens==NULL) return;
    c = gl_search(SYMA_INST(i)->whens,
                 (char *)when,(CmpFunc)CmpWhens);
    if (c>0) gl_delete(SYMA_INST(i)->whens,c,0);
    break;
  case BOOLEAN_CONSTANT_INST:
    if (BC_INST(i)->whens==NULL) return;
    c = gl_search(BC_INST(i)->whens,
                 (char *)when,(CmpFunc)CmpWhens);
    if (c>0) gl_delete(BC_INST(i)->whens,c,0);
    break;
  case INTEGER_CONSTANT_INST:
    if (IC_INST(i)->whens==NULL) return;
    c = gl_search(IC_INST(i)->whens,
                 (char *)when,(CmpFunc)CmpWhens);
    if (c>0) gl_delete(IC_INST(i)->whens,c,0);
    break;
  case SYMBOL_CONSTANT_INST:
    if (SYMC_INST(i)->whens==NULL) return;
    c = gl_search(SYMC_INST(i)->whens,
                 (char *)when,(CmpFunc)CmpWhens);
    if (c>0) gl_delete(SYMC_INST(i)->whens,c,0);
    break;
  case MODEL_INST:
    if (MOD_INST(i)->whens==NULL) return;
    c = gl_search(MOD_INST(i)->whens,
                 (char *)when,(CmpFunc)CmpWhens);
    if (c>0) gl_delete(MOD_INST(i)->whens,c,0);
    break;
  case REL_INST:
    if (RELN_INST(i)->whens==NULL) return;
    c = gl_search(RELN_INST(i)->whens,
                 (char *)when,(CmpFunc)CmpWhens);
    if (c>0) gl_delete(RELN_INST(i)->whens,c,0);
    break;
  case LREL_INST:
    if (LRELN_INST(i)->whens==NULL) return;
    c = gl_search(LRELN_INST(i)->whens,
                 (char *)when,(CmpFunc)CmpWhens);
    if (c>0) gl_delete(LRELN_INST(i)->whens,c,0);
    break;
  case WHEN_INST:
    if (W_INST(i)->whens==NULL) return;
    c = gl_search(W_INST(i)->whens,
                 (char *)when,(CmpFunc)CmpWhens);
    if (c>0) gl_delete(W_INST(i)->whens,c,0);
    break;
  default:
    Asc_Panic(2, NULL, "Bad argument to RemoveWhen.\n");
    /*NOTREACHED*/
  }
}

