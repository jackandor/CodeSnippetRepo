// fir_test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdint.h>
#include <cmath>
#include <fstream>

struct arm_fir_decimate_instance_f32
{
    uint8_t M;
    uint16_t numTaps;
    float *pCoeffs;
    float *pState;
};

int arm_fir_decimate_init_f32(
    arm_fir_decimate_instance_f32 * S,
    uint16_t numTaps,
    uint8_t M,
    float * pCoeffs,
    float * pState,
    uint32_t blockSize)
{
    int status;

    if ((blockSize % M) != 0u)
    {
        status = -1;
    }
    else
    {
        S->numTaps = numTaps;
        S->pCoeffs = pCoeffs;
        memset(pState, 0, (numTaps + (blockSize - 1u)) * sizeof(float));
        S->pState = pState;
        S->M = M;
        status = 0;
    }
    return (status);

}

void arm_fir_decimate_f32(const arm_fir_decimate_instance_f32 *S, float *pSrc, float *pDst, uint32_t blockSize)
{
    float *pState = S->pState;
    float *pCoeffs = S->pCoeffs;
    float *pStateCurnt;
    float *px, *pb;
    float x0, c0;
    uint32_t numTaps = S->numTaps;
    uint32_t i, tapCnt, blkCnt, outBlockSize = blockSize / S->M;
    uint32_t blkCntN4;
    float *px0, *px1, *px2, *px3;
    float acc0, acc1, acc2, acc3;
    float x1, x2, x3;

    pStateCurnt = S->pState + (numTaps - 1u);

    blkCnt = outBlockSize / 4;
    blkCntN4 = outBlockSize - (4 * blkCnt);
    while (blkCnt > 0u)
    {
        i = 4 * S->M;
        do
        {
            *pStateCurnt++ = *pSrc++;
        } while (--i);

        acc0 = 0.0f;
        acc1 = 0.0f;
        acc2 = 0.0f;
        acc3 = 0.0f;

        px0 = pState;
        px1 = pState + S->M;
        px2 = pState + 2 * S->M;
        px3 = pState + 3 * S->M;

        pb = pCoeffs;

        tapCnt = numTaps >> 2;

        while (tapCnt > 0u)
        {
            c0 = *(pb++);

            x0 = *(px0++);
            x1 = *(px1++);
            x2 = *(px2++);
            x3 = *(px3++);

            acc0 += x0 * c0;
            acc1 += x1 * c0;
            acc2 += x2 * c0;
            acc3 += x3 * c0;

            c0 = *(pb++);

            x0 = *(px0++);
            x1 = *(px1++);
            x2 = *(px2++);
            x3 = *(px3++);

            acc0 += x0 * c0;
            acc1 += x1 * c0;
            acc2 += x2 * c0;
            acc3 += x3 * c0;

            c0 = *(pb++);

            x0 = *(px0++);
            x1 = *(px1++);
            x2 = *(px2++);
            x3 = *(px3++);

            acc0 += x0 * c0;
            acc1 += x1 * c0;
            acc2 += x2 * c0;
            acc3 += x3 * c0;

            c0 = *(pb++);

            x0 = *(px0++);
            x1 = *(px1++);
            x2 = *(px2++);
            x3 = *(px3++);

            acc0 += x0 * c0;
            acc1 += x1 * c0;
            acc2 += x2 * c0;
            acc3 += x3 * c0;

            tapCnt--;
        }
        tapCnt = numTaps % 0x4u;
        while (tapCnt > 0u)
        {
            c0 = *(pb++);

            x0 = *(px0++);
            x1 = *(px1++);
            x2 = *(px2++);
            x3 = *(px3++);

            acc0 += x0 * c0;
            acc1 += x1 * c0;
            acc2 += x2 * c0;
            acc3 += x3 * c0;

            tapCnt--;
        }
        pState = pState + 4 * S->M;

        *pDst++ = acc0;
        *pDst++ = acc1;
        *pDst++ = acc2;
        *pDst++ = acc3;

        blkCnt--;
    }
    while (blkCntN4 > 0u)
    {
        blkCntN4--;
    }
    pStateCurnt = S->pState;
    i = (numTaps - 1u) >> 2;
    while (i > 0u)
    {
        *pStateCurnt++ = *pState++;
        *pStateCurnt++ = *pState++;
        *pStateCurnt++ = *pState++;
        *pStateCurnt++ = *pState++;

        i--;
    }
    i = (numTaps - 1u) % 0x04u;
    while (i > 0u)
    {
        *pStateCurnt++ = *pState++;
        i--;
    }
}

