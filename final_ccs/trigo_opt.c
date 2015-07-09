#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
//#include "utils/uartstdio.h"
#include "usb_device.h"
#define BACKSTEP
//#include <trigo_m4.h>

const unsigned int sinbuf[] = {0,
114,228,343,457,571,686,800,915,1029,1143,
1258,1372,1486,1601,1715,1829,1944,2058,2172,2287,
2401,2515,2630,2744,2858,2972,3087,3201,3315,3429,
3544,3658,3772,3886,4000,4115,4229,4343,4457,4571,
4685,4799,4913,5027,5141,5255,5369,5483,5597,5711,
5825,5939,6053,6167,6281,6395,6509,6622,6736,6850,
6964,7077,7191,7305,7418,7532,7646,7759,7873,7986,
8100,8213,8327,8440,8554,8667,8780,8894,9007,9120,
9234,9347,9460,9573,9686,9799,9913,10026,10139,10252,
10365,10477,10590,10703,10816,10929,11042,11154,11267,11380,
11492,11605,11717,11830,11942,12055,12167,12280,12392,12504,
12617,12729,12841,12953,13065,13177,13289,13401,13513,13625,
13737,13849,13961,14072,14184,14296,14407,14519,14630,14742,
14853,14965,15076,15187,15299,15410,15521,15632,15743,15854,
15965,16076,16187,16298,16408,16519,16630,16740,16851,16961,
17072,17182,17293,17403,17513,17623,17734,17844,17954,18064,
18174,18283,18393,18503,18613,18722,18832,18941,19051,19160,
19270,19379,19488,19597,19707,19816,19925,20034,20142,20251,
20360,20469,20577,20686,20794,20903,21011,21119,21228,21336,
21444,21552,21660,21768,21876,21984,22091,22199,22307,22414,
22522,22629,22736,22843,22951,23058,23165,23272,23379,23485,
23592,23699,23806,23912,24019,24125,24231,24337,24444,24550,
24656,24762,24868,24973,25079,25185,25290,25396,25501,25606,
25712,25817,25922,26027,26132,26237,26342,26446,26551,26655,
26760,26864,26968,27073,27177,27281,27385,27489,27592,27696,
27800,27903,28007,28110,28213,28317,28420,28523,28626,28729,
28831,28934,29037,29139,29241,29344,29446,29548,29650,29752,
29854,29956,30058,30159,30261,30362,30463,30565,30666,30767,
30868,30969,31069,31170,31271,31371,31471,31572,31672,31772,
31872,31972,32072,32171,32271,32370,32470,32569,32668,32767,
32866,32965,33064,33163,33262,33360,33458,33557,33655,33753,
33851,33949,34047,34144,34242,34339,34437,34534,34631,34728,
34825,34922,35019,35115,35212,35308,35405,35501,35597,35693,
35789,35885,35980,36076,36171,36267,36362,36457,36552,36647,
36742,36836,36931,37025,37119,37214,37308,37402,37496,37589,
37683,37777,37870,37963,38056,38149,38242,38335,38428,38521,
38613,38705,38798,38890,38982,39074,39165,39257,39349,39440,
39531,39622,39714,39804,39895,39986,40077,40167,40257,40347,
40438,40527,40617,40707,40797,40886,40975,41065,41154,41243,
41331,41420,41509,41597,41685,41774,41862,41950,42038,42125,
42213,42300,42387,42475,42562,42649,42735,42822,42909,42995,
43081,43167,43253,43339,43425,43511,43596,43681,43767,43852,
43937,44021,44106,44191,44275,44359,44443,44527,44611,44695,
44778,44862,44945,45028,45111,45194,45277,45360,45442,45525,
45607,45689,45771,45853,45934,46016,46097,46178,46259,46340,
46421,46502,46582,46663,46743,46823,46903,46983,47063,47142,
47222,47301,47380,47459,47538,47616,47695,47773,47851,47929,
48007,48085,48163,48240,48318,48395,48472,48549,48626,48702,
48779,48855,48931,49007,49083,49159,49234,49310,49385,49460,
49535,49610,49685,49759,49833,49908,49982,50056,50129,50203,
50276,50350,50423,50496,50569,50641,50714,50786,50858,50931,
51002,51074,51146,51217,51288,51360,51431,51501,51572,51643,
51713,51783,51853,51923,51993,52062,52132,52201,52270,52339,
52408,52476,52545,52613,52681,52749,52817,52884,52952,53019,
53086,53153,53220,53287,53353,53420,53486,53552,53618,53683,
53749,53814,53880,53945,54009,54074,54139,54203,54267,54331,
54395,54459,54522,54586,54649,54712,54775,54838,54900,54963,
55025,55087,55149,55210,55272,55333,55395,55456,55516,55577,
55638,55698,55758,55818,55878,55938,55997,56057,56116,56175,
56234,56292,56351,56409,56467,56525,56583,56641,56698,56755,
56812,56869,56926,56983,57039,57095,57151,57207,57263,57319,
57374,57429,57484,57539,57594,57648,57702,57757,57811,57864,
57918,57971,58025,58078,58131,58183,58236,58288,58340,58392,
58444,58496,58547,58599,58650,58701,58752,58802,58853,58903,
58953,59003,59052,59102,59151,59200,59249,59298,59347,59395,
59444,59492,59539,59587,59635,59682,59729,59776,59823,59870,
59916,59962,60008,60054,60100,60145,60191,60236,60281,60326,
60370,60415,60459,60503,60547,60591,60634,60677,60720,60763,
60806,60849,60891,60933,60975,61017,61059,61100,61142,61183,
61224,61264,61305,61345,61385,61425,61465,61505,61544,61583,
61622,61661,61700,61738,61776,61815,61852,61890,61928,61965,
62002,62039,62076,62112,62149,62185,62221,62257,62292,62328,
62363,62398,62433,62468,62502,62537,62571,62605,62638,62672,
62705,62738,62771,62804,62837,62869,62901,62933,62965,62997,
63028,63059,63090,63121,63152,63182,63213,63243,63273,63302,
63332,63361,63390,63419,63448,63477,63505,63533,63561,63589,
63616,63644,63671,63698,63725,63751,63778,63804,63830,63856,
63881,63907,63932,63957,63982,64007,64031,64055,64079,64103,
64127,64151,64174,64197,64220,64243,64265,64287,64309,64331,
64353,64375,64396,64417,64438,64459,64479,64500,64520,64540,
64560,64579,64599,64618,64637,64655,64674,64692,64711,64729,
64746,64764,64781,64799,64816,64832,64849,64865,64882,64898,
64914,64929,64945,64960,64975,64990,65004,65019,65033,65047,
65061,65074,65088,65101,65114,65127,65140,65152,65164,65176,
65188,65200,65211,65223,65234,65245,65255,65266,65276,65286,
65296,65306,65315,65324,65333,65342,65351,65359,65368,65376,
65384,65391,65399,65406,65413,65420,65427,65433,65440,65446,
65452,65457,65463,65468,65473,65478,65483,65487,65491,65496,
65499,65503,65507,65510,65513,65516,65519,65521,65523,65526,
65527,65529,65531,65532,65533,65534,65535,65535,65536,65536,
};
	
