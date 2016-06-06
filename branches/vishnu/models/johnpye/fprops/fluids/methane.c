/* This file is created by Hongke Zhu, 01-21-2010.
Chemical & Materials Engineering Department,
University of Alabama in Huntsville, United States.

Revised for new format by John Pye, 2012.

LITERATURE REFERENCE
Setzmann, U. and Wagner, W.,
"A New Equation of State and Tables of Thermodynamic Properties for Methane Covering the Range from the Melting Line to 625 K at Pressures up to 1000 MPa"
J. Phys. Chem. Ref. Data, 20(6):1061-1151, 1991.
*/

#include "../helmholtz.h"

#define METHANE_M 16.0428 /* kg/kmol */
#define METHANE_R 518.2705 /* J/kg/K */
#define METHANE_TC 190.564 /* K */



static const IdealData ideal_data_methane = {
	IDEAL_CP0
    ,{.cp0={
    	METHANE_R
		, 1. /* Tstar */
		, 1 /* power terms */
		, (const Cp0PowTerm[]){
			{4.0016, 0.0}
		}
		, 5 /* exponential terms */
		, (const Cp0ExpTerm[]){
		   /* a_i,       theta_i*METHANE_TC */
			{0.008449, 3.40043240*METHANE_TC}
			,{4.6942, 10.26951575*METHANE_TC}
			,{3.4865, 20.43932747*METHANE_TC}
			,{1.6572, 29.93744884*METHANE_TC}
			,{1.4115, 79.13351945*METHANE_TC}
		}
	}}
};


static HelmholtzData helmholtz_data_methane = {
    /* R */ METHANE_R /* J/kg/K */
    , /* M */ METHANE_M /* kg/kmol */
    , /* rho_star */ 162.66 /* kg/m3(= rho_c for this model) */
    , /* T_star */ METHANE_TC /* K (= T_c for this model) */

    , /* T_c */ METHANE_TC
    , /* rho_c */ 162.66 /* kg/m3 */
	, /* T_t */ 90.6941

	,{FPROPS_REF_NBP}

    , 0.01142 /* acentric factor */
    , &ideal_data_methane
    , 36 /* power terms */
    , (const HelmholtzPowTerm[]){
        /* n_i, 		t_i, 	d_i, 	l_i */
        {0.4367901028e-01,	-0.5,	1.0,	0.0}
        , {0.6709236199e+00,	0.5,	1.0,	0.0}
        , {-0.1765577859e+01,	1.0,	1.0,	0.0}
        , {0.8582330241e+00,	0.5,	2.0,	0.0}
        , {-0.1206513052e+01,	1.0,	2.0,	0.0}
        , {0.5120467220e+00,	1.5,	2.0,	0.0}
        , {-0.4000010791e-03,	4.5,	2.0,	0.0}
        , {-0.1247842423e-01,	0.0,	3.0,	0.0}
        , {0.3100269701e-01,	1.0,	4.0,	0.0}
        , {0.1754748522e-02,	3.0,	4.0,	0.0}
        , {-0.3171921605e-05,	1.0,	8.0,	0.0}
        , {-0.2240346840e-05,	3.0,	9.0,	0.0}
        , {0.2947056156e-06,	3.0,	10.0,	0.0}
        , {0.1830487909e+00,	0.0,	1.0,	1.0}
        , {0.1511883679e+00,	1.0,	1.0,	1.0}
        , {-0.4289363877e+00,	2.0,	1.0,	1.0}
        , {0.6894002446e-01,	0.0,	2.0,	1.0}
        , {-0.1408313996e-01,	0.0,	4.0,	1.0}
        , {-0.3063054830e-01,	2.0,	5.0,	1.0}
        , {-0.2969906708e-01,	2.0,	6.0,	1.0}
        , {-0.1932040831e-01,	5.0,	1.0,	2.0}
        , {-0.1105739959e+00,	5.0,	2.0,	2.0}
        , {0.9952548995e-01,	5.0,	3.0,	2.0}
        , {0.8548437825e-02,	2.0,	4.0,	2.0}
        , {-0.6150555662e-01,	4.0,	4.0,	2.0}
        , {-0.4291792423e-01,	12.0,	3.0,	3.0}
        , {-0.1813207290e-01,	8.0,	5.0,	3.0}
        , {0.3445904760e-01,	10.0,	5.0,	3.0}
        , {-0.2385919450e-02,	10.0,	8.0,	3.0}
        , {-0.1159094939e-01,	10.0,	2.0,	4.0}
        , {0.6641693602e-01,	14.0,	3.0,	4.0}
        , {-0.2371549590e-01,	12.0,	4.0,	4.0}
        , {-0.3961624905e-01,	18.0,	4.0,	4.0}
        , {-0.1387292044e-01,	22.0,	4.0,	4.0}
        , {0.3389489599e-01,	18.0,	5.0,	4.0}
        , {-0.2927378753e-02,	14.0,	6.0,	4.0}
    }
    , 4 /* gaussian terms */
    , (const HelmholtzGausTerm[]){
        /* n_i,			t_i,	d_i,	alpha_i,beta_i,	gamma_i,delta_i */
        {0.9324799946e-04,	2.0,	2.0,	20.0,	200.0,	1.07,	1.0}
        , {-0.6287171518e+01,	0.0,	0.0,	40.0,	250.0,	1.11,	1.0}
        , {0.1271069467e+02,	1.0,	0.0,	40.0,	250.0,	1.11,	1.0}
        , {-0.6423953466e+01,	2.0,	0.0,	40.0,	250.0,	1.11,	1.0}
    }
    , 0 /* critical terms */
    , 0
};

