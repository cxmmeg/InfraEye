/*********************************************************************************************************/
/* INCLUDES                                                                                              */
/*********************************************************************************************************/
#include "stdint.h"
#include "stdio.h"
#include "module_ImageUpscaling.h"

/*********************************************************************************************************/
/* DEFINES                                                                                               */
/*********************************************************************************************************/



/*********************************************************************************************************/
/* MODULE VARIABLES                                                                                      */
/*********************************************************************************************************/
#if(SCALING_FACTOR_D == 4)
static float img_up_afRxCol_C[SCALING_FACTOR_D] = {0.0, 0.25, 0.5, 0.75};
static float img_up_afRxRow_C[SCALING_FACTOR_D] = {0.0, 0.25, 0.5, 0.75};

static FACTOR_T img_up_aeRxCol[SCALING_FACTOR_D] = {FACTOR_0_E, FACTOR_0_25_E, FACTOR_0_5_E, FACTOR_0_75_E};
static FACTOR_T img_up_aeRxRow[SCALING_FACTOR_D] = {FACTOR_0_E, FACTOR_0_25_E, FACTOR_0_5_E, FACTOR_0_75_E};

#if LOOK_UP_TABLE_D
static volatile uint16_t img_up_au16LowIndexA_C[OUTPUT_ARRAY_LENGTH_D] = 
{
    1u,1u,1u,1u,        2u,2u,2u,2u,        3u,3u,3u,3u,        4u,4u,4u,4u,        5u,5u,5u,5u,        6u,6u,6u,6u,        7u,7u,7u,7u,        8u,8u,8u,8u,
    9u,9u,9u,9u,        10u,10u,10u,10u,    11u,11u,11u,11u,    12u,12u,12u,12u,    13u,13u,13u,13u,    14u,14u,14u,14u,    15u,15u,15u,15u,    16u,16u,16u,16u,
    17u,17u,17u,17u,    18u,18u,18u,18u,    19u,19u,19u,19u,    20u,20u,20u,20u,    21u,21u,21u,21u,    22u,22u,22u,22u,    23u,23u,23u,23u 
};
static volatile uint16_t img_up_au16LowIndexB_C[OUTPUT_ARRAY_WIDTH_D] = 
{
    1u,1u,1u,1u,        2u,2u,2u,2u,        3u,3u,3u,3u,        4u,4u,4u,4u,        5u,5u,5u,5u,        6u,6u,6u,6u,        7u,7u,7u,7u,        8u,8u,8u,8u,
    9u,9u,9u,9u,        10u,10u,10u,10u,    11u,11u,11u,11u,    12u,12u,12u,12u,    13u,13u,13u,13u,    14u,14u,14u,14u,    15u,15u,15u,15u,    16u,16u,16u,16u,
    17u,17u,17u,17u,    18u,18u,18u,18u,    19u,19u,19u,19u,    20u,20u,20u,20u,    21u,21u,21u,21u,    22u,22u,22u,22u,    23u,23u,23u,23u,    24u,24u,24u,24u,
    25u,25u,25u,25u,    26u,26u,26u,26u,    27u,27u,27u,27u,    28u,28u,28u,28u,    29u,29u,29u,29u,    30u,30u,30u,30u,    31u,31u,31u,31u
};
#endif
#endif

static uint16_t img_up_u16RowIterator = 0u;
static uint16_t img_up_u16ColumnIterator = 0u;

/*********************************************************************************************************/
/* FUNCTION PROTOTYPES                                                                                   */
/*********************************************************************************************************/

static uint16_t img_up_u16FactorElement(uint16_t u16Element, FACTOR_T eFactor);

/*********************************************************************************************************/
/* FUNCTION DEFINITIONS                                                                                  */
/*********************************************************************************************************/

