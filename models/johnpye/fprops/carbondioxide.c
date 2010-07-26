/*	ASCEND modelling environment
	Copyright (C) 2008-2009 Carnegie Mellon University

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
	This file implements a Helmholtz fundamental equation correlation for
	Carbon Dioxide using the results from 

	R Span & W Wagner ''A new equation of state for carbon dioxide covering the
	fluid region from the triple-point temperature to 11100 K at pressures up 
	to 800 MPa'', J Phys Chem Ref Data, Vol 25, No. 6, 1996.
*/

#include "carbondioxide.h"
#include "sat2.h"

//#define CARBONDIOXIDE_R 188.9241
#define GAS_C 8314.510
#define CARBONDIOXIDE_TC 304.1282
#define CARBONDIOXIDE_M 44.0098
#define CARBONDIOXIDE_R (GAS_C/CARBONDIOXIDE_M)
#define CARBONDIOXIDE_PC 7.3773e6
#define CARBONDIOXIDE_RHOC 467.6
#define CARBONDIOXIDE_PT 0.51795e6

#define CARBONDIOXIDE_TREF 298.15
#define CARBONDIOXIDE_PREF 103.325e3

/**
	Ideal gas data for CO2.
	
	The constant and linear factors for phi_0 have been offset to match the
	reference state used by REFPROP, for the purpose of testing.
*/
const IdealData ideal_data_carbondioxide = {
	-1.1571354956e+003/CARBONDIOXIDE_R /* constant, adjust to solve s */
	, 2.9392750129e+005/CARBONDIOXIDE_TC/CARBONDIOXIDE_R /* linear, adjust to solver h */
	, CARBONDIOXIDE_TC /* Tstar / [K] */
	, CARBONDIOXIDE_R /* cpstar / [J/kgK] */
	, 1 /* power terms */
	, (const IdealPowTerm[]){
		{1. + 2.5,  0.}
	}
	, 5
	, (const IdealExpTerm[]){
		{1.99427042 , 3.15163 * CARBONDIOXIDE_TC}
		,{0.62105248, 6.11190 * CARBONDIOXIDE_TC}
		,{0.41195293, 6.77708 * CARBONDIOXIDE_TC}
		,{1.04028922, 11.32384 * CARBONDIOXIDE_TC}
		,{0.08327678, 27.08792 * CARBONDIOXIDE_TC}
	}
};

