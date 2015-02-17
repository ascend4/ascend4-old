/*
 *  Ascend Instance Tree Link Management
 *  by Tom Epperly & Ben Allan
 *  9/3/89
 *  Version: $Revision: 1.13 $
 *  Version control file: $RCSfile: linkinst.c,v $
 *  Date last modified: $Date: 1998/02/05 16:36:59 $
 *  Last modified by: $Author: ballan $
 *
 *  This file is part of the Ascend Language Interpreter.
 *
 *  Copyright (C) 1996 Ben Allan
 *  based on instance.c
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
#include "utilities/ascPanic.h"
#include "utilities/ascMalloc.h"
#include "general/list.h"
#include "general/dstring.h"
#include "compiler/compiler.h"
#include "compiler/symtab.h"
#include "compiler/bit.h"
#include "compiler/fractions.h"
#include "compiler/dimen.h"
#include "compiler/functype.h"
#include "compiler/types.h"
#include "compiler/instance_enum.h"
#include "compiler/instance_name.h"
#include "compiler/instance_io.h"
#include "compiler/check.h"
#include "compiler/dump.h"
#include "compiler/child.h"
#include "compiler/type_desc.h"
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
#include "general/pool.h"
#include "compiler/instance_types.h"
/* new */
#include "compiler/atomsize.h"
#include "compiler/atomvalue.h"
#include "compiler/cmpfunc.h"
#include "compiler/destroyinst.h"
#include "compiler/instmacro.h"
#include "compiler/instquery.h"
#include "compiler/linkinst.h"
#include "compiler/mathinst.h"
#include "compiler/parentchild.h"

#ifndef lint
static CONST char LinkInstModuleID[] = "$Id: linkinst.c,v 1.13 1998/02/05 16:36:59 ballan Exp $";
#endif


void ChangeRelationPointers(struct Instance *rel, struct Instance *old,
			    struct Instance *new)
{
  assert(rel!=NULL);
  assert(rel->t==REL_INST);
  AssertMemory(rel);
  if (RELN_INST(rel)->ptr!=NULL) {
    switch (RELN_INST(rel)->type) {
    case e_token:
      ModifyTokenRelationPointers(rel,RELN_INST(rel)->ptr,old,new);
      return;
    case e_glassbox:
      ModifyGlassBoxRelPointers(rel,RELN_INST(rel)->ptr,old,new);
      return;
    case e_blackbox:
      ModifyBlackBoxRelPointers(rel,RELN_INST(rel)->ptr,old,new);
      return;
    case e_undefined:
    default:
      Asc_Panic(2, NULL, "Unknown relation type in ChangeRelationPointers\n");
    }
  }
}

void ChangeLogRelPointers(struct Instance *lrel, struct Instance *old,
			  struct Instance *new)
{
  struct gl_list_t *varlist,*rellist;
  struct Instance *inst;
  struct logrelation *logrel;

  logrel = LRELN_INST(lrel)->ptr;
  varlist = logrel->bvars;
  rellist = logrel->satrels;

  if (new == NULL) {
    inst = old;
  }
  else {
    inst = new;
  }
  assert(lrel->t==LREL_INST);
  AssertMemory(lrel);
  switch(inst->t){
    case BOOLEAN_ATOM_INST:
      if (logrel!=NULL){
        ModifyLogRelPointers(varlist,logrel,old,new);
      return;
      }
      break;
    case REL_INST:
    case LREL_INST:
      if (logrel!=NULL){
        ModifyLogRelPointers(rellist,logrel,old,new);
      return;
      }
      break;
    default:
      Asc_Panic(2, "ChangeLogRelPointers",
                "Wrong instance type passed to ChangeLogRelPointers\n");
  }
}

void ChangeWhenPointers(struct Instance *when, struct Instance *old,
			struct Instance *new)
{
  struct gl_list_t *varlist,*caselist,*reflist;
  struct Instance *scratch;
  struct Case *cur_case;
  unsigned long c,len,pos;

  varlist = W_INST(when)->bvar;
  caselist = W_INST(when)->cases;
  len = gl_length(caselist);

