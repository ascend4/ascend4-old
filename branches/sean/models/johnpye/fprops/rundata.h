/*	ASCEND modelling environment
	Copyright (C) 2011 Carnegie Mellon University

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2, or (at your option)
	any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330,
	Boston, MA 02111-1307, USA.
*//** @file
This file contains declarations of the data structures passed to
functions that EVALUATE fluid properties. We allow from some preprocessing of
data loaded from input files, if deisred/needed.

Data declarations as provided in input files are given in filedata.h
*/

#ifndef FPROPS_RUNDATA_H
#define FPROPS_RUNDATA_H

#include "common.h"

/* some of the helmholtz terms are not being copied into new structures yet... */
#include "filedata.h"


/** Power terms for phi0 (including polynomial) */
typedef struct Cp0RunPowTerm_struct{
	double a;
	double p;
} Phi0RunPowTerm;

/** Planck-Einstein aka 'exponential' terms for phi0 */
typedef struct Cp0RunExpTerm_struct{
	double n;
	double gamma;
} Phi0RunExpTerm;

/**
	Zero-pressure specific heat capacity data for a fluid

	There is no 'R' or 'cp0star' in this structure. If cp0star != R in the filedata, that
	difference will be corrected for when this structure is created.
*/
typedef struct 	Phi0RunData_struct{
	double c;       /**< second integration constant in phi0, value determined by reference point for entropy */
	double m;       /**< first integration constant in phi0, value determined by reference point for enthalpy */

	unsigned np;    /**< number of power terms */
	Phi0RunPowTerm *pt; /**< power term data, may be NULL if np == 0 */
	unsigned ne;    /**< number of Planck-Einstein aka 'exponential' terms */
	Phi0RunExpTerm *et; /**< exponential term data, maybe NULL if ne == 0 */
} Phi0RunData;

typedef struct HelmholtzRunData_struct{
	double rho_star;/**< normalisation density, kg/m3 */
	double T_star;  /**< normalisation temperature, K */

	//REMOVED: double p_t; /**< triple-point pressure */

   	unsigned np;                 /**< number of power terms in residual equation */
	const HelmholtzPowTerm *pt;  /**< power term data for residual eqn, maybe NULL if np == 0 */
	unsigned ng;                 /**< number of critical terms of the first kind */
	const HelmholtzGausTerm *gt; /**< critical terms of the first kind */
	unsigned nc;                 /**< number of critical terms of the second kind */
	const HelmholtzCritTerm *ct; /**< critical terms of the second kind */
} HelmholtzRunData;

typedef struct PengrobRunData_struct{
	double aTc;   /**< value of 'a' when evaluated at T =  T_c */
	double b;     /**< coeficient 'b' in PR EOS */
	double kappa; /** parameter used in a(T) */
} PengrobRunData;

typedef union CorrelationUnion_union{
	HelmholtzRunData *helm;
	PengrobRunData *pengrob;
	/* maybe more later */
} CorrelationUnion;

/* TODO Regarding Cp0Data: some source publications present Cp0 data eg in the
form of polynomials etc, and others present phi0 data (nondimensionalised ideal
helmholtz energy). There is a straightforward conversion between the two, see
precalc.c (although that is still incomplete). What is not clear is whether it
is better to keep phi0 or cp0 values in the runtime data here. */

/** All runtime 'core' data for all possible correlations, with exception of correlation-type-ID, function pointers and metadata */
typedef struct FluidData_struct{
	/* common data across all correlations */
	double R;     /**< specific gas constant */
	double M;     /**< molar mass, kg/kmol */
	double T_t;   /**< triple-point temperature */
	double T_c;   /**< critical temperature */
	double p_c;   /**< critical pressure */
	double rho_c; /**< critical density */
	double omega; /**< acentric factor (possibly calculated from correlation data)*/
	Phi0RunData *cp0; /* data for ideal component of Helmholtz energy */

	/* correlation-specific stuff here */
	CorrelationUnion corr;
} FluidData;


/* Definition of a fluid property function pointer */
typedef double PropEvalFn(double,double,const FluidData *data, FpropsError *err);

/**
	Structure containing all the necessary data and metadata for run-time
	calculation of fluid properties.
*/
typedef struct PureFluid_struct{
    const char *name;
	EosType type;
	FluidData *data; // everything we need at runtime in the following functions should be inside this
	//Pointers to departure functions
	PropEvalFn *p_fn;
	PropEvalFn *u_fn;
	PropEvalFn *h_fn;
	PropEvalFn *s_fn;
	PropEvalFn *a_fn;
	PropEvalFn *cv_fn;
	PropEvalFn *cp_fn;
	PropEvalFn *w_fn;
	PropEvalFn *g_fn;
	PropEvalFn *alphap_fn;
	PropEvalFn *betap_fn;
	PropEvalFn *dpdrho_T_fn; // this derivative is required for saturation properties by Akasaka method
} PureFluid;

#endif