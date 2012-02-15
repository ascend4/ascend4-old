/* This file is created by Hongke Zhu, 7-20-2010. 
Chemical & Materials Engineering Department, 
University of Alabama in Huntsville, United States.

LITERATURE REFERENCE
Leachman, J.W., Jacobsen, R.T, Lemmon, E.W.,
"Fundamental Equations of State for Parahydrogen, Normal Hydrogen, and Orthohydrogen,"
International Journal of Thermophysics, 2007.
*/

#include "../helmholtz.h"

#define PARAHYDROGEN_M 2.01594 /* kg/kmol */
#define PARAHYDROGEN_R (8314.472/PARAHYDROGEN_M) /* J/kg/K */
#define PARAHYDROGEN_TSTAR 32.938 /* K */

const IdealData ideal_data_parahydrogen = {
    -1.4485891134 /* constant */
    , 1.884521239 /* linear */
    , PARAHYDROGEN_TSTAR /* Tstar */
    , PARAHYDROGEN_R /* cp0star */
    , 1 /* power terms */
    , (const IdealPowTerm[]){
        {0.25E+01,	0.0}
    }
    , 7 /* exponential terms */
    , (const IdealExpTerm[]){
        {0.430256E+01,      499.0}
	, {0.130289E+02,    826.5}
	, {-0.477365E+02,   970.8}
	, {0.500013E+02,   1166.2}
	, {-0.186261E+02,  1341.4}
	, {0.993973E+00,   5395.0}
	, {0.536078E+00,  10185.0}
    } 
};

const HelmholtzData helmholtz_data_parahydrogen = {
	"parahydrogen"
    , /* R */ PARAHYDROGEN_R /* J/kg/K */
    , /* M */ PARAHYDROGEN_M /* kg/kmol */
    , /* rho_star */ 15.538*PARAHYDROGEN_M /* kg/m3(= rho_c for this model) */
    , /* T_star */ PARAHYDROGEN_TSTAR /* K (= T_c for this model) */

    , /* T_c */ PARAHYDROGEN_TSTAR
    , /* rho_c */ 15.538*PARAHYDROGEN_M /* kg/m3 */
    , /* T_t */ 0

    , -0.219 /* acentric factor */
    , &ideal_data_parahydrogen
    , 9 /* power terms */
    , (const HelmholtzPowTerm[]){
        /* a_i, 	t_i, 	d_i, 	l_i */
        {-0.733375E+01,   0.6855,    1.0,  0.0}
        , {0.100000E-01,   1.000,    4.0,  0.0}
	, {0.260375E+01,   1.000,    1.0,  0.0}
	, {0.466279E+01,   0.489,    1.0,  0.0}
	, {0.682390E+00,   0.774,    2.0,  0.0}
	, {-0.147078E+01,  1.133,    2.0,  0.0}
	, {0.135801E+00,   1.386,    3.0,  0.0}
	, {-0.105327E+01,  1.619,    1.0,  1.0}
	, {0.328239E+00,   1.162,    3.0,  1.0}
    }
    , 5 /* gaussian terms */
    , (const HelmholtzGausTerm[]){
        /* n, t, d, alpha, beta, gamma, epsilon */
        {-0.577833E-01,  3.960,     2.0,  1.7437,  0.1940,  0.8048,  1.5487}
	, {0.449743E-01,  5.276,    1.0,  0.5516,  0.2019,  1.5248,  0.1785}
	, {0.703464E-01,  0.990,    3.0,  0.0634,  0.0301,  0.6648,  1.2800}
	, {-0.401766E-01,  6.791,   1.0,  2.1341,  0.2383,  0.6832,  0.6319}
	, {0.119510E+00,  3.190,    1.0,  1.7770,  0.3253,  1.4930,  1.7104}
    }
    , 0 /* critical terms */
    , 0
};

/*
    Test suite. These tests attempt to validate the current code using a few sample figures output by REFPROP 8.0. To compile and run the test:

    ./test.py parahydrogen
*/

#ifdef TEST

#include "../test.h"
#include <math.h>
#include <assert.h>
#include <stdio.h>

const TestData td[]; const unsigned ntd;

