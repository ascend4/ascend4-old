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
*/

#include "../helmholtz.h"

#define HYDROGEN_M 2.01594
#define HYDROGEN_R (8314.472/HYDROGEN_M)
#define HYDROGEN_TSTAR 33.145

/*
	Ideal gas specific heat for 'normal hydrogen' from the masters thesis of 
	Jacob Leachman, currently awaiting delivery to U of Idaho library. Values 
	here come from a proof copy received from Steve Penoncello by email 20 Sep 
	2008.

	It is understood that this is the same correlation as implemented by REFPROP
	version 8.0.
*/
const IdealData ideal_data_hydrogen = {
	-6.0132647014e+03 / HYDROGEN_R /* const */
	, 2.5810400764e+05/HYDROGEN_R/HYDROGEN_TSTAR /* linear */
	, HYDROGEN_TSTAR /* Tstar */
	, HYDROGEN_R /* cp0star */
	, 1 /* power terms */
	, (const IdealPowTerm[]){
		{2.5,  0.}
	}
	, 5 /* exponential terms */
	, (const IdealExpTerm[]){
		{   1.616, 531.}
		,{ -0.4117,751.}
		,{ -0.792, 1989.}
		,{  0.758, 2484.}
		,{  1.217, 6859.}
	}
};

/*
	Properties of 'normal hydrogen' from the masters thesis of Jacob Leachman,
	currently awaiting delivery to U of Idaho library. Values here come from a
	proof copy received from Steve Penoncello by email 20 Sep 2008.

	It is understood that this is the same correlation as implemented by REFPROP
	version 8.0.
*/
const HelmholtzData helmholtz_data_hydrogen = {
	"hydrogen"
	, /* R */ HYDROGEN_R /* 1000 * kJ/kmolK / kg/kmol = J/kgK */
	, /* M */ HYDROGEN_M /* kg/kmol */
	, /* rho_star */ 15.508 * HYDROGEN_M /* kmol/m3 * kg/kmol = kg/m³ (note, mol/L=kmol/m³) */
	, /* T_star */ HYDROGEN_TSTAR /* K (= T_c for this model) */

	, /* T_c */ HYDROGEN_TSTAR
	, /* rho_c */ 15.508 * HYDROGEN_M
	, /* T_t */ 13.957

	, 0.088 /* acentric factor, from Reid, Prausnitz & Polling */
	, &ideal_data_hydrogen
	, 9 /* power terms */
	, (const HelmholtzPowTerm[]){
		/* a_i, t_i, d_i, l_i */
		{-0.693643e1,  0.6844,   1.0,  0.0}
		, {0.01,         1.,    4.0,  0.0}
		, {2.1101,      0.989,    1.0,  0.0}
			, {4.52059,  0.489,    1.0,  0.0}
		, {0.732564,  0.803,    2.0,  0.0}
		, {-1.34086,  1.1444,   2.0,  0.0}
		, {0.130985,  1.409,    3.0,  0.0}
		, {-0.777414,  1.754,    1.0,  1.0}
		, {0.351944,  1.311,    3.0 , 1.0}
	}
	, 5 /* critical (gaussian) terms */
 	, (const HelmholtzGausTerm[]){
		/* a, t, d, alpha, beta, gamma, epsilon */
		{-0.211716e-1, 4.187,   2.0, 1.685, 0.1710, 0.7164, 1.506}
		, {0.226312e-1, 5.646,  1.0, 0.489, 0.2245, 1.3444, 0.156}
		, {0.321870e-1, 0.791,  3.0, 0.103, 0.1304, 1.4517, 1.736}
		, {-0.231752e-1,  7.249,1.0, 2.506, 0.2785, 0.7204, 0.670}
		, {0.557346e-1,  2.986, 1.0, 1.607, 0.3967, 1.5445, 1.662}
	}
	, 0 /* nc2 */, 0
};

/*
	Test suite. These tests attempt to validate the current code using 
	a few sample figures output by REFPROP 8.0. To compile and run the test:

	./test.py hydrogen
*/
#ifdef TEST

#include "../test.h"
#include <math.h>
#include <assert.h>
#include <stdio.h>

const TestData td[]; const unsigned ntd;

int main(void){
	//return helm_check_u(&helmholtz_data_hydrogen,ntd,td);
	//return helm_check_dpdT_rho(&helmholtz_data_hydrogen, ntd, td);
	//return helm_check_dpdrho_T(&helmholtz_data_hydrogen, ntd, td);
	//return helm_check_dhdT_rho(&helmholtz_data_hydrogen, ntd, td);
	//return helm_check_dhdrho_T(&helmholtz_data_hydrogen, ntd, td);
	//return helm_check_dudT_rho(&helmholtz_data_hydrogen, ntd, td);
	//return helm_check_dudrho_T(&helmholtz_data_hydrogen, ntd, td);

	return helm_run_test_cases(&helmholtz_data_hydrogen, ntd, td, 'C');
}

