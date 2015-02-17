/*
Copyright (C) John Pye 2012

LITERATURE REFERENCE \
Lemmon, E.W. and Span, R.,
"Short Fundamental Equations of State for 20 Industrial Fluids,"
J. Chem. Eng. Data, 51:785-850, 2006.
*/

#include "../helmholtz.h"

#define PROPANE_M 44.09562 /* kg/kmol */
#define PROPANE_R (8314.472/PROPANE_M) /* J/kg/K */
#define PROPANE_TC 369.89 /* K */
#define PROPANE_RHOC (5.00*PROPANE_M)

static const IdealData ideal_data_propane = {
	IDEAL_CP0
	,.data = {.cp0 = {
		.cp0star = PROPANE_R /* cp0star */
		, .Tstar = 1. /* Tstar */
		, .np = 1 /* power terms */
		, .pt = (const Cp0PowTerm[]){
		    {4.0,	0.0}
		}
		, .ne = 4 /* exponential terms */
		, .et = (const Cp0ExpTerm[]){
		    {3.043,	393.0}
		    ,{5.874, 1237.0}
		    ,{9.337, 1984.0}
		    ,{7.922, 4351}
		}
	}}
};

static HelmholtzData helmholtz_data_propane = {
	.R = PROPANE_R /* J/kg/K */
	, .M = PROPANE_M /* kg/kmol */
	, .rho_star = PROPANE_RHOC /* kg/m3 */
	, .T_star = PROPANE_TC /* K (= T_c for this model) */
	, .T_c = PROPANE_TC
	, .rho_c = PROPANE_RHOC /* kg/m3 */
	, .T_t = 85.525 /* K */
	, .ref = {FPROPS_REF_IIR}
	, .omega = 0.3071 /* acentric factor */
	, .ideal = &ideal_data_propane
	, .np = 11 /* power terms */
	, .pt = (const HelmholtzPowTerm[]){
		/* a_i, 	t_i, 	d_i, 	l_i */
		{0.042910051, 1.00, 4, 0}
		, {1.7313671, 0.33, 1, 0}
		, {-2.4516524, 0.80, 1, 0}
		, {0.34157466, 0.43, 2, 0}
		, {-0.46047898, 0.90, 2, 0}
		, {-0.66847295, 2.46, 1, 1}
		, {0.20889705, 2.09, 3, 1}
		, {0.19421381, 0.88, 6, 1}
		, {-0.22917851, 1.09, 6, 1}
		, {-0.60405866, 3.25, 2, 2}
		, {0.066680654, 4.62, 3, 2}
	}
	, .ng = 7 /* gaussian terms */
	, .gt = (const HelmholtzGausTerm[]){
		{0.017534618, 0.76, 1, 0.963, 2.33, 0.684, 1.283}
		, {0.33874242, 2.50, 1, 1.977, 3.47, 0.829, 0.6936}
		, {0.22228777, 2.75, 1, 1.917, 3.15, 1.419, 0.788}
		, {-0.23219062, 3.05, 2, 2.307, 3.19, 0.817, 0.473}
		, {-0.092206940, 2.55, 2, 2.546, 0.92, 1.500, 0.8577}
		, {-0.47575718, 8.40, 4, 3.28, 18.8, 1.426, 0.271}
		, {-0.017486824, 6.75, 1, 14.6, 547.8, 1.093, 0.948}
	}
};

EosData eos_propane = {
	"propane"
	,"Eric W. Lemmon* and Mark O. McLinden, 2009. 'Thermodynamic "
	"Properties of Propane. III. A Reference Equation of State for "
	"Temperatures from the Melting Line to 650 K and Pressures up "
	"to 1000 MPa', J. Chem. Eng. Data 54 (12), pp 3141-3180."
	,"http://dx.doi.org/10.1021/je900217v"
	,100
	,FPROPS_HELMHOLTZ
	,.data = {.helm = &helmholtz_data_propane}
};


