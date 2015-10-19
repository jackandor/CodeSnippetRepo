void fltoq15(const float *x, short *y, const int n)
{
    int i, a;

    for(i = 0; i < n; i++)
    {
        a = floor(32768 * x[i]);
        // saturate to 16-bit //
        if (a > 32767) a = 32767;
        if (a < -32768) a = -32768;
        y[i] = (short) a;
    }
}

void fir(const float *x, const float *h, float *y, const int nh, const int ny)
{
    int i, j;
    float sum;

    for (j = 0; j < ny; j++)
    {
        sum = 0;

        // note: h coeffs given in reverse order: { h[nh-1], h[nh-2], ..., h[0] }
        for (i = 0; i < nh; i++)
            sum += x[i + j] * h[i];

        y[j] = sum;
    }
}

void time(double *t, double fs, int points)
{
    int i;
    for(i = 0; i < points; i++)
        t[i] = i / fs;
}
