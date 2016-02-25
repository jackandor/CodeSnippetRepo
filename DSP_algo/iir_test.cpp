// iir_test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdint.h>
#include <cmath>
#include <fstream>

struct arm_biquad_casd_df1_inst_f32
{
    uint32_t numStages;
    double *pState;
    double *pCoeffs;
};

void arm_biquad_cascade_df1_init_f64(arm_biquad_casd_df1_inst_f32 *S, uint8_t numStages, double *pCoeffs, double *pState)
{
    S->numStages = numStages;
    S->pCoeffs = pCoeffs;
    memset(pState, 0, (4u * (uint32_t)numStages)  * sizeof(double));
    S->pState = pState;
}

void arm_biquad_cascade_df1_f64(arm_biquad_casd_df1_inst_f32 *S, double *pSrc, double *pDst, uint32_t blockSize)
{
    double *pIn = pSrc;                         /*  source pointer            */
    double *pOut = pDst;                        /*  destination pointer       */
    double *pState = S->pState;                 /*  pState pointer            */
    double *pCoeffs = S->pCoeffs;               /*  coefficient pointer       */
    double acc;                                 /*  Simulates the accumulator */
    double b0, b1, b2, a1, a2;                  /*  Filter coefficients       */
    double Xn1, Xn2, Yn1, Yn2;                  /*  Filter pState variables   */
    double Xn;                                  /*  temporary input           */
    uint32_t sample, stage = S->numStages;         /*  loop counters             */

                                                   /* Run the below code for Cortex-M4 and Cortex-M3 */

    do
    {
        /* Reading the coefficients */
        b0 = *pCoeffs++;
        b1 = *pCoeffs++;
        b2 = *pCoeffs++;
        a1 = *pCoeffs++;
        a2 = *pCoeffs++;

        /* Reading the pState values */
        Xn1 = pState[0];
        Xn2 = pState[1];
        Yn1 = pState[2];
        Yn2 = pState[3];

        /* Apply loop unrolling and compute 4 output values simultaneously. */
        /*      The variable acc hold output values that are being computed:
        *
        *    acc =  b0 * x[n] + b1 * x[n-1] + b2 * x[n-2] + a1 * y[n-1]   + a2 * y[n-2]
        *    acc =  b0 * x[n] + b1 * x[n-1] + b2 * x[n-2] + a1 * y[n-1]   + a2 * y[n-2]
        *    acc =  b0 * x[n] + b1 * x[n-1] + b2 * x[n-2] + a1 * y[n-1]   + a2 * y[n-2]
        *    acc =  b0 * x[n] + b1 * x[n-1] + b2 * x[n-2] + a1 * y[n-1]   + a2 * y[n-2]
        */

        sample = blockSize >> 2u;

        /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.
        ** a second loop below computes the remaining 1 to 3 samples. */
        while (sample > 0u)
        {
            /* Read the first input */
            Xn = *pIn++;

            /* acc =  b0 * x[n] + b1 * x[n-1] + b2 * x[n-2] + a1 * y[n-1] + a2 * y[n-2] */
            Yn2 = (b0 * Xn) + (b1 * Xn1) + (b2 * Xn2) + (a1 * Yn1) + (a2 * Yn2);

            /* Store the result in the accumulator in the destination buffer. */
            *pOut++ = Yn2;

            /* Every time after the output is computed state should be updated. */
            /* The states should be updated as:  */
            /* Xn2 = Xn1    */
            /* Xn1 = Xn     */
            /* Yn2 = Yn1    */
            /* Yn1 = acc   */

            /* Read the second input */
            Xn2 = *pIn++;

            /* acc =  b0 * x[n] + b1 * x[n-1] + b2 * x[n-2] + a1 * y[n-1] + a2 * y[n-2] */
            Yn1 = (b0 * Xn2) + (b1 * Xn) + (b2 * Xn1) + (a1 * Yn2) + (a2 * Yn1);

            /* Store the result in the accumulator in the destination buffer. */
            *pOut++ = Yn1;

            /* Every time after the output is computed state should be updated. */
            /* The states should be updated as:  */
            /* Xn2 = Xn1    */
            /* Xn1 = Xn     */
            /* Yn2 = Yn1    */
            /* Yn1 = acc   */

            /* Read the third input */
            Xn1 = *pIn++;

            /* acc =  b0 * x[n] + b1 * x[n-1] + b2 * x[n-2] + a1 * y[n-1] + a2 * y[n-2] */
            Yn2 = (b0 * Xn1) + (b1 * Xn2) + (b2 * Xn) + (a1 * Yn1) + (a2 * Yn2);

            /* Store the result in the accumulator in the destination buffer. */
            *pOut++ = Yn2;

            /* Every time after the output is computed state should be updated. */
            /* The states should be updated as: */
            /* Xn2 = Xn1    */
            /* Xn1 = Xn     */
            /* Yn2 = Yn1    */
            /* Yn1 = acc   */

            /* Read the forth input */
            Xn = *pIn++;

            /* acc =  b0 * x[n] + b1 * x[n-1] + b2 * x[n-2] + a1 * y[n-1] + a2 * y[n-2] */
            Yn1 = (b0 * Xn) + (b1 * Xn1) + (b2 * Xn2) + (a1 * Yn2) + (a2 * Yn1);

            /* Store the result in the accumulator in the destination buffer. */
            *pOut++ = Yn1;

            /* Every time after the output is computed state should be updated. */
            /* The states should be updated as:  */
            /* Xn2 = Xn1    */
            /* Xn1 = Xn     */
            /* Yn2 = Yn1    */
            /* Yn1 = acc   */
            Xn2 = Xn1;
            Xn1 = Xn;

            /* decrement the loop counter */
            sample--;

        }

        /* If the blockSize is not a multiple of 4, compute any remaining output samples here.
        ** No loop unrolling is used. */
        sample = blockSize & 0x3u;

        while (sample > 0u)
        {
            /* Read the input */
            Xn = *pIn++;

            /* acc =  b0 * x[n] + b1 * x[n-1] + b2 * x[n-2] + a1 * y[n-1] + a2 * y[n-2] */
            acc = (b0 * Xn) + (b1 * Xn1) + (b2 * Xn2) + (a1 * Yn1) + (a2 * Yn2);

            /* Store the result in the accumulator in the destination buffer. */
            *pOut++ = acc;

            /* Every time after the output is computed state should be updated. */
            /* The states should be updated as:    */
            /* Xn2 = Xn1    */
            /* Xn1 = Xn     */
            /* Yn2 = Yn1    */
            /* Yn1 = acc   */
            Xn2 = Xn1;
            Xn1 = Xn;
            Yn2 = Yn1;
            Yn1 = acc;

            /* decrement the loop counter */
            sample--;

        }

        /*  Store the updated state variables back into the pState array */
        *pState++ = Xn1;
        *pState++ = Xn2;
        *pState++ = Yn1;
        *pState++ = Yn2;

        /*  The first stage goes from the input buffer to the output buffer. */
        /*  Subsequent numStages  occur in-place in the output buffer */
        pIn = pDst;

        /* Reset the output pointer */
        pOut = pDst;

        /* decrement the loop counter */
        stage--;

    } while (stage > 0u);
}