const unsigned int cosbuf[] = {65536,
65536,65535,65535,65534,65533,65532,65531,65529,65527,65526,
65523,65521,65519,65516,65513,65510,65507,65503,65499,65496,
65491,65487,65483,65478,65473,65468,65463,65457,65452,65446,
65440,65433,65427,65420,65413,65406,65399,65391,65384,65376,
65368,65360,65351,65342,65333,65324,65315,65306,65296,65286,
65276,65266,65255,65245,65234,65223,65211,65200,65188,65176,
65164,65152,65140,65127,65114,65101,65088,65074,65061,65047,
65033,65019,65004,64990,64975,64960,64945,64929,64914,64898,
64882,64865,64849,64832,64816,64799,64781,64764,64746,64729,
64711,64692,64674,64655,64637,64618,64599,64579,64560,64540,
64520,64500,64479,64459,64438,64417,64396,64375,64353,64331,
64309,64287,64265,64243,64220,64197,64174,64151,64127,64103,
64080,64055,64031,64007,63982,63957,63932,63907,63881,63856,
63830,63804,63778,63751,63725,63698,63671,63644,63616,63589,
63561,63533,63505,63477,63448,63419,63390,63361,63332,63302,
63273,63243,63213,63182,63152,63121,63090,63059,63028,62997,
62965,62933,62901,62869,62837,62804,62771,62738,62705,62672,
62638,62605,62571,62537,62502,62468,62433,62398,62363,62328,
62293,62257,62221,62185,62149,62112,62076,62039,62002,61965,
61928,61890,61852,61815,61776,61738,61700,61661,61622,61583,
61544,61505,61465,61425,61385,61345,61305,61264,61224,61183,
61142,61100,61059,61017,60975,60933,60891,60849,60806,60763,
60720,60677,60634,60591,60547,60503,60459,60415,60370,60326,
60281,60236,60191,60145,60100,60054,60008,59962,59916,59870,
59823,59776,59729,59682,59635,59587,59540,59492,59444,59395,
59347,59298,59249,59200,59151,59102,59052,59003,58953,58903,
58853,58802,58752,58701,58650,58599,58547,58496,58444,58393,
58340,58288,58236,58183,58131,58078,58025,57971,57918,57864,
57811,57757,57702,57648,57594,57539,57484,57429,57374,57319,
57263,57207,57151,57095,57039,56983,56926,56869,56812,56755,
56698,56641,56583,56525,56467,56409,56351,56292,56234,56175,
56116,56057,55997,55938,55878,55818,55758,55698,55638,55577,
55516,55456,55395,55333,55272,55210,55149,55087,55025,54963,
54900,54838,54775,54712,54649,54586,54522,54459,54395,54331,
54267,54203,54139,54074,54009,53945,53880,53814,53749,53683,
53618,53552,53486,53420,53353,53287,53220,53153,53086,53019,
52952,52884,52817,52749,52681,52613,52545,52476,52408,52339,
52270,52201,52132,52062,51993,51923,51853,51783,51713,51643,
51572,51501,51431,51360,51289,51217,51146,51074,51002,50931,
50859,50786,50714,50641,50569,50496,50423,50350,50276,50203,
50129,50056,49982,49908,49833,49759,49685,49610,49535,49460,
49385,49310,49234,49159,49083,49007,48931,48855,48779,48702,
48626,48549,48472,48395,48318,48240,48163,48085,48007,47930,
47851,47773,47695,47616,47538,47459,47380,47301,47222,47142,
47063,46983,46903,46823,46743,46663,46582,46502,46421,46340,
46260,46178,46097,46016,45934,45853,45771,45689,45607,45525,
45442,45360,45277,45194,45112,45028,44945,44862,44779,44695,
44611,44527,44443,44359,44275,44191,44106,44021,43937,43852,
43767,43681,43596,43511,43425,43339,43253,43167,43081,42995,
42909,42822,42735,42649,42562,42475,42388,42300,42213,42125,
42038,41950,41862,41774,41686,41597,41509,41420,41332,41243,
41154,41065,40975,40886,40797,40707,40617,40528,40438,40348,
40257,40167,40077,39986,39895,39805,39714,39623,39531,39440,
39349,39257,39166,39074,38982,38890,38798,38705,38613,38521,
38428,38335,38242,38150,38056,37963,37870,37777,37683,37589,
37496,37402,37308,37214,37120,37025,36931,36836,36742,36647,
36552,36457,36362,36267,36171,36076,35980,35885,35789,35693,
35597,35501,35405,35308,35212,35115,35019,34922,34825,34728,
34631,34534,34437,34339,34242,34144,34047,33949,33851,33753,
33655,33557,33458,33360,33262,33163,33064,32965,32867,32768,
32668,32569,32470,32371,32271,32171,32072,31972,31872,31772,
31672,31572,31471,31371,31271,31170,31069,30969,30868,30767,
30666,30565,30463,30362,30261,30159,30058,29956,29854,29752,
29650,29548,29446,29344,29242,29139,29037,28934,28831,28729,
28626,28523,28420,28317,28214,28110,28007,27903,27800,27696,
27593,27489,27385,27281,27177,27073,26969,26864,26760,26655,
26551,26446,26342,26237,26132,26027,25922,25817,25712,25607,
25501,25396,25290,25185,25079,24973,24868,24762,24656,24550,
24444,24338,24231,24125,24019,23912,23806,23699,23592,23486,
23379,23272,23165,23058,22951,22844,22736,22629,22522,22414,
22307,22199,22091,21984,21876,21768,21660,21552,21444,21336,
21228,21120,21011,20903,20794,20686,20577,20469,20360,20251,
20142,20034,19925,19816,19707,19598,19488,19379,19270,19160,
19051,18942,18832,18722,18613,18503,18393,18284,18174,18064,
17954,17844,17734,17623,17513,17403,17293,17182,17072,16962,
16851,16740,16630,16519,16408,16298,16187,16076,15965,15854,
15743,15632,15521,15410,15299,15187,15076,14965,14853,14742,
14630,14519,14407,14296,14184,14072,13961,13849,13737,13625,
13513,13401,13289,13177,13065,12953,12841,12729,12617,12504,
12392,12280,12167,12055,11943,11830,11718,11605,11492,11380,
11267,11154,11042,10929,10816,10703,10590,10478,10365,10252,
10139,10026,9913,9800,9686,9573,9460,9347,9234,9120,
9007,8894,8780,8667,8554,8440,8327,8213,8100,7986,
7873,7759,7646,7532,7418,7305,7191,7077,6964,6850,
6736,6622,6509,6395,6281,6167,6053,5939,5825,5711,
5597,5483,5370,5255,5141,5027,4913,4799,4685,4571,
4457,4343,4229,4115,4000,3886,3772,3658,3544,3429,
3315,3201,3087,2972,2858,2744,2630,2515,2401,2287,
2172,2058,1944,1829,1715,1601,1486,1372,1258,1143,
1029,915,800,686,571,457,343,228,114,0,
};