  if (new == NULL) {
    scratch = old;
  }
  else {
    scratch = new;
  }
  assert(when->t==WHEN_INST);
  AssertMemory(when);
  switch(scratch->t){
    case BOOLEAN_ATOM_INST:
    case INTEGER_ATOM_INST:
    case SYMBOL_ATOM_INST:
    case BOOLEAN_CONSTANT_INST:
    case INTEGER_CONSTANT_INST:
    case SYMBOL_CONSTANT_INST:
      if (varlist!=NULL){
        ModifyWhenPointers(varlist,old,new);
        return;
      }
    case MODEL_INST:
    case WHEN_INST:
    case REL_INST:
    case LREL_INST:
      for(c=1;c<=len;c++) {
        cur_case = (struct Case *)gl_fetch(caselist,c);
        reflist = GetCaseReferences(cur_case);
        if (reflist != NULL) {
          if ((pos = gl_search(reflist,old,(CmpFunc)CmpP)))
            ModifyWhenPointers(reflist,old,new);
	}
      }
    return;
    default:
      Asc_Panic(2, NULL, "ChangeWhenPointers receives wrong instance.\n");
  }
}

/*********************************************************************\
Tell parent to change pointers to oldchild to pointers to newchild.
\*********************************************************************/
void ChangeParent(struct Instance *parent, struct Instance *oldchild,
		  struct Instance *newchild)
{
  register unsigned long c,length;
  AssertMemory(parent);
  length = NumberChildren(parent);
  for(c=1;c<=length;c++){
    if (InstanceChild(parent,c)==oldchild) {
      StoreChildPtr(parent,c,newchild);
    }
  }
}

void ReDirectParents(struct Instance *old, struct Instance *new)
{
  register struct Instance *parent;
  register unsigned long index1,length;
  length = NumberParents(new);
  for(index1=1;index1<=length;index1++) {
    parent = InstanceParent(new,index1);
    ChangeParent(parent,old,new);
  }
}

void ReDirectChildren(struct Instance *old, struct Instance *new)
{
  register struct Instance *child;
  register unsigned long c,length,pos;
  length = NumberChildren(new);
  for(c=1;c<=length;c++){
    if ((child = InstanceChild(new,c))!=NULL){
      if ((pos = SearchForParent(child,old))!=0){
	DeleteParent(child,pos);
	AddParent(child,new);
      }
    }
  }
}


void ReorderChildrenPtrs(register struct Instance **c,
			 register CONST ChildListPtr old,
			 register CONST ChildListPtr new,
			 register unsigned long int olen,
			 register unsigned long int nlen)
{
  register unsigned nzero;
  if (olen==0) return;
  nzero = nlen-olen;
  while (nlen > 0) {
    if (ChildStrPtr(new,nlen) == ChildStrPtr(old,olen)) {
      /* move pointer at olen-1 to nlen-1 and put NULL in olen-1 */
      c[--nlen] = c[--olen];
      c[olen] = NULL;
      if (olen==0)return;
    } else {
      nlen--;
      if (--nzero==0) return;
    }
  }
}

void FixCliques(struct Instance *old, struct Instance *new)

/*********************************************************************\
Remove old from the clique put new in its place.
\*********************************************************************/
{
  register struct Instance *ptr,*next;
  ptr = new;
  /*  SetNextCliqueMember(ptr,NextCliqueMember(old)); not needed */
  while((next=NextCliqueMember(ptr))!=old)
    ptr = next;
  SetNextCliqueMember(ptr,new);
  /*  SetNextCliqueMember(old,old); illegal since old is no longer allocated */
}


/* this is called to tell relations about a change in variable location
 * e.g. If two atoms are merged, point all the relations that know about
 * ATOM old to ATOM new.
 */