int main(void){
    //return helm_check_u(&helmholtz_data_parahydrogen, ntd, td);
    //return helm_check_dpdT_rho(&helmholtz_data_parahydrogen, ntd, td);
    //return helm_check_dpdrho_T(&helmholtz_data_parahydrogen, ntd, td);
    //return helm_check_dhdT_rho(&helmholtz_data_parahydrogen, ntd, td);
    //return helm_check_dhdrho_T(&helmholtz_data_parahydrogen, ntd, td);
    //return helm_check_dudT_rho(&helmholtz_data_parahydrogen, ntd, td);
    //return helm_check_dudrho_T(&helmholtz_data_parahydrogen, ntd, td);
    return helm_run_test_cases(&helmholtz_data_parahydrogen, ntd, td, 'C');
}

/*
A small set of data points calculated using REFPROP 8.0, for validation. 
*/

const TestData td[] = {
    /* Temperature, Pressure, Density, Int. Energy, Enthalpy, Entropy, Cv, Cp, Cp0, Helmholtz */
    /* (C), (MPa), (kg/m3), (kJ/kg), (kJ/kg), (kJ/kg-K), (kJ/kg-K), (kJ/kg-K), (kJ/kg-K), (kJ/kg) */
    {-2.50E+2, 1.E-1, 1.12056230331E+0, 3.90438827674E+2, 4.79679737978E+2, 2.36064810622E+1, 6.26894232836E+0, 1.12860865842E+1, 1.03109156198E+1, -1.56051208917E+2}
    , {-2.40E+2, 1.E-1, 7.50709999157E-1, 4.55498384526E+2, 5.88705615165E+2, 2.75270271957E+1, 6.21227069987E+0, 1.06908899696E+1, 1.03120792365E+1, -4.5702256701E+2}
    , {-2.30E+2, 1.E-1, 5.6902387841E-1, 5.18988300619E+2, 6.94727850038E+2, 3.0323040668E+1, 6.22819858741E+0, 1.05445911024E+1, 1.03335332917E+1, -7.89450904203E+2}
    , {-2.20E+2, 1.E-1, 4.59276476938E-1, 5.82438984268E+2, 8.0017275689E+2, 3.25207842727E+1, 6.33212872455E+0, 1.05756835942E+1, 1.04433745458E+1, -1.14604069982E+3}
    , {-2.10E+2, 1.E-1, 3.85408868066E-1, 6.47492944795E+2, 9.06957654317E+2, 3.43610975211E+1, 6.61593280952E+0, 1.08208933808E+1, 1.07311538751E+1, -1.52241036366E+3}
    , {-2.00E+2, 1.E-1, 3.32172679094E-1, 7.16358151613E+2, 1.01740639036E+3, 3.59838091377E+1, 7.12644081723E+0, 1.13086376071E+1, 1.12441448548E+1, -1.91585748681E+3}
    , {-1.90E+2, 1.E-1, 2.91935568785E-1, 7.91301027889E+2, 1.13384236472E+3, 3.74747929943E+1, 7.8404923536E+0, 1.20081927118E+1, 1.1959771372E+1, -2.32472800959E+3}
    , {-2.50E+2, 1.E+0, 6.85971667337E+1, 2.32293283889E+1, 3.78071899482E+1, 1.14816300797E+0, 5.9081181035E+0, 1.10761642393E+1, 1.03109156198E+1, -3.35064524549E+0}
    , {-2.41905710061E+2, 1.E+0, 4.96508877583E+1, 1.57246310956E+2, 1.77386937743E+2, 6.18107747703E+0, 6.71735624987E+0, 4.07711928338E+1, 1.03114365466E+1, -3.58770658708E+1}
    , {-2.41905710061E+2, 1.E+0, 1.43076349691E+1, 3.44350241772E+2, 4.14242994988E+2, 1.37618564836E+1, 8.01663649616E+0, 5.28966530928E+1, 1.03114365466E+1, -8.56291922994E+1}
    , {-2.40E+2, 1.E+0, 1.08767861561E+1, 3.82900210036E+2, 4.74839132585E+2, 1.56517428101E+1, 7.03363334585E+0, 2.30776199352E+1, 1.03120792365E+1, -1.35955064119E+2}
    , {-2.30E+2, 1.E+0, 6.46152401537E+0, 4.78623150826E+2, 6.33385401592E+2, 1.98790398736E+1, 6.4101084621E+0, 1.31718579124E+1, 1.03335332917E+1, -3.79157419721E+2}
    , {-2.20E+2, 1.E+0, 4.88846499687E+0, 5.53033294078E+2, 7.57596485313E+2, 2.24724114126E+1, 6.44172815358E+0, 1.19365390185E+1, 1.04433745458E+1, -6.41375372501E+2}
    , {-2.10E+2, 1.E+0, 3.9859916064E+0, 6.24297990949E+2, 8.75176592498E+2, 2.45000597873E+1, 6.69192325453E+0, 1.16800251371E+1, 1.07311538751E+1, -9.2288078462E+2}
    , {-2.00E+2, 1.E+0, 3.38369830583E+0, 6.9724705584E+2, 9.92781677907E+2, 2.62284394647E+1, 7.18242798016E+0, 1.19053409944E+1, 1.12441448548E+1, -1.221363291E+3}
    , {-1.90E+2, 1.E+0, 2.94775912539E+0, 7.7509903252E+2, 1.11433977692E+3, 2.77852691165E+1, 7.88377813959E+0, 1.24480404172E+1, 1.1959771372E+1, -1.53524609452E+3}
    , {-2.50E+2, 1.E+1, 7.77836386236E+1, -4.26684681803E+0, 1.24294894146E+2, -3.90246598855E-1, 5.88055339443E+0, 8.54908411928E+0, 1.03109156198E+1, 4.76736194545E+0}
    , {-2.40E+2, 1.E+1, 6.97550518553E+1, 8.15296010769E+1, 2.24888393509E+2, 3.18962134692E+0, 6.5220949036E+0, 1.15277900551E+1, 1.03120792365E+1, -2.42063465737E+1}
    , {-2.30E+2, 1.0E+1, 5.99397887894E+1, 1.87310832913E+2, 3.54144920953E+2, 6.58145662555E+0, 6.69734456447E+0, 1.42537127153E+1, 1.03335332917E+1, -9.66790204796E+1}
    , {-2.20E+2, 1.0E+1, 4.93962777073E+1, 3.04084802278E+2, 5.06529206273E+2, 9.75144734318E+0, 6.80918399758E+0, 1.5955806237E+1, 1.04433745458E+1, -2.14204624012E+2}
    , {-2.10E+2, 1.0E+1, 4.02188394184E+1, 4.1928704823E+2, 6.6792674407E+2, 1.25334750534E+1, 7.05931983682E+0, 1.61225257126E+1, 1.07311538751E+1, -3.72201901389E+2}
    , {-2.00E+2, 1.0E+1, 3.33856275141E+1, 5.27455841524E+2, 8.26985931105E+2, 1.48724572106E+1, 7.51677892978E+0, 1.56788957625E+1, 1.12441448548E+1, -5.60464403431E+2}
    , {-1.90E+2, 1.0E+1, 2.85053688122E+1, 6.31456093722E+2, 9.82267201127E+2, 1.68624727841E+1, 8.17858764168E+0, 1.54381704509E+1, 1.1959771372E+1, -7.70658518274E+2}
    , {-2.30E+2, 1.E+2, 9.99214675543E+1, 1.40613895102E+2, 1.14139983678E+3, 2.39355217264E-1, 6.97272606174E+0, 8.8231407359E+0, 1.03335332917E+1, 1.30285717477E+2}
    , {-2.20E+2, 1.00E+2, 9.68747536247E+1, 2.03142217359E+2, 1.23540290715E+3, 2.19472359635E+0, 7.5112609231E+0, 9.92910224531E+0, 1.04433745458E+1, 8.64926582129E+1}
    , {-2.10E+2, 1.00E+2, 9.37951869592E+1, 2.73289528292E+2, 1.33944231547E+3, 3.98598733395E+0, 8.00497827856E+0, 1.0869229208E+1, 1.07311538751E+1, 2.15744281534E+1}
    , {-2.00E+2, 1.E+2, 9.07639636879E+1, 3.51058923604E+2, 1.4528177708E+3, 5.65089895881E+0, 8.60213043187E+0, 1.18143635022E+1, 1.12441448548E+1, -6.23043352337E+1}
    , {-1.90E+2, 1.00E+2, 8.78229441642E+1, 4.37278674361E+2, 1.57593328167E+3, 7.2270552277E+0, 9.33486986561E+0, 1.28177755434E+1, 1.1959771372E+1, -1.63650967822E+2}
};

const unsigned ntd = sizeof(td)/sizeof(TestData);

#endif