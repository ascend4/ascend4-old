/* This file is created by Hongke Zhu, 05-30-2010.
Chemical & Materials Engineering Department,
University of Alabama in Huntsville, United States.

Revised by John Pye 2012 for the new data structures.

LITERATURE REFERENCE
Lemmon, E.W. and Span, R., 2006.
"Short Fundamental Equations of State for 20 Industrial Fluids,"
J. Chem. Eng. Data, 51:785-850.

Triple point value below comes from
http://en.wikipedia.org/wiki/Xenon
which in turn cites
Lide, David R. (2004). "Section 4, Properties of the Elements and Inorganic Compounds; Melting, boiling, triple, and critical temperatures of the elements". CRC Handbook of Chemistry and Physics (85th edition ed.). Boca Raton, Florida: CRC Press. ISBN 0849304857.

There is also a paper devoted to the properties of Xenon only:
Sifner & Klomfar, J. Phys. Chem. Ref. Data 23, 63 (1994); doi:10.1063/1.555956
*/

#include "../helmholtz.h"

#define XENON_M 131.293 /* kg/kmol */
#define XENON_R (8314.472/XENON_M) /* J/kg/K */
#define XENON_TC 289.733 /* K */

static const IdealData ideal_data_xenon = {
	IDEAL_CP0, {.cp0={
		XENON_R /* cp0star */
		, 1. /* Tstar */
		, 1 /* power terms */
		, (const Cp0PowTerm[]){
			{2.5,	0.0}
		}
	}}
};

static HelmholtzData helmholtz_data_xenon = {
    /* R */ XENON_R /* J/kg/K */
    , /* M */ XENON_M /* kg/kmol */
    , /* rho_star */ 8.40*XENON_M /* kg/m3(= rho_c for this model) */
    , /* T_star */ XENON_TC /* K (= T_c for this model) */

    , /* T_c */ XENON_TC
    , /* rho_c */ 8.40*XENON_M /* kg/m3 */
    , /* T_t */ 161.405

    , {FPROPS_REF_NBP}

    , 0.00363 /* acentric factor */
    , &ideal_data_xenon
    , 12 /* power terms */
    , (const HelmholtzPowTerm[]){
        /* a_i, 	t_i, 	d_i, 	l_i */
        {0.83115,	0.25,	1.0,	0.0}
        , {-2.3553,	1.125,	1.0,	0.0}
        , {0.53904,	1.5,	1.0,	0.0}
        , {0.014382,	1.375,	2.0,	0.0}
        , {0.066309,	0.25,	3.0,	0.0}
        , {0.00019649,	0.875,	7.0,	0.0}
        , {0.14996,	0.625,	2.0,	1.0}
        , {-0.035319,	1.75,	5.0,	1.0}
        , {-0.15929,	3.625,	1.0,	2.0}
        , {-0.027521,	3.625,	4.0,	2.0}
        , {-0.023305,	14.5,	3.0,	3.0}
        , {0.0086941,	12.0,	4.0,	3.0}
    }
};

EosData eos_xenon = {
	"xenon"
	,"Lemmon, E.W. and Span, R., 2006. 'Short Fundamental Equations of "
	"State for 20 Industrial Fluids', J. Chem. Eng. Data, 51:785-850."
	, "http://dx.doi.org/10.1021/je050186n"
	,100
	,FPROPS_HELMHOLTZ
	,.data = {.helm = &helmholtz_data_xenon}
};

/*
    Test suite. These tests attempt to validate the current code using a few sample figures output by REFPROP 8.0. To compile and run the test:

    ./test.py xenon
*/

#ifdef TEST

#include "../test.h"
#include <math.h>
#include <assert.h>
#include <stdio.h>

const TestData td[]; const unsigned ntd;

int main(void){
	test_init();
	PureFluid *P = helmholtz_prepare(&eos_xenon, NULL);
    return helm_run_test_cases(P, ntd, td, 'C');
}

/*
A small set of data points calculated using REFPROP 8.0, for validation.
*/

