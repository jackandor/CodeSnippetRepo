#include <arm_math.h>
#include <stm32f4xx_tim.h>

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


void filter(uint32_t stage, float *pCoeffs, float *pState, float *pIn, float *pOut)
{
    do {
        float acc = (b0 * Xn) + (b1 * Xn1) + (b2 * Xn2) + (a1 * Yn1) + (a2 * Yn2);


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

#define NUM_STAGES 6
#define UNIT_SIZE 256
#define UNIT_SIZE_X2 (UNIT_SIZE * 2)
#define UNIT_SIZE_X3 (UNIT_SIZE * 3)
#define UNIT_SIZE_X4 (UNIT_SIZE * 4)

#define N 16384
#define Fs 100000.0f
#define DIV 400
#define f1 18000.0f
#define f2 2.0f

float reversal[3 * UNIT_SIZE_X4];
float iirState1[4 * NUM_STAGES];
float iirState2[4 * NUM_STAGES];
float iirState3[4 * NUM_STAGES];
float waveform[N];


int cnt;
float *IIRState1, *IIRState2, *IIRState3;
float in;
float tmp;
float *p1, *p2, *p3;
float *end;
float *output;
int head, tail;
int count;
int div;
int finished;


void init_filter()
{
    cnt = -UNIT_SIZE_X4;
    IIRState1 = iirState1;
    IIRState2 = iirState2;
    IIRState3 = iirState3;
    in = 0;
    p1 = reversal;
    p2 = reversal + UNIT_SIZE_X4 + UNIT_SIZE_X2;
    p3 = reversal + 3 * UNIT_SIZE_X4;
    end = p2 - UNIT_SIZE_X2;
    output = waveform;
    head = UNIT_SIZE - 1 - UNIT_SIZE_X4;
    div = DIV;
    finished = 0;
}

float iirCoeffs[] = {
    1,  -1.9997601509094238,  1,  1.9993809461593628,   -0.9995419979095459,
    1,  -1.9997348785400391,  1,  1.9983304738998413,   -0.99847835302352905,
    1,  -1.9996604919433594,  1,  1.9968841075897217,   -0.99700403213500977,
    1,  -1.9994257688522339,  1,  1.9949734210968018,   -0.99505156278610229,
    1,  -1.9981226921081543,  1,  1.9930628538131714,   -0.99309760332107544,
    1,   1,                   0,  0.99610483646392822,   0

};

void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET) //溢出中断
	{
        in = arm_cos_f32(2 * PI * f2 * ((float)cnt / Fs) + 3.0 / 4.0 * PI);
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
                else {
                    filter(NUM_STAGES, iirCoeffs, IIRState1, p1++, &output[pos / div]);
                }
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
            float *tmp = p1;
            float *IIRState = IIRState1;

            p1 = p2;
            p2 = p3;
            p3 = tmp;

            end = p2 - UNIT_SIZE_X2;

            IIRState1 = IIRState2;
            IIRState2 = IIRState3;
            IIRState3 = IIRState;
            memset(IIRState1, 0, (4u * (uint32_t)NUM_STAGES)  * sizeof(float));
            memset(IIRState3, 0, (4u * (uint32_t)NUM_STAGES)  * sizeof(float));

            head += UNIT_SIZE_X2;
            tail += UNIT_SIZE_X2;
        }
        cnt++;
	}
    if(cnt < (((N + UNIT_SIZE) * div) + UNIT_SIZE) / UNIT_SIZE_X2 * UNIT_SIZE_X2 - UNIT_SIZE / 2)
        finished = 1;
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);  //清除中断标志位
}

