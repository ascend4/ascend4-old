/*
created by John Pye 2012

LITERATURE REFERENCE
I Made Astina, Haruki Sato, 2004 , "A fundamental equation of state for
1,1,1,2-tetrafluoroethane with an intermolecular potential energy background
and reliable ideal-gas properties", Fluid Phase Equilibria 221, pp 103-111.
http://dx.doi.org/10.1016/j.fluid.2004.03.004
*/

#include "../fprops.h"
#include "../helmholtz.h"

#define R134A_M 102.031 /* kg/kmol */
#define R134A_R (8314.472/R134A_M) /* J/kg/K */
#define R134A_TC 374.083 /* K */
#define R134A_RHOC 509 /* kg/m3 */

static const IdealData ideal_data_r134a = {
	IDEAL_PHI0
	,.data={.phi0={
		.Tstar = R134A_TC /* Tstar */
		, .np = 2 /* power terms */
		, .pt = (const Phi0PowTerm[]){
			{-1, 0.}
			,{-24.37548384, -0.25}
		}
		, .ne = 3 /* Einstein terms */
		, .et = (const Phi0ExpTerm[]){
			{7.451784998, 4.103830338}
			,{-4.239239505,2.561528683}
			,{6.445739825, 2.084607363}
		}
	}}
};

static HelmholtzData helmholtz_data_r134a = {
	/* R */ R134A_R
	, /* M */ R134A_M
	, /* rho_star */ R134A_RHOC
	, /* T_star */ R134A_TC
	, /* T_c */ R134A_TC
	, /* rho_c */ R134A_RHOC
	, /* T_t */ 169.85 /* K */

#if 0
	,{FPROPS_REF_IIR}
#else
	,{FPROPS_REF_PHI0,{.phi0={
		10.78497786
		, 8.612977410
	}}}
#endif
	, 0.32684 /* acentric factor */
	, &ideal_data_r134a
	, 18 /* power terms */
	, (const HelmholtzPowTerm[]){
		/* a_i, 	t_i, 	d_i, 	l_i */
		{  1.832124209e0, 0.5, 1, 0}
		,{ -2.940698861e0, 1.125, 1, 0}
		,{ 5.156071823e-1, 3.25, 1, 0}
		,{ 2.756965911e-1, 0.5, 2, 0}
		,{ 1.225264939e0, 1.875, 2, 0}
		,{ -6.486749497e-1, 2.75, 2, 0}
		,{ -9.286738053e-1, 1.625, 3, 0}
		,{ 3.920381291e-1, 2.125, 3, 0}
		,{ 1.056692108e-1, 1.125, 4, 0}
		,{ -7.586523371e-1, 3.75, 1, 1}
		,{ -1.198140136e0, 1.5, 2, 1}
		,{ -2.878260390e-1, 0.75, 3, 1}
		,{ -9.723032379e-2, 9, 2, 2}
		,{ 5.307113358e-2, 8.5, 3, 2}
		,{ -4.681610582e-2, 5.5, 4, 2}
		,{ -9.604697902e-3, 32, 4, 3}
		,{ 6.668035048e-3, 23, 5, 3}
		,{ 2.361266290e-3, 31, 6, 3}
	}
	, 0 /* gaussian terms */
	, 0
	, 0 /* critical terms */
	, 0
};

EosData eos_r134a = {
	"r134a"
	,"I Made Astina, Haruki Sato, 2004 , 'A fundamental equation of state for "
	"1,1,1,2-tetrafluoroethane with an intermolecular potential energy "
	"background and reliable ideal-gas properties', Fluid Phase Equilibria "
	"221, pp 103-111."
	,"http://dx.doi.org/10.1016/j.fluid.2004.03.004"
	,100
	,FPROPS_HELMHOLTZ
	,.data = {.helm = &helmholtz_data_r134a}
};

/*
    Test suite. These tests attempt to validate the current code using a few sample figures output by REFPROP 8.0. To compile and run the test:

    ./test.py r134a
*/

#ifdef TEST

#include "../test.h"
#include <math.h>
#include <assert.h>
#include <stdio.h>

const TestData td[]; const unsigned ntd;

int main(void){
	PureFluid *P = helmholtz_prepare(&eos_r134a, NULL);
	ASSERT(P);
	FpropsError err;
	//fprintf(stderr,"p_c = %f MPa\n",fprops_p(P->data->T_c,P->data->rho_c,P,&err)/1e6);
	//ASSERT(err==0);
	return helm_run_test_cases(P, ntd, td, 'C');
	return err;
}

/*
A small set of data points calculated using REFPROP 7.0, for validation.
*/