/**
	Residual (non-ideal) property data.
*/
const HelmholtzData helmholtz_data_carbondioxide = {
	/* R */ CARBONDIOXIDE_R /* 1000 * kJ/kmolK / kg/kmol = J/kgK */
	, /* M */ CARBONDIOXIDE_M /* kg/kmol */
	, /* rho_star */ CARBONDIOXIDE_RHOC /* kg/m³ (= rho_c for this model) */
	, /* T_star */ CARBONDIOXIDE_TC /* K (= T_c for this model) */

	, /* T_c */ CARBONDIOXIDE_TC
	, /* p_c */ CARBONDIOXIDE_PC
	, /* rho_c */ CARBONDIOXIDE_RHOC
	, /* p_t */ CARBONDIOXIDE_PT

	,  0.2239 /* acentric factor, from Reid, Prausnitz & Polling */
	, &ideal_data_carbondioxide
	, 34 /* power terms */
	, (const HelmholtzPowTerm[]){
		/* a_i, t_i, d_i, l_i */
		{  0.38856823203161e00,  0.000,   1.00,    0}
		,{ 0.29385475942740e01,  0.750,   1.00,    0}
		,{-0.55867188534934e01,  1.000,   1.00,    0}
		,{-0.76753199592477e00,  2.000,   1.00,    0}
		,{ 0.31729005580416e00,  0.750,   2.00,    0}
		,{ 0.54803315897767e00,  2.000,   2.00,    0}
		,{ 0.12279411220335e00,  0.750,   3.00,    0}
		,{ 0.21658961543220e01,   1.500,   1.00,    1}
		,{ 0.15841735109724e01,   1.500,   2.00,    1}
		,{-0.23132705405503e00,   2.500,   4.00,    1}
		,{ 0.58116916431436e-01,  0.000,   5.00,    1}
		,{-0.55369137205382e00,   1.500,   5.00,    1}
		,{ 0.48946615909422e00,   2.000,   5.00,    1}
		,{-0.24275739843501e-01,  0.000,   6.00,    1}
		,{ 0.62494790501678e-01,  1.000,   6.00,    1}
		,{-0.12175860225246e00,   2.000,   6.00,    1}
		,{-0.37055685270086e00,   3.000,   1.00,    2}
		,{-0.16775879700426e-01,  6.000,   1.00,    2}
		,{-0.11960736637987e00,   3.000,   4.00,    2}
		,{-0.45619362508778e-01,  6.000,   4.00,    2}
		,{ 0.35612789270346e-01,  8.000,   4.00,    2}
		,{-0.74427727132052e-02,  6.000,   7.00,    2}
		,{-0.17395704902432e-02,  0.000,   8.00,    2}
		,{-0.21810121289527e-01,  7.000,   2.00,    3}
		,{ 0.24332166559236e-01, 12.000,   3.00,    3}
		,{-0.37440133423463e-01, 16.000,   3.00,    3}
		,{ 0.14338715756878e00,  22.000,   5.00,    4}
		,{-0.13491969083286e00,  24.000,   5.00,    4}
		,{-0.23151225053480e-01, 16.000,   6.00,    4}
		,{ 0.12363125492901e-01, 24.000,   7.00,    4}
		,{ 0.21058321972940e-02,  8.000,   8.00,    4}
		,{-0.33958519026368e-03,  2.000,  10.00,    4}
		,{ 0.55993651771592e-02, 28.000,   4.00,    5}
		,{-0.30335118055646e-03, 14.000,   8.00,    6}
	}
	, 5 /* critical (gaussian) terms */
 	, (const HelmholtzGausTerm[]){
		/* a, t, d, alpha, beta, gamma, epsilon */
		{ -0.21365488688320e03,  1.000,  2,  25.,  325.,  1.16,  1.00}
		,{ 0.26641569149272e05,  0.000,  2,  25.,  300.,  1.19,  1.00}
		,{-0.24027212204557e05,  1.000,  2,  25.,  300.,  1.19,  1.00}
		,{-0.28341603423999e03,  3.000,  3,  15.,  275.,  1.25,  1.00}
		,{ 0.21247284400179e03,  3.000,  3,  20.,  275.,  1.22,  1.00}
	}
	, 3 /* critical terms */
	, (const HelmholtzCritTerm[]){
		/* n, a, b, beta, A, B, C, D */
		{ -0.66642276540751e00,  3.5,  0.875,  0.300,  0.70,  0.3,  10.0,  275.}
		,{ 0.72608632349897e00,  3.5,  0.925,  0.300,  0.70,  0.3,  10.0,  275.}
		,{0.55068668612842e-01,  3.0,  0.875,  0.300,  0.70,  1.0,  12.5,  275.}
	}
};

#ifdef TEST
#include "test.h"
/*
	Test suite. These tests attempt to validate the current code using
	a few sample figures output by REFPROP 7.0.

	To run the test, compile and run as follows:

	./test.py carbondioxide 
*/

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

const TestData td[]; const unsigned ntd;

int main(void){
	unsigned n, i;
	double rho, T, cp0, p, u, h, s;
	const HelmholtzData *d;

	d = &helmholtz_data_carbondioxide;
	double maxerr = 0;

	n = ntd;

	/* couple of data from the publication itself */
	ASSERT_TOL(helmholtz_p, 300.000, 679.24, d, 6.7131e6, 0.0001e6);
	ASSERT_TOL(helmholtz_p, 300.000, 268.58, d, 6.7131e6, 0.0001e6);
	ASSERT_TOL(helmholtz_p, 304.1282, 467.60, d, 7.3773e6, 0.0001e6);

#if USING_CITED_REFERENCE_STATES
	fprintf(stderr,"REFERENCE POINT CHECK\n");
	/* solve rho to give p = PREF */
	T = CARBONDIOXIDE_TREF;
	rho = 100;
	p = CARBONDIOXIDE_PREF;
	double err;
	for(i=0; i<100; ++i){
		fprintf(stderr,"rho = %f\n", rho);
		double dpdrho = helmholtz_dpdrho_T(T, rho, d);
		err = helmholtz_p(T,rho,d) - CARBONDIOXIDE_PREF;
		if(fabs(err) < 1e-6)break;
		rho -= err/dpdrho;
	}
	if(fabs(err) < 1e-6){
		fprintf(stderr,"err = %f -> T = %f, rho = %f --> p = %f bar\n", err, T, rho, helmholtz_p(T,rho,d)/1e5);
		fprintf(stderr,"h(Tref, pref) = %.10e\n", helmholtz_h(T,rho,d));
		fprintf(stderr,"s(Tref, pref) = %.10e\n", helmholtz_s(T,rho,d));
	}
	exit(1);
#endif

	fprintf(stderr,"XIANG SATURATION CURVE\n");
	T = 300;
	p = fprops_psat_T_xiang(T, d);
	fprintf(stderr,"T = %f -> psat(T) = %f bar\n", T, p/1e5);

	//phase_criterion(T, 679.24, 268.58, p, d);

	double eq1, eq2, eq3;
	phase_criterion(250., 1045.97, 46.644, 1.785e6, &eq1, &eq2, &eq3, d);

	fprintf(stderr,"ITERATION WITH SUCCESSIVE SUBSTITUTION\n");
	double rf, rg;
	T = 220;	
	int sat_err = 0;
	double p_sat = fprops_sat_succsubs(T, &rf, &rg, d, &sat_err);
	assert(sat_err==0);

	fprintf(stderr,"p_sat(%f) = %f bar\n", T, p_sat / 1e5);

	return helm_run_test_cases(d, ntd, td, 'K');

	//fprintf(stderr,"Tests completed OK (maximum error = %0.2f%% (%5.2e))\n",maxerr,maxerr);
	//exit(0);
}

