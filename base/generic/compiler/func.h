/*
 *  Function module
 *  by Tom Epperly
 *  Version: $Revision: 1.16 $
 *  Version control file: $RCSfile: func.h,v $
 *  Date last modified: $Date: 2001/01/31 22:23:57 $
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
 *  Revision notes:
 *  If CHRIS_FUNC defined, auxillary quantities to func structure.
 *  2/96 baa Probably somebody should properly set these evaluation
 *  defined below based on math.h when possible.
 *
 */

/** @file
 *  Function module.
 *  <pre>
 *  When #including func.h, make sure these files are #included first:
 *         #include "utilities/ascConfig.h"
 *         #include "fractions.h"
 *         #include "compiler.h"
 *         #include "dimen.h"
 *         #include "functype.h"
 *  </pre>
 *  @todo Complete documentation of func.h.
 */

#ifndef ASC_FUNC_H
#define ASC_FUNC_H

#include "general/mathmacros.h"

/*
 *  the following should be ifdefed to deal with math.h values
 */
#define F_ERF_COEF    1.1283791670955130      /**< = 2 / sqrt(PI) */
#define F_LOG10_COEF  0.4342944819032518      /**< = log10(e) = 1/ln(10) */
#define F_PI          3.1415926535897932385E0
#define F_PI_HALF     1.5707963267948966192E0
#define F_LIM_EXP     709.78                  /**< approximately ln(maxdouble) */
#define F_LIM_CUBE    5.6438030941223618e101  /**< cbrt(maxdouble)*/
#define F_LIM_SQR     1.0e154                 /**< sqrt(maxdouble) */

#ifdef __STDC__
# if __STDC__
/**
 * stdc==1 --> erf, cbrt not defined in headers. user should link
 * against a library that does provide them. ASCEND is research
 * code: we aren't going to waste time reimplementing these basic
 * functions.
 */
extern double DLEXPORT cbrt(double);
#  ifdef HAVE_ERF
extern double erf(double);
#  endif /* HAVE_ERF */
# endif /* __STDC__ == 1*/
/**<
 * in the case where __STDC__ is defined but == 0, system headers
 * should provide cbrt, erf.
 */
#endif /* stdc defined */

extern CONST struct Func *LookupFunc(CONST char *name);
/**<
 *  <!--  const struct Func *LookupFunc(name)                          -->
 *  <!--  const char *name; (ascend name, not C)                       -->
 *  Lookup the function with the given name.  If no match is found, it
 *  returns NULL.  name is the ASCEND name, not C name.
 *  <pre>
 *  Currently defined:
 *         "exp"           e^x
 *         "ln"            natural logarithm of x
 *         "log10"           logarithm of x base 10
 *         "sin"           sine of x
 *         "cos"           cosine of x
 *         "tan"           tangent of x
 *         "sqr"           x*x
 *         "sqrt"          the square root of x
 *         "arcsin"        the inverse sine of x
 *         "arccos"        the inverse cosine of x
 *         "arctan"        the inverse tangent of x
 *
 *         "erf"           the error function
 *         "lnm"           modified natural log:
 *                         x>epsilon? ln(x): x/epsilon +ln(epsilon) -1
 *         "sinh"          hyperbolic sine
 *         "cosh"          hyperbolic cosine
 *         "tanh"          hyperbolic tangent
 *         "arcsinh"       inv hyperbolic sine
 *         "arccosh"       inv hyperbolic cosine
 *         "arctanh"       inv hyperbolic tangent
 *
 *         "cube"          the cube of x
 *         "cbrt"          the cube root of x
 *         "abs"           absolute value of x
 *
 *         "hold"          returns the current value of x.
 *  </pre>
 *  The last 10 are not fully supported for chris.[ch]
 *  All functions return principal (postive) values if there is a choice.
 *  None of them perform range checking. ln(-1) -> float error.
 */

extern CONST struct Func *LookupFuncById(enum Func_enum id);
/**<
 *  <!--  const struct Func *LookupFuncById(id)                        -->
 *  <!--  enum Func_emum id;                                           -->
 *  Lookups a function by its enumerated type rather than by a string as
 *  in the above function.
 */

extern double FuncGetLnmEpsilon(void);
/**<
 *  Return the current epsilon for the modified log function lnm.
 */
#define FuncGetLnmEpsilon() (g_lnm_epsilon)