const TestData td[] = {
	/* Temperature, Pressure, Density, Int. Energy, Enthalpy, Entropy, Cv, Cp, Cp0, Helmholtz */
	/* (C), (MPa), (kg/m3), (kJ/kg), (kJ/kg), (kJ/kg-K), (kJ/kg-K), (kJ/kg-K), (kJ/kg-K), (kJ/kg) */
	{-90.0000000000, 0.0999999999990, 1553.86627377, 87.7500835043, 87.8144391067, 0.504220084848, 0.790874066100, 1.19066501244, 0.616609541678, -4.59782503553}
	, {-40.0000000000, 0.100000000000, 1415.93170212, 148.394544003, 148.465168879, 0.796746688578, 0.830397293018, 1.24592369727, 0.711723950914, -37.3669464386}
	, {-26.3506225637, 0.100000000000, 1375.88103991, 165.569143058, 165.641823760, 0.868335993761, 0.845677277905, 1.27168875175, 0.737236181083, -48.7356396083}
	, {-26.3506225637, 0.100000000000, 5.19009637157, 363.552987853, 382.820452817, 1.74831645122, 0.682881118985, 0.787921566698, 0.737236181083, -67.9304238703}
	, {10.0000000000, 0.100000000000, 4.44231519005, 389.505379187, 412.016163704, 1.85860943736, 0.733100563888, 0.825964107595, 0.804175118546, -136.759883000}
	, {60.0000000000, 0.100000000000, 3.73339924701, 428.428061646, 455.213303027, 1.99893024250, 0.815589930640, 0.902942209821, 0.892523748639, -237.515548643}
	, {110.000000000, 0.100000000000, 3.22869774985, 471.335346058, 502.307585563, 2.13051165123, 0.895140599017, 0.980167856262, 0.974293127097, -344.970193111}
	, {160.000000000, 0.100000000000, 2.84751910181, 518.014531269, 553.132820708, 2.25510390895, 0.967836437614, 1.05167883913, 1.04798462133, -458.783726891}
	, {-90.0000000000, 0.999999999999, 1555.09730037, 87.5672534258, 88.2103000055, 0.503220303667, 0.791212867457, 1.18998529370, 0.616609541678, -4.59754519070}
	, {-40.0000000000, 1.00000000000, 1418.00603357, 148.095396499, 148.800612116, 0.795461188252, 0.830768558307, 1.24398202750, 0.711723950914, -37.3663795421}
	, {10.0000000000, 1.00000000000, 1262.70613681, 212.837209248, 213.629159149, 1.04701247221, 0.892001500259, 1.36189820983, 0.804175118546, -83.6243722591}
	, {39.3864196984, 1.00000000000, 1149.04384664, 254.500876025, 255.371164833, 1.18716875648, 0.934303233119, 1.49355613106, 0.856766564623, -116.532596703}
	, {39.3864196984, 1.00000000000, 49.2280434942, 398.800462172, 419.114086867, 1.71108504819, 0.889097107476, 1.14996083089, 0.856766564623, -135.975932589}
	, {60.0000000000, 1.00000000000, 43.3343000897, 418.446388689, 441.522796936, 1.78055063867, 0.871054535600, 1.05194684769, 0.892523748639, -174.744056584}
	, {110.000000000, 1.00000000000, 34.9782256120, 464.900670678, 493.489885325, 1.92589118182, 0.918268387089, 1.04504512224, 0.974293127097, -273.004535637}
	, {160.000000000, 1.00000000000, 29.8817969144, 513.302148630, 546.767338253, 2.05653338923, 0.980250041930, 1.08883123650, 1.04798462133, -377.485288915}
	, {-90.0000000000, 10.0000000000, 1566.92553585, 85.8183655991, 92.2002898012, 0.493527321390, 0.794837919719, 1.18412083232, 0.616609541678, -4.57116331341}
	, {-40.0000000000, 10.0000000000, 1437.29249459, 145.317569192, 152.275095261, 0.783328372707, 0.833810763302, 1.22782759929, 0.711723950914, -37.3154409045}
	, {10.0000000000, 10.0000000000, 1298.22080026, 207.999519941, 215.702369871, 1.02952496084, 0.891883688630, 1.31561047691, 0.804175118546, -83.5104727197}
	, {60.0000000000, 10.0000000000, 1134.83959113, 275.793302850, 284.605120906, 1.25332230090, 0.957876243652, 1.45172534571, 0.892523748639, -141.751021696}
	, {110.000000000, 10.0000000000, 910.312379555, 352.008394579, 362.993634618, 1.47211799921, 1.02680595993, 1.72231026437, 0.974293127097, -212.033616820}
	, {160.000000000, 10.0000000000, 569.625793149, 441.478374015, 459.033758831, 1.70730726610, 1.08840304147, 1.99218905976, 1.04798462133, -298.041768296}
	, {-40.0000000000, 70.0000000000, 1527.70171359, 132.487928578, 178.308391681, 0.721812142825, 0.849210036659, 1.18366481745, 0.711723950914, -35.8025725217}
	, {10.0000000000, 70.0000000000, 1430.44008859, 189.664674366, 238.600663059, 0.955953253156, 0.903439840608, 1.23054914351, 0.804175118546, -81.0134892648}
	, {60.0000000000, 70.0000000000, 1336.80012652, 249.097058038, 301.460907062, 1.16027734745, 0.965878641588, 1.28377872355, 0.892523748639, -137.449340265}
	, {110.000000000, 70.0000000000, 1246.90703119, 310.773211361, 366.912120076, 1.34324240263, 1.02759250894, 1.33322305313, 0.974293127097, -203.890115206}
	, {160.000000000, 70.0000000000, 1161.44150845, 374.390395707, 434.660327073, 1.50938606234, 1.08468174133, 1.37538505346, 1.04798462133, -279.400177197}
};

const unsigned ntd = sizeof(td)/sizeof(TestData);

#endif