/*
	A small set of data points calculated using REFPROP 8.0, for validation
*/
const TestData td[] = {
	/* Temperature, Pressure, Density, Int. Energy, Enthalpy, Entropy, Cv, Cp, Cp0, Helmholtz */
	/* (C), (MPa), (kg/m³), (kJ/kg), (kJ/kg), (kJ/kg-K), (kJ/kg-K), (kJ/kg-K), (kJ/kg-K), (kJ/kg) */
	{-2.50E+2, 1.E-1, 1.12119687314E+0, 3.92130160933E+2, 4.81320563079E+2, 2.35808248576E+1, 6.29359688434E+0, 1.13471316035E+1, 1.03109124142E+1, -1.53765934519E+2}
	, {-2.00E+2, 1.E-1, 3.32141763015E-1, 7.11029863688E+2, 1.0121061243E+3, 3.58592769541E+1, 6.43467663919E+0, 1.06158457937E+1, 1.05522028485E+1, -1.9120762455E+3}
	, {-1.50E+2, 1.E-1, 1.96817196167E-1, 1.06413088038E+3, 1.57221656571E+3, 4.16649978447E+1, 7.75342108402E+0, 1.18952575073E+1, 1.18753688873E+1, -4.06691360419E+3}
	, {-1.00E+2, 1.E-1, 1.39938543997E-1, 1.48454981051E+3, 2.19914921353E+3, 4.59258960798E+1, 8.96686502588E+0, 1.30991457347E+1, 1.30897332175E+1, -6.46751909571E+3}
	, {-5.0E+1, 1.E-1, 1.08581934666E-1, 1.9528909533E+3, 2.87385446631E+3, 4.93453061419E+1, 9.68940945915E+0, 1.38180310023E+1, 1.38126553227E+1, -9.05851411228E+3}
	, {0.E+0, 1.E-1, 8.87103722423E-2, 2.44804786923E+3, 3.57531176715E+3, 5.21803872531E+1, 1.00702380168E+1, 1.41971116314E+1, 1.4193698084E+1, -1.1805024909E+4}
	, {5.0E+1, 1.E-1, 7.49888355328E-2, 2.95677858029E+3, 4.29031042259E+3, 5.45837218543E+1, 1.02532679106E+1, 1.43791894172E+1, 1.43768706065E+1, -1.46819511369E+4}
	, {1.00E+2, 1.E-1, 6.49442856068E-2, 3.47175361489E+3, 5.01153496848E+3, 5.66587496407E+1, 1.03329030673E+1, 1.44582589508E+1, 1.44566094102E+1, -1.76704588135E+4}
	, {1.50E+2, 1.E-1, 5.7273126285E-2, 3.98934780891E+3, 5.73536739063E+3, 5.84790920075E+1, 1.0365343467E+1, 1.44903421201E+1, 1.44891292445E+1, -2.0756079974E+4}
	, {2.00E+2, 1.E-1, 5.12229341173E-2, 4.50804821919E+3, 6.46029874374E+3, 6.00983661133E+1, 1.03809307362E+1, 1.4505693379E+1, 1.45047796021E+1, -2.39274937073E+4}
	, {2.50E+2, 1.E-1, 4.63289716715E-2, 5.02743074204E+3, 7.18590731495E+3, 6.15561863119E+1, 1.03944126869E+1, 1.4519014005E+1, 1.45183129674E+1, -2.7175688127E+4}
	, {3.00E+2, 1.E-1, 4.22886761408E-2, 5.54757539468E+3, 7.91227462686E+3, 6.28822191039E+1, 1.04121920383E+1, 1.45366801652E+1, 1.45361350542E+1, -3.04933684847E+4}
	, {3.50E+2, 1.E-1, 3.88965894888E-2, 6.06876957213E+3, 8.63968906184E+3, 6.40990208002E+1, 1.04367133537E+1, 1.45611204512E+1, 1.45606924538E+1, -3.38745352395E+4}
	, {4.00E+2, 1.E-1, 3.60082932387E-2, 6.5913750597E+3, 9.36851307447E+3, 6.52240311847E+1, 1.04687799749E+1, 1.45931281763E+1, 1.45927899394E+1, -3.73141815323E+4}
	, {4.50E+2, 1.E-1, 3.35193065641E-2, 7.11577643923E+3, 1.00991317127E+4, 6.62709684997E+1, 1.0508616187E+1, 1.46329211013E+1, 1.463265284E+1, -4.08080744313E+4}
	, {5.00E+2, 1.E-1, 3.13521728343E-2, 7.64236406465E+3, 1.08319356624E+4, 6.72508035874E+1, 1.05562675962E+1, 1.46805404595E+1, 1.4680327543E+1, -4.4352594729E+4}
	, {-2.50E+2, 1.E+0, 6.87471567321E+1, 2.21627578819E+1, 3.67088140031E+1, 1.0959022357E+0, 5.84824114876E+0, 1.09523053994E+1, 1.03109124142E+1, -3.20737887451E+0}
	, {-2.41757489134E+2, 1.E+0, 4.98895610683E+1, 1.56307956894E+2, 1.76352230257E+2, 6.11770562126E+0, 6.60973051048E+0, 3.9719489458E+1, 1.0310997953E+1, -3.57421832965E+1}
	, {-2.41757489134E+2, 1.E+0, 1.41283875399E+1, 3.47702632428E+2, 4.18482117856E+2, 1.38306881422E+1, 7.86635245518E+0, 5.03389973071E+1, 1.0310997953E+1, -8.64773953615E+1}
	, {-2.00E+2, 1.E+0, 3.38169240321E+0, 6.92162261452E+2, 9.87872184404E+2, 2.61064733931E+1, 6.49321956878E+0, 1.12126408189E+1, 1.05522028485E+1, -1.21752626726E+3}
	, {-1.50E+2, 1.E+0, 1.96173093828E+0, 1.05448999655E+3, 1.56424389831E+3, 3.20906374991E+1, 7.77491463031E+0, 1.20729348292E+1, 1.18753688873E+1, -2.89747201147E+3}
	, {-1.00E+2, 1.E+0, 1.39109580576E+0, 1.47842016287E+3, 2.19727791217E+3, 3.63940124784E+1, 8.98030262021E+0, 1.31827311759E+1, 1.30897332175E+1, -4.82320309776E+3}
	, {-5.0E+1, 1.E+0, 1.07932860316E+0, 1.94864454176E+3, 2.87514644032E+3, 3.98296627541E+1, 9.69947421063E+0, 1.38657597977E+1, 1.38126553227E+1, -6.93934470181E+3}
	, {0.E+0, 1.E+0, 8.82203860223E-1, 2.4449846721E+3, 3.57850952399E+3, 4.26725088948E+1, 1.00783770948E+1, 1.42274496735E+1, 1.4193698084E+1, -9.21101113253E+3}
	, {5.0E+1, 1.E+0, 7.46140060966E-1, 2.95453023404E+3, 4.29476118037E+3, 4.50800787422E+1, 1.02601227013E+1, 1.43998225502E+1, 1.43768706065E+1, -1.16130972115E+4}
	, {1.00E+2, 1.E+0, 6.46514142474E-1, 3.47010116416E+3, 5.01685773807E+3, 4.71576257639E+1, 1.03388245399E+1, 1.4472953494E+1, 1.44566094102E+1, -1.41267668896E+4}
	, {1.50E+2, 1.E+0, 5.70394855669E-1, 3.98815000324E+3, 5.74132149499E+3, 4.8979560991E+1, 1.03705499091E+1, 1.45011577504E+1, 1.44891292445E+1, -1.67375512301E+4}
	, {2.00E+2, 1.E+0, 5.10330417673E-1, 4.50720840705E+3, 6.46672319466E+3, 5.05998884773E+1, 1.03855695176E+1, 1.45138491722E+1, 1.45047796021E+1, -1.9434128826E+4}
	, {2.50E+2, 1.E+0, 4.61721043707E-1, 5.02687978165E+3, 7.19268966541E+3, 5.20584293278E+1, 1.03985890061E+1, 1.45252759813E+1, 1.45183129674E+1, -2.22074875212E+4}
	, {3.00E+2, 1.E+0, 4.21572442743E-1, 5.5472620731E+3, 7.91933362856E+3, 5.33849681337E+1, 1.04159840654E+1, 1.45415527565E+1, 1.45361350542E+1, -2.50503324128E+4}
	, {3.50E+2, 1.E+0, 3.87851030443E-1, 6.06865488025E+3, 8.6469643896E+3, 5.46021323077E+1, 1.04401809632E+1, 1.45649486006E+1, 1.45606924538E+1, -2.79566638673E+4}
	, {4.00E+2, 1.E+0, 3.59126990851E-1, 6.59142859323E+3, 9.37595892783E+3, 5.57274063161E+1, 1.0471970134E+1, 1.45961551977E+1, 1.45927899394E+1, -3.09214749684E+4}
	, {4.50E+2, 1.E+0, 3.34365561465E-1, 7.11597404035E+3, 1.01067126667E+4, 5.67745374934E+1, 1.0511566458E+1, 1.46353231361E+1, 1.463265284E+1, -3.3940532748E+4}
	, {5.00E+2, 1.E+0, 3.12799342931E-1, 7.64268622261E+3, 1.08396238844E+4, 5.77545161962E+1, 1.05590085072E+1, 1.46824478634E+1, 1.4680327543E+1, -3.70102179745E+4}
	, {-2.50E+2, 1.E+1, 7.78200304125E+1, -5.00100718869E+0, 1.23500613114E+2, -4.23614127753E-1, 5.87865849715E+0, 8.51491486287E+0, 1.03109124142E+1, 4.80565986879E+0}
	, {-2.00E+2, 1.0E+1, 3.3379503856E+1, 5.21304642113E+2, 8.20889682177E+2, 1.47315001764E+1, 6.86090577417E+0, 1.50717219707E+1, 1.05522028485E+1, -5.56304595789E+2}
	, {-1.50E+2, 1.0E+1, 1.83549808977E+1, 9.6769718555E+2, 1.51250843083E+3, 2.19729616577E+1, 7.97109894831E+0, 1.34093781542E+1, 1.18753688873E+1, -1.73827304259E+3}
	, {-1.00E+2, 1.0E+1, 1.29919136818E+1, 1.42285898542E+3, 2.19256853283E+3, 2.66032447028E+1, 9.11156602413E+0, 1.38453278239E+1, 1.30897332175E+1, -3.18349283486E+3}
	, {-5.0E+1, 1.E+1, 1.01411130703E+1, 1.90990191255E+3, 2.89598696364E+3, 3.01699603401E+1, 9.79911823843E+0, 1.4259188347E+1, 1.38126553227E+1, -4.82252473734E+3}
	, {0.E+0, 1.E+1, 8.3446984819E+0, 2.41691964503E+3, 3.61528529259E+3, 3.30777020101E+1, 1.01590927403E+1, 1.44849738031E+1, 1.4193698084E+1, -6.61825465903E+3}
	, {5.0E+1, 1.0E+1, 7.10028432831E+0, 2.93389679075E+3, 4.34229109408E+3, 3.55216103184E+1, 1.03280534049E+1, 1.45789594953E+1, 1.43768706065E+1, -8.54491158365E+3}
	, {1.00E+2, 1.0E+1, 6.1840215355E+0, 3.4549540974E+3, 5.07202479202E+3, 3.76212162978E+1, 1.03974507269E+1, 1.46028168458E+1, 1.44566094102E+1, -1.05834027641E+4}
	, {1.50E+2, 1.0E+1, 5.47980019113E+0, 3.9772233595E+3, 5.8021074157E+3, 3.94573258297E+1, 1.04220606342E+1, 1.45981187871E+1, 1.44891292445E+1, -1.27191440653E+4}
	, {2.00E+2, 1.0E+1, 4.92096526303E+0, 4.49962860085E+3, 6.53175024071E+3, 4.10871506324E+1, 1.04314448296E+1, 1.4587830864E+1, 1.45047796021E+1, -1.49407567209E+4}
	, {2.50E+2, 1.0E+1, 4.46635527952E+0, 5.0220169872E+3, 7.26097904332E+3, 4.25522598407E+1, 1.0439883135E+1, 1.45826416418E+1, 1.45183129674E+1, -1.72391977485E+4}
	, {3.00E+2, 1.0E+1, 4.08910751615E+0, 5.54464647448E+3, 7.99016788973E+3, 4.38834531141E+1, 1.04534778656E+1, 1.45865666652E+1, 1.45361350542E+1, -1.96071546778E+4}
	, {3.50E+2, 1.0E+1, 3.77091264557E+0, 6.06792633149E+3, 8.71980425599E+3, 4.51039777264E+1, 1.04744706114E+1, 1.46005713587E+1, 1.45606924538E+1, -2.20386173887E+4}
	, {4.00E+2, 1.0E+1, 3.49884526801E+0, 6.59230477835E+3, 9.45039058495E+3, 4.62317124612E+1, 1.05035219669E+1, 1.46245029969E+1, 1.45927899394E+1, -2.45285724649E+4}
	, {4.50E+2, 1.0E+1, 3.26351205288E+0, 7.11822948459E+3, 1.01824130506E+4, 4.7280664133E+1, 1.05407525537E+1, 1.46579459186E+1, 1.463265284E+1, -2.70727827832E+4}
	, {5.00E+2, 1.0E+1, 3.05791778912E+0, 7.64613797458E+3, 1.0916337074E+4, 4.82619987866E+1, 1.05861306952E+1, 1.47005046006E+1, 1.4680327543E+1, -2.96676263873E+4}
	, {-2.00E+2, 1.E+2, 9.07763435355E+1, 3.41853678886E+2, 1.44346227101E+3, 5.47500667028E+0, 7.9473077242E+0, 1.11417850173E+1, 1.05522028485E+1, -5.86430590448E+1}
	, {-1.50E+2, 1.00E+2, 7.72473936996E+1, 7.63561461938E+2, 2.05810351974E+3, 1.18306705061E+1, 9.14548015013E+0, 1.3249049133E+1, 1.18753688873E+1, -6.93385610894E+2}
	, {-1.00E+2, 1.00E+2, 6.66603467556E+1, 1.25212339229E+3, 2.75226560377E+3, 1.65504896319E+1, 1.00846361177E+1, 1.43815892259E+1, 1.30897332175E+1, -1.61359388747E+3}
	, {-5.0E+1, 1.00E+2, 5.85031121419E+1, 1.77653662121E+3, 3.48584739697E+3, 2.02697790905E+1, 1.05994944893E+1, 1.48863934193E+1, 1.38126553227E+1, -2.74666458283E+3}
	, {0.E+0, 1.00E+2, 5.21154458221E+1, 2.31666841694E+3, 4.23548535158E+3, 2.33003770469E+1, 1.08312851262E+1, 1.50613782288E+1, 1.4193698084E+1, -4.04782957343E+3}
	, {5.0E+1, 1.00E+2, 4.7001286219E+1, 2.86174574872E+3, 4.98934709847E+3, 2.5834761132E+1, 1.09048298678E+1, 1.5075667395E+1, 1.43768706065E+1, -5.4867573111E+3}
	, {1.00E+2, 1.00E+2, 4.2819177161E+1, 3.40653252169E+3, 5.74193470899E+3, 2.80002588678E+1, 1.09013588877E+1, 1.50215372415E+1, 1.44566094102E+1, -7.04176407483E+3}
	, {1.50E+2, 1.00E+2, 3.93353493231E+1, 3.94895435328E+3, 6.4911969346E+3, 2.98846939542E+1, 1.08687826149E+1, 1.49483470606E+1, 1.44891292445E+1, -8.69675389343E+3}
	, {2.00E+2, 1.00E+2, 3.63869503303E+1, 4.48862020072E+3, 7.23685821166E+3, 3.15503597409E+1, 1.08321805959E+1, 1.48800250011E+1, 1.45047796021E+1, -1.04394325107E+4}
	, {2.50E+2, 1.00E+2, 3.38578717763E+1, 5.0259225085E+3, 7.97944541923E+3, 3.30423483908E+1, 1.08028610477E+1, 1.4826237422E+1, 1.45183129674E+1, -1.22601820521E+4}
	, {3.00E+2, 1.00E+2, 3.16633684162E+1, 5.56154476642E+3, 8.71976844893E+3, 3.43938962532E+1, 1.07849040874E+1, 1.47895480762E+1, 1.45361350542E+1, -1.41513168711E+4}
	, {3.50E+2, 1.00E+2, 2.97402169359E+1, 6.09622835539E+3, 9.45867860971E+3, 3.56299538582E+1, 1.07791366461E+1, 1.47695356673E+1, 1.45606924538E+1, -1.61065773913E+4}
	, {4.00E+2, 1.00E+2, 2.80403054252E+1, 6.63068266252E+3, 1.01969776256E+4, 3.67696082369E+1, 1.07852033844E+1, 1.47648604338E+1, 1.45927899394E+1, -1.81207791222E+4}
	, {4.50E+2, 1.00E+2, 2.65263661206E+1, 7.16556285136E+3, 1.09353969683E+4, 3.78277367773E+1, 1.08024776861E+1, 1.4774131959E+1, 1.463265284E+1, -2.01895649991E+4}
	, {5.00E+2, 1.00E+2, 2.51690712101E+1, 7.70147310913E+3, 1.16746034311E+4, 3.88161423725E+1, 1.08303663675E+1, 1.4796167121E+1, 1.4680327543E+1, -2.23092273662E+4}
};

const unsigned ntd = sizeof(td)/sizeof(TestData);

#endif