void FixRelations(struct RealAtomInstance *old, struct RealAtomInstance *new)
{
  register unsigned long c,len;
  AssertMemory(old);
  AssertMemory(new);
  if ((new->relations==NULL)||(new->relations==old->relations)){
    new->relations = old->relations;
    if ((len=RelationsCount(INST(new)))>0){
      for(c=1;c<=len;c++) {
	ChangeRelationPointers(RelationsForAtom(INST(new),c),
			       INST(old),INST(new));
      }
    }
  } else {
    len=RelationsCount(INST(old));
    if (len>0) {
      for(c=1;c<=len;c++){
	ChangeRelationPointers(RelationsForAtom(INST(old),c),
			       INST(old),INST(new));
	AddRelation(INST(new),RelationsForAtom(INST(old),c));
      }
    }
    if (old->relations) {
      gl_destroy(old->relations);
    }
  }
  old->relations=NULL;
}


static
void FixLogRelationsIf(struct Instance *old, struct Instance *new)
{
  register unsigned long c,len;
  if ((len=LogRelationsCount(new))>0){
    for(c=1;c<=len;c++) {
      ChangeLogRelPointers(LogRelationsForInstance(new,c),old,new);
    }
  }
}

static
void FixLogRelationsElse(struct Instance *old, struct Instance *new)
{
  register unsigned long c,len;
  if ((len=LogRelationsCount(INST(old)))>0){
    for(c=1;c<=len;c++){
      ChangeLogRelPointers(LogRelationsForInstance(old,c),old,new);
      AddLogRel(new,LogRelationsForInstance(old,c));
    }
  }
}


void FixLogRelations(struct Instance *old,
                     struct Instance *new)
{
  switch(old->t){
    case BOOLEAN_ATOM_INST:
      AssertMemory(BA_INST(old));
      AssertMemory(BA_INST(new));
      if((BA_INST(new)->logrelations==NULL) ||
         (BA_INST(new)->logrelations==BA_INST(old)->logrelations))  {
        BA_INST(new)->logrelations = BA_INST(old)->logrelations;
        FixLogRelationsIf(old,new);
      }
      else {
        FixLogRelationsElse(old,new);
        if (BA_INST(old)->logrelations) gl_destroy(BA_INST(old)->logrelations);
      }
      BA_INST(old)->logrelations=NULL;
      break;
    case REL_INST:
      AssertMemory(RELN_INST(old));
      AssertMemory(RELN_INST(new));
      if((RELN_INST(new)->logrels==NULL) ||
         (RELN_INST(new)->logrels==RELN_INST(old)->logrels))  {
        RELN_INST(new)->logrels = RELN_INST(old)->logrels;
        FixLogRelationsIf(old,new);
      }
      else {
        FixLogRelationsElse(old,new);
        if (RELN_INST(old)->logrels) gl_destroy(RELN_INST(old)->logrels);
      }
      RELN_INST(old)->logrels=NULL;
      break;
    case LREL_INST:
      AssertMemory(LRELN_INST(old));
      AssertMemory(LRELN_INST(new));
      if((LRELN_INST(new)->logrels==NULL) ||
         (LRELN_INST(new)->logrels==LRELN_INST(old)->logrels))  {
        LRELN_INST(new)->logrels = LRELN_INST(old)->logrels;
        FixLogRelationsIf(old,new);
      }
      else {
        FixLogRelationsElse(old,new);
        if (LRELN_INST(old)->logrels) gl_destroy(LRELN_INST(old)->logrels);
      }
      LRELN_INST(old)->logrels=NULL;
      break;
    default:
      Asc_Panic(2, "FixLogRelationss",
                "FixLogRelations called with inappropriate argument.\n");
  }
}

static
void FixWhensIf(struct Instance *old, struct Instance *new)
{
  register unsigned long c,len;
  if ((len=WhensCount(new))>0){
    for(c=1;c<=len;c++) {
      ChangeWhenPointers(WhensForInstance(new,c),old,new);
    }
  }
}

static
void FixWhensElse(struct Instance *old, struct Instance *new)
{
  register unsigned long c,len;
  if ((len=WhensCount(INST(old)))>0){
    for(c=1;c<=len;c++){
      ChangeWhenPointers(WhensForInstance(old,c),old,new);
      AddWhen(new,WhensForInstance(old,c));
    }
  }
}

