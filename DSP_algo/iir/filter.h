#ifndef FILTER_H
#define FILTER_H

struct FilterStruct
{
    int stageNum;
    double *pCoeffs;
    double *pState1, *pState2, *pState3;
    double scale;
    int unitSize;
    int absolute;
    int points;
    int N;
    int M;
    int cnt;
    int head;
    int tail;
    double *p1, *p2, *p3;
    double *end;
    double *output;
    struct FilterStruct *next;
};

void InitFilterStruct(struct FilterStruct *F, int stageNum, double *pCoeffs, double scale, int absolute, double *pState1, double *pState2, double *pState3, int unitSize, int N, int M, double *reversal, double *output, struct FilterStruct *next);
bool Filter(struct FilterStruct *F, double in);

#endif //FILTER_H