extern void FuncSetLnmEpsilon(double e);
/**<
 *  Change the current epsilon for the modified log function lnm.
 *  epsilon > 0.0.
 */
extern double g_lnm_epsilon;
#define FuncSetLnmEpsilon(e) \
   (e>(double)0.0 ? g_lnm_epsilon=e : FPRINTF(ASCERR,"bad lnm eps"))

/**<
 *  declare cbrt() and erf() since some vendors put
 *  these functions in odd headers
 */
extern double DLEXPORT cbrt(double);
#ifdef HAVE_ERF
extern double erf(double);
#endif /* HAVE_ERF */

#ifdef NDEBUG
# define ascnint(d) (((int) (d)>=0.0 ? floor((d) + 0.5) : -floor(0.5 - (d))))
/**< 
 *  Converts a double to the nearest integer (release version).
 *  @param d double, the real number to convert.
 *  @return The nearest integer as an int.
 */
#else
# define ascnint(a) ascnintF(a)
/**< 
 *  Converts a double to the nearest integer (debug version).
 *  @param d double, the real number to convert.
 *  @return The nearest integer as an int.
 */
extern int DLEXPORT ascnintF(double);
/**<
 *  Implementation function for debug version of ascnint().
 *  Do not call this function directly - use ascnint() instead.
 */

#endif

extern double DLEXPORT dln(double x);
extern double DLEXPORT dln2(double x);
extern double DLEXPORT dlog10(double x);
extern double DLEXPORT dlog102(double x);
extern double DLEXPORT lnm(double x);
extern double DLEXPORT dlnm(double x);
extern double DLEXPORT dlnm2(double x);
/**<
 *  Modified natural log function and derivatives.
 *  <pre>
 *
 *                   ( ln(x)                       if x.ge.epsilon
 *         lnm(x)=   (
 *                   ( x/epsilon + ln(epsilon) - 1      otherwise
 *
 *
 *                                  ( 1/x          if x.ge.epsilon
 *         d( lnm(x) )/dx       =   (
 *                                  ( 1/epsilon    otherwise
 *
 *
 *                                  ( -1/x^2       if x.ge.epsilon
 *         d^2( lnm(x) )/dx^2   =   (
 *                                  ( 0            otherwise
 *
 *  epsilon determined by FuncGet/SetLnmEpsilon.
 *  </pre>
 */

extern double DLEXPORT dtanh(double x);
extern double DLEXPORT dtanh2(double x);
extern double DLEXPORT arcsinh(double x);
extern double DLEXPORT arccosh(double x);
extern double DLEXPORT arctanh(double x);
extern double DLEXPORT darcsinh(double x);
extern double DLEXPORT darcsinh2(double x);
extern double DLEXPORT darccosh(double x);
extern double DLEXPORT darccosh2(double x);
extern double DLEXPORT darctanh(double x);
extern double DLEXPORT darctanh2(double x);
/**<
 *  Zero, first and second partials of (inverse) hyperbolic functions.
 */

extern double DLEXPORT sqr(double x);
extern double DLEXPORT dsqr(double x);
extern double DLEXPORT dsqr2(double x);
extern double DLEXPORT cube(double x);
extern double DLEXPORT dcube(double x);
extern double DLEXPORT dcube2(double x);
/**<
 *  Zero, first and second partials of x for sqr, cube.
 */

extern double DLEXPORT asc_ipow(double a, int n);
extern double DLEXPORT asc_d1ipow(double a, int n);
extern double DLEXPORT asc_d2ipow(double a, int n);
/**<
 *  Integer power function, a^n, and its first and second derivatives.
 *  d = asc_ipow(a,n);
 *  d1 = asc_d1ipow(a,n);
 *  d2 = asc_d2ipow(a,n);
 *
 *  Special cases ipow:
 *  a^0 = 1, 0^n = 0, 0^0=1 -- the last is mathematically undefined,
 *  so this function should not be called with 0.0,0.
 *
 *  Special cases d1ipow,d2ipow:
 */

extern double DLEXPORT hold(double x);
/**<
 *  Returns the value it is passed.
 *  The primary purpose is as an operator so we can write
 *  equations of the form x = hold(x) which act essentially
 *  as a fixed flag when a solver is partitioning.
 *  If x is fixed, the equation is singular.
 *  hold(x) is a constant value, so its derivatives are 0.
 */

