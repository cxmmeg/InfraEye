
#include "stdint.h"
#include "stdio.h"
#include "module_ImageUpscaling.h"

    int16_t img_up_s16RowIterator;
    int16_t img_up_s16ColumnIterator;

void img_up_vUpscaleImage(uint16_t* pu16Array, float* fOutput, uint16_t u16BufferSize)
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

        if (pu16Array[(s16LowIndexB + INPUT_ARRAY_WIDTH_D * (s16LowIndexA - 1)) - 1] == pu16Array[(s16LowIndexB +
           INPUT_ARRAY_WIDTH_D * s16LowIndexA) - 1])
        {
            fQx_1 = (float)pu16Array[(s16LowIndexB + INPUT_ARRAY_WIDTH_D * (s16LowIndexA - 1)) - 1];
        }
        else
        {
            fQx_1 = (float)(1.0 - dRx) * (float)pu16Array[(s16LowIndexB + INPUT_ARRAY_WIDTH_D * (s16LowIndexA - 1)) - 1] +
                    (float)dRx * (float)pu16Array[(s16LowIndexB + INPUT_ARRAY_WIDTH_D* s16LowIndexA) - 1];
        }
        if (pu16Array[s16LowIndexB + INPUT_ARRAY_WIDTH_D * (s16LowIndexA - 1)] == pu16Array[s16LowIndexB + 32 *
          s16LowIndexA])
        {
            fQx_2 = (float)pu16Array[s16LowIndexB + INPUT_ARRAY_WIDTH_D * (s16LowIndexA - 1)];
        }
        else
        {
            fQx_2 = (float)(1.0 - dRx) * (float)pu16Array[s16LowIndexB + INPUT_ARRAY_WIDTH_D * (s16LowIndexA - 1)] +
                    (float)dRx * (float)pu16Array[s16LowIndexB + INPUT_ARRAY_WIDTH_D * s16LowIndexA];
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
        img_up_s16ColumnIterator = u16Counter % OUTPUT_ARRAY_LENGTH_D;
        img_up_s16RowIterator = u16Counter/OUTPUT_ARRAY_LENGTH_D;
    }
}

void img_up_vResetUpscaling(void)
{
  img_up_s16RowIterator = 0;
  img_up_s16ColumnIterator = 0;
}