/*
	Some test data generated by REFPROP 7.0 for p=0.1, 1, 10, 20, 100 MPa.
*/
const TestData td[] = {
/*, {Temperature, Pressure, Density, Int. Energy, Enthalpy, Entropy, Cv, Cp, Cp0, Helmholtz}
, {(K), (MPa), (kg/m�), (kJ/kg), (kJ/kg), (kJ/kg-K), (kJ/kg-K), (kJ/kg-K), (kJ/kg-K), (kJ/kg)}*/
 {2.16E+2, 1.E-2, 2.454062144E-1, 4.0018002E+2, 4.409287843E+2, 2.91905617E+0, 5.653701779E-1, 7.555917727E-1, 7.53186339E-1, -2.303361127E+2}
, {2.66E+2, 1.E-2, 1.991337854E-1, 4.298608404E+2, 4.80078336E+2, 3.081890365E+0, 6.208933259E-1, 8.104414236E-1, 8.094088439E-1, -3.899219966E+2}
, {3.16E+2, 1.E-2, 1.675727826E-1, 4.622510848E+2, 5.219266469E+2, 3.225924974E+0, 6.734607926E-1, 8.62745442E-1, 8.621948769E-1, -5.571412072E+2}
, {3.66E+2, 1.E-2, 1.446570587E-1, 4.971403497E+2, 5.662693649E+2, 3.356110837E+0, 7.209482393E-1, 9.10105514E-1, 9.097699996E-1, -7.311962166E+2}
, {4.16E+2, 1.E-2, 1.272587039E-1, 5.342743398E+2, 6.128544267E+2, 3.475359028E+0, 7.634284574E-1, 9.525151636E-1, 9.522915251E-1, -9.114750158E+2}
, {4.66E+2, 1.E-2, 1.135979244E-1, 5.734184073E+2, 6.614481847E+2, 3.58562638E+0, 8.015193523E-1, 9.905630878E-1, 9.904041983E-1, -1.097483486E+3}
, {5.16E+2, 1.E-2, 1.02586621E-1, 6.143700244E+2, 7.118486226E+2, 3.688334069E+0, 8.358645706E-1, 1.024880263E+0, 1.024761888E+0, -1.288810355E+3}
, {5.66E+2, 1.E-2, 9.352189158E-2, 6.569560172E+2, 7.638828536E+2, 3.784562017E+0, 8.669955064E-1, 1.055991905E+0, 1.055900437E+0, -1.485106085E+3}
, {6.16E+2, 1.E-2, 8.592935538E-2, 7.010265433E+2, 8.17401209E+2, 3.875154775E+0, 8.953216241E-1, 1.084304186E+0, 1.084231447E+0, -1.686068798E+3}
, {6.66E+2, 1.E-2, 7.947723266E-2, 7.464496665E+2, 8.722718632E+2, 3.960786755E+0, 9.211575282E-1, 1.110129832E+0, 1.110070631E+0, -1.891434312E+3}
, {7.16E+2, 1.E-2, 7.392649286E-2, 7.931074144E+2, 9.283769181E+2, 4.04200591E+0, 9.447532117E-1, 1.133717699E+0, 1.133668593E+0, -2.100968817E+3}
, {7.66E+2, 1.E-2, 6.910055494E-2, 8.408931762E+2, 9.856098142E+2, 4.119264426E+0, 9.663171895E-1, 1.155275584E+0, 1.155234203E+0, -2.314463374E+3}
, {8.16E+2, 1.E-2, 6.486613978E-2, 8.897100411E+2, 1.043873678E+3, 4.192940888E+0, 9.860314115E-1, 1.174984966E+0, 1.174949628E+0, -2.531729723E+3}
, {8.66E+2, 1.E-2, 6.112076176E-2, 9.394697103E+2, 1.103080237E+3, 4.263356512E+0, 1.00405981E+0, 1.193009454E+0, 1.192978932E+0, -2.752597029E+3}
, {9.16E+2, 1.E-2, 5.778432004E-2, 9.900917223E+2, 1.16314905E+3, 4.330787182E+0, 1.020552721E+0, 1.209499163E+0, 1.209472541E+0, -2.976909336E+3}
, {9.66E+2, 1.E-2, 5.479330284E-2, 1.041502834E+3, 1.22400689E+3, 4.395472433E+0, 1.035648963E+0, 1.22459275E+0, 1.224569332E+0, -3.204523536E+3}
, {1.016E+3, 1.E-2, 5.209670393E-2, 1.093636473E+3, 1.285587197E+3, 4.457622249E+0, 1.049476742E+0, 1.238418302E+0, 1.238397548E+0, -3.435307731E+3}
, {1.066E+3, 1.E-2, 4.965308888E-2, 1.146432218E+3, 1.347829557E+3, 4.517422242E+0, 1.062154072E+0, 1.251093748E+0, 1.251075233E+0, -3.669139892E+3}
, {2.16E+2, 1.E-1, 2.486826186E+0, 3.989032282E+2, 4.391151258E+2, 2.478112397E+0, 5.755764177E-1, 7.781063746E-1, 7.53186339E-1, -1.363690496E+2}
, {2.66E+2, 1.E-1, 2.004455136E+0, 4.291098246E+2, 4.789986937E+2, 2.644049351E+0, 6.246026263E-1, 8.199018006E-1, 8.094088439E-1, -2.742073029E+2}
, {3.16E+2, 1.E-1, 1.68194314E+0, 4.61736887E+2, 5.211919289E+2, 2.789283073E+0, 6.751779588E-1, 8.677495114E-1, 8.621948769E-1, -4.196765641E+2}
, {3.66E+2, 1.E-1, 1.449818149E+0, 4.967556457E+2, 5.657298133E+2, 2.920045807E+0, 7.218710051E-1, 9.131429027E-1, 9.097699996E-1, -5.719811197E+2}
, {4.16E+2, 1.E-1, 1.274385234E+0, 5.339695968E+2, 6.124388048E+2, 3.03961277E+0, 7.639780203E-1, 9.545352243E-1, 9.522915251E-1, -7.305093154E+2}
, {4.66E+2, 1.E-1, 1.137005248E+0, 5.731673256E+2, 6.611176673E+2, 3.150073938E+0, 8.018724634E-1, 9.919963349E-1, 9.904041983E-1, -8.947671292E+2}
, {5.16E+2, 1.E-1, 1.026454403E+0, 6.141572033E+2, 7.115799429E+2, 3.252907994E+0, 8.361052864E-1, 1.025947113E+0, 1.024761888E+0, -1.064343322E+3}
, {5.66E+2, 1.E-1, 9.355473622E-1, 6.567717566E+2, 7.636610538E+2, 3.349222831E+0, 8.671676509E-1, 1.056815769E+0, 1.055900437E+0, -1.238888366E+3}
, {6.16E+2, 1.E-1, 8.594627307E-1, 7.008643796E+2, 8.17216138E+2, 3.439877873E+0, 8.954497237E-1, 1.084959095E+0, 1.084231447E+0, -1.41810039E+3}
, {6.66E+2, 1.E-1, 7.948415686E-1, 7.463050944E+2, 8.721163302E+2, 3.525556019E+0, 9.212561086E-1, 1.110662697E+0, 1.110070631E+0, -1.601715214E+3}
, {7.16E+2, 1.E-1, 7.392705665E-1, 7.929771765E+2, 9.282456486E+2, 3.606810343E+0, 9.448312846E-1, 1.134159609E+0, 1.133668593E+0, -1.789499029E+3}
, {7.66E+2, 1.E-1, 6.909704835E-1, 8.407748444E+2, 9.854988267E+2, 3.684096267E+0, 9.663805679E-1, 1.155647918E+0, 1.155234203E+0, -1.981242896E+3}
, {8.16E+2, 1.E-1, 6.486004016E-1, 8.896017587E+2, 1.043779893E+3, 3.757794502E+0, 9.860839726E-1, 1.175302894E+0, 1.174949628E+0, -2.176758555E+3}
, {8.66E+2, 1.E-1, 6.111303977E-1, 9.39370026E+2, 1.103001225E+3, 3.828227711E+0, 1.004104215E+0, 1.193284034E+0, 1.192978932E+0, -2.375875172E+3}
, {9.16E+2, 1.E-1, 5.777562235E-1, 9.899994803E+2, 1.16308286E+3, 3.895672784E+0, 1.020590846E+0, 1.209738641E+0, 1.209472541E+0, -2.57843679E+3}
, {9.66E+2, 1.E-1, 5.478406447E-1, 1.041417099E+3, 1.223951931E+3, 3.960369979E+0, 1.035682158E+0, 1.2248034E+0, 1.224569332E+0, -2.784300301E+3}
, {1.016E+3, 1.E-1, 5.208721919E-1, 1.093556477E+3, 1.285542153E+3, 4.022529808E+0, 1.049506E+0, 1.238604985E+0, 1.238397548E+0, -2.993333808E+3}
, {1.066E+3, 1.E-1, 4.964355705E-1, 1.146357321E+3, 1.34779333E+3, 4.082338276E+0, 1.062180136E+0, 1.251260285E+0, 1.251075233E+0, -3.205415281E+3}
, {2.66E+2, 5.1795E-1, 1.072031066E+1, 4.254876367E+2, 4.738024679E+2, 2.319596193E+0, 6.427453842E-1, 8.683836586E-1, 8.094088439E-1, -1.915249506E+2}
, {3.16E+2, 5.1795E-1, 8.866368952E+0, 4.59302791E+2, 5.17720166E+2, 2.470828925E+0, 6.833341373E-1, 8.922113512E-1, 8.621948769E-1, -3.214791494E+2}
, {3.66E+2, 5.1795E-1, 7.588812072E+0, 4.949499667E+2, 5.632017556E+2, 2.60438178E+0, 7.262056716E-1, 9.276786804E-1, 9.097699996E-1, -4.582537647E+2}
, {4.16E+2, 5.1795E-1, 6.6442523E+0, 5.325457223E+2, 6.105003178E+2, 2.725465421E+0, 7.665452876E-1, 9.640907818E-1, 9.522915251E-1, -6.012478927E+2}
, {4.66E+2, 5.1795E-1, 5.9138178E+0, 5.719973032E+2, 6.595803194E+2, 2.836840196E+0, 8.035169363E-1, 9.987288679E-1, 9.904041983E-1, -7.499702279E+2}
, {5.16E+2, 5.1795E-1, 5.330607466E+0, 6.131671242E+2, 7.103324104E+2, 2.940266523E+0, 8.372243184E-1, 1.030936206E+0, 1.024761888E+0, -9.040104016E+2}
, {5.66E+2, 5.1795E-1, 4.853493743E+0, 6.559154804E+2, 7.626324203E+2, 3.036987084E+0, 8.679670625E-1, 1.060657083E+0, 1.055900437E+0, -1.063019209E+3}
, {6.16E+2, 5.1795E-1, 4.455590046E+0, 7.001113458E+2, 8.163585756E+2, 3.127932232E+0, 8.96044226E-1, 1.088006386E+0, 1.084231447E+0, -1.226694909E+3}
, {6.66E+2, 5.1795E-1, 4.11849327E+0, 7.456340944E+2, 8.713961064E+2, 3.213825034E+0, 9.2171345E-1, 1.113138541E+0, 1.110070631E+0, -1.394773378E+3}
, {7.16E+2, 5.1795E-1, 3.829145224E+0, 7.923729248E+2, 9.276381001E+2, 3.295242675E+0, 9.451934164E-1, 1.136210722E+0, 1.133668593E+0, -1.567020831E+3}
, {7.66E+2, 5.1795E-1, 3.578005866E+0, 8.402259753E+2, 9.849853802E+2, 3.372655764E+0, 9.66674516E-1, 1.1573748E+0, 1.155234203E+0, -1.74322834E+3}
, {8.16E+2, 5.1795E-1, 3.357934289E+0, 8.890995969E+2, 1.043346213E+3, 3.446454959E+0, 9.863277451E-1, 1.176776641E+0, 1.174949628E+0, -1.923207649E+3}
, {8.66E+2, 5.1795E-1, 3.163474379E+0, 9.38907801E+2, 1.102636012E+3, 3.516969662E+0, 1.004310168E+0, 1.194556335E+0, 1.192978932E+0, -2.106787927E+3}
, {9.16E+2, 5.1795E-1, 2.990382931E+0, 9.895718066E+2, 1.162777049E+3, 3.584481464E+0, 1.020767676E+0, 1.210847972E+0, 1.209472541E+0, -2.293813215E+3}
, {9.66E+2, 5.1795E-1, 2.835308799E+0, 1.041019622E+3, 1.223698136E+3, 3.649233983E+0, 1.03583613E+0, 1.225778994E+0, 1.224569332E+0, -2.484140406E+3}
, {1.016E+3, 5.1795E-1, 2.695569229E+0, 1.093185622E+3, 1.285334277E+3, 3.71144018E+0, 1.049641719E+0, 1.239469451E+0, 1.238397548E+0, -2.6776376E+3}
, {1.066E+3, 5.1795E-1, 2.568990553E+0, 1.146010121E+3, 1.347626277E+3, 3.771287889E+0, 1.06230105E+0, 1.252031389E+0, 1.251075233E+0, -2.874182768E+3}
, {2.330282499E+2, 1.E+0, 1.116903622E+3, 1.117618604E+2, 1.126571928E+2, 6.646002577E-1, 9.53030319E-1, 2.011052229E+0, 7.724408378E-1, -4.310877453E+1}
, {2.330282499E+2, 1.E+0, 2.60056432E+1, 3.968431044E+2, 4.352962968E+2, 2.049149574E+0, 6.802616759E-1, 1.032158413E+0, 7.724408378E-1, -8.066663447E+1}
, {2.66E+2, 1.E+0, 2.155818097E+1, 4.209870371E+2, 4.6737314E+2, 2.178030874E+0, 6.662310299E-1, 9.366092546E-1, 8.094088439E-1, -1.583691755E+2}
, {3.16E+2, 1.E+0, 1.748525303E+1, 4.563947588E+2, 5.135858099E+2, 2.337259039E+0, 6.931434181E-1, 9.232229315E-1, 8.621948769E-1, -2.821790975E+2}
, {3.66E+2, 1.E+0, 1.483421042E+1, 4.928271476E+2, 5.602388919E+2, 2.474277192E+0, 7.313069027E-1, 9.453710516E-1, 9.097699996E-1, -4.127583047E+2}
, {4.16E+2, 1.E+0, 1.292629419E+1, 5.308856036E+2, 6.082472964E+2, 2.597187325E+0, 7.695363473E-1, 9.754769274E-1, 9.522915251E-1, -5.495443234E+2}
, {4.66E+2, 1.E+0, 1.147281341E+1, 5.706396611E+2, 6.578022396E+2, 2.709643922E+0, 8.054224988E-1, 1.006651426E+0, 9.904041983E-1, -6.920544065E+2}
, {5.16E+2, 1.E+0, 1.032286536E+1, 6.120216711E+2, 7.088939992E+2, 2.813764447E+0, 8.38516999E-1, 1.036760927E+0, 1.024761888E+0, -8.398807836E+2}
, {5.66E+2, 1.E+0, 9.387718047E+0, 6.549267294E+2, 7.61448889E+2, 2.910957441E+0, 8.688888527E-1, 1.065118347E+0, 1.055900437E+0, -9.92675182E+2}
, {6.16E+2, 1.E+0, 8.611007831E+0, 6.992429352E+2, 8.153733603E+2, 3.002238907E+0, 8.967290163E-1, 1.091532789E+0, 1.084231447E+0, -1.150136231E+3}
, {6.66E+2, 1.E+0, 7.954902838E+0, 7.448609802E+2, 8.70569618E+2, 3.088379797E+0, 9.222399371E-1, 1.11599643E+0, 1.110070631E+0, -1.311999965E+3}
, {7.16E+2, 1.E+0, 7.392928421E+0, 7.916771576E+2, 9.269415539E+2, 3.169985785E+0, 9.456101719E-1, 1.138574074E+0, 1.133668593E+0, -1.478032665E+3}
, {7.66E+2, 1.E+0, 6.905937903E+0, 8.39594265E+2, 9.843971888E+2, 3.2475453E+0, 9.670127599E-1, 1.159361973E+0, 1.155234203E+0, -1.648025435E+3}
, {8.16E+2, 1.E+0, 6.479708386E+0, 8.885218325E+2, 1.042849765E+3, 3.321460616E+0, 9.866082474E-1, 1.178470915E+0, 1.174949628E+0, -1.82179003E+3}
, {8.66E+2, 1.E+0, 6.103436029E+0, 9.383761121E+2, 1.102218249E+3, 3.392068976E+0, 1.004547163E+0, 1.19601801E+0, 1.192978932E+0, -1.999155621E+3}
, {9.16E+2, 1.E+0, 5.768757078E+0, 9.890799457E+2, 1.162427512E+3, 3.45965742E+0, 1.020971177E+0, 1.212121783E+0, 1.209472541E+0, -2.179966251E+3}
, {9.66E+2, 1.E+0, 5.469090071E+0, 1.040562545E+3, 1.223408318E+3, 3.524473452E+0, 1.036013344E+0, 1.226898838E+0, 1.224569332E+0, -2.36407881E+3}
, {1.016E+3, 1.E+0, 5.199181651E+0, 1.092759198E+3, 1.285097159E+3, 3.586732867E+0, 1.049797941E+0, 1.240461488E+0, 1.238397548E+0, -2.551361396E+3}
, {1.066E+3, 1.E+0, 4.954785492E+0, 1.145610919E+3, 1.347436004E+3, 3.646625605E+0, 1.062440245E+0, 1.252916138E+0, 1.251075233E+0, -2.741691976E+3}
, {2.66E+2, 1.E+1, 1.008230103E+3, 1.703194875E+2, 1.802378583E+2, 9.018677036E-1, 9.301612759E-1, 2.102126092E+0, 8.094088439E-1, -6.957732164E+1}
, {3.16E+2, 1.0E+1, 5.597379223E+2, 3.135772661E+2, 3.3144277E+2, 1.414740357E+0, 1.057816023E+0, 7.326747935E+0, 8.621948769E-1, -1.334806868E+2}
, {3.66E+2, 1.0E+1, 1.98484141E+2, 4.425772486E+2, 4.929591076E+2, 1.899941649E+0, 8.452988087E-1, 1.609567063E+0, 9.097699996E-1, -2.528013949E+2}
, {4.16E+2, 1.0E+1, 1.500068446E+2, 4.96255415E+2, 5.629190397E+2, 2.07955064E+0, 8.285930224E-1, 1.273130713E+0, 9.522915251E-1, -3.688376511E+2}
, {4.66E+2, 1.0E+1, 1.248716971E+2, 5.438608125E+2, 6.239430107E+2, 2.218168838E+0, 8.412988511E-1, 1.184505847E+0, 9.904041983E-1, -4.898058661E+2}
, {5.16E+2, 1.0E+1, 1.08420603E+2, 5.90109262E+2, 6.823426542E+2, 2.33723529E+0, 8.623121952E-1, 1.157175578E+0, 1.024761888E+0, -6.159041474E+2}
, {5.66E+2, 1.0E+1, 9.645739419E+1, 6.36360029E+2, 7.400327446E+2, 2.443950306E+0, 8.856716458E-1, 1.152836473E+0, 1.055900437E+0, -7.469158442E+2}
, {6.16E+2, 1.0E+1, 8.721333487E+1, 6.83130483E+2, 7.977918478E+2, 2.541736295E+0, 9.091353898E-1, 1.158643119E+0, 1.084231447E+0, -8.825790747E+2}
, {6.66E+2, 1.0E+1, 7.978137012E+1, 7.306321252E+2, 8.559746706E+2, 2.632545825E+0, 9.317628034E-1, 1.169190991E+0, 1.110070631E+0, -1.022643394E+3}
, {7.16E+2, 1.0E+1, 7.363621112E+1, 7.789433503E+2, 9.147461007E+2, 2.717630134E+0, 9.531498283E-1, 1.181886486E+0, 1.133668593E+0, -1.166879825E+3}
, {7.66E+2, 1.0E+1, 6.844726264E+1, 8.28078421E+2, 9.741763015E+2, 2.797858158E+0, 9.731393965E-1, 1.195380226E+0, 1.155234203E+0, -1.315080928E+3}
, {8.16E+2, 1.0E+1, 6.399347241E+1, 8.780190691E+2, 1.034285007E+3, 2.873869735E+0, 9.916976438E-1, 1.208937453E+0, 1.174949628E+0, -1.467058634E+3}
, {8.66E+2, 1.0E+1, 6.012007272E+1, 9.287304392E+2, 1.095064237E+3, 2.946157327E+0, 1.008855348E+0, 1.222152034E+0, 1.192978932E+0, -1.622641806E+3}
, {9.16E+2, 1.0E+1, 5.671471735E+1, 9.801696947E+2, 1.156490771E+3, 3.015113221E+0, 1.024677616E+0, 1.234803749E+0, 1.209472541E+0, -1.781674016E+3}
, {9.66E+2, 1.0E+1, 5.369344469E+1, 1.032290871E+3, 1.218533345E+3, 3.081058518E+0, 1.039246983E+0, 1.246781594E+0, 1.224569332E+0, -1.944011658E+3}
, {1.016E+3, 1.0E+1, 5.099198481E+1, 1.085047668E+3, 1.28115692E+3, 3.144261845E+0, 1.052653463E+0, 1.258040108E+0, 1.238397548E+0, -2.109522366E+3}
, {1.066E+3, 1.0E+1, 4.856014402E+1, 1.138395059E+3, 1.344325256E+3, 3.204951926E+0, 1.06498854E+0, 1.268573381E+0, 1.251075233E+0, -2.278083694E+3}
, {2.66E+2, 1.00E+2, 1.201223185E+3, 1.219547998E+2, 2.052032762E+2, 6.942957691E-1, 9.718044513E-1, 1.650019263E+0, 8.094088439E-1, -6.272787475E+1}
, {3.16E+2, 1.E+2, 1.096322907E+3, 1.952242518E+2, 2.864382539E+2, 9.74260521E-1, 9.371579106E-1, 1.602434682E+0, 8.621948769E-1, -1.126420729E+2}
, {3.66E+2, 1.00E+2, 9.964902957E+2, 2.652422026E+2, 3.655944092E+2, 1.206875731E+0, 9.241881172E-1, 1.564208585E+0, 9.097699996E-1, -1.764743149E+2}
, {4.16E+2, 1.00E+2, 9.042838571E+2, 3.322798721E+2, 4.428646174E+2, 1.404819045E+0, 9.23833771E-1, 1.526675777E+0, 9.522915251E-1, -2.521248506E+2}
, {4.66E+2, 1.00E+2, 8.216796311E+2, 3.965925437E+2, 5.182944774E+2, 1.576083662E+0, 9.308032174E-1, 1.491059241E+0, 9.904041983E-1, -3.378624427E+2}
, {5.16E+2, 1.00E+2, 7.493262617E+2, 4.585756675E+2, 5.920288842E+2, 1.726413415E+0, 9.418566817E-1, 1.458855999E+0, 1.024761888E+0, -4.322536548E+2}
, {5.66E+2, 1.00E+2, 6.869405554E+2, 5.186601111E+2, 6.642331148E+2, 1.85999357E+0, 9.55105217E-1, 1.429920847E+0, 1.055900437E+0, -5.340962496E+2}
, {6.16E+2, 1.00E+2, 6.336268853E+2, 5.772738229E+2, 7.350954089E+2, 1.979982167E+0, 9.694646332E-1, 1.405425192E+0, 1.084231447E+0, -6.423951922E+2}
, {6.66E+2, 1.00E+2, 5.881088279E+2, 6.348278144E+2, 8.048643711E+2, 2.088891096E+0, 9.842611237E-1, 1.38621029E+0, 1.110070631E+0, -7.563736553E+2}
, {7.16E+2, 1.00E+2, 5.490721929E+2, 6.916740689E+2, 8.737994821E+2, 2.188702252E+0, 9.990470337E-1, 1.371968091E+0, 1.133668593E+0, -8.754367438E+2}
, {7.66E+2, 1.00E+2, 5.153528008E+2, 7.480905022E+2, 9.421323311E+2, 2.280958121E+0, 1.013517439E+0, 1.361993577E+0, 1.155234203E+0, -9.991234187E+2}
, {8.16E+2, 1.00E+2, 4.85979865E+2, 8.042873443E+2, 1.010057186E+3, 2.366861016E+0, 1.027466929E+0, 1.355520557E+0, 1.174949628E+0, -1.127071245E+3}
, {8.66E+2, 1.00E+2, 4.601677832E+2, 8.604182512E+2, 1.077730292E+3, 2.447353251E+0, 1.040764859E+0, 1.351804563E+0, 1.192978932E+0, -1.258989664E+3}
, {9.16E+2, 1.00E+2, 4.372911706E+2, 9.165918456E+2, 1.145272429E+3, 2.523178446E+0, 1.053337501E+0, 1.350179618E+0, 1.209472541E+0, -1.394639611E+3}
, {9.66E+2, 1.00E+2, 4.168552987E+2, 9.728824079E+2, 1.212773805E+3, 2.594929012E+0, 1.065153173E+0, 1.350091707E+0, 1.224569332E+0, -1.533819018E+3}
, {1.016E+3, 1.00E+2, 3.984683178E+2, 1.029338886E+3, 1.280299867E+3, 2.663082573E+0, 1.076209938E+0, 1.351103331E+0, 1.238397548E+0, -1.676353008E+3}
, {1.066E+3, 1.00E+2, 3.81818007E+2, 1.085991969E+3, 1.34789685E+3, 2.72802938E+0, 1.086525894E+0, 1.352880503E+0, 1.251075233E+0, -1.822087351E+3}
};

const unsigned ntd = sizeof(td)/sizeof(TestData);

#endif
