/* This file is created by Hongke Zhu, 7-24-2010. 
Chemical & Materials Engineering Department, 
University of Alabama in Huntsville, United States.

LITERATURE REFERENCE
Lemmon, E.W. and Span, R.,
"Short Fundamental Equations of State for 20 Industrial Fluids,"
J. Chem. Eng. Data, 51:785-850, 2006.
*/

#include "r141b.h"

#define R141B_M 116.94962 /* kg/kmol */
#define R141B_R (8314.472/R141B_M) /* J/kg/K */
#define R141B_TSTAR 477.5 /* K */


const IdealData ideal_data_r141b = {
    -15.5074814985 /* constant */
    , 9.1871858933 /* linear */
    , R141B_TSTAR /* Tstar */
    , R141B_R /* cp0star */
    , 1 /* power terms */
    , (const IdealPowTerm[]){
        {4.0,	0.0}
    }
    , 3 /* exponential terms */
    , (const IdealExpTerm[]){
        {6.8978,    502.0}
        ,{7.8157,   1571.0}
        ,{3.2039,   4603.0}
    } 
};

const HelmholtzData helmholtz_data_r141b = {
	"r141b"
    , /* R */ R141B_R /* J/kg/K */
    , /* M */ R141B_M /* kg/kmol */
    , /* rho_star */ 3.921*R141B_M /* kg/m3(= rho_c for this model) */
    , /* T_star */ R141B_TSTAR /* K (= T_c for this model) */

    , /* T_c */ R141B_TSTAR
    , /* rho_c */ 3.921*R141B_M /* kg/m3 */
    , /* T_t */ 0

    , 0.2195 /* acentric factor */
    , &ideal_data_r141b
    , 12 /* power terms */
    , (const HelmholtzPowTerm[]){
        /* a_i, 	t_i, 	d_i, 	l_i */
        {1.1469,          0.25,    1.0,   0}
        , {-3.6799,         1.25,    1.0,   0}
  	, {1.3469,          1.5,     1.0,   0}
  	, {0.083329,        0.25,    3.0,   0}
  	, {0.00025137,      0.875,   7.0,   0}
  	, {0.32720,         2.375,   1.0,   1}
  	, {0.46946,         2.0,     2.0,   1}
 	, {-0.029829,       2.125,   5.0,   1}
 	, {-0.31621,        3.5,     1.0,   2}
 	, {-0.026219,       6.5,     1.0,   2}
 	, {-0.078043,       4.75,    4.0,   2}
 	, {-0.020498,       12.5,    2.0,   3}
    }
    , 0 /* gaussian terms */
    , 0
    , 0 /* critical terms */
    , 0
};

/*
    Test suite. These tests attempt to validate the current code using a few sample figures output by REFPROP 8.0. To compile and run the test:

    ./test.py r141b
*/

#ifdef TEST

#include "test.h"
#include <math.h>
#include <assert.h>
#include <stdio.h>

const TestData td[]; const unsigned ntd;

int main(void){
    //return helm_check_u(&helmholtz_data_r141b, ntd, td);
    //return helm_check_dpdT_rho(&helmholtz_data_r141b, ntd, td);
    //return helm_check_dpdrho_T(&helmholtz_data_r141b, ntd, td);
    //return helm_check_dhdT_rho(&helmholtz_data_r141b, ntd, td);
    //return helm_check_dhdrho_T(&helmholtz_data_r141b, ntd, td);
    //return helm_check_dudT_rho(&helmholtz_data_r141b, ntd, td);
    //return helm_check_dudrho_T(&helmholtz_data_r141b, ntd, td);
    return helm_run_test_cases(&helmholtz_data_r141b, ntd, td, 'C');
}

/*
A small set of data points calculated using REFPROP 8.0, for validation. 
*/