float firCoeffs[67] = {
    -0.0004558710614208,-0.001515259712487,-0.001907842155507,-0.0004853859840942,
    0.001644843207092, 0.001372813937189,-0.001496967551878,-0.002649329694081,
    0.0007229776972018, 0.003974406995704, 0.000979818446907,-0.004874107841169,
    -0.003629997671552, 0.004746445826266, 0.006980939217685,-0.003004918507104,
    -0.01045007035187,-0.0008059755424497,  0.01316004505201, 0.006885038905929,
    -0.01396466232492, -0.01506773642595,  0.01153032183793,  0.02479537557988,
    -0.004304580258341, -0.03515572502989,-0.009847514571588,  0.04500345747829,
    0.03533784553375, -0.05313704972584, -0.08785759257471,   0.0585002451498,
    0.312008644261,   0.4396264960867,    0.312008644261,   0.0585002451498,
    -0.08785759257471, -0.05313704972584,  0.03533784553375,  0.04500345747829,
    -0.009847514571588, -0.03515572502989,-0.004304580258341,  0.02479537557988,
    0.01153032183793, -0.01506773642595, -0.01396466232492, 0.006885038905929,
    0.01316004505201,-0.0008059755424497, -0.01045007035187,-0.003004918507104,
    0.006980939217685, 0.004746445826266,-0.003629997671552,-0.004874107841169,
    0.000979818446907, 0.003974406995704,0.0007229776972018,-0.002649329694081,
    -0.001496967551878, 0.001372813937189, 0.001644843207092,-0.0004853859840942,
    -0.001907842155507,-0.001515259712487,-0.0004558710614208
};

float firCoeffs2[168] = {
    -4.926677502e-05,-0.0002141200821,-0.0003268258297,-0.0005153834936,-0.0006712941104,
    -0.0007728089113,-0.0007671668427,-0.0006299804663,-0.0003633254091,-7.84396525e-06,
    0.00036238553,0.0006542136543,0.0007798916195,0.0006847510813,0.0003698830551,
    -9.856536053e-05,-0.0005988603807,-0.0009821938584,-0.001114490442,-0.0009203490335,
    -0.0004151361645,0.0002866902214,0.0009934123373, 0.001485495945,  0.00158097269,
    0.001197942533,0.0003937910951,-0.0006343726418,-0.001593664172,-0.002175598405,
    -0.002151526278,-0.001454785117,-0.0002218869486, 0.001225651475, 0.002460420132,
    0.003071156563, 0.002794308821, 0.001614463166,-0.0002020553366,  -0.0021587417,
    -0.003658897476,-0.004182051402, -0.00345627428,-0.001571194385, 0.001010143431,
    0.003555519041, 0.005264014471, 0.005509688519, 0.004056830425, 0.001176284044,
    -0.00238458626,-0.005584118422,-0.007380189374,-0.007055993658,-0.004479289055,
    -0.0002077529789, 0.004608809482, 0.008521019481,  0.01019727904, 0.008851042017,
    0.004547505639,-0.001717750798,-0.008216846734, -0.01293530222, -0.01415767148,
    -0.01103226189,-0.003943414893, 0.005442429334,  0.01450802758,  0.02034206875,
    0.0205695685,  0.01413918845, 0.001841875724, -0.01361347362, -0.02803492732,
    -0.03662984818, -0.03514569253, -0.02099788748, 0.005903118756,  0.04285569489,
    0.08470995724,   0.1248044372,   0.1563003957,   0.1736141741,   0.1736141741,
    0.1563003957,   0.1248044372,  0.08470995724,  0.04285569489, 0.005903118756,
    -0.02099788748, -0.03514569253, -0.03662984818, -0.02803492732, -0.01361347362,
    0.001841875724,  0.01413918845,   0.0205695685,  0.02034206875,  0.01450802758,
    0.005442429334,-0.003943414893, -0.01103226189, -0.01415767148, -0.01293530222,
    -0.008216846734,-0.001717750798, 0.004547505639, 0.008851042017,  0.01019727904,
    0.008521019481, 0.004608809482,-0.0002077529789,-0.004479289055,-0.007055993658,
    -0.007380189374,-0.005584118422, -0.00238458626, 0.001176284044, 0.004056830425,
    0.005509688519, 0.005264014471, 0.003555519041, 0.001010143431,-0.001571194385,
    -0.00345627428,-0.004182051402,-0.003658897476,  -0.0021587417,-0.0002020553366,
    0.001614463166, 0.002794308821, 0.003071156563, 0.002460420132, 0.001225651475,
    -0.0002218869486,-0.001454785117,-0.002151526278,-0.002175598405,-0.001593664172,
    -0.0006343726418,0.0003937910951, 0.001197942533,  0.00158097269, 0.001485495945,
    0.0009934123373,0.0002866902214,-0.0004151361645,-0.0009203490335,-0.001114490442,
    -0.0009821938584,-0.0005988603807,-9.856536053e-05,0.0003698830551,0.0006847510813,
    0.0007798916195,0.0006542136543,  0.00036238553,-7.84396525e-06,-0.0003633254091,
    -0.0006299804663,-0.0007671668427,-0.0007728089113,-0.0006712941104,-0.0005153834936,
    -0.0003268258297,-0.0002141200821,-4.926677502e-05
};


