/* This file is created by Hongke Zhu, 02-03-2010. 
Chemical & Materials Engineering Department, 
University of Alabama in Huntsville, United States.

LITERATURE REFERENCE \
Lemmon, E.W. and Span, R.,
"Short Fundamental Equations of State for 20 Industrial Fluids,"
J. Chem. Eng. Data, 51:785-850, 2006.
*/

#include "carbonylsulfide.h"

#define CARBONYLSULFIDE_M 60.0751 /* kg/kmol */
#define CARBONYLSULFIDE_R (8314.472/CARBONYLSULFIDE_M) /* J/kg/K */
#define CARBONYLSULFIDE_TSTAR 378.77 /* K */


const IdealData ideal_data_carbonylsulfide = {
    -3.6587449805 /* constant, a_1, adjust to solver s */
    , 3.7349245016 /* linear, a_2, adjust to solver h */
    , CARBONYLSULFIDE_TSTAR /* Tstar */
    , CARBONYLSULFIDE_R /* cp0star */
    , 1 /* power terms */
    , (const IdealPowTerm[]){
        {3.5,	0.0}
    }
    , 4 /* exponential terms */
    , (const IdealExpTerm[]){
        {2.1651,	768.0}
        ,{0.93456,	1363.0}
        ,{1.0623,	3175.0}
        ,{0.34269,	12829.0}
    }
};

const HelmholtzData helmholtz_data_carbonylsulfide = {
    "carbonylsulfide"
	, /* R */ CARBONYLSULFIDE_R /* J/kg/K */
    , /* M */ CARBONYLSULFIDE_M /* kg/kmol */
    , /* rho_star */ 7.41*CARBONYLSULFIDE_M /* kg/m3(= rho_c for this model) */
    , /* T_star */ CARBONYLSULFIDE_TSTAR /* K (= T_c for this model) */

    , /* T_c */ CARBONYLSULFIDE_TSTAR
    , /* rho_c */ 7.41*CARBONYLSULFIDE_M /* kg/m3 */
    , /* T_t */ 0

    , 0.0978 /* acentric factor */
    , &ideal_data_carbonylsulfide
    , 12 /* power terms */
    , (const HelmholtzPowTerm[]){
        /* a_i, 	t_i, 	d_i, 	l_i */
        {0.94374,	0.25,	1.0,	0.0}
        , {-2.5348,	1.125,	1.0,	0.0}
        , {0.59058,	1.5,	1.0,	0.0}
        , {-0.021488,	1.375,	2.0,	0.0}
        , {0.082083,	0.25,	3.0,	0.0}
        , {0.00024689,	0.875,	7.0,	0.0}
        , {0.21226,	0.625,	2.0,	1.0}
        , {-0.041251,	1.75,	5.0,	1.0}
        , {-0.22333,	3.625,	1.0,	2.0}
        , {-0.050828,	3.625,	4.0,	2.0}
        , {-0.028333,	14.5,	3.0,	3.0}
        , {0.016983,	12.0,	4.0,	3.0}
    }
    , 0 /* gaussian terms */
    , 0
    , 0 /* critical terms */
    , 0
};

/*
    Test suite. These tests attempt to validate the current code using a few sample figures output by REFPROP 8.0. To compile and run the test:

    ./test.py carbonylsulfide
*/

#ifdef TEST

#include "test.h"
#include <math.h>
#include <assert.h>
#include <stdio.h>

const TestData td[]; const unsigned ntd;

int main(void){
    //return helm_check_u(&helmholtz_data_carbonylsulfide, ntd, td);
    //return helm_check_dpdT_rho(&helmholtz_data_carbonylsulfide, ntd, td);
    //return helm_check_dpdrho_T(&helmholtz_data_carbonylsulfide, ntd, td);
    //return helm_check_dhdT_rho(&helmholtz_data_carbonylsulfide, ntd, td);
    //return helm_check_dhdrho_T(&helmholtz_data_carbonylsulfide, ntd, td);
    //return helm_check_dudT_rho(&helmholtz_data_carbonylsulfide, ntd, td);
    //return helm_check_dudrho_T(&helmholtz_data_carbonylsulfide, ntd, td);
    return helm_run_test_cases(&helmholtz_data_carbonylsulfide, ntd, td, 'C');
}