const TestData td[] = {
    /* Temperature, Pressure, Density, Int. Energy, Enthalpy, Entropy, Cv, Cp, Cp0, Helmholtz */
    /* (C), (MPa), (kg/m3), (kJ/kg), (kJ/kg), (kJ/kg-K), (kJ/kg-K), (kJ/kg-K), (kJ/kg-K), (kJ/kg) */
    {-1.00E+2, 9.99999999998E-2, 1.46248534033E+3, 8.87904306508E+1, 8.88588074046E+1, 4.92715595865E-1, 8.02029221607E-1, 1.15209126069E+0, 5.438352246E-1, 3.47672522675E+0}
    , {-5.0E+1, 9.99999999999E-2, 1.37200943192E+3, 1.44574576282E+2, 1.44647462078E+2, 7.76049544236E-1, 7.67567013309E-1, 1.09913447316E+0, 6.35547429534E-1, -2.86008795141E+1}
    , {0.E+0, 1.E-1, 1.28126910505E+3, 1.99955825236E+2, 2.00033872853E+2, 9.99918551683E-1, 7.87513460713E-1, 1.12481458738E+0, 7.16056879163E-1, -7.31719271564E+1}
    , {5.0E+1, 1.E-1, 4.48916187938E+0, 4.51761420045E+2, 4.74037293067E+2, 1.90399211543E+0, 7.35939176981E-1, 8.20258261979E-1, 7.90253145557E-1, -1.63513632056E+2}
    , {1.00E+2, 1.E-1, 3.83806684282E+0, 4.90186890097E+2, 5.16241673428E+2, 2.02533850857E+0, 7.92805565507E-1, 8.70363806051E-1, 8.57850364822E-1, -2.65568174376E+2}
    , {1.50E+2, 1.E-1, 3.36319932414E+0, 5.31395622429E+2, 5.61129215464E+2, 2.1381554999E+0, 8.49722976626E-1, 9.24757402882E-1, 9.18052313589E-1, -3.73364877351E+2}
    , {2.00E+2, 1.E-1, 2.99679923312E+0, 5.75276469651E+2, 6.08645405046E+2, 2.24424086033E+0, 9.01207667854E-1, 9.74999157841E-1, 9.70844810863E-1, -4.86586093414E+2}
    , {-1.00E+2, 1.E+0, 1.46319763016E+3, 8.86559140259E+1, 8.93393487031E+1, 4.9193766027E-1, 8.0244143879E-1, 1.15195651379E+0, 5.438352246E-1, 3.47690815021E+0}
    , {-5.0E+1, 1.E+0, 1.37303777893E+3, 1.44384341527E+2, 1.45112653607E+2, 7.75195703229E-1, 7.67916853549E-1, 1.09862667708E+0, 6.35547429534E-1, -2.86005796483E+1}
    , {0.E+0, 1.E+0, 1.28279417593E+3, 1.99679255969E+2, 2.00458804255E+2, 9.98904169064E-1, 7.87797609974E-1, 1.12363430107E+0, 7.16056879163E-1, -7.31714178103E+1}
    , {5.0E+1, 1.E+0, 1.18619897535E+3, 2.57290363511E+2, 2.58133392396E+2, 1.19264731901E+0, 8.27772283811E-1, 1.18908019323E+0, 7.90253145557E-1, -1.28113617627E+2}
    , {1.00E+2, 1.E+0, 1.0740996829E+3, 3.1908982981E+2, 3.20020842095E+2, 1.37053310133E+0, 8.7531252101E-1, 1.29556744847E+0, 8.57850364822E-1, -1.92324596953E+2}
    , {1.50E+2, 1.E+0, 3.8252842489E+1, 5.22833974048E+2, 5.48975822207E+2, 1.95361146903E+0, 8.84300552477E-1, 1.01943596654E+0, 9.18052313589E-1, -3.03836719071E+2}
    , {2.00E+2, 1.E+0, 3.25126032673E+1, 5.69101440253E+2, 5.99858743561E+2, 2.06726415206E+0, 9.16592873811E-1, 1.02336259937E+0, 9.70844810863E-1, -4.09024593292E+2}
    , {-5.0E+1, 1.E+1, 1.38292098856E+3, 1.42564933473E+2, 1.49796004577E+2, 7.66915612375E-1, 7.71500273343E-1, 1.09418498915E+0, 6.35547429534E-1, -2.85722854287E+1}
    , {0.E+0, 1.E+1, 1.29714371507E+3, 1.97084769428E+2, 2.04794015431E+2, 9.89235217173E-1, 7.90821009737E-1, 1.1135777883E+0, 7.16056879163E-1, -7.31248301427E+1}
    , {5.0E+1, 1.E+1, 1.20807237242E+3, 2.534381584E+2, 2.61715808174E+2, 1.18047422414E+0, 8.29946112911E-1, 1.16746143731E+0, 7.90253145557E-1, -1.28032087131E+2}
    , {1.00E+2, 1.E+1, 1.11087969992E+3, 3.12892411827E+2, 3.21894286648E+2, 1.35349428589E+0, 8.7550481076E-1, 1.24306809547E+0, 8.57850364822E-1, -1.92163980953E+2}
    , {1.50E+2, 1.E+1, 9.98434717881E+2, 3.76412651092E+2, 3.86428328452E+2, 1.5156606304E+0, 9.2194562522E-1, 1.34377545033E+0, 9.18052313589E-1, -2.64939144661E+2}
    , {2.00E+2, 1.0E+1, 8.56340050315E+2, 4.45484773166E+2, 4.5716237717E+2, 1.67349970294E+0, 9.68584677668E-1, 1.5002422066E+0, 9.70844810863E-1, -3.46331611278E+2}
    , {-5.0E+1, 1.00E+2, 1.45716981183E+3, 1.29705725771E+2, 1.98331907282E+2, 7.00910602813E-1, 8.0862477239E-1, 1.08058616822E+0, 6.35547429534E-1, -2.67024752468E+1}
    , {0.E+0, 1.00E+2, 1.394116221E+3, 1.80451419348E+2, 2.52181450528E+2, 9.18652378864E-1, 8.24236394411E-1, 1.08048928781E+0, 7.16056879163E-1, -7.04784779385E+1}
    , {5.0E+1, 1.00E+2, 1.33481136468E+3, 2.31919132599E+2, 3.06836085396E+2, 1.10233017287E+0, 8.60540575675E-1, 1.10841705611E+0, 7.90253145557E-1, -1.24298862762E+2}
    , {1.00E+2, 1.00E+2, 1.27810273277E+3, 2.84937458515E+2, 3.63178430415E+2, 1.26437780817E+0, 9.03258168292E-1, 1.14595253392E+0, 8.57850364822E-1, -1.86865120605E+2}
    , {1.50E+2, 1.E+2, 1.22347982842E+3, 3.39708499733E+2, 4.21442581225E+2, 1.41085714176E+0, 9.45769640563E-1, 1.18431332188E+0, 9.18052313589E-1, -2.57295699803E+2}
    , {2.00E+2, 1.00E+2, 1.17080149547E+3, 3.96143587298E+2, 4.8155516252E+2, 1.5450949142E+0, 9.8527652757E-1, 1.21947882268E+0, 9.70844810863E-1, -3.34918071353E+2}
};

const unsigned ntd = sizeof(td)/sizeof(TestData);

#endif
