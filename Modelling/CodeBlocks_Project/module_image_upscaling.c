
#include "stdint.h"
#include "stdio.h"
#include "module_image_upscaling.h"

void img_up_vUpscaleImage(uint16_t* pu16Array, float* fOutput)
{
    int16_t s16RowIterator;
    int16_t s16ColumnIterator;
    int16_t s16LowIndexA;
    int16_t s16LowIndexPlus1;
    int16_t s16HighIndex;
    int16_t s16MiddleIndex;
    int16_t s16LowIndexB;
    double dRx;
    float fQx_1;
    float fQx_2;

  /*  Interpolation */
    for(s16RowIterator = 0; s16RowIterator < OUTPUT_ARRAY_WIDTH_D; s16RowIterator++)
    {
        for(s16ColumnIterator = 0; s16ColumnIterator < OUTPUT_ARRAY_LENGTH_D; s16ColumnIterator++)
        {
            s16LowIndexA = 1;
            s16LowIndexPlus1 = 2;
            s16HighIndex = INPUT_ARRAY_LENGTH_D;
            while (s16HighIndex > s16LowIndexPlus1)
            {
                s16MiddleIndex = (s16LowIndexA + s16HighIndex) >> 1;
                if (s16ColumnIterator >= SCALING_FACTOR_D * (s16MiddleIndex - 1))
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
                if (s16RowIterator >= SCALING_FACTOR_D * (s16MiddleIndex - 1))
                {
                    s16LowIndexB = s16MiddleIndex;
                    s16LowIndexPlus1 = s16MiddleIndex + 1;
                }
                else
                {
                  s16HighIndex = s16MiddleIndex;
                }
            }

            dRx = ((double)s16ColumnIterator - (double)SCALING_FACTOR_D * ((double)s16LowIndexA - 1.0)) / (double)((SCALING_FACTOR_D * s16LowIndexA) -
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
                dRx = ((double)s16RowIterator - (double)SCALING_FACTOR_D * ((double)s16LowIndexB - 1.0)) / (double)((SCALING_FACTOR_D * s16LowIndexB) -
                (SCALING_FACTOR_D * (s16LowIndexB - 1)));

                fQx_1 = (float)(1.0 - dRx) * fQx_1 + (float)dRx * fQx_2;
            }

            fOutput[s16RowIterator*OUTPUT_ARRAY_LENGTH_D + s16ColumnIterator] = fQx_1;
        }
    }
}
