#include "stdafx.h"

#include <math.h>
#include "filter.h"

#define UNIT_SIZE (F->unitSize)
#define UNIT_SIZE_X2 (F->unitSize * 2)
#define UNIT_SIZE_X3 (F->unitSize * 3)
#define UNIT_SIZE_X4 (F->unitSize * 4)

void InitFilterStruct(struct FilterStruct *F, int stageNum, double *pCoeffs, double scale, int absolute, double *pState1, double *pState2, double *pState3, int unitSize, int N, int M, double *reversal, double *output, struct FilterStruct *next)
{
    F->stageNum = stageNum;
    F->pCoeffs = pCoeffs;
    F->scale = scale;
    F->absolute = absolute;
    F->pState1 = pState1;
    F->pState2 = pState2;
    F->pState3 = pState3;
    F->unitSize = unitSize;
    F->N = N;
    F->M = M;
    F->points = (((F->N + UNIT_SIZE) * F->M) + UNIT_SIZE) / UNIT_SIZE_X2 * UNIT_SIZE_X2 - UNIT_SIZE / 2;

    F->cnt = -UNIT_SIZE_X4;
    F->head = UNIT_SIZE - 1 - UNIT_SIZE_X4;;
    F->tail = UNIT_SIZE_X3 - 1 - UNIT_SIZE_X4;
    F->p1 = reversal;
    F->p2 = reversal + UNIT_SIZE_X4 + UNIT_SIZE_X2;
    F->p3 = reversal + 3 * UNIT_SIZE_X4;
    F->end = F->p2 - UNIT_SIZE_X2;

    F->output = output;

    F->next = next;

    memset(F->pState1, 0, 4 * F->stageNum * sizeof(double));
    memset(F->pState2, 0, 4 * F->stageNum * sizeof(double));
    memset(F->pState3, 0, 4 * F->stageNum * sizeof(double));
}

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


void iir(int stage, double *pCoeffs, double *pState, double *pIn, double *pOut)
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


bool Filter(struct FilterStruct *F, double in)
{
    bool ret = false;
    double tmp;
    if (F->cnt >= 0) {
        int i = (F->cnt / UNIT_SIZE_X2) * UNIT_SIZE_X2 + UNIT_SIZE_X4 - (F->cnt % UNIT_SIZE_X2) * 2 - 1;
        int pos = i - UNIT_SIZE;
        if ((i <= F->head) || (i > F->tail) || (pos > (F->N * F->M))) {
            iir(F->stageNum, F->pCoeffs, F->pState1, F->p1++, &tmp);
            iir(F->stageNum, F->pCoeffs, F->pState1, F->p1++, &tmp);
        }
        else {
#if 0
            if (pos % F->M)
                iir(F->stageNum, F->pCoeffs, F->pState1, F->p1++, &tmp);
            else {
                if (F->next) {
                    iir(F->stageNum, F->pCoeffs, F->pState1, F->p1++, &tmp);
                    Filter(F->next, tmp);
                } else
                    iir(F->stageNum, F->pCoeffs, F->pState1, F->p1++, &(F->output[pos / F->M]));
            }
            pos--;
            if (pos % F->M)
                iir(F->stageNum, F->pCoeffs, F->pState1, F->p1++, &tmp);
            else {
                if (F->next) {
                    iir(F->stageNum, F->pCoeffs, F->pState1, F->p1++, &tmp);
                    Filter(F->next, tmp);
                } else
                    iir(F->stageNum, F->pCoeffs, F->pState1, F->p1++, &(F->output[pos / F->M]));
            }
#else
            iir(F->stageNum, F->pCoeffs, F->pState1, F->p1++, &tmp);
            tmp *= F->scale;
            if (F->absolute)
                tmp = fabs(tmp);
            if (F->next)
                ret = Filter(F->next, tmp);
            else if (0 == pos % F->M)
                F->output[pos / F->M] = tmp;
            if (ret)
                return true;
            pos--;
            iir(F->stageNum, F->pCoeffs, F->pState1, F->p1++, &tmp);
            tmp *= F->scale;
            if (F->absolute)
                tmp = fabs(tmp);
            if (F->next)
                ret = Filter(F->next, tmp);
            else if (0 == pos % F->M)
                F->output[pos / F->M] = tmp;
            if (ret)
                return true;
#endif
        }
    }
    else
        F->p1 += 2;
    iir(F->stageNum, F->pCoeffs, F->pState2, &in, --(F->p2));
    iir(F->stageNum, F->pCoeffs, F->pState3, &in, --(F->p3));
    if (F->p2 == F->end) {
        double *tmp = F->p1;
        double *iirState = F->pState1;

        F->p1 = F->p2;
        F->p2 = F->p3;
        F->p3 = tmp;

        F->end = F->p2 - UNIT_SIZE_X2;

        F->pState1 = F->pState2;
        F->pState2 = F->pState3;
        F->pState3 = iirState;
        memset(F->pState1, 0, 4 * F->stageNum * sizeof(double));
        memset(F->pState3, 0, 4 * F->stageNum * sizeof(double));

        F->head += UNIT_SIZE_X2;
        F->tail += UNIT_SIZE_X2;
    }
    F->cnt++;
    if (F->next)
        return false;
    if (F->cnt == F->points)
        return true;
    else
        return false;
}