const TestData td[] = {
    /* Temperature, Pressure, Density, Int. Energy, Enthalpy, Entropy, Cv, Cp, Cp0, Helmholtz */
    /* (C), (MPa), (kg/m3), (kJ/kg), (kJ/kg), (kJ/kg-K), (kJ/kg-K), (kJ/kg-K), (kJ/kg-K), (kJ/kg) */
    {-1.00E+2, 1.E-1, 9.36848917969E+0, 8.6327702303E+1, 9.70017819845E+1, 5.8831349063E-1, 1.00512841587E-1, 1.71403887144E-1, 1.58319026909E-1, -1.55387785997E+1}
    , {-5.0E+1, 1.E-1, 7.16397983839E+0, 9.13514459681E+1, 1.05310167552E+2, 6.30511180633E-1, 9.66067672329E-2, 1.6299817289E-1, 1.58319026909E-1, -4.93471239903E+1}
    , {0.E+0, 1.E-1, 5.82034868922E+0, 9.62098864264E+1, 1.13390987652E+2, 6.63194216164E-1, 9.56423273432E-2, 1.60632440044E-1, 1.58319026909E-1, -8.49416137187E+1}
    , {5.0E+1, 1.E-1, 4.9066948069E+0, 1.01015445343E+2, 1.21395763242E+2, 6.90107769162E-1, 9.53075089878E-2, 1.59682724893E-1, 1.58319026909E-1, -1.21992880262E+2}
    , {1.00E+2, 1.E-1, 4.24297240672E+0, 1.05798689091E+2, 1.29367072388E+2, 7.13044232876E-1, 9.51679763144E-2, 1.59218498729E-1, 1.58319026909E-1, -1.60273766407E+2}
    , {1.50E+2, 1.E-1, 3.73829034393E+0, 1.10570743884E+2, 1.37320940043E+2, 7.33047953063E-1, 9.51015512228E-2, 1.58958806012E-1, 1.58319026909E-1, -1.99618497454E+2}
    , {2.00E+2, 1.E-1, 3.34133806444E+0, 1.15336455243E+2, 1.45264585253E+2, 7.50791956275E-1, 9.50664557477E-2, 1.58798785207E-1, 1.58319026909E-1, -2.39900758868E+2}
    , {2.50E+2, 1.E-1, 3.02083040141E+0, 1.20098228639E+2, 1.53201709044E+2, 7.66738642268E-1, 9.50462664985E-2, 1.58692898017E-1, 1.58319026909E-1, -2.81021092064E+2}
    , {3.00E+2, 1.E-1, 2.75656613035E+0, 1.24857383813E+2, 1.61134402129E+2, 7.81220496289E-1, 9.50337940351E-2, 1.58618954777E-1, 1.58319026909E-1, -3.22899143635E+2}
    , {-1.00E+2, 1.E+0, 2.89190097483E+3, 2.59488457532E+0, 2.94067788177E+0, 1.55711568178E-2, 1.62312288167E-1, 3.40783851272E-1, 1.58319026909E-1, -1.0126122768E-1}
    , {-5.0E+1, 1.E+0, 8.220805774E+1, 8.77991384438E+1, 9.99633961515E+1, 4.68134634274E-1, 1.18618539647E-1, 2.33786309142E-1, 1.58319026909E-1, -1.66651051943E+1}
    , {0.E+0, 1.E+0, 6.22427285811E+1, 9.40458273468E+1, 1.10111961059E+2, 5.09322785095E-1, 1.02055739967E-1, 1.85936707426E-1, 1.58319026909E-1, -4.50756914018E+1}
    , {5.0E+1, 1.E+0, 5.10118952611E+1, 9.94432538322E+1, 1.19046524694E+2, 5.39389046968E-1, 9.82802043354E-2, 1.73399540812E-1, 1.58319026909E-1, -7.48603166954E+1}
    , {1.00E+2, 1.E+0, 4.34757142389E+1, 1.04561710034E+2, 1.27563057281E+2, 5.63902880904E-1, 9.68008045183E-2, 1.67901578071E-1, 1.58319026909E-1, -1.05858649975E+2}
    , {1.50E+2, 1.E+0, 3.79824360431E+1, 1.09550491693E+2, 1.35878450197E+2, 5.84819290392E-1, 9.61110390802E-2, 1.64986961881E-1, 1.58319026909E-1, -1.37915791036E+2}
    , {2.00E+2, 1.E+0, 3.37697496582E+1, 1.1446867617E+2, 1.44080977419E+2, 6.03143196038E-1, 9.57505193261E-2, 1.63249621469E-1, 1.58319026909E-1, -1.70908527036E+2}
    , {2.50E+2, 1.E+0, 3.04235132216E+1, 1.19344169015E+2, 1.52213482717E+2, 6.19483280164E-1, 9.55444896312E-2, 1.62125484579E-1, 1.58319026909E-1, -2.04738509002E+2}
    , {3.00E+2, 1.E+0, 2.76950599339E+1, 1.24191797101E+2, 1.60299319612E+2, 6.34245195387E-1, 9.54177669404E-2, 1.61352847776E-1, 1.58319026909E-1, -2.39325836634E+2}
    , {-1.00E+2, 1.E+1, 2.93725369531E+3, 1.42454184988E+0, 4.82908263435E+0, 8.64631175564E-3, 1.64104494073E-1, 3.31802768541E-1, 1.58319026909E-1, -7.25670306076E-2}
    , {-5.0E+1, 1.E+1, 2.59640858653E+3, 1.80237322025E+1, 2.18752061392E+1, 9.50063202207E-2, 1.45676797587E-1, 3.56265748871E-1, 1.58319026909E-1, -3.17692815476E+0}
    , {0.E+0, 1.0E+1, 2.14081053724E+3, 3.68872867114E+1, 4.15584146912E+1, 1.74280614775E-1, 1.35823640808E-1, 4.54345678644E-1, 1.58319026909E-1, -1.07174632145E+1}
    , {5.0E+1, 1.0E+1, 1.12665337176E+3, 6.80469634926E+1, 7.69228078744E+1, 2.91556011398E-1, 1.45659224313E-1, 9.95770179441E-1, 1.58319026909E-1, -2.61693615907E+1}
    , {1.00E+2, 1.E+1, 5.90618508367E+2, 8.83520550852E+1, 1.05283458112E+2, 3.74218522256E-1, 1.15963058923E-1, 3.54895079475E-1, 1.58319026909E-1, -5.12875864945E+1}
    , {1.50E+2, 1.0E+1, 4.48916496289E+2, 9.7788962879E+1, 1.200648206E+2, 4.11518346431E-1, 1.07063808522E-1, 2.56303368253E-1, 1.58319026909E-1, -7.63450254132E+1}
    , {2.00E+2, 1.0E+1, 3.73342628802E+2, 1.05076328624E+2, 1.31861376E+2, 4.37905546704E-1, 1.02956395259E-1, 2.20222161002E-1, 1.58319026909E-1, -1.02118680799E+2}
    , {2.50E+2, 1.0E+1, 3.23688475808E+2, 1.11477967087E+2, 1.42371868933E+2, 4.59037600924E-1, 1.00708596166E-1, 2.01960415432E-1, 1.58319026909E-1, -1.28667553836E+2}
    , {3.00E+2, 1.0E+1, 2.87613513443E+2, 1.17409760542E+2, 1.52178641462E+2, 4.76948171948E-1, 9.93558515127E-2, 1.91149270166E-1, 1.58319026909E-1, -1.5595308421E+2}
    , {-5.0E+1, 1.E+2, 3.02045242396E+3, 7.53560940932E+0, 4.06432323623E+1, 3.71753573062E-2, 1.60523260949E-1, 2.90674275637E-1, 1.58319026909E-1, -7.60071573554E-1}
    , {0.E+0, 1.00E+2, 2.82643277558E+3, 1.9605915742E+1, 5.49862017562E+1, 9.51959404914E-2, 1.4814571451E-1, 2.83509955688E-1, 1.58319026909E-1, -6.39685540325E+0}
    , {5.0E+1, 1.00E+2, 2.63925746546E+3, 3.11263826506E+1, 6.90158274315E+1, 1.42375874881E-1, 1.39266126262E-1, 2.77708360918E-1, 1.58319026909E-1, -1.48823813172E+1}
    , {1.00E+2, 1.00E+2, 2.46058140062E+3, 4.21087780967E+1, 8.27495794841E+1, 1.81902374513E-1, 1.32573793307E-1, 2.714987457E-1, 1.58319026909E-1, -2.57680929528E+1}
    , {1.50E+2, 1.00E+2, 2.29270432412E+3, 5.25327453412E+1, 9.61493595501E+1, 2.15611253722E-1, 1.27377081617E-1, 2.64333143261E-1, 1.58319026909E-1, -3.87031566715E+1}
    , {2.00E+2, 1.00E+2, 2.13754842506E+3, 6.23873424151E+1, 1.09169908287E+2, 2.4470369902E-1, 1.23255173711E-1, 2.56397884819E-1, 1.58319026909E-1, -5.33942127762E+1}
    , {2.50E+2, 1.00E+2, 1.99610598678E+3, 7.16869013071E+1, 1.21784441549E+2, 2.70054668603E-1, 1.19928609398E-1, 2.48178743072E-1, 1.58319026909E-1, -6.95921985727E+1}
    , {3.00E+2, 1.00E+2, 1.86839405494E+3, 8.04690099729E+1, 1.33990910097E+2, 2.92344194281E-1, 1.17203426749E-1, 2.40141558603E-1, 1.58319026909E-1, -8.70880649793E+1}
};

const unsigned ntd = sizeof(td)/sizeof(TestData);

#endif