const unsigned int atan2buf[] = {
0x00000000,0x00000065,0x000000C9,0x0000012E,0x00000192,0x000001F7,0x0000025B,0x000002C0,
0x00000324,0x00000389,0x000003ED,0x00000452,0x000004B7,0x0000051B,0x00000580,0x000005E4,
0x00000649,0x000006AD,0x00000712,0x00000777,0x000007DB,0x00000840,0x000008A5,0x00000909,
0x0000096E,0x000009D3,0x00000A37,0x00000A9C,0x00000B01,0x00000B65,0x00000BCA,0x00000C2F,
0x00000C94,0x00000CF8,0x00000D5D,0x00000DC2,0x00000E27,0x00000E8C,0x00000EF1,0x00000F55,
0x00000FBA,0x0000101F,0x00001084,0x000010E9,0x0000114E,0x000011B3,0x00001218,0x0000127D,
0x000012E2,0x00001347,0x000013AC,0x00001412,0x00001477,0x000014DC,0x00001541,0x000015A6,
0x0000160C,0x00001671,0x000016D6,0x0000173C,0x000017A1,0x00001806,0x0000186C,0x000018D1,
0x00001937,0x0000199C,0x00001A02,0x00001A67,0x00001ACD,0x00001B33,0x00001B98,0x00001BFE,
0x00001C64,0x00001CCA,0x00001D2F,0x00001D95,0x00001DFB,0x00001E61,0x00001EC7,0x00001F2D,
0x00001F93,0x00001FF9,0x0000205F,0x000020C5,0x0000212C,0x00002192,0x000021F8,0x0000225E,
0x000022C5,0x0000232B,0x00002392,0x000023F8,0x0000245F,0x000024C5,0x0000252C,0x00002593,
0x000025F9,0x00002660,0x000026C7,0x0000272E,0x00002795,0x000027FC,0x00002863,0x000028CA,
0x00002931,0x00002998,0x000029FF,0x00002A66,0x00002ACE,0x00002B35,0x00002B9D,0x00002C04,
0x00002C6C,0x00002CD3,0x00002D3B,0x00002DA2,0x00002E0A,0x00002E72,0x00002EDA,0x00002F42,
0x00002FAA,0x00003012,0x0000307A,0x000030E2,0x0000314A,0x000031B3,0x0000321B,0x00003283,
0x000032EC,0x00003354,0x000033BD,0x00003426,0x0000348E,0x000034F7,0x00003560,0x000035C9,
0x00003632,0x0000369B,0x00003704,0x0000376D,0x000037D7,0x00003840,0x000038AA,0x00003913,
0x0000397D,0x000039E6,0x00003A50,0x00003ABA,0x00003B24,0x00003B8E,0x00003BF8,0x00003C62,
0x00003CCC,0x00003D36,0x00003DA0,0x00003E0B,0x00003E75,0x00003EE0,0x00003F4A,0x00003FB5,
0x00004020,0x0000408B,0x000040F6,0x00004161,0x000041CC,0x00004237,0x000042A2,0x0000430E,
0x00004379,0x000043E5,0x00004451,0x000044BC,0x00004528,0x00004594,0x00004600,0x0000466C,
0x000046D8,0x00004745,0x000047B1,0x0000481D,0x0000488A,0x000048F7,0x00004963,0x000049D0,
0x00004A3D,0x00004AAA,0x00004B17,0x00004B84,0x00004BF2,0x00004C5F,0x00004CCD,0x00004D3A,
0x00004DA8,0x00004E16,0x00004E84,0x00004EF2,0x00004F60,0x00004FCE,0x0000503D,0x000050AB,
0x0000511A,0x00005188,0x000051F7,0x00005266,0x000052D5,0x00005344,0x000053B3,0x00005423,
0x00005492,0x00005502,0x00005571,0x000055E1,0x00005651,0x000056C1,0x00005731,0x000057A1,
0x00005812,0x00005882,0x000058F3,0x00005964,0x000059D5,0x00005A46,0x00005AB7,0x00005B28,
0x00005B99,0x00005C0B,0x00005C7C,0x00005CEE,0x00005D60,0x00005DD2,0x00005E44,0x00005EB6,
0x00005F28,0x00005F9B,0x0000600E,0x00006080,0x000060F3,0x00006166,0x000061D9,0x0000624D,
0x000062C0,0x00006334,0x000063A7,0x0000641B,0x0000648F,0x00006503,0x00006577,0x000065EC,
0x00006660,0x000066D5,0x0000674A,0x000067BF,0x00006834,0x000068A9,0x0000691F,0x00006994,
0x00006A0A,0x00006A80,0x00006AF6,0x00006B6C,0x00006BE2,0x00006C59,0x00006CCF,0x00006D46,
0x00006DBD,0x00006E34,0x00006EAB,0x00006F23,0x00006F9A,0x00007012,0x0000708A,0x00007102,
0x0000717A,0x000071F2,0x0000726B,0x000072E4,0x0000735D,0x000073D6,0x0000744F,0x000074C8,
0x00007542,0x000075BB,0x00007635,0x000076AF,0x0000772A,0x000077A4,0x0000781F,0x00007899,
0x00007914,0x0000798F,0x00007A0B,0x00007A86,0x00007B02,0x00007B7E,0x00007BFA,0x00007C76,
0x00007CF2,0x00007D6F,0x00007DEC,0x00007E68,0x00007EE6,0x00007F63,0x00007FE0,0x0000805E,
0x000080DC,0x0000815A,0x000081D8,0x00008257,0x000082D6,0x00008355,0x000083D4,0x00008453,
0x000084D2,0x00008552,0x000085D2,0x00008652,0x000086D2,0x00008753,0x000087D4,0x00008855,
0x000088D6,0x00008957,0x000089D9,0x00008A5A,0x00008ADC,0x00008B5F,0x00008BE1,0x00008C64,
0x00008CE7,0x00008D6A,0x00008DED,0x00008E71,0x00008EF4,0x00008F78,0x00008FFD,0x00009081,
0x00009106,0x0000918B,0x00009210,0x00009295,0x0000931B,0x000093A1,0x00009427,0x000094AD,
0x00009534,0x000095BA,0x00009641,0x000096C9,0x00009750,0x000097D8,0x00009860,0x000098E8,
0x00009971,0x000099FA,0x00009A83,0x00009B0C,0x00009B95,0x00009C1F,0x00009CA9,0x00009D34,
0x00009DBE,0x00009E49,0x00009ED4,0x00009F5F,0x00009FEB,0x0000A077,0x0000A103,0x0000A190,
0x0000A21C,0x0000A2A9,0x0000A336,0x0000A3C4,0x0000A452,0x0000A4E0,0x0000A56E,0x0000A5FD,
0x0000A68C,0x0000A71B,0x0000A7AB,0x0000A83A,0x0000A8CA,0x0000A95B,0x0000A9EC,0x0000AA7C,
0x0000AB0E,0x0000AB9F,0x0000AC31,0x0000ACC3,0x0000AD56,0x0000ADE9,0x0000AE7C,0x0000AF0F,
0x0000AFA3,0x0000B037,0x0000B0CB,0x0000B160,0x0000B1F5,0x0000B28A,0x0000B320,0x0000B3B5,
0x0000B44C,0x0000B4E2,0x0000B579,0x0000B610,0x0000B6A8,0x0000B740,0x0000B7D8,0x0000B870,
0x0000B909,0x0000B9A3,0x0000BA3C,0x0000BAD6,0x0000BB70,0x0000BC0B,0x0000BCA6,0x0000BD41,
0x0000BDDD,0x0000BE79,0x0000BF15,0x0000BFB2,0x0000C04F,0x0000C0EC,0x0000C18A,0x0000C228,
0x0000C2C7,0x0000C366,0x0000C405,0x0000C4A5,0x0000C545,0x0000C5E5,0x0000C686,0x0000C727,
0x0000C7C9,0x0000C86B,0x0000C90D,0x0000C9B0,0x0000CA53,0x0000CAF6,0x0000CB9A,0x0000CC3F,
0x0000CCE3,0x0000CD89,0x0000CE2E,0x0000CED4,0x0000CF7A,0x0000D021,0x0000D0C8,0x0000D170,
0x0000D218,0x0000D2C0,0x0000D369,0x0000D413,0x0000D4BC,0x0000D567,0x0000D611,0x0000D6BC,
0x0000D768,0x0000D814,0x0000D8C0,0x0000D96D,0x0000DA1A,0x0000DAC8,0x0000DB76,0x0000DC25,
0x0000DCD4,0x0000DD83,0x0000DE33,0x0000DEE4,0x0000DF95,0x0000E046,0x0000E0F8,0x0000E1AB,
0x0000E25E,0x0000E311,0x0000E3C5,0x0000E479,0x0000E52E,0x0000E5E3,0x0000E699,0x0000E750,
0x0000E806,0x0000E8BE,0x0000E976,0x0000EA2E,0x0000EAE7,0x0000EBA0,0x0000EC5A,0x0000ED15,
0x0000EDD0,0x0000EE8B,0x0000EF47,0x0000F004,0x0000F0C1,0x0000F17F,0x0000F23D,0x0000F2FC,
0x0000F3BB,0x0000F47B,0x0000F53C,0x0000F5FD,0x0000F6BF,0x0000F781,0x0000F844,0x0000F907,
0x0000F9CB,0x0000FA8F,0x0000FB55,0x0000FC1A,0x0000FCE1,0x0000FDA8,0x0000FE6F,0x0000FF37,
0x00010000,0x000100C9,0x00010193,0x0001025E,0x00010329,0x000103F5,0x000104C2,0x0001058F,
0x0001065D,0x0001072B,0x000107FA,0x000108CA,0x0001099A,0x00010A6B,0x00010B3D,0x00010C10,
0x00010CE3,0x00010DB6,0x00010E8B,0x00010F60,0x00011036,0x0001110C,0x000111E4,0x000112BC,
0x00011394,0x0001146E,0x00011548,0x00011623,0x000116FE,0x000117DA,0x000118B8,0x00011995,
0x00011A74,0x00011B53,0x00011C33,0x00011D14,0x00011DF6,0x00011ED8,0x00011FBB,0x0001209F,
0x00012184,0x00012269,0x00012350,0x00012437,0x0001251F,0x00012607,0x000126F1,0x000127DB,
0x000128C6,0x000129B2,0x00012A9F,0x00012B8D,0x00012C7C,0x00012D6B,0x00012E5C,0x00012F4D,
0x0001303F,0x00013132,0x00013226,0x0001331A,0x00013410,0x00013507,0x000135FE,0x000136F7,
0x000137F0,0x000138EA,0x000139E5,0x00013AE1,0x00013BDF,0x00013CDD,0x00013DDC,0x00013EDC,
0x00013FDD,0x000140DF,0x000141E2,0x000142E6,0x000143EB,0x000144F0,0x000145F8,0x00014700,
0x00014809,0x00014913,0x00014A1E,0x00014B2A,0x00014C37,0x00014D46,0x00014E55,0x00014F66,
0x00015077,0x0001518A,0x0001529E,0x000153B3,0x000154C9,0x000155E0,0x000156F9,0x00015812,
0x0001592D,0x00015A49,0x00015B66,0x00015C84,0x00015DA4,0x00015EC4,0x00015FE6,0x00016109,
0x0001622E,0x00016353,0x0001647A,0x000165A2,0x000166CC,0x000167F6,0x00016922,0x00016A4F,
0x00016B7E,0x00016CAE,0x00016DDF,0x00016F12,0x00017045,0x0001717B,0x000172B1,0x000173E9,
0x00017523,0x0001765E,0x0001779A,0x000178D7,0x00017A17,0x00017B57,0x00017C99,0x00017DDD,
0x00017F22,0x00018068,0x000181B0,0x000182F9,0x00018444,0x00018591,0x000186DF,0x0001882F,
0x00018980,0x00018AD3,0x00018C27,0x00018D7D,0x00018ED5,0x0001902F,0x0001918A,0x000192E6,
0x00019445,0x000195A5,0x00019707,0x0001986A,0x000199CF,0x00019B36,0x00019C9F,0x00019E0A,
0x00019F76,0x0001A0E4,0x0001A254,0x0001A3C6,0x0001A53A,0x0001A6B0,0x0001A827,0x0001A9A1,
0x0001AB1C,0x0001AC9A,0x0001AE19,0x0001AF9A,0x0001B11D,0x0001B2A3,0x0001B42A,0x0001B5B3,
0x0001B73F,0x0001B8CC,0x0001BA5C,0x0001BBEE,0x0001BD82,0x0001BF18,0x0001C0B0,0x0001C24B,
0x0001C3E7,0x0001C586,0x0001C727,0x0001C8CB,0x0001CA71,0x0001CC19,0x0001CDC3,0x0001CF70,
0x0001D11F,0x0001D2D1,0x0001D485,0x0001D63B,0x0001D7F4,0x0001D9B0,0x0001DB6E,0x0001DD2E,
0x0001DEF1,0x0001E0B7,0x0001E27F,0x0001E44A,0x0001E618,0x0001E7E8,0x0001E9BB,0x0001EB91,
0x0001ED6A,0x0001EF45,0x0001F123,0x0001F304,0x0001F4E8,0x0001F6CF,0x0001F8B9,0x0001FAA6,
0x0001FC96,0x0001FE88,0x0002007E,0x00020277,0x00020473,0x00020673,0x00020875,0x00020A7B,
0x00020C84,0x00020E90,0x0002109F,0x000212B2,0x000214C9,0x000216E2,0x000218FF,0x00021B20,
0x00021D44,0x00021F6C,0x00022197,0x000223C6,0x000225F9,0x0002282F,0x00022A69,0x00022CA7,
0x00022EE9,0x0002312F,0x00023378,0x000235C6,0x00023818,0x00023A6D,0x00023CC7,0x00023F25,
0x00024187,0x000243ED,0x00024658,0x000248C7,0x00024B3A,0x00024DB2,0x0002502E,0x000252AF,
0x00025534,0x000257BE,0x00025A4D,0x00025CE0,0x00025F78,0x00026215,0x000264B7,0x0002675E,
0x00026A0A,0x00026CBB,0x00026F71,0x0002722C,0x000274ED,0x000277B3,0x00027A7E,0x00027D4F,
0x00028026,0x00028302,0x000285E3,0x000288CB,0x00028BB8,0x00028EAB,0x000291A4,0x000294A3,
0x000297A8,0x00029AB3,0x00029DC4,0x0002A0DC,0x0002A3FB,0x0002A71F,0x0002AA4B,0x0002AD7D,
0x0002B0B5,0x0002B3F5,0x0002B73B,0x0002BA89,0x0002BDDD,0x0002C139,0x0002C49C,0x0002C807,
0x0002CB79,0x0002CEF2,0x0002D274,0x0002D5FD,0x0002D98E,0x0002DD27,0x0002E0C8,0x0002E471,
0x0002E823,0x0002EBDD,0x0002EFA0,0x0002F36C,0x0002F740,0x0002FB1D,0x0002FF04,0x000302F3,
0x000306EC,0x00030AEF,0x00030EFB,0x00031311,0x00031730,0x00031B5A,0x00031F8E,0x000323CD,
0x00032816,0x00032C69,0x000330C8,0x00033531,0x000339A6,0x00033E26,0x000342B1,0x00034748,
0x00034BEB,0x0003509A,0x00035556,0x00035A1D,0x00035EF2,0x000363D3,0x000368C2,0x00036DBD,
0x000372C6,0x000377DD,0x00037D02,0x00038235,0x00038776,0x00038CC6,0x00039225,0x00039793,
0x00039D11,0x0003A29E,0x0003A83B,0x0003ADE8,0x0003B3A6,0x0003B974,0x0003BF54,0x0003C545,
0x0003CB48,0x0003D15C,0x0003D784,0x0003DDBD,0x0003E40A,0x0003EA6A,0x0003F0DE,0x0003F766,
0x0003FE02,0x000404B4,0x00040B7A,0x00041256,0x00041949,0x00042051,0x00042771,0x00042EA8,
0x000435F7,0x00043D5E,0x000444DE,0x00044C78,0x0004542B,0x00045BF9,0x000463E1,0x00046BE5,
0x00047405,0x00047C42,0x0004849B,0x00048D13,0x000495A9,0x00049E5E,0x0004A733,0x0004B029,
0x0004B940,0x0004C279,0x0004CBD4,0x0004D553,0x0004DEF6,0x0004E8BF,0x0004F2AD,0x0004FCC3,
0x00050700,0x00051166,0x00051BF5,0x000526B0,0x00053196,0x00053CA8,0x000547E9,0x00055359,
0x00055EF9,0x00056ACB,0x000576CF,0x00058307,0x00058F75,0x00059C19,0x0005A8F6,0x0005B60C,
0x0005C35D,0x0005D0EC,0x0005DEB8,0x0005ECC5,0x0005FB14,0x000609A7,0x0006187F,0x0006279F,
0x00063709,0x000646BF,0x000656C4,0x00066718,0x000677C0,0x000688BD,0x00069A12,0x0006ABC2,
0x0006BDD0,0x0006D03E,0x0006E310,0x0006F649,0x000709EC,0x00071DFC,0x0007327E,0x00074776,
0x00075CE6,0x000772D4,0x00078944,0x0007A03A,0x0007B7BB,0x0007CFCD,0x0007E873,0x000801B5,
0x00081B98,0x00083621,0x00085158,0x00086D43,0x000889E9,0x0008A752,0x0008C586,0x0008E48C,
0x0009046E,0x00092535,0x000946EB,0x0009699A,0x00098D4D,0x0009B20F,0x0009D7EE,0x0009FEF7,
0x000A2736,0x000A50BC,0x000A7B97,0x000AA7D9,0x000AD593,0x000B04DA,0x000B35C0,0x000B685C,
0x000B9CC6,0x000BD317,0x000C0B69,0x000C45DA,0x000C828A,0x000CC199,0x000D032C,0x000D476C,
0x000D8E82,0x000DD89D,0x000E25EF,0x000E76B0,0x000ECB1B,0x000F2371,0x000F7FFA,0x000FE106,
0x001046EA,0x0010B206,0x001122C3,0x00119997,0x00121703,0x00129B97,0x001327F6,0x0013BCD5,
0x00145B00,0x0015035D,0x0015B6F1,0x001676E5,0x0017448D,0x0018216E,0x00190F4A,0x001A102B,
0x001B2672,0x001C54E8,0x001D9EDA,0x001F0835,0x002095AF,0x00224CFD,0x00243517,0x0026569A,
0x0028BC49,0x002B73C6,0x002E8E9A,0x003223B0,0x0036519A,0x003B4205,0x00412F4C,0x00486DB7,
0x00517BB6,0x005D1FF3,0x006CA58F,0x0082608E,0x00A2F8FD,0x00D94C4B,0x0145F2C4,0x028BE5EC,
};