double iirCoeffs[] = {
    1, -1.97719323207566,    1, 1.9802369565674973,   -0.99554757578230135,
    1, -1.9748153453156747,  1, 1.9712741991857123,   -0.98527183842069732,
    1, -1.96779467503603,    1, 1.9598791975932992,   -0.97116707701245208,
    1, -1.9458324884454772,  1, 1.9454279089853921,   -0.95272940230598191,
    1, -1.8281501258257986,  1, 1.9313224937577653,   -0.93454637856878398,
    1, 1,                    0, 0.96255430172189049,  0.0
};


#define NUM_STAGES 6
#define UNIT_SIZE 1024
#define UNIT_SIZE_X2 (UNIT_SIZE * 2)
#define UNIT_SIZE_X3 (UNIT_SIZE * 3)
#define UNIT_SIZE_X4 (UNIT_SIZE * 4)

#define N 16384
#define PI 3.14159265
#define Fs 102400.0
#define DIV 40
#define f1 18000.0
#define f2 25.0

double reversal1[UNIT_SIZE_X4 + 2];
double reversal2[UNIT_SIZE_X4 + 2];
double reversal3[UNIT_SIZE_X4 + 2];
double iirState1[4 * NUM_STAGES];
double iirState2[4 * NUM_STAGES];
double iirState3[4 * NUM_STAGES];
double waveform[2 * N];