/*
A small set of data points calculated using REFPROP 8.0, for validation. 
*/

const TestData td[] = {
    /* Temperature, Pressure, Density, Int. Energy, Enthalpy, Entropy, Cv, Cp, Cp0, Helmholtz */
    /* (C), (MPa), (kg/m3), (kJ/kg), (kJ/kg), (kJ/kg-K), (kJ/kg-K), (kJ/kg-K), (kJ/kg-K), (kJ/kg) */
    {-1.00E+2, 9.99999999999E-2, 1.27622789263E+3, -5.8290521228E+1, -5.82121653163E+1, -2.95390152407E-1, 7.43168548616E-1, 1.16891418867E+0, 5.59004634633E-1, -7.14371633876E+0}
    , {-5.0E+1, 1.E-1, 3.33111970832E+0, 2.79043203476E+2, 3.09063139341E+2, 1.38777409653E+0, 4.93605311433E-1, 6.51087576931E-1, 6.16471293519E-1, -3.06385861654E+1}
    , {0.E+0, 1.E-1, 2.68405891929E+0, 3.05078652556E+2, 3.42335658832E+2, 1.5222036634E+0, 5.35384421038E-1, 6.82531319795E-1, 6.67979502845E-1, -1.10711278102E+2}
    , {5.0E+1, 1.E-1, 2.2552758466E+0, 3.33036004879E+2, 3.77376478862E+2, 1.63992225947E+0, 5.75375918047E-1, 7.18750689013E-1, 7.11092311406E-1, -1.96904873268E+2}
    , {1.00E+2, 1.E-1, 1.9470522898E+0, 3.62787144534E+2, 4.14146833498E+2, 1.74566458331E+0, 6.09711517468E-1, 7.51312975876E-1, 7.46701780621E-1, -2.88607594729E+2}
    , {1.50E+2, 1.E-1, 1.71390439336E+0, 3.94087786721E+2, 4.52434098445E+2, 1.84191705432E+0, 6.3885370514E-1, 7.79488807867E-1, 7.76437557365E-1, -3.85319414815E+2}
    , {2.00E+2, 1.E-1, 1.53106015998E+0, 4.26719269767E+2, 4.92033489688E+2, 1.93034539625E+0, 6.63880207642E-1, 8.03931688635E-1, 8.01769074057E-1, -4.86623654467E+2}
    , {2.50E+2, 1.E-1, 1.38369261799E+0, 4.60507436131E+2, 5.32777822415E+2, 2.01218740894E+0, 6.85730083236E-1, 8.25401561775E-1, 8.23788301947E-1, -5.92168406857E+2}
    , {3.00E+2, 1.E-1, 1.26232845843E+0, 4.95315424076E+2, 5.74534108669E+2, 2.08840379066E+0, 7.0508361563E-1, 8.44493235684E-1, 8.43242045267E-1, -7.01653208539E+2}
    , {3.50E+2, 1.E-1, 1.16061199463E+0, 5.31033146616E+2, 6.17194585979E+2, 2.15975607707E+0, 7.22406006146E-1, 8.61627099103E-1, 8.60627009051E-1, -8.14818852808E+2}
    , {-1.00E+2, 1.E+0, 1.27710568297E+3, -5.84791305388E+1, -5.76961099843E+1, -2.96481144052E-1, 7.43556319505E-1, 1.16820150212E+0, 5.59004634633E-1, -7.14342044613E+0}
    , {-5.0E+1, 1.E+0, 1.17532453844E+3, -2.0613296741E-1, 6.44695860968E-1, -5.39141073123E-4, 7.06202958333E-1, 1.1798816406E+0, 6.16471293519E-1, -8.58236369422E-2}
    , {0.E+0, 1.E+0, 1.05919821388E+3, 6.0660832483E+1, 6.16049428364E+1, 2.4564140941E-1, 7.0261919678E-1, 1.27405373611E+0, 6.67979502845E-1, -6.43611849726E+0}
    , {5.0E+1, 1.E+0, 2.46422188373E+1, 3.25204007874E+2, 3.65784769152E+2, 1.29646292776E+0, 6.03238099812E-1, 8.08729453907E-1, 7.11092311406E-1, -9.3747987232E+1}
    , {1.00E+2, 1.E+0, 2.05425338763E+1, 3.57164920157E+2, 4.05844406633E+2, 1.41173994026E+0, 6.23264983556E-1, 7.99638037339E-1, 7.46701780621E-1, -1.69625838549E+2}
    , {1.50E+2, 1.E+0, 1.77509017636E+1, 3.89710759378E+2, 4.46045925519E+2, 1.51283010563E+0, 6.46510648878E-1, 8.09835957217E-1, 7.76437557365E-1, -2.5044329982E+2}
    , {2.00E+2, 1.E+0, 1.56827474219E+1, 4.23138648115E+2, 4.86902985645E+2, 1.60407753322E+0, 6.68620131596E-1, 8.2478951574E-1, 8.01769074057E-1, -3.3583063673E+2}
    , {2.50E+2, 1.E+0, 1.40726368917E+1, 4.57478946775E+2, 5.28538834677E+2, 1.68771540358E+0, 6.88879496722E-1, 8.40651636666E-1, 8.23788301947E-1, -4.25449366607E+2}
    , {3.00E+2, 1.E+0, 1.27764030308E+1, 4.92692528412E+2, 5.70961818883E+2, 1.76515138191E+0, 7.07301949577E-1, 8.56157450975E-1, 8.43242045267E-1, -5.1900398613E+2}
    , {3.50E+2, 1.E+0, 1.17069202891E+1, 5.28721450286E+2, 6.1414101199E+2, 1.83737278972E+0, 7.24047061073E-1, 8.7085787159E-1, 8.60627009051E-1, -6.16237403631E+2}
    , {-1.00E+2, 1.0E+1, 1.28558796704E+3, -6.02885053369E+1, -5.2509963334E+1, -3.07093331955E-1, 7.47416081172E-1, 1.16178083003E+0, 5.59004634633E-1, -7.11529490882E+0}
    , {-5.0E+1, 1.E+1, 1.18857276837E+3, -3.12487279197E+0, 5.28857926254E+0, -1.38485986721E-2, 7.09702250085E-1, 1.16312526954E+0, 6.16471293519E-1, -3.4557998296E-2}
    , {0.E+0, 1.E+1, 1.08229738606E+3, 5.55728670062E+1, 6.48124716923E+1, 2.26622096008E-1, 7.05042991442E-1, 1.22724930851E+0, 6.67979502845E-1, -6.32895851846E+0}
    , {5.0E+1, 1.E+1, 9.54389538397E+2, 1.18754310982E+2, 1.29232212927E+2, 4.42870150165E-1, 7.13462076273E-1, 1.36818649071E+0, 7.11092311406E-1, -2.43591780434E+1}
    , {1.00E+2, 1.0E+1, 7.6801911315E+2, 1.92835698935E+2, 2.05856208228E+2, 6.62635807547E-1, 7.38216406591E-1, 1.80473715004E+0, 7.46701780621E-1, -5.44268526516E+1}
    , {1.50E+2, 1.0E+1, 3.42073193892E+2, 3.10965381486E+2, 3.40198891093E+2, 9.98498153736E-1, 7.72810027815E-1, 2.46165756423E+0, 7.76437557365E-1, -1.11549112268E+2}
    , {2.00E+2, 1.0E+1, 2.12804982382E+2, 3.76000992944E+2, 4.22992373916E+2, 1.18453786861E+0, 7.26384907923E-1, 1.28117637193E+0, 8.01769074057E-1, -1.8446309959E+2}
    , {2.50E+2, 1.0E+1, 1.6886724235E+2, 4.21920769685E+2, 4.81138886005E+2, 1.30152242664E+0, 7.24050597292E-1, 1.08356791595E+0, 8.23788301947E-1, -2.5897068781E+2}
    , {3.00E+2, 1.0E+1, 1.43715050788E+2, 4.63735014093E+2, 5.33317148637E+2, 1.39682608573E+0, 7.3114236723E-1, 1.01377562168E+0, 8.43242045267E-1, -3.36855856944E+2}
    , {3.50E+2, 1.0E+1, 1.26589854641E+2, 5.04156960606E+2, 5.83152232608E+2, 1.48020744501E+0, 7.41309099387E-1, 9.83608558638E-1, 8.60627009051E-1, -4.18234308752E+2}
};

const unsigned ntd = sizeof(td)/sizeof(TestData);

#endif
