
#include "stdint.h"
#include "stdio.h"
#include "module_ImageUpscaling.h"

#if(SCALING_FACTOR_D == 4)
static const double img_up_adRxCol_C[SCALING_FACTOR_D] = {0.000000, 0.250000, 0.500000, 0.750000};
static const double img_up_adRxRow_C[SCALING_FACTOR_D] = {0.000000, 0.250000, 0.500000, 0.750000};
#endif


static uint16_t img_up_u16RowIterator;
static uint16_t img_up_u16ColumnIterator;

void img_up_vUpscaleImage(float* pfArray, float* fOutput, uint16_t u16BufferSize)
{
    uint16_t u16LowIndexA;
    uint16_t u16LowIndexPlus1;
    uint16_t u16HighIndex;
    uint16_t u16MiddleIndex;
    uint16_t u16LowIndexB;
    double dRx;
    float fQx_1;
    float fQx_2;
    uint16_t u16Counter = 0;

    /*  Interpolation */
    while((img_up_u16RowIterator < OUTPUT_ARRAY_WIDTH_D) && (img_up_u16ColumnIterator < OUTPUT_ARRAY_LENGTH_D) && (u16Counter < u16BufferSize))
    {
        u16LowIndexA = 1;
        u16LowIndexPlus1 = 2;
        u16HighIndex = INPUT_ARRAY_LENGTH_D;
        while (u16HighIndex > u16LowIndexPlus1)
        {
            u16MiddleIndex = (u16LowIndexA + u16HighIndex) >> 1;
            if (img_up_u16ColumnIterator >= SCALING_FACTOR_D * (u16MiddleIndex - 1))
            {
                u16LowIndexA = u16MiddleIndex;
                u16LowIndexPlus1 = u16MiddleIndex + 1;
            }
            else
            {
                u16HighIndex = u16MiddleIndex;
            }
        }

        u16LowIndexB = 1;
        u16LowIndexPlus1 = 2;
        u16HighIndex = INPUT_ARRAY_WIDTH_D;
        while (u16HighIndex > u16LowIndexPlus1)
        {
            u16MiddleIndex = (u16LowIndexB + u16HighIndex) >> 1;
            if (img_up_u16RowIterator >= SCALING_FACTOR_D * (u16MiddleIndex - 1))
            {
                u16LowIndexB = u16MiddleIndex;
                u16LowIndexPlus1 = u16MiddleIndex + 1;
            }
            else
            {
              u16HighIndex = u16MiddleIndex;
            }
        }

#if(SCALING_FACTOR_D == 4)
        dRx = img_up_adRxCol_C[img_up_u16ColumnIterator % SCALING_FACTOR_D];
#else
        dRx = ((double)img_up_u16ColumnIterator - (double)SCALING_FACTOR_D * ((double)u16LowIndexA - 1.0)) / (double)((SCALING_FACTOR_D * u16LowIndexA) -
        (SCALING_FACTOR_D * (u16LowIndexA - 1)));
#endif
        if (pfArray[(u16LowIndexB + INPUT_ARRAY_WIDTH_D * (u16LowIndexA - 1)) - 1] == pfArray[(u16LowIndexB +
           INPUT_ARRAY_WIDTH_D * u16LowIndexA) - 1])
        {
            fQx_1 = pfArray[(u16LowIndexB + INPUT_ARRAY_WIDTH_D * (u16LowIndexA - 1)) - 1];
        }
        else
        {
            fQx_1 = (float)(1.0 - dRx) * pfArray[(u16LowIndexB + INPUT_ARRAY_WIDTH_D * (u16LowIndexA - 1)) - 1] +
                    (float)dRx * pfArray[(u16LowIndexB + INPUT_ARRAY_WIDTH_D* u16LowIndexA) - 1];
        }
        if (pfArray[u16LowIndexB + INPUT_ARRAY_WIDTH_D * (u16LowIndexA - 1)] == pfArray[u16LowIndexB + INPUT_ARRAY_WIDTH_D * u16LowIndexA])
        {
            fQx_2 = pfArray[u16LowIndexB + INPUT_ARRAY_WIDTH_D * (u16LowIndexA - 1)];
        }
        else
        {
            fQx_2 = (float)(1.0 - dRx) * pfArray[u16LowIndexB + INPUT_ARRAY_WIDTH_D * (u16LowIndexA - 1)] +
                    (float)dRx * pfArray[u16LowIndexB + INPUT_ARRAY_WIDTH_D * u16LowIndexA];
        }
        if (fQx_1 == fQx_2)
        {
        }
        else
        {
#if(SCALING_FACTOR_D == 4)
            dRx = img_up_adRxRow_C[img_up_u16RowIterator % SCALING_FACTOR_D];
#else
            dRx = ((double)img_up_u16RowIterator - (double)SCALING_FACTOR_D * ((double)u16LowIndexB - 1.0)) / (double)((SCALING_FACTOR_D * u16LowIndexB) -
            (SCALING_FACTOR_D * (u16LowIndexB - 1)));
#endif
            fQx_1 = (float)(1.0 - dRx) * fQx_1 + (float)dRx * fQx_2;
        }

        fOutput[u16Counter] = fQx_1;
        
        /* Increment counter */
        u16Counter++;
        /* Set column and row */
        if(u16Counter % OUTPUT_ARRAY_LENGTH_D == 0)
        {
          img_up_u16RowIterator++;
          img_up_u16ColumnIterator = 0;
        }
        else
        {
          img_up_u16ColumnIterator++;
        }
    }
}

