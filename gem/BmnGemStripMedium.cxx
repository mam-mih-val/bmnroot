#include "BmnGemStripMedium.h"

BmnGemStripMedium::BmnGemStripMedium() {
    SetCurrentConfiguration(BmnGemStripMediumConfiguration::MEDIUM::DEFAULT);
}

BmnGemStripMedium::~BmnGemStripMedium() {

}

Bool_t BmnGemStripMedium::SetCurrentConfiguration(BmnGemStripMediumConfiguration::MEDIUM medium) {
    Bool_t status = true;
    switch(medium) {
        case BmnGemStripMediumConfiguration::MEDIUM::ARCO2_70_30_E_1000_2500_3750_6300_B_0_0T:
            NameOfCurrentMedium = "ARCO2_70_30_E_1000_2500_3750_6300_B_0_0T";
            MCD = 0.0333; //pi+, p = 1 GeV/c
            p0_xmean = +0.0;
            p1_xmean = +0.0;
            p2_xmean = +0.0;
            p3_xmean = +0.0;

            p0_sigma = +1.19E-04;
            p1_sigma = +0.110434;
            p2_sigma = -0.424199;
            p3_sigma = +0.976967;
            p4_sigma = -1.06588;
            p5_sigma = +0.428521;
        break;

        case BmnGemStripMediumConfiguration::MEDIUM::ARCO2_70_30_E_1000_2500_3750_6300_B_0_1T:
            NameOfCurrentMedium = "ARCO2_70_30_E_1000_2500_3750_6300_B_0_1T";
            MCD = 0.0333; //pi+, p = 1 GeV/c
            p0_xmean = +1.34E-04;
            p1_xmean = +0.0124203;
            p2_xmean = +0.031108;
            p3_xmean = -0.0152303;

            p0_sigma = +9.39E-05;
            p1_sigma = +0.108189;
            p2_sigma = -0.392529;
            p3_sigma = +0.858685;
            p4_sigma = -0.907579;
            p5_sigma = +0.357084;
        break;

        case BmnGemStripMediumConfiguration::MEDIUM::ARCO2_70_30_E_1000_2500_3750_6300_B_0_2T:
            NameOfCurrentMedium = "ARCO2_70_30_E_1000_2500_3750_6300_B_0_2T";
            MCD = 0.0333; //pi+, p = 1 GeV/c
            p0_xmean = +2.60E-05;
            p1_xmean = +0.0289914;
            p2_xmean = +0.051339;
            p3_xmean = -0.0235155;

            p0_sigma = +0.000112255;
            p1_sigma = +0.106632;
            p2_sigma = -0.382417;
            p3_sigma = +0.839771;
            p4_sigma = -0.896514;
            p5_sigma = +0.356917;
        break;

        case BmnGemStripMediumConfiguration::MEDIUM::ARCO2_70_30_E_1000_2500_3750_6300_B_0_3T:
            NameOfCurrentMedium = "ARCO2_70_30_E_1000_2500_3750_6300_B_0_3T";
            MCD = 0.0333; //pi+, p = 1 GeV/c
            p0_xmean = +7.65E-05;
            p1_xmean = +0.0429225;
            p2_xmean = +0.0801664;
            p3_xmean = -0.0380721;

            p0_sigma = +9.25E-05;
            p1_sigma = +0.109133;
            p2_sigma = -0.407553;
            p3_sigma = +0.919968;
            p4_sigma = -0.99243;
            p5_sigma = +0.395476;
        break;

        case BmnGemStripMediumConfiguration::MEDIUM::ARCO2_70_30_E_1000_2500_3750_6300_B_0_4T:
            NameOfCurrentMedium = "ARCO2_70_30_E_1000_2500_3750_6300_B_0_4T";
            MCD = 0.0333; //pi+, p = 1 GeV/c
            p0_xmean = +2.25E-05;
            p1_xmean = +0.0576697;
            p2_xmean = +0.107259;
            p3_xmean = -0.0516003;

            p0_sigma = +0.000102878;
            p1_sigma = +0.108431;
            p2_sigma = -0.396422;
            p3_sigma = +0.875805;
            p4_sigma = -0.932093;
            p5_sigma = +0.368517;
        break;

        case BmnGemStripMediumConfiguration::MEDIUM::ARCO2_70_30_E_1000_2500_3750_6300_B_0_5T:
            NameOfCurrentMedium = "ARCO2_70_30_E_1000_2500_3750_6300_B_0_5T";
            MCD = 0.0333; //pi+, p = 1 GeV/c
            p0_xmean = +9.89E-05;
            p1_xmean = +0.0720483;
            p2_xmean = +0.135686;
            p3_xmean = -0.0666723;

            p0_sigma = +9.21E-05;
            p1_sigma = +0.112049;
            p2_sigma = -0.423599;
            p3_sigma = +0.952072;
            p4_sigma = -1.02308;
            p5_sigma = +0.407135;
        break;

        case BmnGemStripMediumConfiguration::MEDIUM::ARCO2_70_30_E_1000_2500_3750_6300_B_0_6T:
            NameOfCurrentMedium = "ARCO2_70_30_E_1000_2500_3750_6300_B_0_6T";
            MCD = 0.0333; //pi+, p = 1 GeV/c
            p0_xmean = +0.000122056;
            p1_xmean = +0.086374;
            p2_xmean = +0.166674;
            p3_xmean = -0.0842464;

            p0_sigma = +0.00011739;
            p1_sigma = +0.106558;
            p2_sigma = -0.373914;
            p3_sigma = +0.810349;
            p4_sigma = -0.85823;
            p5_sigma = +0.339713;
        break;

        case BmnGemStripMediumConfiguration::MEDIUM::ARCO2_70_30_E_1000_2500_3750_6300_B_0_7T:
            NameOfCurrentMedium = "ARCO2_70_30_E_1000_2500_3750_6300_B_0_7T";
            MCD = 0.0333; //pi+, p = 1 GeV/c
            p0_xmean = +0.000216219;
            p1_xmean = +0.0988192;
            p2_xmean = +0.19974;
            p3_xmean = -0.101547;

            p0_sigma = +0.000125918;
            p1_sigma = +0.108594;
            p2_sigma = -0.395022;
            p3_sigma = +0.881684;
            p4_sigma = -0.953201;
            p5_sigma = +0.382968;
        break;

        case BmnGemStripMediumConfiguration::MEDIUM::ARCO2_70_30_E_1000_2500_3750_6300_B_0_8T:
            NameOfCurrentMedium = "ARCO2_70_30_E_1000_2500_3750_6300_B_0_8T";
            MCD = 0.0333; //pi+, p = 1 GeV/c
            p0_xmean = +0.000244936;
            p1_xmean = +0.114616;
            p2_xmean = +0.226636;
            p3_xmean = -0.116325;

            p0_sigma = +9.99E-05;
            p1_sigma = +0.109244;
            p2_sigma = -0.389104;
            p3_sigma = +0.842158;
            p4_sigma = -0.887254;
            p5_sigma = +0.349421;
        break;

        case BmnGemStripMediumConfiguration::MEDIUM::ARCO2_70_30_E_1000_2500_3750_6300_B_0_9T:
            NameOfCurrentMedium = "ARCO2_70_30_E_1000_2500_3750_6300_B_0_9T";
            MCD = 0.0333; //pi+, p = 1 GeV/c
            p0_xmean = +0.000135136;
            p1_xmean = +0.130862;
            p2_xmean = +0.255838;
            p3_xmean = -0.134158;

            p0_sigma = +0.000106373;
            p1_sigma = +0.105564;
            p2_sigma = -0.364215;
            p3_sigma = +0.788651;
            p4_sigma = -0.834975;
            p5_sigma = +0.32915;
        break;
 //-----------------------------------------------------------------------------

        case BmnGemStripMediumConfiguration::MEDIUM::ARC4H10_90_10_E_880_1920_2780_3160_B_0_0T:
            NameOfCurrentMedium = "ARC4H10_90_10_E_880_1920_2780_3160_B_0_0T";
            MCD = 0.0293; //pi+, p = 1 GeV/c
            p0_xmean = +0.0;
            p1_xmean = +0.0;
            p2_xmean = +0.0;
            p3_xmean = +0.0;

            p0_sigma = +0.000218351;
            p1_sigma = +0.159335;
            p2_sigma = -0.577004;
            p3_sigma = +1.2323;
            p4_sigma = -1.24324;
            p5_sigma = +0.46784;
        break;

        case BmnGemStripMediumConfiguration::MEDIUM::ARC4H10_90_10_E_880_1920_2780_3160_B_0_1T:
            NameOfCurrentMedium = "ARC4H10_90_10_E_880_1920_2780_3160_B_0_1T";
            MCD = 0.0293; //pi+, p = 1 GeV/c
            p0_xmean = +0.000136557;
            p1_xmean = +0.01686;
            p2_xmean = -0.0104319;
            p3_xmean = +0.04;

            p0_sigma = +0.000217813;
            p1_sigma = +0.161528;
            p2_sigma = -0.589826;
            p3_sigma = +1.26357;
            p4_sigma = -1.28083;
            p5_sigma = +0.485656;
        break;

        case BmnGemStripMediumConfiguration::MEDIUM::ARC4H10_90_10_E_880_1920_2780_3160_B_0_2T:
            NameOfCurrentMedium = "ARC4H10_90_10_E_880_1920_2780_3160_B_0_2T";
            MCD = 0.0293; //pi+, p = 1 GeV/c
            p0_xmean = +0.0;
            p1_xmean = +0.0375998;
            p2_xmean = -0.03123;
            p3_xmean = +0.088;

            p0_sigma = +0.000203127;
            p1_sigma = +0.156997;
            p2_sigma = -0.54365;
            p3_sigma = +1.10862;
            p4_sigma = -1.07919;
            p5_sigma = +0.3969;
        break;

        case BmnGemStripMediumConfiguration::MEDIUM::ARC4H10_90_10_E_880_1920_2780_3160_B_0_3T:
            NameOfCurrentMedium = "ARC4H10_90_10_E_880_1920_2780_3160_B_0_3T";
            MCD = 0.0293; //pi+, p = 1 GeV/c
            p0_xmean = +0.000376744;
            p1_xmean = +0.0545607;
            p2_xmean = -0.0453631;
            p3_xmean = +0.135;

            p0_sigma = +0.000233989;
            p1_sigma = +0.155712;
            p2_sigma = -0.53633;
            p3_sigma = +1.1029;
            p4_sigma = -1.08523;
            p5_sigma = +0.403;
        break;

        case BmnGemStripMediumConfiguration::MEDIUM::ARC4H10_90_10_E_880_1920_2780_3160_B_0_4T:
            NameOfCurrentMedium = "ARC4H10_90_10_E_880_1920_2780_3160_B_0_4T";
            MCD = 0.0293; //pi+, p = 1 GeV/c
            p0_xmean = +0.00048425;
            p1_xmean = +0.0698332;
            p2_xmean = -0.0501295;
            p3_xmean = +0.178;

            p0_sigma = +0.000213628;
            p1_sigma = +0.15727;
            p2_sigma = -0.541332;
            p3_sigma = +1.10864;
            p4_sigma = -1.08878;
            p5_sigma = +0.404879;
        break;

        case BmnGemStripMediumConfiguration::MEDIUM::ARC4H10_90_10_E_880_1920_2780_3160_B_0_5T:
            NameOfCurrentMedium = "ARC4H10_90_10_E_880_1920_2780_3160_B_0_5T";
            MCD = 0.0293; //pi+, p = 1 GeV/c
            p0_xmean = +0.000574288;
            p1_xmean = +0.0924847;
            p2_xmean = -0.0822836;
            p3_xmean = +0.245;

            p0_sigma = +0.000229163;
            p1_sigma = +0.15691;
            p2_sigma = -0.52096;
            p3_sigma = +1.00636;
            p4_sigma = -0.922778;
            p5_sigma = +0.320137;
        break;

        case BmnGemStripMediumConfiguration::MEDIUM::ARC4H10_90_10_E_880_1920_2780_3160_B_0_6T:
            NameOfCurrentMedium = "ARC4H10_90_10_E_880_1920_2780_3160_B_0_6T";
            MCD = 0.0293; //pi+, p = 1 GeV/c
            p0_xmean = +0.000708821;
            p1_xmean = +0.110947;
            p2_xmean = -0.104191;
            p3_xmean = +0.308;

            p0_sigma = +0.000251849;
            p1_sigma = +0.156613;
            p2_sigma = -0.523413;
            p3_sigma = +1.01115;
            p4_sigma = -0.909296;
            p5_sigma = +0.304849;
        break;

        case BmnGemStripMediumConfiguration::MEDIUM::ARC4H10_90_10_E_880_1920_2780_3160_B_0_7T:
            NameOfCurrentMedium = "ARC4H10_90_10_E_880_1920_2780_3160_B_0_7T";
            MCD = 0.0293; //pi+, p = 1 GeV/c
            p0_xmean = +0.000587651;
            p1_xmean = +0.136483;
            p2_xmean = -0.14637;
            p3_xmean = +0.395;

            p0_sigma = +0.000247633;
            p1_sigma = +0.159139;
            p2_sigma = -0.542189;
            p3_sigma = +1.08029;
            p4_sigma = -1.00945;
            p5_sigma = +0.355662;
        break;

        case BmnGemStripMediumConfiguration::MEDIUM::ARC4H10_90_10_E_880_1920_2780_3160_B_0_8T:
            NameOfCurrentMedium = "ARC4H10_90_10_E_880_1920_2780_3160_B_0_8T";
            MCD = 0.0293; //pi+, p = 1 GeV/c
            p0_xmean = +0.000824255;
            p1_xmean = +0.156657;
            p2_xmean = -0.17709;
            p3_xmean = +0.475568;

            p0_sigma = +0.000252085;
            p1_sigma = +0.15543;
            p2_sigma = -0.518142;
            p3_sigma = +1.03222;
            p4_sigma = -0.97707;
            p5_sigma = +0.355;
        break;

        case BmnGemStripMediumConfiguration::MEDIUM::ARC4H10_90_10_E_880_1920_2780_3160_B_0_9T:
            NameOfCurrentMedium = "ARC4H10_90_10_E_880_1920_2780_3160_B_0_9T";
            MCD = 0.0293; //pi+, p = 1 GeV/c
            p0_xmean = +0.00092319;
            p1_xmean = +0.182364;
            p2_xmean = -0.226267;
            p3_xmean = +0.584;

            p0_sigma = +0.0;
            p1_sigma = +0.160185;
            p2_sigma = -0.542958;
            p3_sigma = +1.0662;
            p4_sigma = -0.969753;
            p5_sigma = +0.336722;
        break;
//------------------------------------------------------------------------------

        case BmnGemStripMediumConfiguration::MEDIUM::ARC4H10_80_20_E_1720_2240_3230_3730_B_0_0T:
            NameOfCurrentMedium = "ARC4H10_80_20_E_1720_2240_3230_3730_B_0_0T";
            MCD = 0.0245; //pi+, p = 1 GeV/c
            p0_xmean = +0.0;
            p1_xmean = +0.0;
            p2_xmean = +0.0;
            p3_xmean = +0.0;

            p0_sigma = +1.78E-04;
            p1_sigma = +0.117013;
            p2_sigma = -0.398094;
            p3_sigma = +0.813952;
            p4_sigma = -0.80685;
            p5_sigma = +0.303486;
        break;

        case BmnGemStripMediumConfiguration::MEDIUM::ARC4H10_80_20_E_1720_2240_3230_3730_B_0_1T:
            NameOfCurrentMedium = "ARC4H10_80_20_E_1720_2240_3230_3730_B_0_1T";
            MCD = 0.0245; //pi+, p = 1 GeV/c
            p0_xmean = +1.23E-04;
            p1_xmean = +0.0127338;
            p2_xmean = +0.0165141;
            p3_xmean = +0.000570247;

            p0_sigma = +1.65E-04;
            p1_sigma = +0.122198;
            p2_sigma = -0.438275;
            p3_sigma = +0.929611;
            p4_sigma = -0.95206;
            p5_sigma = +0.369934;
        break;

        case BmnGemStripMediumConfiguration::MEDIUM::ARC4H10_80_20_E_1720_2240_3230_3730_B_0_2T:
            NameOfCurrentMedium = "ARC4H10_80_20_E_1720_2240_3230_3730_B_0_2T";
            MCD = 0.0245; //pi+, p = 1 GeV/c
            p0_xmean = +1.54E-04;
            p1_xmean = +0.0265235;
            p2_xmean = +0.0322077;
            p3_xmean = +0.00114531;

            p0_sigma = +0.000185458;
            p1_sigma = +0.118534;
            p2_sigma = -0.417449;
            p3_sigma = +0.89304;
            p4_sigma = -0.926657;
            p5_sigma = +0.364416;
        break;

        case BmnGemStripMediumConfiguration::MEDIUM::ARC4H10_80_20_E_1720_2240_3230_3730_B_0_3T:
            NameOfCurrentMedium = "ARC4H10_80_20_E_1720_2240_3230_3730_B_0_3T";
            MCD = 0.0245; //pi+, p = 1 GeV/c
            p0_xmean = +4.25E-04;
            p1_xmean = +0.0373764;
            p2_xmean = +0.054244;
            p3_xmean = -0.00116018;

            p0_sigma = +1.79E-04;
            p1_sigma = +0.116144;
            p2_sigma = -0.378046;
            p3_sigma = +0.737007;
            p4_sigma = -0.697782;
            p5_sigma = +0.251924;
        break;

        case BmnGemStripMediumConfiguration::MEDIUM::ARC4H10_80_20_E_1720_2240_3230_3730_B_0_4T:
            NameOfCurrentMedium = "ARC4H10_80_20_E_1720_2240_3230_3730_B_0_4T";
            MCD = 0.0245; //pi+, p = 1 GeV/c
            p0_xmean = +4.45E-04;
            p1_xmean = +0.0518626;
            p2_xmean = +0.0676802;
            p3_xmean = +0.00195376;

            p0_sigma = +0.000181581;
            p1_sigma = +0.120391;
            p2_sigma = -0.425589;
            p3_sigma = +0.887732;
            p4_sigma = -0.885041;
            p5_sigma = +0.332821;
        break;

        case BmnGemStripMediumConfiguration::MEDIUM::ARC4H10_80_20_E_1720_2240_3230_3730_B_0_5T:
            NameOfCurrentMedium = "ARC4H10_80_20_E_1720_2240_3230_3730_B_0_5T";
            MCD = 0.0245; //pi+, p = 1 GeV/c
            p0_xmean = +5.52E-04;
            p1_xmean = +0.0659937;
            p2_xmean = +0.0809207;
            p3_xmean = +0.00718051;

            p0_sigma = +1.87E-04;
            p1_sigma = +0.12034;
            p2_sigma = -0.412872;
            p3_sigma = +0.830354;
            p4_sigma = -0.800251;
            p5_sigma = +0.292069;
        break;

        case BmnGemStripMediumConfiguration::MEDIUM::ARC4H10_80_20_E_1720_2240_3230_3730_B_0_6T:
            NameOfCurrentMedium = "ARC4H10_80_20_E_1720_2240_3230_3730_B_0_6T";
            MCD = 0.0245; //pi+, p = 1 GeV/c
            p0_xmean = +0.000602183;
            p1_xmean = +0.0809795;
            p2_xmean = +0.0918447;
            p3_xmean = +0.0157542;

            p0_sigma = +0.000137712;
            p1_sigma = +0.125844;
            p2_sigma = -0.468967;
            p3_sigma = +1.01775;
            p4_sigma = -1.04621;
            p5_sigma = +0.403336;
        break;

        case BmnGemStripMediumConfiguration::MEDIUM::ARC4H10_80_20_E_1720_2240_3230_3730_B_0_7T:
            NameOfCurrentMedium = "ARC4H10_80_20_E_1720_2240_3230_3730_B_0_7T";
            MCD = 0.0245; //pi+, p = 1 GeV/c
            p0_xmean = +0.000909404;
            p1_xmean = +0.0896112;
            p2_xmean = +0.125486;
            p3_xmean = +0.00723235;

            p0_sigma = +0.000179813;
            p1_sigma = +0.122855;
            p2_sigma = -0.442513;
            p3_sigma = +0.950076;
            p4_sigma = -0.978707;
            p5_sigma = +0.380569;
        break;

        case BmnGemStripMediumConfiguration::MEDIUM::ARC4H10_80_20_E_1720_2240_3230_3730_B_0_8T:
            NameOfCurrentMedium = "ARC4H10_80_20_E_1720_2240_3230_3730_B_0_8T";
            MCD = 0.0245; //pi+, p = 1 GeV/c
            p0_xmean = +0.000962017;
            p1_xmean = +0.104726;
            p2_xmean = +0.137705;
            p3_xmean = +0.016483;

            p0_sigma = +1.86E-04;
            p1_sigma = +0.122116;
            p2_sigma = -0.435668;
            p3_sigma = +0.934093;
            p4_sigma = -0.963362;
            p5_sigma = +0.376114;
        break;

        case BmnGemStripMediumConfiguration::MEDIUM::ARC4H10_80_20_E_1720_2240_3230_3730_B_0_9T:
            NameOfCurrentMedium = "ARC4H10_80_20_E_1720_2240_3230_3730_B_0_9T";
            MCD = 0.0245; //pi+, p = 1 GeV/c
            p0_xmean = +0.00103555;
            p1_xmean = +0.117826;
            p2_xmean = +0.160241;
            p3_xmean = +0.0178768;

            p0_sigma = +0.000190609;
            p1_sigma = +0.121689;
            p2_sigma = -0.424431;
            p3_sigma = +0.881712;
            p4_sigma = -0.871142;
            p5_sigma = +0.323333;
        break;
//------------------------------------------------------------------------------

        case BmnGemStripMediumConfiguration::MEDIUM::ARCO2_70_30_E_1000_2500_3750_6300_B_0_59T:
            NameOfCurrentMedium = "ARCO2_70_30_E_1000_2500_3750_6300_B_0_59T";
            MCD = 0.0333; //pi+, p = 1 GeV/c
            p0_xmean = +7.89E-05;
            p1_xmean = +0.0849255;
            p2_xmean = +0.162055;
            p3_xmean = -0.0801631;

            p0_sigma = +9.17E-05;
            p1_sigma = +0.107935;
            p2_sigma = -0.389298;
            p3_sigma = +0.851033;
            p4_sigma = -0.894719;
            p5_sigma = +0.348757;
        break;
//------------------------------------------------------------------------------

        default:
            NameOfCurrentMedium = "(DEFAULT)_ARCO2_70_30_E_1000_2500_3750_6300_B_0_0T";
            MCD = 0.0333; //pi+, p = 1 GeV/c
            p0_xmean = +0.0;
            p1_xmean = +0.0;
            p2_xmean = +0.0;
            p3_xmean = +0.0;

            p0_sigma = +1.19E-04;
            p1_sigma = +0.110434;
            p2_sigma = -0.424199;
            p3_sigma = +0.976967;
            p4_sigma = -1.06588;
            p5_sigma = +0.428521;
            status = false;
    }
    return status;
}

ClassImp(BmnGemStripMedium)