void PrintPos(char *s, double *p)
{
    printf("%s", s);
    if (reversal1 > p || p <= reversal1 + UNIT_SIZE_X4 + 1)
        printf("reversal1[%d]", p - reversal1 - 1);
    else if (reversal2 > p || p <= reversal2 + UNIT_SIZE_X4 + 1)
        printf("reversal2[%d]", p - reversal2 - 1);
    else if (reversal3 > p || p <= reversal3 + UNIT_SIZE_X4 + 1)
        printf("reversal3[%d]", p - reversal3 - 1);
    printf("\n");
}

#define FILTER

#define b0 pCoeffs[0]
#define b1 pCoeffs[1]
#define b2 pCoeffs[2]
#define a1 pCoeffs[3]
#define a2 pCoeffs[4]

#define Xn (*pIn)
#define Xn1 pState[0]
#define Xn2 pState[1]
#define Yn1 pState[2]
#define Yn2 pState[3]


void filter(uint32_t stage, double *pCoeffs, double *pState, double *pIn, double *pOut)
{
    do {
        double acc = (b0 * Xn) + (b1 * Xn1) + (b2 * Xn2) + (a1 * Yn1) + (a2 * Yn2);


        Xn2 = Xn1;
        Xn1 = Xn;
        Yn2 = Yn1;
        Yn1 = acc;

        *pOut = acc;

        pCoeffs += 5;
        pState += 4;

        pIn = pOut;

        stage--;
    } while (stage > 0u);
}