extern double DLEXPORT dsqrt(double x);
extern double DLEXPORT dsqrt2(double x);
extern double DLEXPORT dcbrt(double x);
extern double DLEXPORT dcbrt2(double x);
extern double DLEXPORT dfabs(double x);
extern double DLEXPORT dfabs2(double x);
extern double DLEXPORT dhold(double x);
#define dhold2 dhold
/**<
 *  first and second partials of sqrt cbrt fabs hold
 *  dfabs is undefined at 0. We take the standard kluge: d(abs(x=0))/dx=0
 *  dfabs2(x) = 0. It might be thought of as infinite at x=0, but not here.
 *  dhold, dhold2 = 0 for all x.
 */

extern double DLEXPORT dasin(double x);
extern double DLEXPORT dasin2(double x);
extern double DLEXPORT dcos(double x);
extern double DLEXPORT dcos2(double x);
extern double DLEXPORT dacos(double x);
extern double DLEXPORT dacos2(double x);
extern double DLEXPORT dtan(double x);
extern double DLEXPORT dtan2(double x);
extern double DLEXPORT datan(double x);
extern double DLEXPORT datan2(double x);
/**<
 *  First and second partials of the cosine, tangent, arctangent functions
 */

#ifdef HAVE_ERF
extern double DLEXPORT derf(double x);
extern double DLEXPORT derf2(double x);
#endif /* HAVE_ERF */
/**<
 *  First and second derivatives of erf()
 */

extern CONST char *FuncName(CONST struct Func *f);
/**<
 *  <!--  const char *FuncName(f)                                      -->
 *  <!--  const struct Func *f;                                        -->
 *  Return the ASCEND language name of the function.
 *  Not a symchar.
 */

extern CONST char *FuncCName(CONST struct Func *f);
/**<
 *  <!--  const char *FuncCName(f)                                     -->
 *  <!--  const struct Func *f;                                        -->
 *  Return the C language name of the function, if any.
 */

extern CONST char *FuncDeriv1CName(CONST struct Func *f);
/**<
 *  <!--  const char *FuncDerive1CName(f)                              -->
 *  <!--  const struct Func *f;                                        -->
 *  Return the C language name of the function first derivative, if any.
 */

extern CONST char *FuncDeriv2CName(CONST struct Func *f);
/**<
 *  <!--  const char *FuncName(f)                                      -->
 *  <!--  const struct Func *f;                                        -->
 *  Return the C language name of the function second derivative, if any.
 */

extern enum Func_enum FuncId(CONST struct Func *f);
/**<
 *  <!--  enum Func_enum FuncId(f)                                     -->
 *  <!--  const struct Func *f;                                        -->
 *  Return the identification of the function.
 */

extern CONST dim_type *FuncDimens(CONST struct Func *f);
/**<
 *  <!--  dim_type *FuncDimens(f)                                      -->
 *  <!--  const struct Func *f;                                        -->
 *  Return the dimensionality required for the arg of the function.
 *  sin, cos, tan -> P.
 *  arc(sin,cos,tan),exp,ln,log,lnm,erf ->dimensionless.
 *  sqrt->wilddimension (user must check for even poweredness of arg)
 *  as this may be the case for an infinite # of different dims.
 *  sqr ->wilddimension (sqr, abs, cube anything you like.)
 */

extern double FuncEval(CONST struct Func *f, double u);
/**<
 *  <!--  double FuncEval(f,u)                                         -->
 *  <!--  const struct Func *f;                                        -->
 *  <!--  double u;                                                    -->
 *  Return f(u).
 */

extern double FuncEvalSafe(CONST struct Func *f,
                           double u,
                           enum safe_err *not_safe);
/**<
 *  <!--  double FuncEval(f,u,not_safe)                                -->
 *  <!--  const struct Func *f;                                        -->
 *  <!--  double u;                                                    -->
 *  <!--  enum safe_err *not_safe;                                     -->
 *  Return f(u) (safe version).
 */

extern double FuncDeriv(CONST struct Func *f, double u);
/**<
 *  <!--  double FuncDeriv(f,u)                                        -->
 *  <!--  const struct Func *f;                                        -->
 *  <!--  double u;                                                    -->
 *  Return df/du evaluated at u.
 */

extern double FuncDerivSafe(CONST struct Func *f,
                            double u,
                            enum safe_err *not_safe);
/**<
 *  <!--  double FuncDerivSafe(f,u,not_safe)                           -->
 *  <!--  const struct Func *f;                                        -->
 *  <!--  double u;                                                    -->
 *  <!--  enum safe_err *not_safe;                                     -->
 *  Return df/du evaluated at u (safe version).
 */