void img_up_vUpscaleImage_u16(uint16_t* pfArray, uint16_t* fOutput, uint16_t u16BufferSize)
{
    int16_t s16LowIndexA;
    int16_t s16LowIndexPlus1;
    int16_t s16HighIndex;
    int16_t s16MiddleIndex;
    int16_t s16LowIndexB;
    double dRx;
    float fQx_1;
    float fQx_2;
    uint16_t u16Counter = 0;

    /*  Interpolation */
    while((img_up_s16RowIterator < OUTPUT_ARRAY_WIDTH_D) && (img_up_s16ColumnIterator < OUTPUT_ARRAY_LENGTH_D) && (u16Counter < u16BufferSize))
    {
        s16LowIndexA = 1;
        s16LowIndexPlus1 = 2;
        s16HighIndex = INPUT_ARRAY_LENGTH_D;
        while (s16HighIndex > s16LowIndexPlus1)
        {
            s16MiddleIndex = (s16LowIndexA + s16HighIndex) >> 1;
            if (img_up_s16ColumnIterator >= SCALING_FACTOR_D * (s16MiddleIndex - 1))
            {
                s16LowIndexA = s16MiddleIndex;
                s16LowIndexPlus1 = s16MiddleIndex + 1;
            }
            else
            {
                s16HighIndex = s16MiddleIndex;
            }
        }

        s16LowIndexB = 1;
        s16LowIndexPlus1 = 2;
        s16HighIndex = INPUT_ARRAY_WIDTH_D;
        while (s16HighIndex > s16LowIndexPlus1)
        {
            s16MiddleIndex = (s16LowIndexB + s16HighIndex) >> 1;
            if (img_up_s16RowIterator >= SCALING_FACTOR_D * (s16MiddleIndex - 1))
            {
                s16LowIndexB = s16MiddleIndex;
                s16LowIndexPlus1 = s16MiddleIndex + 1;
            }
            else
            {
              s16HighIndex = s16MiddleIndex;
            }
        }

        dRx = ((double)img_up_s16ColumnIterator - (double)SCALING_FACTOR_D * ((double)s16LowIndexA - 1.0)) / (double)((SCALING_FACTOR_D * s16LowIndexA) -
        (SCALING_FACTOR_D * (s16LowIndexA - 1)));

        if (pfArray[(s16LowIndexB + INPUT_ARRAY_WIDTH_D * (s16LowIndexA - 1)) - 1] == pfArray[(s16LowIndexB +
           INPUT_ARRAY_WIDTH_D * s16LowIndexA) - 1])
        {
            fQx_1 = pfArray[(s16LowIndexB + INPUT_ARRAY_WIDTH_D * (s16LowIndexA - 1)) - 1];
        }
        else
        {
            fQx_1 = (float)(1.0 - dRx) * pfArray[(s16LowIndexB + INPUT_ARRAY_WIDTH_D * (s16LowIndexA - 1)) - 1] +
                    (float)dRx * pfArray[(s16LowIndexB + INPUT_ARRAY_WIDTH_D* s16LowIndexA) - 1];
        }
        if (pfArray[s16LowIndexB + INPUT_ARRAY_WIDTH_D * (s16LowIndexA - 1)] == pfArray[s16LowIndexB + 32 *
          s16LowIndexA])
        {
            fQx_2 = pfArray[s16LowIndexB + INPUT_ARRAY_WIDTH_D * (s16LowIndexA - 1)];
        }
        else
        {
            fQx_2 = (float)(1.0 - dRx) * pfArray[s16LowIndexB + INPUT_ARRAY_WIDTH_D * (s16LowIndexA - 1)] +
                    (float)dRx * pfArray[s16LowIndexB + INPUT_ARRAY_WIDTH_D * s16LowIndexA];
        }
        if (fQx_1 == fQx_2)
        {
        }
        else
        {
            dRx = ((double)img_up_s16RowIterator - (double)SCALING_FACTOR_D * ((double)s16LowIndexB - 1.0)) / (double)((SCALING_FACTOR_D * s16LowIndexB) -
            (SCALING_FACTOR_D * (s16LowIndexB - 1)));

            fQx_1 = (float)(1.0 - dRx) * fQx_1 + (float)dRx * fQx_2;
        }

        fOutput[u16Counter] = fQx_1;
        
        /* Increment counter */
        u16Counter++;
        /* Set column and row */
        if(u16Counter % OUTPUT_ARRAY_LENGTH_D == 0)
        {
          img_up_s16RowIterator++;
          img_up_s16ColumnIterator = 0;
        }
        else
        {
          img_up_s16ColumnIterator++;
        }
    }
}

void img_up_vResetUpscaling(void)
{
  img_up_u16RowIterator = 0;
  img_up_u16ColumnIterator = 0;
}

