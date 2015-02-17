/* 
 *  Initialization Routines
 *  by Tom Epperly
 *  Created: 3/24/1990
 *  Version: $Revision: 1.11 $
 *  Version control file: $RCSfile: initialize.h,v $
 *  Date last modified: $Date: 1998/03/17 22:08:40 $
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
 */

/** @file
 *  Initialization Routines.
 *  <pre>
 *  When #including initialize.h, make sure these files are #included first:
 *         #include "utilities/ascConfig.h"
 *         #include "instance_enum.h"
 *         #include "fractions.h"
 *         #include "compiler.h"
 *         #include "dimen.h"
 *         #include "types.h"
 *  </pre>
 */

#ifndef __INITIALIZE_H_SEEN__
#define __INITIALIZE_H_SEEN__

extern void SetProcStackLimit(unsigned long l);
/**<
 *  <!--  void SetProcStackLimit(l);                                   -->
 *  Sets the procedure stack limit = l.
 *  The stack limit starts out at INITSTACKLIMIT.
 *  The limit exists to prevent infinite loops from running
 *  the machine out of C automatic variable space.
 */

/** Initial stack limit. */
#define INITSTACKLIMIT 20

extern unsigned long GetProcStackLimit(void);
/**<
 *  <!--  int GetProcStackLimit();                                     -->
 *  Gets the procedure stack limit currently set.
 */

extern enum Proc_enum Initialize(struct Instance *context,
                                 struct Name *name,
                                 char *cname,
                                 FILE *err,
                                 wpflags options,
                                 struct gl_list_t *watchpoints,
                                 FILE *log);
/**<
 *  <!--  enum Proc_enum Initialize(context,name,cname,err,options,watchpoints,log)  -->
 *  <!--  struct Instance *context;                                    -->
 *  <!--  struct Name *name;                                           -->
 *  <!--  char *cname;                                                 -->
 *  <!--  FILE *err;                                                   -->
 *  <!--  wpflags options;                                             -->
 *  <!--  watchlist watchpoints;                                       -->
 *  <!--  FILE *log;                                                   -->
 *  This procedure will execute the initialization code indicated by name
 *  with respect to the context instance.
 *  cname is the string form of the instance name used in issuing
 *  error messages.
 *  Will return Proc_all_ok if all went well; otherwise it will return
 *  one of the error codes above.<br><br>
 *
 *  If watchlist is NULL or flog is NULL, the debug output options
 *  corresponding to watchlist and flog will be ignored.
 *  flog and ferr should not be the same pointer in a good ui design.
 *  error (and possibly debugging) messages are issued on the files given.
 *  Maximum speed comes from 
 *  Initialize(context,procname,cname,ferr,0,NULL,NULL);
 */

extern enum Proc_enum ClassAccessInitialize(struct Instance *context,
                                            struct Name *class,
                                            struct Name *name,
                                            char *cname,
                                            FILE *err,
                                            wpflags options,
                                            struct gl_list_t *watchpoints,
                                            FILE *log);
/**<
 *  <!--  enum Proc_enum ClassAccessInitialize(context, class, name,   -->
 *  <!--                                       cname, err, options, watchpoints, log);  -->
 *  <!--  struct Instance *context;                                    -->
 *  <!--  struct Name *class;                                          -->
 *  <!--  struct Name *name;                                           -->
 *  <!--  char *cname;                                                 -->
 *  <!--  FILE *err;                                                   -->
 *  <!--  wpflags options;                                             -->
 *  <!--  watchlist watchpoints;                                       -->
 *  <!--  FILE *log;                                                   -->
 *  Will attempt to run the initialization procedure given by using so-called
 *  class access, i.e., deals with syntax such as RUN Mytype::values.
 *  The context instance will be used.
 *  Will return Proc_all_ok if all went well; otherwise it will return
 *  one of the error codes above.<br><br>
 *
 *  If watchlist is NULL or flog is NULL, the debug output options
 *  corresponding to watchlist and flog will be ignored.
 *  error (and possibly debugging) messages are issued on the files given.
 *  Maximum speed comes from 
 *  ClassAccessInitialize(context,class,procname,cname,ferr,0,NULL,NULL);
 */

extern struct InitProcedure *FindProcedure(CONST struct Instance *i,
                                           symchar *procname);
/**<
 *  <!--  struct InitProcedure *FindProcedure(i,procname)              -->
 *  Search an Instance for a named procedure.
 *  This will return the pointer to the named procedure if it exists in i.
 *  Returns null if procname not found.
 */

extern struct InitProcedure *SearchProcList(CONST struct gl_list_t *list,
                                            symchar *name);
/**<
 * <!--  struct InitProcedure *proc = SearchProcList(list,name);       -->
 *  Search a list for a named procedure.
 *  list generally comes from GetInitializationList().
 *  Returns null if name not found.
 */

#endif /* __INITIALIZE_H_SEEN__ */