signed short atan2s16(signed short y, signed short x)
{
        signed short i;
        unsigned int z;

        if(x == 0)
    {
        if (y == 0) i = 0;
        else        i = 1023;
    }
    else
    {
        z = ((y < 0) ? (0 - y) : (y)) * 65536u / ((x < 0) ? (0 - x) : (x));
        i = 0;
        if (atan2buf[i + 512] <= z) i += 512;
        if (atan2buf[i + 256] <= z) i += 256;
        if (atan2buf[i + 128] <= z) i += 128;
        if (atan2buf[i + 64] <= z) i += 64;
        if (atan2buf[i + 32] <= z) i += 32;
        if (atan2buf[i + 16] <= z) i += 16;
        if (atan2buf[i + 8] <= z) i += 8;
        if (atan2buf[i + 4] <= z) i += 4;
        if (atan2buf[i + 2] <= z) i += 2;
        if (atan2buf[i + 1] <= z) i += 1;
    }
    i = i * 900 / 1024;

    if (y < 0)
    {
        if (x < 0)
	       return  i;
        else       
           return -i;
    }
    else
    { 
        if (x < 0) 
		   return -i;
        else       return  i;
    }
}

int sin_m4(signed short x)
{int i;
	if(x<0)
		{
		x=-x;
	  i=- sinbuf[x];
	  return i;	
     }
  else
     	{
  	  i= sinbuf[x];
	    return i;	
  	  }
     	
	}

