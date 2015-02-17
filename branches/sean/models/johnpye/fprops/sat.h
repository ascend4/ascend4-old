#ifndef FPROPS_SAT_H
#define FPROPS_SAT_H
#include <complex.h>
#include "helmholtz.h"

double fprops_psat_T_xiang(double T, const PureFluid *d);
double fprops_psat_T_acentric(double T, const PureFluid *d);
double fprops_rhof_T_rackett(double T, const PureFluid *D);
double fprops_rhog_T_chouaieb(double T, const PureFluid *D);

int fprops_sat_T(double T, double *p_sat, double *rho_f, double *rho_g, const PureFluid *d);
int fprops_sat_T_cubic(double T, double *p_sat, double *rho_f, double *rho_g, const PureFluid *d);
double MidpointPressureCubic(double T, const PureFluid *d);
int CubicRoots(double A, double B, double C, double D);
int fprops_sat_p(double p, double *T_sat, double *rho_f, double *rho_g, const PureFluid *d);

double fprops_T_rhof_rackett(double rhof, const PureFluid *D);

int fprops_sat_hf(double hf, double *T_sat, double *p_sat, double *rho_f, double *rho_g, const PureFluid *d);

int fprops_triple_point(double *p_sat, double *rho_f, double *rho_g, const PureFluid *d);
double AZ;
double BZ;
double complex returnedroots[3];

#endif