extern double FuncDeriv2(CONST struct Func *f, double u);
/**<
 *  <!--  double FuncDeriv2(f,u)                                       -->
 *  <!--  const struct Func *f;                                        -->
 *  <!--  double u;                                                    -->
 *  Return the second derivative (d^2f/du^2) evaluated at u.
 */

extern double FuncDeriv2Safe(CONST struct Func *f,
                             double u,
                             enum safe_err *not_safe);
/**<
 *  <!--  double FuncDeriv2(f,u,not_safe)                              -->
 *  <!--  const struct Func *f;                                        -->
 *  <!--  double u;                                                    -->
 *  <!--  enum safe_err *not_safe;                                     -->
 *  Return the second derivative (d^2f/du^2) evaluated at u (safe version).
 */

#ifdef CHRIS_FUNC

extern struct Interval FuncRange(CONST struct Func *f, struct Interval i);
/**<
 *  <!--  struct Interval FuncRange(f,i)                               -->
 *  <!--  const struct Func *f;                                        -->
 *  <!--  struct Interval i;                                           -->
 *  Return a bound on the range of the function over the given interval.
 */

extern void FuncSlope(CONST struct Func *f,
                      unsigned long nvar,
                      struct Interval *center,
                      struct Interval *range,
                      struct Interval *slope);
/**<
 *  <!--  struct Interval FuncSlope(f,nvar,center,range,slope)         -->
 *  <!--  const struct Func *f;                                        -->
 *  <!--  unsigned long nvar;                                          -->
 *  <!--  struct Interval *center,*range;                              -->
 *  <!--  struct Interval slope[];                                     -->
 *  Perform the interval slope calculation.
 */

extern struct Interval FuncIDeriv(CONST struct Func *f, struct Interval i);
/**<
 *  <!--  struct Interval FuncIDeriv(f,i)                              -->
 *  <!--  const struct Func *f                                         -->
 *  <!--  struct Interval i;                                           -->
 */

extern double ArgMin(CONST struct Func *f, double lower, double upper);
/**<
 *  <!--  double ArgMin(f,lower,upper)                                 -->
 *  <!--  const struct Func *f;                                        -->
 *  <!--  double lower,upper;                                          -->
 *  Return the arg min of the function over the range.
 */

extern double ArgMax(CONST struct Func *f, double lower, double upper);
/**<
 *  <!--  double ArgMax(f,lower,upper)                                 -->
 *  <!--  CONST struct Func *f;                                        -->
 *  <!--  double lower,upper;                                          -->
 *  Return the arg max of the function over the range.
 */

extern double ConvexEnv(CONST struct Func *f, double x,
                        double lower, double upper);
/**<
 *  <!--  double ConvexEnv(f,x,lower,upper)                            -->
 *  <!--  CONST struct Func *f;                                        -->
 *  <!--  double x,lower,upper;                                        -->
 *  Return the value of the convex envelope of the function at the value
 *  x which ranges from lower to upper.
 */

extern double ConvexEnvDeriv(CONST struct Func *f, double x,
                             double lower, double upper);
/**<
 *  <!--  double ConvexEnvDeriv(f,x,lower,upper)                       -->
 *  <!--  CONST struct Func *f;                                        -->
 *  <!--  double x,lower,upper;                                        -->
 *  Return the derivative of the convex envelope of the function at the
 *  value x which ranges from lower to upper.
 */

extern double ConcaveEnv(CONST struct Func *f, double x,
                         double lower, double upper);
/**<
 *  <!--  double ConcaveEnv(f,x,lower,upper)                           -->
 *  <!--  CONST struct Func *f;                                        -->
 *  <!--  double x,lower,upper;                                        -->
 *  Return the value of the concave envelope of the function at the value
 *  x which ranges from lower to upper.
 */

extern double ConcaveEnvDeriv(CONST struct Func *f, double x,
                              double lower, double upper);
/**<
 *  <!--  double ConcaveEnvDeriv(f,x,lower,upper)                      -->
 *  <!--  CONST struct Func *f;                                        -->
 *  <!--  double x,lower,upper;                                        -->
 *  Return the derivative of the concave envelope of the function at the
 *  value x which ranges from lower to upper.
 */

#endif /* CHRIS_FUNC */

#endif /* ASC_FUNC_H */