EosData eos_methane = {
	"methane"
	,"U Setzmann and W Wagner, 1991. 'A New Equation of State and Tables "
	"of Thermodynamic Properties for Methane Covering the Range from the "
	"Melting Line to 625 K at Pressures up to 1000 MPa', J. Phys. Chem. "
	"Ref. Data, 20(6):1061-1151."
	,"http://link.aip.org/link/doi/10.1063/1.555898"
	,100
	,FPROPS_HELMHOLTZ
	,.data = {.helm = &helmholtz_data_methane}
};


/*
    Test suite. These tests attempt to validate the current code using a few sample figures output by REFPROP 8.0. To compile and run the test:

    ./test.py methane
*/

#ifdef TEST

#include "../test.h"
#include <math.h>
#include <assert.h>
#include <stdio.h>

const TestData td[]; const unsigned ntd;

int main(void){
	test_init();
	PureFluid *P = helmholtz_prepare(&eos_methane, NULL);
	return helm_run_test_cases(P, ntd, td, 'C');
}

/*
A small set of data points calculated using REFPROP 8.0, for validation.
*/

const TestData td[] = {
    /* Temperature, Pressure, Density, Int. Energy, Enthalpy, Entropy, Cv, Cp, Cp0, Helmholtz */
    /* (C), (MPa), (kg/m3), (kJ/kg), (kJ/kg), (kJ/kg-K), (kJ/kg-K), (kJ/kg-K), (kJ/kg-K), (kJ/kg) */
	{-1.80E+2, 1.E-1, 4.4825326337E+2, -6.36195233809E+1, -6.33964352114E+1, -6.2043265267E-1, 2.15238924363E+0, 3.37617580547E+0, 2.0741142516E+0, -5.82622178463E+0}
	, {-1.61642374015E+2, 1.E-1, 4.22588489333E+2, -7.93975509839E-1, -5.57338681047E-1, -4.96653183407E-3, 2.05715451449E+0, 3.47992535377E+0, 2.0743744872E+0, -2.40169335646E-1}
	, {-1.61642374015E+2, 1.E-1, 1.79460704721E+0, 4.54839442357E+2, 5.10561947268E+2, 4.57874943769E+0, 1.61468333116E+0, 2.21645660671E+0, 2.0743744872E+0, -5.57260374172E+1}
	, {-1.30E+2, 1.E-1, 1.37093848711E+0, 5.05896365108E+2, 5.7883909809E+2, 5.11821275416E+0, 1.57238085649E+0, 2.12464886839E+0, 2.07542848813E+0, -2.2677579065E+2}
	, {-8.0E+1, 1.E-1, 1.00600723494E+0, 5.84898694146E+2, 6.8430155779E+2, 5.75023733765E+0, 1.57172801717E+0, 2.1043573582E+0, 2.08569716289E+0, -5.25759647622E+2}
	, {-3.0E+1, 1.E-1, 7.9629465684E-1, 6.64535152012E+2, 7.90116805915E+2, 6.23729449702E+0, 1.61049991556E+0, 2.13667695845E+0, 2.12685899846E+0, -8.52063004938E+2}
	, {2.0E+1, 1.E-1, 6.59406381133E-1, 7.47183828535E+2, 8.98835378871E+2, 6.64367115593E+0, 1.6972746309E+0, 2.22051761876E+0, 2.21449150071E+0, -1.20040837083E+3}
	, {7.0E+1, 1.E-1, 5.62848706794E-1, 8.35261450972E+2, 1.01292908846E+3, 7.00276109798E+0, 1.8280543766E+0, 2.34972131608E+0, 2.34566937707E+0, -1.5677360198E+3}
	, {1.20E+2, 1.E-1, 4.91030648054E-1, 9.30677241818E+2, 1.13433051763E+3, 7.3327823962E+0, 1.98961235307E+0, 2.51033926146E+0, 2.50744283497E+0, -1.95220615725E+3}
	, {1.70E+2, 1.E-1, 4.35501979492E-1, 1.0346362273E+3, 1.26425630874E+3, 7.643656056E+0, 2.16856860632E+0, 2.68869232974E+0, 2.68652814E+0, -2.35264995392E+3}
	, {2.20E+2, 1.E-1, 3.91275385257E-1, 1.14775274957E+3, 1.40332722159E+3, 7.94082646493E+0, 2.35510493554E+0, 2.87482000175E+0, 2.87314750095E+0, -2.76826582161E+3}
	, {2.70E+2, 1.E-1, 3.55214204954E-1, 1.27024171431E+3, 1.55176198745E+3, 8.2273738662E+0, 2.54302801954E+0, 3.06245429243E+0, 3.06112694336E+0, -3.19845640111E+3}
	, {3.20E+2, 1.E-1, 3.25245664786E-1, 1.40207588153E+3, 1.70953578285E+3, 8.50513086018E+0, 2.7287123589E+0, 3.24792753319E+0, 3.24685115895E+0, -3.64274248818E+3}
	, {-1.80E+2, 1.E+0, 4.48863497278E+2, -6.41747958411E+1, -6.19469470495E+1, -6.26411583801E-1, 2.15471136096E+0, 3.37078661023E+0, 2.0741142516E+0, -5.82455681004E+0}
	, {-1.30E+2, 1.E+0, 3.71605882553E+2, 1.12875890373E+2, 1.15566913435E+2, 8.92140844283E-1, 1.93633820764E+0, 3.86390825883E+0, 2.07542848813E+0, -1.48340714858E+1}
	, {-1.24011222553E+2, 1.E+0, 3.59619776007E+2, 1.36375535613E+2, 1.3915625032E+2, 1.05355245289E+0, 1.92049223861E+0, 4.02270770852E+0, 2.07585010455E+0, -2.07499891861E+1}
	, {-1.24011222553E+2, 1.E+0, 1.56981334502E+1, 4.91110379319E+2, 5.54812220253E+2, 3.84059400605E+0, 1.7673141217E+0, 2.87649562927E+0, 2.07585010455E+0, -8.16711154131E+1}
	, {-8.0E+1, 1.E+0, 1.07811632239E+1, 5.716608791E+2, 6.64415248849E+2, 4.4873467733E+0, 1.61672673227E+0, 2.31326726145E+0, 2.08569716289E+0, -2.95070150162E+2}
	, {-3.0E+1, 1.E+0, 8.22282231704E+0, 6.55690130277E+2, 7.77302876053E+2, 5.00744997698E+0, 1.6284626968E+0, 2.23328313337E+0, 2.12685899846E+0, -5.61871331626E+2}
	, {2.0E+1, 1.E+0, 6.70463584989E+0, 7.40563054771E+2, 8.89713586193E+2, 5.42775112424E+0, 1.70683588195E+0, 2.27706318767E+0, 2.21449150071E+0, -8.505821873E+2}
	, {7.0E+1, 1.E+0, 5.67850592238E+0, 8.29997986967E+2, 1.00610064736E+3, 5.79410407893E+0, 1.83393062943E+0, 2.38692927152E+0, 2.34566937707E+0, -1.15824882772E+3}
	, {1.20E+2, 1.E+0, 4.93247544635E+0, 9.26328538542E+2, 1.12906649657E+3, 6.12839808801E+0, 1.99354072225E+0, 2.53664029782E+0, 2.50744283497E+0, -1.48305116976E+3}
	, {1.70E+2, 1.E+0, 4.36335198679E+0, 1.03094410406E+3, 1.26012570642E+3, 6.44199360627E+0, 2.17134235866E+0, 2.70822091169E+0, 2.68652814E+0, -1.82382536256E+3}
	, {2.20E+2, 1.E+0, 3.91388772315E+0, 1.14455314474E+3, 1.4000535757E+3, 6.7410005455E+0, 2.35713902925E+0, 2.88985541092E+0, 2.87314750095E+0, -2.17977127427E+3}
	, {2.70E+2, 1.E+0, 3.54945344311E+0, 1.26742390645E+3, 1.5491574229E+3, 7.02884266806E+0, 2.54456080875E+0, 3.07435928927E+0, 3.06112694336E+0, -2.55029198871E+3}
	, {3.20E+2, 1.E+0, 3.2477523725E+0, 1.39956170112E+3, 1.70746694921E+3, 7.30754467246E+0, 2.72989047479E+0, 3.25756740323E+0, 3.24685115895E+0, -2.93490842135E+3}
	, {-1.30E+2, 1.E+1, 3.86420855687E+2, 9.81455525619E+1, 1.2402407297E+2, 7.8550105903E-1, 1.9584968293E+0, 3.57858329536E+0, 2.07542848813E+0, -1.42989240382E+1}
	, {-8.0E+1, 1.E+1, 2.86878169685E+2, 2.89487188334E+2, 3.24345190987E+2, 1.97734728191E+0, 1.87014450706E+0, 4.81344867253E+0, 2.08569716289E+0, -9.2437439166E+1}
	, {-3.0E+1, 1.0E+1, 1.2271606274E+2, 5.33014463415E+2, 6.14503388146E+2, 3.31341251292E+0, 1.83729568172E+0, 4.60366458085E+0, 2.12685899846E+0, -2.72641789103E+2}
	, {2.0E+1, 1.0E+1, 7.83223768271E+1, 6.66644522047E+2, 7.94321954795E+2, 3.9901579744E+0, 1.80029673037E+0, 3.06735153301E+0, 2.21449150071E+0, -5.03070288149E+2}
	, {7.0E+1, 1.0E+1, 6.09359941705E+1, 7.75395596368E+2, 9.39502215717E+2, 4.44794031366E+0, 1.88779426104E+0, 2.81141235314E+0, 2.34566937707E+0, -7.50915122265E+2}
	, {1.20E+2, 1.E+1, 5.08222612615E+1, 8.82700252754E+2, 1.07946442168E+3, 4.82870324547E+0, 2.0290534698E+0, 2.81001787942E+0, 2.50744283497E+0, -1.0157044282E+3}
	, {1.70E+2, 1.0E+1, 4.39637040398E+1, 9.94548014199E+2, 1.22200837539E+3, 5.16989226594E+0, 2.19653715929E+0, 2.90156068571E+0, 2.68652814E+0, -1.29648974345E+3}
	, {2.20E+2, 1.0E+1, 3.89146582287E+1, 1.11332723041E+3, 1.3702998072E+3, 5.48682739626E+0, 2.37583328318E+0, 3.03468797111E+0, 2.87314750095E+0, -1.59250170006E+3}
	, {2.70E+2, 1.0E+1, 3.50012242646E+1, 1.24009044417E+3, 1.52579473623E+3, 5.78703832943E+0, 2.55884868033E+0, 3.18720585469E+0, 3.06112694336E+0, -1.90313942446E+3}
	, {3.20E+2, 1.0E+1, 3.1858257067E+1, 1.37526640365E+3, 1.68915677056E+3, 6.07465341075E+0, 2.74103603514E+0, 3.34806451817E+0, 3.24685115895E+0, -2.22791426694E+3}
	, {-1.30E+2, 1.00E+2, 4.52834004597E+2, 3.98774348532E+1, 2.6070890728E+2, 2.59781346011E-1, 2.13753364395E+0, 3.10104335143E+0, 2.07542848813E+0, 2.68973517165E+0}
	, {-8.0E+1, 1.00E+2, 4.14117404618E+2, 1.72444833562E+2, 4.1392224766E+2, 1.17828344144E+0, 2.01021521018E+0, 3.03122646893E+0, 2.08569716289E+0, -5.51406131524E+1}
	, {-3.0E+1, 1.E+2, 3.78198251327E+2, 3.00004019945E+2, 5.64415607383E+2, 1.87134841569E+0, 1.95594186673E+0, 2.99566463684E+0, 2.12685899846E+0, -1.55014347332E+2}
	, {2.0E+1, 1.00E+2, 3.45392964741E+2, 4.24718087654E+2, 7.14243382633E+2, 2.43169563858E+0, 1.97574833881E+0, 3.00488794053E+0, 2.21449150071E+0, -2.88133488796E+2}
	, {7.0E+1, 1.00E+2, 3.16013757124E+2, 5.49145876723E+2, 8.65587796562E+2, 2.90827646273E+0, 2.05655489494E+0, 3.05494292585E+0, 2.34566937707E+0, -4.48829191462E+2}
	, {1.20E+2, 1.00E+2, 2.90136537574E+2, 6.75592319989E+2, 1.02025763114E+3, 3.32892591298E+0, 2.17954292505E+0, 3.13622560759E+0, 2.50744283497E+0, -6.33174902698E+2}
	, {1.70E+2, 1.E+2, 2.6758046871E+2, 8.05855918048E+2, 1.17957527239E+3, 3.71026346681E+0, 2.32799118966E+0, 3.23948188296E+0, 2.68652814E+0, -8.38347337268E+2}
	, {2.20E+2, 1.00E+2, 2.48000592845E+2, 9.41227733547E+2, 1.34445257609E+3, 4.06267535202E+0, 2.48997378587E+0, 3.35761019648E+0, 2.87314750095E+0, -1.0622806163E+3}
	, {2.70E+2, 1.00E+2, 2.30991948762E+2, 1.08258238022E+3, 1.51549790189E+3, 4.39294021459E+0, 2.65786980977E+0, 3.48548077927E+0, 3.06112694336E+0, -1.30344309733E+3}
	, {3.20E+2, 1.00E+2, 2.16159562384E+2, 1.2304790058E+3, 1.69310022365E+3, 4.7056532676E+0, 2.82703230566E+0, 3.61935590506E+0, 3.24685115895E+0, -1.56067922988E+3}
};

const unsigned ntd = sizeof(td)/sizeof(TestData);

#endif