/*
    Test suite. These tests attempt to validate the current code using a few sample figures output by REFPROP 8.0. To compile and run the test:

    ./test.py propane
*/

#ifdef TEST

#include "../test.h"
#include <math.h>
#include <assert.h>
#include <stdio.h>

const TestData td[]; const unsigned ntd;

int main(void){
	test_init();
	PureFluid *P = helmholtz_prepare(&eos_propane,NULL);
	return helm_run_test_cases(P, ntd, td, 'C');
}

/*
A small set of data points calculated using REFPROP 8.0, for validation.
*/

/*
reference values from the paper... TODO, check these...

Table 5. Calculated Values of Properties for Algorithm Verification
T F p cV cp w
K mol �dm-3 MPa J � mol-1 �K-1 J � mol-1 �K-1 m� s-1
200.0 14.0 2.3795138 61.078424 93.475362 1381.9552
300.0 12.0 19.053797 73.972542 108.61529 958.40520
300.0 0.4 0.84694991 69.021875 85.753997 221.88959
400.0 5.0 6.6462840 97.017439 271.07044 194.65847
369.9 5.0 4.2519399 117.71621 753625.00 130.89800
*/

const TestData td[] = {
    /* Temperature, Pressure, Density, Int. Energy, Enthalpy, Entropy, Cv, Cp, Cp0, Helmholtz */
    /* (C), (MPa), (kg/m3), (kJ/kg), (kJ/kg), (kJ/kg-K), (kJ/kg-K), (kJ/kg-K), (kJ/kg-K), (kJ/kg) */
	{-1.80E+2, 9.99999999989E-2, 7.25234992826E+2, -1.82029081025E+2, -1.81891194683E+2, -1.23222155438E+0, 1.34655250718E+0, 1.92263935734E+0, 9.09347446848E-1, -6.72476432337E+1}
	, {-1.30E+2, 1.E-1, 6.74448980361E+2, -8.42658746369E+1, -8.41176054529E+1, -3.93075710427E-1, 1.33584693346E+0, 1.99361544706E+0, 1.08633730659E+0, -2.79970866893E+1}
	, {-8.0E+1, 9.99999999999E-2, 6.22822729975E+2, 1.79609859406E+1, 1.81215452702E+1, 2.18662855787E-1, 1.37237523626E+0, 2.10564083219E+0, 1.24697884454E+0, -2.42737446546E+1}
	, {-4.24117715989E+1, 1.E-1, 5.81227004323E+2, 9.95139362506E+1, 9.96859860681E+1, 6.04151569701E-1, 1.44553741986E+0, 2.24468145231E+0, 1.38221677547E+0, -3.98869266279E+1}
	, {-4.24117715989E+1, 1.E-1, 2.38675832078E+0, 4.83692995987E+2, 5.25590828344E+2, 2.44998719576E+0, 1.21591004504E+0, 1.43871494816E+0, 1.38221677547E+0, -8.16127091672E+1}
	, {-3.0E+1, 1.E-1, 2.25100679786E+0, 4.99254600841E+2, 5.43679166815E+2, 2.52633607468E+0, 1.26017205725E+0, 1.47669815919E+0, 1.43059313142E+0, -1.15024015719E+2}
	, {2.0E+1, 1.E-1, 1.84017864047E+0, 5.67660354991E+2, 6.22002905111E+2, 2.8187304274E+0, 1.46084720473E+0, 1.66366568427E+0, 1.6410311072E+0, -2.58650469801E+2}
	, {7.0E+1, 1.E-1, 1.56146508567E+0, 6.46464865985E+2, 7.10507284171E+2, 3.09704424177E+0, 1.68191133055E+0, 1.87895162126E+0, 1.86607565728E+0, -4.16285865579E+2}
	, {1.20E+2, 1.E-1, 1.35793673677E+0, 7.36354094187E+2, 8.09995227306E+2, 3.36735668984E+0, 1.90622791039E+0, 2.10038325565E+0, 2.09240802507E+0, -5.87522188426E+2}
	, {1.70E+2, 1.E-1, 1.20211996873E+0, 8.37261861089E+2, 9.20448233998E+2, 3.63156113511E+0, 2.12361975439E+0, 2.31616797739E+0, 2.3108363806E+0, -7.72064455935E+2}
	, {2.20E+2, 1.E-1, 1.07874431318E+0, 9.4871909825E+2, 1.04141947106E+3, 3.89001525417E+0, 2.32895985288E+0, 2.5205238098E+0, 2.51669915504E+0, -9.69641924342E+2}
	, {2.70E+2, 1.E-1, 9.78526655977E-1, 1.07008111478E+3, 1.1722755715E+3, 4.14260793299E+0, 2.52053097044E+0, 2.71144248298E+0, 2.70853972026E+0, -1.17997638402E+3}
	, {3.20E+2, 1.E-1, 8.95456190084E-1, 1.20066488687E+3, 1.31233980867E+3, 4.38917937668E+0, 2.69850125777E+0, 2.88895336392E+0, 2.8866576145E+0, -1.4027768604E+3}
	, {-1.80E+2, 1.E+0, 7.25530112513E+2, -1.82193282014E+2, -1.8081497947E+2, -1.233987623E+0, 1.34733858576E+0, 1.92237580271E+0, 9.09347446848E-1, -6.72473349317E+1}
	, {-1.30E+2, 9.99999999999E-1, 6.74888343313E+2, -8.45511489894E+1, -8.30694224047E+1, -3.95072250641E-1, 1.3367359526E+0, 1.99266092022E+0, 1.08633730659E+0, -2.79965563102E+1}
	, {-8.0E+1, 1.E+0, 6.235120041E+2, 1.74868560384E+1, 1.90906743986E+1, 2.16203086417E-1, 1.37335970625E+0, 2.10325641103E+0, 1.24697884454E+0, -2.4272770103E+1}
	, {-3.0E+1, 1.E+0, 5.67737824869E+2, 1.26925900685E+2, 1.28687277075E+2, 7.19952650307E-1, 1.47864162548E+0, 2.29951463567E+0, 1.43059313142E+0, -4.81305862368E+1}
	, {2.0E+1, 1.E+0, 5.0052182725E+2, 2.49663418239E+2, 2.51661333107E+2, 1.1788558685E+0, 1.6466728911E+0, 2.66175028436E+0, 1.6410311072E+0, -9.59181796109E+1}
	, {2.69423309499E+1, 1.E+0, 4.89300842942E+2, 2.68363068302E+2, 2.7040680073E+2, 1.24205181697E+0, 1.67515053463E+0, 2.74066307103E+0, 1.67173407094E+0, -1.04367156613E+2}
	, {2.69423309499E+1, 1.E+0, 2.16811191866E+1, 5.56567432054E+2, 6.02690512297E+2, 2.34932340407E+0, 1.58843150866E+0, 2.04209705678E+0, 1.67173407094E+0, -1.48446504428E+2}
	, {7.0E+1, 1.E+0, 1.73771720326E+1, 6.32081387027E+2, 6.89628149994E+2, 2.62003349924E+0, 1.7290057823E+0, 2.03859180923E+0, 1.86607565728E+0, -2.66983108239E+2}
	, {1.20E+2, 1.E+0, 1.44824654218E+1, 7.2594011139E+2, 7.94989128314E+2, 2.90643469401E+0, 1.92988094159E+0, 2.18672301065E+0, 2.09240802507E+0, -4.16724688561E+2}
	, {1.70E+2, 1.E+0, 1.25344733497E+1, 8.29070545781E+2, 9.08850523136E+2, 3.17883848164E+0, 2.13638522932E+0, 2.37001453059E+0, 2.3108363806E+0, -5.79631727359E+2}
	, {2.20E+2, 1.E+0, 1.10961515057E+1, 9.41931523796E+2, 1.03205285996E+3, 3.44207880678E+0, 2.33650345059E+0, 2.55759108237E+0, 2.51669915504E+0, -7.55529639769E+2}
	, {2.70E+2, 1.E+0, 9.97668319401E+0, 1.06426720065E+3, 1.16450091365E+3, 3.69775372261E+0, 2.52547779201E+0, 2.73885662741E+0, 2.70853972026E+0, -9.44167733784E+2}
	, {3.20E+2, 1.E+0, 9.0746481658E+0, 1.19557584945E+3, 1.30577295922E+3, 3.94645632762E+0, 2.70206691818E+0, 2.91027405956E+0, 2.8866576145E+0, -1.14526472127E+3}
	, {-1.80E+2, 1.E+1, 7.28427470562E+2, -1.83781797128E+2, -1.70053594384E+2, -1.25136280431E+0, 1.3549967825E+0, 1.92001963404E+0, 9.09347446848E-1, -6.72173519066E+1}
	, {-1.30E+2, 1.0E+1, 6.79149304169E+2, -8.72844576797E+1, -7.25601548812E+1, -4.14520149562E-1, 1.34538781168E+0, 1.98409972074E+0, 1.08633730659E+0, -2.79458982699E+1}
	, {-8.0E+1, 1.E+1, 6.300466967E+2, 1.30322508581E+1, 2.89040902826E+1, 1.92673960506E-1, 1.38287703721E+0, 2.08266951505E+0, 1.24697884454E+0, -2.41827246136E+1}
	, {-3.0E+1, 1.0E+1, 5.78499956913E+2, 1.1957538706E+2, 1.3686147305E+2, 6.89002530835E-1, 1.48656064513E+0, 2.24942324566E+0, 1.43059313142E+0, -4.79555783121E+1}
	, {2.0E+1, 1.E+1, 5.21242687623E+2, 2.3615207713E+2, 2.55336998545E+2, 1.13137037573E+0, 1.64728467568E+0, 2.50618689319E+0, 1.6410311072E+0, -9.55091485158E+1}
	, {7.0E+1, 1.0E+1, 4.51771231353E+2, 3.67473398527E+2, 3.89608495466E+2, 1.55348456331E+0, 1.84691044343E+0, 2.89545882399E+0, 1.86607565728E+0, -1.65604829374E+2}
	, {1.20E+2, 1.E+1, 3.52422611487E+2, 5.22222290175E+2, 5.50597314008E+2, 1.99031687673E+0, 2.07756162026E+0, 3.64164427104E+0, 2.09240802507E+0, -2.6027078991E+2}
	, {1.70E+2, 1.E+1, 2.16447743743E+2, 7.03169914387E+2, 7.49370442178E+2, 2.46587990199E+0, 2.27808283008E+0, 3.88925751245E+0, 2.3108363806E+0, -3.89584764181E+2}
	, {2.20E+2, 1.0E+1, 1.48693469966E+2, 8.57705892215E+2, 9.24958341178E+2, 2.84191047445E+0, 2.41759546029E+0, 3.25668270305E+0, 2.51669915504E+0, -5.43782258262E+2}
	, {2.70E+2, 1.0E+1, 1.18624550765E+2, 9.99304970583E+2, 1.08360455221E+3, 3.14841791142E+0, 2.57310210304E+0, 3.1343255351E+0, 2.70853972026E+0, -7.10758218007E+2}
	, {3.20E+2, 1.0E+1, 1.00931937568E+2, 1.14185987054E+3, 1.24093653786E+3, 3.42549098293E+0, 2.73358079138E+0, 3.17277260493E+0, 2.8866576145E+0, -8.89970105983E+2}
};

const unsigned ntd = sizeof(td)/sizeof(TestData);

#endif