void FixWhens(struct Instance *old, struct Instance *new)
{
  switch(old->t){
    case BOOLEAN_ATOM_INST:
      AssertMemory(BA_INST(old));
      AssertMemory(BA_INST(new));
      if((BA_INST(new)->whens==NULL) ||
        (BA_INST(new)->whens==BA_INST(old)->whens))  {
        BA_INST(new)->whens = BA_INST(old)->whens;
        FixWhensIf(old,new);
      }
      else{
        FixWhensElse(old,new);
        if (BA_INST(old)->whens) gl_destroy(BA_INST(old)->whens);
      }
      BA_INST(old)->whens=NULL;
      break;
    case INTEGER_ATOM_INST:
      AssertMemory(IA_INST(old));
      AssertMemory(IA_INST(new));
      if((IA_INST(new)->whens==NULL) ||
        (IA_INST(new)->whens==IA_INST(old)->whens))  {
        IA_INST(new)->whens = IA_INST(old)->whens;
        FixWhensIf(old,new);
      }
      else{
        FixWhensElse(old,new);
        if (IA_INST(old)->whens) gl_destroy(IA_INST(old)->whens);
      }
      IA_INST(old)->whens=NULL;
      break;
    case SYMBOL_ATOM_INST:
      AssertMemory(SYMA_INST(old));
      AssertMemory(SYMA_INST(new));
      if((SYMA_INST(new)->whens==NULL) ||
        (SYMA_INST(new)->whens==SYMA_INST(old)->whens))  {
        SYMA_INST(new)->whens = SYMA_INST(old)->whens;
        FixWhensIf(old,new);
      }
      else{
        FixWhensElse(old,new);
        if (SYMA_INST(old)->whens) gl_destroy(SYMA_INST(old)->whens);
      }
      SYMA_INST(old)->whens=NULL;
      break;
    case BOOLEAN_CONSTANT_INST:
      AssertMemory(BC_INST(old));
      AssertMemory(BC_INST(new));
      if((BC_INST(new)->whens==NULL) ||
        (BC_INST(new)->whens==BC_INST(old)->whens))  {
        BC_INST(new)->whens = BC_INST(old)->whens;
        FixWhensIf(old,new);
      }
      else{
        FixWhensElse(old,new);
        if (BC_INST(old)->whens) gl_destroy(BC_INST(old)->whens);
      }
      BC_INST(old)->whens=NULL;
      break;
    case INTEGER_CONSTANT_INST:
      AssertMemory(IC_INST(old));
      AssertMemory(IC_INST(new));
      if((IC_INST(new)->whens==NULL) ||
        (IC_INST(new)->whens==IC_INST(old)->whens))  {
        IC_INST(new)->whens = IC_INST(old)->whens;
        FixWhensIf(old,new);
      }
      else{
        FixWhensElse(old,new);
        if (IC_INST(old)->whens) gl_destroy(IC_INST(old)->whens);
      }
      IC_INST(old)->whens=NULL;
      break;
    case SYMBOL_CONSTANT_INST:
      AssertMemory(SYMC_INST(old));
      AssertMemory(SYMC_INST(new));
      if((SYMC_INST(new)->whens==NULL) ||
        (SYMC_INST(new)->whens==SYMC_INST(old)->whens))  {
        SYMC_INST(new)->whens = SYMC_INST(old)->whens;
        FixWhensIf(old,new);
      }
      else{
        FixWhensElse(old,new);
        if (SYMC_INST(old)->whens) gl_destroy(SYMC_INST(old)->whens);
      }
      SYMC_INST(old)->whens=NULL;
      break;
    case MODEL_INST:
      AssertMemory(MOD_INST(old));
      AssertMemory(MOD_INST(new));
      if((MOD_INST(new)->whens==NULL) ||
        (MOD_INST(new)->whens==MOD_INST(old)->whens))  {
        MOD_INST(new)->whens = MOD_INST(old)->whens;
        FixWhensIf(old,new);
      }
      else{
        FixWhensElse(old,new);
        if (MOD_INST(old)->whens) gl_destroy(MOD_INST(old)->whens);
      }
      MOD_INST(old)->whens=NULL;
      break;
    case REL_INST:
      AssertMemory(RELN_INST(old));
      AssertMemory(RELN_INST(new));
      if((RELN_INST(new)->whens==NULL) ||
        (RELN_INST(new)->whens==RELN_INST(old)->whens))  {
        RELN_INST(new)->whens = RELN_INST(old)->whens;
        FixWhensIf(old,new);
      }
      else{
        FixWhensElse(old,new);
        if (RELN_INST(old)->whens) gl_destroy(RELN_INST(old)->whens);
      }
      RELN_INST(old)->whens=NULL;
      break;
    case LREL_INST:
      AssertMemory(LRELN_INST(old));
      AssertMemory(LRELN_INST(new));
      if((LRELN_INST(new)->whens==NULL) ||
        (LRELN_INST(new)->whens==LRELN_INST(old)->whens))  {
        LRELN_INST(new)->whens = LRELN_INST(old)->whens;
        FixWhensIf(old,new);
      }
      else{
        FixWhensElse(old,new);
        if (LRELN_INST(old)->whens) gl_destroy(LRELN_INST(old)->whens);
      }
      LRELN_INST(old)->whens=NULL;
      break;
    case WHEN_INST:
      AssertMemory(W_INST(old));
      AssertMemory(W_INST(new));
      if((W_INST(new)->whens==NULL) ||
        (W_INST(new)->whens==W_INST(old)->whens))  {
        W_INST(new)->whens = W_INST(old)->whens;
        FixWhensIf(old,new);
      }
      else{
        FixWhensElse(old,new);
        if (W_INST(old)->whens) gl_destroy(W_INST(old)->whens);
      }
      W_INST(old)->whens=NULL;
      break;
    default:
      Asc_Panic(2, "FixWhens",
                "FixWhens called with inappropriate instance type.\n");
  }
}