void img_up_vUpscaleImage(float* pfArray, float* fOutput, uint16_t u16BufferSize)
{
    uint16_t u16LowIndexA;
    uint16_t u16LowIndexPlus1;
    uint16_t u16HighIndex;
    uint16_t u16MiddleIndex;
    uint16_t u16LowIndexB;
    float fRx;
    float fQx_1;
    float fQx_2;
    uint16_t u16Counter = 0;

    /*  Interpolation */
    while((img_up_u16RowIterator < OUTPUT_ARRAY_WIDTH_D) && (img_up_u16ColumnIterator < OUTPUT_ARRAY_LENGTH_D) && (u16Counter < u16BufferSize))
    {

#if(LOOK_UP_TABLE_D)
        u16LowIndexA = img_up_au16LowIndexA_C[img_up_u16ColumnIterator];
        u16LowIndexB = img_up_au16LowIndexB_C[img_up_u16RowIterator];
#else
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
#endif

#if(SCALING_FACTOR_D == 4)
        fRx = img_up_afRxCol_C[img_up_u16ColumnIterator % SCALING_FACTOR_D];
#else
        fRx = ((float)img_up_u16ColumnIterator - (float)SCALING_FACTOR_D * ((float)u16LowIndexA - 1.0)) / (float)((SCALING_FACTOR_D * u16LowIndexA) -
        (SCALING_FACTOR_D * (u16LowIndexA - 1)));
#endif

        if (pfArray[(u16LowIndexB + INPUT_ARRAY_WIDTH_D * (u16LowIndexA - 1)) - 1] == pfArray[(u16LowIndexB +
           INPUT_ARRAY_WIDTH_D * u16LowIndexA) - 1])
        {
            fQx_1 = pfArray[(u16LowIndexB + INPUT_ARRAY_WIDTH_D * (u16LowIndexA - 1)) - 1];
        }
        else
        {
            fQx_1 = (1.0 - fRx) * pfArray[(u16LowIndexB + INPUT_ARRAY_WIDTH_D * (u16LowIndexA - 1)) - 1] +
                    fRx * pfArray[(u16LowIndexB + INPUT_ARRAY_WIDTH_D* u16LowIndexA) - 1];
        }
        if (pfArray[u16LowIndexB + INPUT_ARRAY_WIDTH_D * (u16LowIndexA - 1)] == pfArray[u16LowIndexB + INPUT_ARRAY_WIDTH_D * u16LowIndexA])
        {
            fQx_2 = pfArray[u16LowIndexB + INPUT_ARRAY_WIDTH_D * (u16LowIndexA - 1)];
        }
        else
        {
            fQx_2 = (1.0 - fRx) * pfArray[u16LowIndexB + INPUT_ARRAY_WIDTH_D * (u16LowIndexA - 1)] +
                    fRx * pfArray[u16LowIndexB + INPUT_ARRAY_WIDTH_D * u16LowIndexA];
        }
        if (fQx_1 == fQx_2)
        {
        }
        else
        {
#if(SCALING_FACTOR_D == 4)
            fRx = img_up_afRxRow_C[img_up_u16RowIterator % SCALING_FACTOR_D];
#else
            fRx = ((float)img_up_u16RowIterator - (float)SCALING_FACTOR_D * ((float)u16LowIndexB - 1.0)) / (float)((SCALING_FACTOR_D * u16LowIndexB) -
            (SCALING_FACTOR_D * (u16LowIndexB - 1)));
#endif
            fQx_1 = (1.0 - fRx) * fQx_1 + fRx * fQx_2;
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

void img_up_vUpscaleImage_u16(uint16_t* pu16Array, uint16_t* pu16Output, uint16_t u16BufferSize)
{
    uint16_t u16LowIndexA;
    uint16_t u16LowIndexPlus1;
    uint16_t u16HighIndex;
    uint16_t u16MiddleIndex;
    uint16_t u16LowIndexB;
    FACTOR_T eRx;
    uint16_t u16Elem_1;
    uint16_t u16Elem_2;
    uint16_t u16Qx_1;
    uint16_t u16Qx_2;
    uint16_t u16Counter = 0;

    /*  Interpolation */
    while((img_up_u16RowIterator < OUTPUT_ARRAY_WIDTH_D) && (img_up_u16ColumnIterator < OUTPUT_ARRAY_LENGTH_D) && (u16Counter < u16BufferSize))
    {

#if(LOOK_UP_TABLE_D)
        u16LowIndexA = img_up_au16LowIndexA_C[img_up_u16ColumnIterator];
        u16LowIndexB = img_up_au16LowIndexB_C[img_up_u16RowIterator];
#else
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
#endif

#if(SCALING_FACTOR_D == 4)
        eRx = img_up_aeRxCol[img_up_u16ColumnIterator % SCALING_FACTOR_D];
#else
#endif

        if (pu16Array[(u16LowIndexB + INPUT_ARRAY_WIDTH_D * (u16LowIndexA - 1)) - 1] == pu16Array[(u16LowIndexB + INPUT_ARRAY_WIDTH_D * u16LowIndexA) - 1])
        {
            u16Qx_1 = pu16Array[(u16LowIndexB + INPUT_ARRAY_WIDTH_D * (u16LowIndexA - 1)) - 1];
        }
        else
        {
            u16Elem_1 = img_up_u16FactorElement(pu16Array[(u16LowIndexB + INPUT_ARRAY_WIDTH_D * (u16LowIndexA - 1)) - 1], (FACTOR_T)((uint16_t)FACTOR_1_E - (uint16_t)eRx));
            u16Elem_2 = img_up_u16FactorElement(pu16Array[(u16LowIndexB + INPUT_ARRAY_WIDTH_D* u16LowIndexA) - 1], eRx);
            u16Qx_1 = u16Elem_1 + u16Elem_2;
        }

        if (pu16Array[u16LowIndexB + INPUT_ARRAY_WIDTH_D * (u16LowIndexA - 1)] == pu16Array[u16LowIndexB + INPUT_ARRAY_WIDTH_D * u16LowIndexA])
        {
            u16Qx_2 = pu16Array[u16LowIndexB + INPUT_ARRAY_WIDTH_D * (u16LowIndexA - 1)];
        }
        else
        {
            u16Elem_1 = img_up_u16FactorElement(pu16Array[u16LowIndexB + INPUT_ARRAY_WIDTH_D * (u16LowIndexA - 1)], (FACTOR_T)((uint16_t)FACTOR_1_E - (uint16_t)eRx));
            u16Elem_2 = img_up_u16FactorElement(pu16Array[u16LowIndexB + INPUT_ARRAY_WIDTH_D * u16LowIndexA], eRx);
            u16Qx_2 = u16Elem_1 + u16Elem_2;
        }
        if (u16Qx_1 == u16Qx_2)
        {
            /* Do nothing */
        }
        else
        {
#if(SCALING_FACTOR_D == 4)
            eRx = img_up_aeRxRow[img_up_u16RowIterator % SCALING_FACTOR_D];
#else
#endif
            u16Elem_1 = img_up_u16FactorElement(u16Qx_1, (FACTOR_T)((uint16_t)FACTOR_1_E - (uint16_t)eRx));
            u16Elem_2 = img_up_u16FactorElement(u16Qx_2, eRx);

            u16Qx_1 = u16Elem_1 + u16Elem_2;
        }

        pu16Output[u16Counter] = u16Qx_1;
        
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

void img_up_vResetUpscaling(void)
{
  img_up_u16RowIterator = 0;
  img_up_u16ColumnIterator = 0;
}


static uint16_t img_up_u16FactorElement(uint16_t u16Element, FACTOR_T eFactor)
{
    uint16_t u16Result;

    switch(eFactor)
    {
        case FACTOR_0_25_E:
        u16Result = u16Element >> 2u;
        break;

        case FACTOR_0_5_E:
        u16Result = u16Element >> 1u;
        break;

        case FACTOR_0_75_E:
        u16Result = (u16Element >> 1u) + (u16Element >> 2u);
        break;

        case FACTOR_1_E:
        u16Result = u16Element;
        break;

        case FACTOR_0_E:
        default:
        u16Result = 0u;
        break;
    }
    
    return u16Result;
}