int cos_m4(signed short x)
{int i;
	if(x<0)
		x=-x;
	i=cosbuf[x];
	return i;	
	}

short  area[4][150]={0};
short tp[4]={0};
	
void add(int x,int y)
{
	int a;
	a=tp[x];
area[x][a]=y;
tp[x]++;

}

void del(int x,int y)
{
	int i,j,a;
	for(i=0;;i++)
	{
	if(area[x][i]==y)
	   break;
	}
	a=tp[x]-1;
	for(j=i;j<a;j++)
       area[x][j]=area[x][j+1];
tp[x]--;

}
	
/*
typedef struct{
	unsigned int x_pos;
	unsigned int y_pos;
	int x_velocity;
	int y_velocity;
}
tPosInitData;

typedef struct{
	unsigned int x_pos;
	unsigned int y_pos;
}
tPosDisplayData;

typedef struct{
	unsigned int time;
	unsigned int mode;
	unsigned char ucBallNum;
}
tSysPara;
*/

int flag = 0;
int counter=0;
int x[256],y[256];                     //maximum 256 balls
int vx[256],vy[256],r;
int ca[150][4]={0};
void PosDetectorProc(tPosDetecthandler *ps, tBallData *pb)
{ 
	
 	 int NUMBALL,RUNMODE,RUNTIME;
//   int x[256],y[256];                     //maximum 256 balls
//   int vx[256],vy[256],r;

   int i,j,k,h,d,a,b,bs,c,g,v,w;
   int tx,ty,xd,yd,xds,yds,xda,yda;
   int tkvx,tkvy,tjvx,tjvy;
   int z1,z2,z3,z4;
   int si,co,sit,cot;
   signed short agl;
   
   
   int ct[10][10]={0};

   
	NUMBALL=ps->ucBallNum; 
	RUNMODE=ps->mode;
  
	if(flag == 0)
	{
  for(i=0;i<NUMBALL;i++)
  { 
  	x[i] = (pb[i].x_pos)     *10000;         //initialize all ball pos and vel, then scale up
  	y[i] = (pb[i].y_pos)     *10000;
  	vx[i]= (pb[i].x_velocity)*100;
  	vy[i]= (pb[i].y_velocity)*100;  	      //unit for velocity mm/10ms
  
  	}
  flag = 1;
	}
  	   
    r=200000;
    a=625000000;                         //a=(4*r*r)>>8
    b=400000;
  
  if( RUNMODE == 1 )
    RUNTIME=4;                      //visualization mode (01), update display pos every 40 ms 
      
 if( RUNMODE == 2 )                  //computation mode(02), caculate the pos after given time
    RUNTIME=(ps->time)*100;         //convert from second to 10 ms
    

//  printf("    RUNTIME=%d\n",RUNTIME);

 for(i=0;i<RUNTIME;i++)
   { 

 
  
  	 for(j=0;j<NUMBALL;j++)             //update position for each ball after 1 ms
  	 {
 	  	x[j]=x[j]+vx[j];
    	y[j]=y[j]+vy[j];
    	
    	//area 0
    	if(x[j]<5200001 && y[j]<5200001)
         {
			if (ca[j][0]==0)
			{
			ca[j][0]=1;
			add(0,j);
			}
          }
        
		else
		{
			if (ca[j][0]==1)
			{
			ca[j][0]=0;
			del(0,j);
			}	
		}  
		
		//area1
		if(x[j]>4799999 && y[j]<5200001)
         {
			if (ca[j][1]==0)
			{
			ca[j][1]=1;
			add(1,j);
			}
          }
        
		else
		{
			if (ca[j][1]==1)
			{
			ca[j][1]=0;
			del(1,j);
			}	
		}  
		
		//area 2
		if(x[j]<5200001 && y[j]>4799999)
         {
			if (ca[j][2]==0)
			{
			ca[j][2]=1;
			add(2,j);
			}
          }
        
		else
		{
			if (ca[j][2]==1)
			{
			ca[j][2]=0;
			del(2,j);
			}	
		}  
		
		//area 3
		if(x[j]>4799999 && y[j]>4799999)
         {
			if (ca[j][3]==0)
			{
			ca[j][3]=1;
			add(3,j);
			}
          }
        
		else
		{
			if (ca[j][3]==1)
			{
			ca[j][3]=0;
			del(3,j);
			}	
		}  
  	 

    	 if(x[j]<=200000)                 //wall collision detect & speed update
	    {
	    	x[j]=400000-x[j];
         vx[j]=-vx[j];
//	     printf("left wall collision happen, i=%d,j=%d\n",i,j);
	    }
	    
	     if(x[j]>=9800000)
	    {   x[j]=19600000-x[j];
         vx[j]=-vx[j];
//	     printf("right wall collision happen, i=%d,j=%d\n",i,j);
	    }
	    
	     if(y[j]<=200000)
	    {   y[j]=400000-y[j];
         vy[j]=-vy[j];
//	     printf("bottom wall collision happen, i=%d,j=%d\n",i,j);
	    }
	    
	     if(y[j]>=9800000)
	    {    y[j]=19600000-y[j];
         vy[j]=-vy[j];
//     printf("top wall collision happen, i=%d,j=%d\n",i,j);
	    }
	    
  	 }
	    
	for(c=0;c<4;c++) 
	{	
 	  if(tp[c]>1)
 	    {
    	 	g=tp[c]-1;
    	 
 	    for(v=0;v<g;v++)
          for(w=v+1;w<tp[c];w++)
 	       {
 	       	j=area[c][v];
 	       	k=area[c][w];
 	       	
 	       	counter++;
 	  //ball collision detect & speed change    	
			xd=x[j]-x[k];
			if(xd<0)
				xda=-xd;
			else
			    xda=xd;
			    
			yd=y[j]-y[k];
			if(yd<0)
				yda=-yd;
			else
			    yda=yd;

			if(xda > b || yda > b)                         
    	  continue;
    	  
    	else 
    		{
    			xds=xd>>4;
    			yds=yd>>4;
    		
    			}
    			
    d = xds*xds + yds*yds;		
    		
    		if ( d <= a )
    	{
		   
	  // printf("x1=%d,y1=%d\n",x[j],y[j]);
    // printf("x2=%d,y2=%d\n",x[k],y[k]);
	  // printf("d=%d\n",d);	
    // printf("ball collision happen, i=%d,j=%d,k=%d,g=%d,tp%d=%d\n",i,j,k,g,c,tp[c]);
	

#ifdef BACKSTEP	
   	if(d<a)
  		{
    		  tkvx=vx[k]>>3;
    		  tkvy=vy[k]>>3;
    		  tjvx=vx[j]>>3;
    		  tjvy=vy[j]>>3;
    		
    		for(bs=1;;)            //bs means back step
    		{ 
    			x[k]=x[k]-tkvx;
    	        y[k]=y[k]-tkvy;
    	        x[j]=x[j]-tjvx;
    	        y[j]=y[j]-tjvy;
    	    
    			xd=x[j]-x[k];
    			yd=y[j]-y[k];
    			xds=xd>>4;
    			yds=yd>>4; 
    			
    		
    	    
    			if(( xds*xds + yds*yds ) > a)
    			{
//    			printf("bs=%d, d after bs=%d", bs, xds*xds + yds*yds );
    				break;
    			}
    			if(bs==7)
    				{
					bs=8;
//						printf("bs=%d, d after bs=%d", bs, xds*xds + yds*yds );
    	 			x[k]=x[k]-tkvx;
    	            y[k]=y[k]-tkvy;
    	            x[j]=x[j]-tjvx;
   	                y[j]=y[j]-tjvy;
    				break;
    		     	}
    		  bs++;
    		  		   			
    		}
  		}
  		else 
		  bs=0;
  		#endif
	     
	     if((x[j]-x[k])==0)
	        agl=900;
	     else        	
          agl=atan2s16(yds,xds);	
         
//		printf("agl=%d\n",agl);
	     
	     si=sin_m4(agl);
	     co=cos_m4(agl);
	    
	      if(agl>0)
		 {
		 sit = -co;
	     cot = si;
		 }
 		else
		 	{
 		 sit = co;
		 cot = - si;	
		 }
		 
//	     printf("si=%d,co=%d\n",si,co);
//	     printf("sit=%d,cot=%d\n",sit,cot);
//	     printf("vx[%d]=%d,vy[%d]=%d\n",k,vx[k],k,vy[k]);
//      	printf("bfvx[0]=%d,vy[0]=%d\n",vx[j],vy[j]);
//		     printf("bfvx[1]=%d,vy[1]=%d\n",vx[k],vy[k]);	
		 	     
	     z1=(vx[k]*co+vy[k]*si)>>16;  
	     z2=(vx[j]*cot+vy[j]*sit)>>16;
	     z3=(vx[j]*co+vy[j]*si)>>16;
	     z4=(vx[k]*cot+vy[k]*sit)>>16;
	     
	  //   printf("%d,%d,%d,%d",z1,z2,z3,z4);
	     
	     tx=(z1*co>>16)+(z2*cot>>16);   //temp x,temp y
	     ty=(z1*si>>16)+(z2*sit>>16);
	     
	     vx[k]=(z3*co>>16)+(z4*cot>>16);
	     vy[k]=(z3*si>>16)+(z4*sit>>16);
	     
	     vx[j]=tx;
	     vy[j]=ty;
	     


#ifdef BACKSTEP	
    if(bs!=0)
	    {  
	     tkvx=vx[k]>>3;
	     tkvy=vy[k]>>3;
	     tjvx=vx[j]>>3;
	     tjvy=vy[j]>>3;
	     
	     if(bs==8)
	     {
     	x[k]=x[k]+vx[k];
        y[k]=y[k]+vy[k];
        x[j]=x[j]+vx[j];
        y[j]=y[j]+vy[j];	
     	}
	     
	     for(h=0;h<bs;h++)
	     {  //printf("x[j]=%d,add bs tjvx=%d\n",x[j],tjvx);
	     	x[k]=x[k]+tkvx;
            y[k]=y[k]+tkvy;
            x[j]=x[j]+tjvx;
            y[j]=y[j]+tjvy;
	     }
      }
#endif 	
	     
//	     printf("afvx[0]=%d,vy[0]=%d\n",vx[j],vy[j]);
//	     printf("afvx[1]=%d,vy[1]=%d\n\n\n",vx[k],vy[k]);
	     
//	     printf("si=%d,co=%d,agl=%d\n",si,co,agl);
	     
//	     printf("vx[%d]=%d,vy[%d]=%d\n",j,vx[j],j,vy[j]);
//	     printf("vx[%d]=%d,vy[%d]=%d\n",k,vx[k],k,vy[k]);
	    }
	    
		
		 
			
	}    	 
  }
}
}
//    printf("x1=%d,y1=%d\n",x[0],y[0]);
//     printf("x2=%d,y2=%d\n",x[1],y[1]);
    
     for(i=0;i<NUMBALL;i++)         //scale down
     { 
    	 pb[i].x_pos = x[i]/10000;
    	 pb[i].y_pos = y[i]/10000;
  	  }

return;
  
   
   
}
   
void PosDetectorInit(tPosDetecthandler *ps, tBallData *pb)
{

}
   
 






