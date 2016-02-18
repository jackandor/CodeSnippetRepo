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
    1, -2, 1, 1.927121312653638, -0.93466838971091215,
    1, -2, 1, 1.8420262841919222, -0.84924010836429775
};

#define NUM_STAGES 2
#define BLOCK_SIZE 40
#define N 16384 + 256

double waveform[N];
double waveform2[N];
double iirState[4 * NUM_STAGES];

#define PI 3.14159265
#define Fs 256.0
#define f1 0.2

int main()
{
    for (int i = 0; i < N; i++) {
        waveform[i] = sin(2 * PI * f1 * ((double)i / Fs));
    }
    arm_biquad_casd_df1_inst_f32 S;
    arm_biquad_cascade_df1_init_f64(&S, NUM_STAGES, iirCoeffs, iirState);
    arm_biquad_cascade_df1_f64(&S, waveform, waveform2, N);

    std::ofstream ofs("D:\\work\\matlab\\data1.txt");
    for (int i = 0; i < N; i++)
        ofs << waveform2[i] << "," << std::endl;
    return 0;
}