void FixWhensForRefinement(struct Instance *old, struct Instance *new)
{
  switch(new->t){
    case BOOLEAN_ATOM_INST:
      AssertMemory(BA_INST(new));
      if(BA_INST(new)->whens!=NULL) {
        FixWhensIf(old,new);
      }
      break;
    case INTEGER_ATOM_INST:
      AssertMemory(IA_INST(new));
      if(IA_INST(new)->whens!=NULL) {
        FixWhensIf(old,new);
      }
      break;
    case SYMBOL_ATOM_INST:
      AssertMemory(SYMA_INST(new));
      if(SYMA_INST(new)->whens!=NULL) {
        FixWhensIf(old,new);
      }
      break;
    case BOOLEAN_CONSTANT_INST:
      AssertMemory(BC_INST(new));
      if(BC_INST(new)->whens!=NULL) {
        FixWhensIf(old,new);
      }
      break;
    case INTEGER_CONSTANT_INST:
      AssertMemory(IC_INST(new));
      if(IC_INST(new)->whens!=NULL) {
        FixWhensIf(old,new);
      }
      break;
    case SYMBOL_CONSTANT_INST:
      AssertMemory(SYMC_INST(new));
      if(SYMC_INST(new)->whens!=NULL) {
        FixWhensIf(old,new);
      }
      break;
    case MODEL_INST:
      AssertMemory(MOD_INST(new));
      if(MOD_INST(new)->whens!=NULL) {
        FixWhensIf(old,new);
      }
      break;
    case REL_INST:
      AssertMemory(RELN_INST(new));
      if(RELN_INST(new)->whens!=NULL) {
        FixWhensIf(old,new);
      }
      break;
    case LREL_INST:
      AssertMemory(LRELN_INST(new));
      if(LRELN_INST(new)->whens!=NULL) {
        FixWhensIf(old,new);
      }
      break;
    case WHEN_INST:
      AssertMemory(W_INST(new));
      if(W_INST(new)->whens!=NULL) {
        FixWhensIf(old,new);
      }
      break;
    default:
      Asc_Panic(2, NULL,
                "FixWhensForRefinement called with"
                " inappropriate instance type.\n");
  }
}
