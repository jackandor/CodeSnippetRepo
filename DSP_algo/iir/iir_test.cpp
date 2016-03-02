// iir_test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdint.h>
#include <cmath>
#include <fstream>

#include "filter.h"

//double iirCoeffs[] = {
//    1.0, -1.9999427795410156, 1.0, 1.9997379779815674, -0.99977630376815796,
//    1.0, -1.9999368190765381, 1.0, 1.9992214441299438, -0.99925673007965088,
//    1.0, -1.999919056892395,  1.0, 1.9985073804855347, -0.99853599071502686,
//    1.0, -1.9998630285263062, 1.0, 1.997562050819397,  -0.99758070707321167,
//    1.0, -1.9995522499084473, 1.0, 1.9966154098510742, -0.99662375450134277,
//    1.0,  1.0,                0.0, 0.99809616804122925, 0.0
//};

#if 1
double iirCoeffs[] = {
    1, -1.3413187922157477,   1,  1.59181078831514,    -0.91580877121824544,
    1, -1.9995207910952515,   1,  1.9939425010832523,  -0.99496249633589418,
    1, -1.1354356904308402,   1,  1.6008370080136465,  -0.84509927601672963,
    1, -1.9996573918683256,   1,  1.9864272788502599,  -0.98775153665894433,
    1, -0.52792359046561554,  1,  1.6319972014935724,  -0.77304588170866373,
    1, -1.9998377628730877,   1,  1.9669745856711891,  -0.96921900662826344,
    1,  1.268720683394057,    1,  1.9004285476969371,  -0.90690281443636456,
    1, -1.9999788632867248,   1,  1.7095333324641129,  -0.75769334644959074,
    1, -1.4111371695433728,   1,  1.6051878070511088,  -0.97362033910177881,
    1, -1.9994527826026185,   1,  1.9976925046885761,  -0.99860938577822234
};

double scale = 0.77027591694975983 * 0.77027591694975983 * 0.69532570427944784 * 0.69532570427944784 * 0.55596277113749271 * 0.55596277113749271 * 0.52036107416393829 * 0.52036107416393829 * 0.05464814569159835 * 0.05464814569159835;

#define NUM_STAGES 10
#define UNIT_SIZE 2048
#define UNIT_SIZE_X2 (UNIT_SIZE * 2)
#define UNIT_SIZE_X3 (UNIT_SIZE * 3)
#define UNIT_SIZE_X4 (UNIT_SIZE * 4)


#define N 1392128 * 2
#define PI 3.14159265f
#define Fs 102400.0f
#define DIV 1
#define f1 5000.0f
#define f2 200.0f

double reversal[3 * UNIT_SIZE_X4];
double iirState1[4 * NUM_STAGES];
double iirState2[4 * NUM_STAGES];
double iirState3[4 * NUM_STAGES];

double iirCoeffs2[] = {
    1, -1.9985702868507862,  1,  1.997922615889667,    -0.99888232583358838,
    1, -1.998420336382378,   1,  1.9954090691083508,   -0.99628961794824999,
    1, -1.9979766273668347,  1,  1.9919885655034351,   -0.99270200300244948,
    1, -1.9965790575039211,  1,  1.9874978862333781,   -0.98796218427537053,
    1, -1.9888348788020944,  1,  1.9830251948191635,   -0.98323143360034815,
    1,  1,                   0,  0.99051637073821275,  0
};

double scale2 = 0.20847112532324794 * 0.55742806891045593 * 0.35259817560079521 * 0.13572225856595097 * 0.01847170107058458 * 0.01526826696267757;

#define NUM_STAGES2 6
#define UNIT_SIZE2 1024
#define UNIT_SIZE2_X2 (UNIT_SIZE2 * 2)
#define UNIT_SIZE2_X3 (UNIT_SIZE2 * 3)
#define UNIT_SIZE2_X4 (UNIT_SIZE2 * 4)

#define N2 16384
#define DIV2 80
#define f2 200.0f

double reversal1[3 * UNIT_SIZE2_X4];
double iirState11[4 * NUM_STAGES2];
double iirState12[4 * NUM_STAGES2];
double iirState13[4 * NUM_STAGES2];
#endif
double waveform[2 * N2];

int main()
{
    FilterStruct F2;
    InitFilterStruct(&F2, NUM_STAGES2, iirCoeffs2, scale2, 0, iirState11, iirState12, iirState13, UNIT_SIZE2, N2, DIV2, reversal1, waveform, NULL);
    FilterStruct F;
    InitFilterStruct(&F, NUM_STAGES, iirCoeffs, scale, 1, iirState1, iirState2, iirState3, UNIT_SIZE, N2 * 1000, DIV, reversal, NULL, &F2);
    bool done = false;
    double in;
    while (!done)
    {
        in = cos(2 * PI * f1 * ((double)(F.cnt) / Fs) + 3.0 / 4.0 * PI) * (1 + cos(2 * PI * f2 * ((double)(F.cnt) / Fs) + 3.0 / 4.0 * PI));
        //in = cos(2 * PI * f2 * ((double)(F2.cnt) / Fs) + 3.0 / 4.0 * PI);
        done = Filter(&F, in);
    }
    std::ofstream ofs("D:\\work\\matlab\\data1.txt");
    for (int i = 0; i < N2; i++) {
        ofs << waveform[i] << "," << std::endl;
    }
    ofs.close();
    return 0;
}