int main()
{
#if 0
    for (int i = 0; i < M; i++) {
        waveform[i] = cos(2 * PI * f1 * ((double)i / Fs) + 1.0 / 2.0 * PI) + cos(2 * PI * f2 * ((double)i / Fs) + 3.0 / 4.0 * PI);
    }

    arm_biquad_casd_df1_inst_f32 S;
    arm_biquad_cascade_df1_init_f64(&S, NUM_STAGES, iirCoeffs, iirState);
    arm_biquad_cascade_df1_f64(&S, waveform, waveform2, N);
    for (int i = 0; i < N; i++) {
        waveform2[i] *= (0.72984788802843714 * 0.69461685976910936 * 0.62651101204385007 * 1.0069358880002877 * 0.0034819675793526421);
        waveform3[N - i - 1] = waveform2[i];
    }
#if 1
    arm_biquad_cascade_df1_init_f64(&S, NUM_STAGES, iirCoeffs, iirState);
    arm_biquad_cascade_df1_f64(&S, waveform3, waveform4, N);
    for (int i = 0; i < N; i++) {
        waveform4[i] *= (0.72984788802843714 * 0.69461685976910936 * 0.62651101204385007 * 1.0069358880002877 * 0.0034819675793526421);
        waveform5[N - i - 1] = waveform4[i];
    }
#endif
    for (int i = 4096; i < 4096 + 8192; i++)
        waveform6[i - 4096] = waveform5[i];
#if 1
    arm_biquad_cascade_df1_init_f64(&S, NUM_STAGES, iirCoeffs, iirState);
    arm_biquad_cascade_df1_f64(&S, waveform + 8192, waveform2, N);
    for (int i = 0; i < N; i++) {
        waveform2[i] *= (0.72984788802843714 * 0.69461685976910936 * 0.62651101204385007 * 1.0069358880002877 * 0.0034819675793526421);
        waveform3[N - i - 1] = waveform2[i];
    }
#if 1
    arm_biquad_cascade_df1_init_f64(&S, NUM_STAGES, iirCoeffs, iirState);
    arm_biquad_cascade_df1_f64(&S, waveform3, waveform4, N);
    for (int i = 0; i < N; i++) {
        waveform4[i] *= (0.72984788802843714 * 0.69461685976910936 * 0.62651101204385007 * 1.0069358880002877 * 0.0034819675793526421);
        waveform5[N - i - 1] = waveform4[i];
    }
#endif
    for (int i = 4096; i < 4096 + 8192; i++)
        waveform6[i + 4096] = waveform5[i];
#endif
#endif
#if 0
    int cnt = -UNIT_SIZE_X4;
    uint32_t skip = 0;
    uint32_t div = 5;
    arm_biquad_casd_df1_inst_f32 s1, s2, s3;
    arm_biquad_casd_df1_inst_f32 *S1 = &s1;
    arm_biquad_casd_df1_inst_f32 *S2 = &s2;
    arm_biquad_casd_df1_inst_f32 *S3 = &s3;
    arm_biquad_cascade_df1_init_f64(S1, NUM_STAGES, iirCoeffs, iirState1);
    arm_biquad_cascade_df1_init_f64(S2, NUM_STAGES, iirCoeffs, iirState2);
    arm_biquad_cascade_df1_init_f64(S3, NUM_STAGES, iirCoeffs, iirState3);
    double in = 0;
    double tmp[2];
    double *p1 = reversal1 + 1;
    double *p2 = reversal2 + 1 + UNIT_SIZE_X2;
    double *p3 = reversal3 + 1 + UNIT_SIZE_X4;
    double *end = p1 + UNIT_SIZE_X4;
    double *output = waveform;
    double *o = (double *)&tmp[2];
    int head = UNIT_SIZE - 1 - UNIT_SIZE_X4;//-12289;
    int tail = UNIT_SIZE_X3 - 1 - UNIT_SIZE_X4;//-4097;
    while (cnt < N * 2 + UNIT_SIZE_X2)
    {
#ifdef FILTER
        in = cos(2 * PI * f2 * ((double)cnt / Fs) + 3.0 / 4.0 * PI);
#endif
#if 0
        *output++ = in;
#else
        if (cnt >= 0) {
            int i = (cnt / UNIT_SIZE_X2) * UNIT_SIZE_X2 + UNIT_SIZE_X4 - (cnt % UNIT_SIZE_X2) * 2 - 1;
            if (i <= head)
                o = &tmp[2];
            else if (i > tail)
                o = &tmp[2];
            else {
                o = &output[i - UNIT_SIZE + 1];
            }
        }
#ifdef FILTER
        arm_biquad_cascade_df1_f64(S1, p1++, --o, 1);
#else
        *(--o) = *p1++;
#endif
#ifdef FILTER
        arm_biquad_cascade_df1_f64(S1, p1++, --o, 1);
#else
        *(--o) = *p1++;
#endif
        if (cnt < 0)
            o += 2;
#ifdef FILTER
        arm_biquad_cascade_df1_f64(S2, &in, --p2, 1);
#else
        *(--p2) = in;
#endif
#ifdef FILTER
        arm_biquad_cascade_df1_f64(S3, &in, --p3, 1);
#else
        *(--p3) = in;
#endif
        if (p1 == end) {
            double *tmp = p1;
            p1 = p2;
            p2 = p3;
            p3 = tmp;

            end = p1 + UNIT_SIZE_X4;

            arm_biquad_casd_df1_inst_f32 *S = S1;
            S1 = S2;
            S2 = S3;
            S3 = S; 
            memset(S1->pState, 0, (4u * (uint32_t)NUM_STAGES)  * sizeof(double));
            memset(S3->pState, 0, (4u * (uint32_t)NUM_STAGES)  * sizeof(double));
            head += UNIT_SIZE_X2;
            tail += UNIT_SIZE_X2;
        }
#endif
        cnt++;
#ifndef FILTER
        in = (int)in + 1;
#endif
    }
#endif
#if 0
    for (int i = 0; i < N; i++) {
        waveform[i] = cos(2 * PI * f1 * ((double)i / Fs) + 1.0 / 2.0 * PI) + cos(2 * PI * f2 * ((double)i / Fs) + 3.0 / 4.0 * PI);
    }

    double *in = waveform;
    double *out = waveform3;

    arm_biquad_casd_df1_inst_f32 S;
    arm_biquad_cascade_df1_init_f64(&S, NUM_STAGES, iirCoeffs, iirState1);
    for (int i = 0; i < N; i++) {
        arm_biquad_cascade_df1_f64(&S, waveform + i, waveform2 + i, 1);
        filter(NUM_STAGES, iirCoeffs, iirState2, in++, out++);
        if (*(waveform2 + i) != *(out - 1))
            printf("error");
    }
#endif
    int cnt = -UNIT_SIZE_X4;
    double *IIRState1 = iirState1;
    double *IIRState2 = iirState2;
    double *IIRState3 = iirState3;
    memset(IIRState1, 0, (4u * (uint32_t)NUM_STAGES)  * sizeof(double));
    memset(IIRState2, 0, (4u * (uint32_t)NUM_STAGES)  * sizeof(double));
    memset(IIRState3, 0, (4u * (uint32_t)NUM_STAGES)  * sizeof(double));
    double in = 0;
    double tmp;
    double *p1 = reversal1 + 1;
    double *p2 = reversal2 + 1 + UNIT_SIZE_X2;
    double *p3 = reversal3 + 1 + UNIT_SIZE_X4;
    double *end = p2 - UNIT_SIZE_X2;
    double *output = waveform;
    int head = UNIT_SIZE - 1 - UNIT_SIZE_X4;//-12289;
    int tail = UNIT_SIZE_X3 - 1 - UNIT_SIZE_X4;//-4097;
    int count = 0;
    int div = DIV;
    while (cnt < (((N + UNIT_SIZE) * div) + UNIT_SIZE) / UNIT_SIZE_X2 * UNIT_SIZE_X2 - UNIT_SIZE / 2)
    {
        in = cos(2 * PI * f2 * ((double)cnt / Fs) + 3.0 / 4.0 * PI);
        if (cnt >= 0) {
            int i = (cnt / UNIT_SIZE_X2) * UNIT_SIZE_X2 + UNIT_SIZE_X4 - (cnt % UNIT_SIZE_X2) * 2 - 1;
            int pos = i - UNIT_SIZE;
            if ((i <= head) || (i > tail) || (pos > (N * div))) {
                filter(NUM_STAGES, iirCoeffs, IIRState1, p1++, &tmp);
                filter(NUM_STAGES, iirCoeffs, IIRState1, p1++, &tmp);
            }
            else {
                if (pos % div)
                    filter(NUM_STAGES, iirCoeffs, IIRState1, p1++, &tmp);
                else
                    filter(NUM_STAGES, iirCoeffs, IIRState1, p1++, &output[pos / div]);
                pos--;
                if (pos % div)
                    filter(NUM_STAGES, iirCoeffs, IIRState1, p1++, &tmp);
                else
                    filter(NUM_STAGES, iirCoeffs, IIRState1, p1++, &output[pos / div]);
            }
        }
        else
            p1 += 2;
        filter(NUM_STAGES, iirCoeffs, IIRState2, &in, --p2);
        filter(NUM_STAGES, iirCoeffs, IIRState3, &in, --p3);
        if (p2 == end) {
            double *tmp = p1;
            p1 = p2;
            p2 = p3;
            p3 = tmp;

            end = p2 - UNIT_SIZE_X2;

            double *IIRState = IIRState1;
            IIRState1 = IIRState2;
            IIRState2 = IIRState3;
            IIRState3 = IIRState;
            memset(IIRState1, 0, (4u * (uint32_t)NUM_STAGES)  * sizeof(double));
            memset(IIRState3, 0, (4u * (uint32_t)NUM_STAGES)  * sizeof(double));

            head += UNIT_SIZE_X2;
            tail += UNIT_SIZE_X2;
        }
        cnt++;
    }
    std::ofstream ofs("D:\\work\\matlab\\data1.txt");
    for (int i = 0; i < N; i++) {
        waveform[i] *= 0.20848933281704451 * 0.55580032406388613 * 0.35049729918207673 * 0.13479469724652104 * 0.018759890436406867 * 0.060286089020554802;
        waveform[i] *= 0.20848933281704451 * 0.55580032406388613 * 0.35049729918207673 * 0.13479469724652104 * 0.018759890436406867 * 0.060286089020554802;
        ofs << waveform[i] << "," << std::endl;
    }
    return 0;
}

