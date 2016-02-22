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
    1, 0.023284929919725805, 1, 0.6960669996516633,  -0.84973189572218899,
    1, 0.36895643891338603,  1, 0.8461543697739129,  -0.7128337497705779,
    1, 1.0304293270546507,   1, 1.0582455862616562,  -0.54676504484478561,
    1, 1.8421701394305356,   1, 1.2325373353745148,  -0.41622696852602936,
    1, -0.11649687319924533, 1, 0.63360452379601617, -0.95334790177622908
};

#define NUM_STAGES 5
#define UNIT_SIZE 4096
#define UNIT_SIZE_X2 (UNIT_SIZE * 2)
#define UNIT_SIZE_X3 (UNIT_SIZE * 3)
#define UNIT_SIZE_X4 (UNIT_SIZE * 4)
#define N 16384
double waveform[UNIT_SIZE * 100];
double reversal1[UNIT_SIZE_X4 + 2];
double reversal2[UNIT_SIZE_X4 + 2];
double reversal3[UNIT_SIZE_X4 + 2];
double iirState1[4 * NUM_STAGES];
double iirState2[4 * NUM_STAGES];
double iirState3[4 * NUM_STAGES];
double iirState4[4 * NUM_STAGES];

#define PI 3.14159265
#define Fs 102400.0
#define f1 18000.0
#define f2 10.0

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

    int cnt = -UNIT_SIZE_X4;
    uint32_t skip = 0;
    uint32_t div = 5;
    arm_biquad_casd_df1_inst_f32 s1, s2, s3;
    arm_biquad_casd_df1_inst_f32 *S1 = &s1;
    arm_biquad_casd_df1_inst_f32 *S2 = &s2;
    arm_biquad_casd_df1_inst_f32 *S3 = &s3;
    arm_biquad_cascade_df1_init_f64(S1, NUM_STAGES, iirCoeffs, iirState1);
    arm_biquad_cascade_df1_init_f64(S2, NUM_STAGES, iirCoeffs, iirState3);
    arm_biquad_cascade_df1_init_f64(S3, NUM_STAGES, iirCoeffs, iirState4);
    double in = 0;
    double tmp[2];
    double *p1 = reversal1 + 1;
    double *p2 = reversal2 + 1 + UNIT_SIZE_X2;
    double *p3 = reversal3 + 1 + UNIT_SIZE_X4;
    double *end = p1 + UNIT_SIZE_X4;
    double *output = waveform;
    double *o = (double *)&tmp[2];
    int state = 0;
    while (cnt < UNIT_SIZE * 4 + UNIT_SIZE_X4)
    {
        //in = cos(2 * PI * f2 * ((double)cnt / Fs) + 3.0 / 4.0 * PI);
#if 0
        *output++ = in;
#else
        if (cnt >= 0) {
            uint32_t i = ((cnt / UNIT_SIZE_X4) * UNIT_SIZE_X4 + (UNIT_SIZE_X4 - cnt % UNIT_SIZE_X4 - 1));
            if (((i % UNIT_SIZE_X4) < UNIT_SIZE) || ((i % UNIT_SIZE_X4) > UNIT_SIZE_X3) || (i % 2 == 0))
                o = (double *)&tmp[2];
            else {
                uint32_t pos = 0;
                if ((i - UNIT_SIZE) / UNIT_SIZE_X4 == 0) {
                    pos = (i - UNIT_SIZE) % UNIT_SIZE_X4;
                }
                else {
                    pos = (i - UNIT_SIZE) / UNIT_SIZE_X4 * UNIT_SIZE_X2 + (i - UNIT_SIZE) % UNIT_SIZE_X4;
                }
                printf("%d\n", pos);
                printf("%d\n", pos - 1);
                o = &output[pos];
            }
        }
        //arm_biquad_cascade_df1_f64(S1, p1++, --o, 1);
        *(--o) = *p1++;
        //arm_biquad_cascade_df1_f64(S1, p1++, --o, 1);
        *(--o) = *p1++;
        if (cnt < 0)
            o += 2;
        //arm_biquad_cascade_df1_f64(S2, &in, --p2, 1);
        *(--p2) = in;
        //arm_biquad_cascade_df1_f64(S3, &in, --p3, 1);
        *(--p3) = in;
        if (p1 == end) {
            arm_biquad_casd_df1_inst_f32 tmp;
            double *t;
            tmp = *S3;
            t = p3;
            arm_biquad_cascade_df1_init_f64(S3, NUM_STAGES, iirCoeffs, iirState4);
            p3 = p1;
            arm_biquad_cascade_df1_init_f64(S1, NUM_STAGES, iirCoeffs, iirState4);
            p1 = p2;
            end = p1 + UNIT_SIZE_X4;
            *S2 = tmp;
            p2 = t;
        }
#endif
        cnt++;
        in = (int)in + 1;
    }
    std::ofstream ofs("D:\\work\\matlab\\data1.txt");
    for (int i = 0; i < N; i++)
        ofs << waveform[i] << "," << std::endl;
    return 0;
}

