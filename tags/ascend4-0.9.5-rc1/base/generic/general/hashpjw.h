/*
 *  Hash function
 *  by Tom Epperly
 *  10/24/89
 *  Version: $Revision: 1.1 $
 *  Version control file: $RCSfile: hashpjw.h,v $
 *  Date last modified: $Date: 1997/07/18 11:38:39 $
 *  Last modified by: $Author: mthomas $
 */

/** @file
 *  Hash function.
 *  <pre>
 *  When #including hashpjw.h, make sure these files are #included first:
 *         #include "utilities/ascConfig.h"
 *         #include "compiler.h"
 *  </pre>
 */

#ifndef __HASHPJW_H_SEEN__
#define __HASHPJW_H_SEEN__

extern unsigned long hashpjw(CONST char *str, unsigned long size);
/**<
 *  <!--  unsigned long hashpjw(str,size)                              -->
 *  <!--  CONST char *str;                                             -->
 *  <!--  unsigned long size;                                          -->
 *  Return a hash value base on str.  The value will be >= 0 and < size.
 */

extern unsigned long hashpjw_int(int id, unsigned long size);
/**<
 *  <!--  unsigned long hashpjw_id(id,size)                            -->
 *  <!--  int id;                                                      -->
 *  <!--  unsigned long size;                                          -->
 *  Return a hash value base on id.  The value will be >= 0 and < size.
 *  This function at the moment does a conversion of the integer to a string
 *  and so is relatively expensive. This needs to be cleaned up, with a
 *  dedicated integer hashing function.
 */

#endif /* __HASHPJW_H_SEEN__ */
