/*
 *  Scanner Module
 *  by Tom Epperly
 *  7/21/89
 *  Part of ASCEND
 *  Version: $Revision: 1.11 $
 *  Version control file: $RCSfile: scanner.h,v $
 *  Date last modified: $Date: 1998/02/18 22:57:22 $
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

/*
 *  This module is a module to convert a stream of character into a stream
 *  of tokens.  It has been defined to be consistent with the routines
 *  required by the common compiler-compiler yacc.
 */

/*
 *  When #including scanner.h, make sure these files are #included first:
 *         #include "compiler.h"
 */


#ifndef __SCANNER_H_SEEN__
#define __SCANNER_H_SEEN__


#define ZZ_MAXLEN MAXTOKENLENGTH
/* maximum token size(in characters).  This
 * must be greater than 2 and usually much
 * greater than 2
 */

extern
int zz_lex(void);
/*
 *  int zz_lex();
 *  Declaration of the Lexical Analyzer Function generated by Flex.
 *  (This is typically only called by the parser.)
 */

extern
unsigned long LineNum(void);
/*
 *  unsigned long LineNum();
 *
 *  This function returns the line number position of the scanner.
 */

extern
void Asc_ScannerAssignFile(FILE *,unsigned long);
/*
 *  Asc_ScannerAssignFile(f,linenum)
 *  FILE *f;
 *  unsigned long linenum;
 *
 *  Change the input to file f and reset the line count to linenum.
 */

extern
void Asc_ScannerAssignString(void *,unsigned long,int);
/*
 *  Asc_ScannerAssignString(yybs,linenum,first)
 *  unsigned long linenum;
 *  int first;
 *
 *  Change the input to string buffer yybs and reset the line count to linenum.
 *  If first !=0, calls BEGIN(INITIAL) as well.
 *  Current (file) buffer is pushed on the stack.
 */

extern
void *Asc_ScannerCreateStringBuffer(CONST char *, int);
/*
 * yybs = (YY_BUFFER_STATE)Asc_ScannerCreateStringBuffer(string,len);
 * Pushes current buffer on stack and returns a YY_BUFFER_STATE
 * (as void *) created from the string given. len is the length
 * of the string given and is passed to yy_scan_bytes.
 */

extern
void Asc_ScannerReleaseStringBuffer(void *);
/*
 * Asc_ScannerReleaseStringBuffer(yybs);
 * Pops the buffer stack. Do not call this without a matching
 * call to Asc_ScannerCreateStringBuffer preceding it.
 * This function is NOT the opposite of Asc_ScannerPushBuffer.
 */

extern
int Asc_ScannerPushBuffer(CONST char *);
/*
 *  void Asc_ScannerPushBuffer(filename)
 *  const char *filename;
 *
 *  Create a new scanner buffer to parse the file named in filename.  This
 *  function calls Asc_RequireModule to open the file and create the module
 *  definition and prepares the scanner for parsing the file.
 *  Returns 0 on success or error codes: 1 if the REQUIRE statements are
 *  too deeply nested, or 2 if the call to Asc_RequireModule fails.
 */

extern
void Asc_ErrMsgTypeDefnEOF(void);
/*
 *  void Asc_ErrMsgTypeDefnEOF()
 *  const char *filename;
 *
 *  Print an error message on the filehandle ASCERR that an End of File
 *  was reached inside a type definition.
 *  Note: This function is actually defined in ascParse.y (to avoid making
 *  g_type_name a global variable) and only used in scanner.l.  Since there
 *  is no easy way to have yacc define a header file of function
 *  declarations, we are declaring it here.
 */

extern
void Asc_DestroyScannerWorkBuffer(void);
/*
 *  void Asc_DestroyScannerWorkBuffer()
 *
 *  Free the memory used by the Work Buffer in the scanner.  Typically
 *  you only want to call this at shutdown or when you've completely
 *  finished with the scanner; however, it should do the right thing if
 *  you destroy the Work Buffer and then scan another file.
 */

extern
void Asc_DestroyScannerInputBuffer(void);
/*
 * calls yy_delete_buffer on the 0th file buffer that we get at
 * startup. Call this only at the final shutdown of the scanner.
 */
#endif /* __SCANNER_H_SEEN__ */