#define N 16384
#define BLOCK_SIZE            32
#define NO (N + BLOCK_SIZE * 6)
#define NUM_TAPS              67

static float firState[BLOCK_SIZE + NUM_TAPS - 1];
static float waveform[BLOCK_SIZE];
static float waveform2[16384 * 2];

float f1 = 2600;
float f2 = 100;
float f3 = 15000;
float Fs = 100000;
#define PI 3.1415926

class FIR_DECIMATE_INSTANCE
{
public:
    FIR_DECIMATE_INSTANCE(uint16_t numTaps, uint8_t M, float *pCoeffs, uint32_t _blockSize = BLOCK_SIZE) : blockSize(_blockSize), pos(0)
    {
        pState = new float[blockSize + numTaps - 1];
        input = new float[blockSize];
        output = new float[blockSize / M];
        arm_fir_decimate_init_f32(&S, numTaps, M, pCoeffs, pState, blockSize);
    }

    ~FIR_DECIMATE_INSTANCE()
    {
        delete[] pState;
    }

    bool Input(float *i, int n)
    {
        memcpy(&input[pos], i, sizeof(float) * n);
        pos += n;
        if (pos == blockSize) {
            arm_fir_decimate_f32(&S, input, output, blockSize);
            pos = 0;
            return true;
        }
        return false;
    }

    float *Output(int &n)
    {
        n = blockSize / S.M;
        return output;
    }
private:
    arm_fir_decimate_instance_f32 S;
    float *pState;
    uint32_t blockSize;
    float *input, *output;
    int pos;
};

int main()
{
    float s = 0.0;
    int cnt = 0;
    FIR_DECIMATE_INSTANCE fdi1(67, 2, firCoeffs), fdi2(67, 2, firCoeffs), fdi3(168, 5, firCoeffs2, 160u);
    while (cnt < 16384)
    {
        for (int i = 0; i < BLOCK_SIZE; i++)
            waveform[i] = sin(2 * PI * f1 * (s + (float)i / Fs)) + sin(2 * PI * f2 * (s + (float)i / Fs)) + sin(2 * PI * f3 * (s + (float)i / Fs));
        s += (float)BLOCK_SIZE / (float)Fs;
        if (fdi1.Input(waveform, BLOCK_SIZE)) { //20kHz
            int n;
            float *o = fdi1.Output(n);
            if (fdi2.Input(o, n)) { //10kHz
                float *o2 = fdi2.Output(n);
                if (fdi3.Input(o2, n)) { // 2kHz
                    float *o3 = fdi3.Output(n);
                    memcpy(&waveform2[cnt], o3, n * sizeof(float));
                    cnt += n;
                }
            }
        }
    }
    std::ofstream ofs("D:\\work\\matlab\\data.txt");
    for (int i = 0; i < 16384; i++)
        ofs << waveform2[i] << std::endl;
    return 0;
}

