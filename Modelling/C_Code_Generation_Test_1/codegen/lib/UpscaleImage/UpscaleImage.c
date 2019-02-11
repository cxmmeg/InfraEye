/*
 * File: UpscaleImage.c
 *
 * MATLAB Coder version            : 2.7
 * C/C++ source code generated on  : 11-Feb-2019 21:56:40
 */

/* Include Files */
#include "rt_nonfinite.h"
#include "UpscaleImage.h"

/* Function Declarations */
static int b_bsearch(const double x[32], double xi);
static int c_bsearch(const double x[24], double xi);
static void meshgrid(const double x[124], const double y[92], double xx[11408],
                     double yy[11408]);

/* Function Definitions */

/*
 * Arguments    : const double x[32]
 *                double xi
 * Return Type  : int
 */
static int b_bsearch(const double x[32], double xi)
{
  int n;
  int low_ip1;
  int high_i;
  int mid_i;
  n = 1;
  low_ip1 = 2;
  high_i = 32;
  while (high_i > low_ip1) {
    mid_i = (n + high_i) >> 1;
    if (xi >= x[mid_i - 1]) {
      n = mid_i;
      low_ip1 = mid_i + 1;
    } else {
      high_i = mid_i;
    }
  }

  return n;
}

/*
 * Arguments    : const double x[24]
 *                double xi
 * Return Type  : int
 */
static int c_bsearch(const double x[24], double xi)
{
  int n;
  int low_ip1;
  int high_i;
  int mid_i;
  n = 1;
  low_ip1 = 2;
  high_i = 24;
  while (high_i > low_ip1) {
    mid_i = (n + high_i) >> 1;
    if (xi >= x[mid_i - 1]) {
      n = mid_i;
      low_ip1 = mid_i + 1;
    } else {
      high_i = mid_i;
    }
  }

  return n;
}

/*
 * Arguments    : const double x[124]
 *                const double y[92]
 *                double xx[11408]
 *                double yy[11408]
 * Return Type  : void
 */
static void meshgrid(const double x[124], const double y[92], double xx[11408],
                     double yy[11408])
{
  int jcol;
  int ibtile;
  int itilerow;
  for (jcol = 0; jcol < 124; jcol++) {
    ibtile = jcol * 92;
    for (itilerow = 0; itilerow < 92; itilerow++) {
      xx[ibtile + itilerow] = x[jcol];
    }
  }

  for (jcol = 0; jcol < 124; jcol++) {
    ibtile = jcol * 92;
    memcpy(&yy[ibtile], &y[0], 92U * sizeof(double));
  }
}

/*
 * Arguments    : const float InputArray[768]
 *                float Output[11408]
 * Return Type  : void
 */
void UpscaleImage(const float InputArray[768], float Output[11408])
{
  boolean_T b0;
  boolean_T b1;
  double dv0[124];
  int iy;
  double dv1[92];
  static double Yq[11408];
  static double Xq[11408];
  static float VV[884];
  int ix;
  int k;
  double dv2[32];
  double dv3[24];
  float s;
  float t;
  float ss;
  float zik;
  b0 = false;
  b1 = false;

  /*  Mesh defined - 24x32 */
  /*  Mesh define */
  for (iy = 0; iy < 124; iy++) {
    dv0[iy] = iy;
  }

  for (iy = 0; iy < 92; iy++) {
    dv1[iy] = iy;
  }

  meshgrid(dv0, dv1, Xq, Yq);

  /*  Interpolation */
  memset(&VV[0], 0, 884U * sizeof(float));
  for (ix = 0; ix < 32; ix++) {
    memcpy(&VV[1 + 26 * (ix + 1)], &InputArray[24 * ix], 24U * sizeof(float));
  }

  for (ix = 0; ix < 34; ix++) {
    VV[26 * ix] = (3.0F * VV[1 + 26 * ix] - 3.0F * VV[2 + 26 * ix]) + VV[3 + 26 *
      ix];
    VV[25 + 26 * ix] = (3.0F * VV[24 + 26 * ix] - 3.0F * VV[23 + 26 * ix]) + VV
      [22 + 26 * ix];
  }

  for (iy = 0; iy < 26; iy++) {
    VV[iy] = (3.0F * VV[26 + iy] - 3.0F * VV[52 + iy]) + VV[78 + iy];
    VV[858 + iy] = (3.0F * VV[832 + iy] - 3.0F * VV[806 + iy]) + VV[780 + iy];
  }

  for (k = 0; k < 11408; k++) {
    if ((Xq[k] >= 0.0) && (Xq[k] <= 124.0) && (Yq[k] >= 0.0) && (Yq[k] <= 92.0))
    {
      if (!b0) {
        for (iy = 0; iy < 32; iy++) {
          dv2[iy] = 4.0 * (double)iy;
        }

        b0 = true;
      }

      ix = b_bsearch(dv2, Xq[k]);
      if (!b1) {
        for (iy = 0; iy < 24; iy++) {
          dv3[iy] = 4.0 * (double)iy;
        }

        b1 = true;
      }

      iy = c_bsearch(dv3, Yq[k]);
      s = ((float)Xq[k] - 4.0F * ((float)ix - 1.0F)) / 4.0F;
      t = ((float)Yq[k] - 4.0F * ((float)iy - 1.0F)) / 4.0F;
      ss = ((2.0F - s) * s - 1.0F) * s;
      zik = ((VV[(iy + 26 * (ix - 1)) - 1] * ss * (((2.0F - t) * t - 1.0F) * t)
              + VV[iy + 26 * (ix - 1)] * ss * ((3.0F * t - 5.0F) * t * t + 2.0F))
             + VV[(iy + 26 * (ix - 1)) + 1] * ss * (((4.0F - 3.0F * t) * t +
               1.0F) * t)) + VV[(iy + 26 * (ix - 1)) + 2] * ss * ((t - 1.0F) * t
        * t);
      ss = (3.0F * s - 5.0F) * s * s + 2.0F;
      zik += VV[(iy + 26 * ix) - 1] * ss * (((2.0F - t) * t - 1.0F) * t);
      zik += VV[iy + 26 * ix] * ss * ((3.0F * t - 5.0F) * t * t + 2.0F);
      zik += VV[(iy + 26 * ix) + 1] * ss * (((4.0F - 3.0F * t) * t + 1.0F) * t);
      zik += VV[(iy + 26 * ix) + 2] * ss * ((t - 1.0F) * t * t);
      ss = ((4.0F - 3.0F * s) * s + 1.0F) * s;
      zik += VV[(iy + 26 * (ix + 1)) - 1] * ss * (((2.0F - t) * t - 1.0F) * t);
      zik += VV[iy + 26 * (ix + 1)] * ss * ((3.0F * t - 5.0F) * t * t + 2.0F);
      zik += VV[(iy + 26 * (ix + 1)) + 1] * ss * (((4.0F - 3.0F * t) * t + 1.0F)
        * t);
      zik += VV[(iy + 26 * (ix + 1)) + 2] * ss * ((t - 1.0F) * t * t);
      ss = (s - 1.0F) * s * s;
      zik += VV[(iy + 26 * (ix + 2)) - 1] * ss * (((2.0F - t) * t - 1.0F) * t);
      zik += VV[iy + 26 * (ix + 2)] * ss * ((3.0F * t - 5.0F) * t * t + 2.0F);
      zik += VV[(iy + 26 * (ix + 2)) + 1] * ss * (((4.0F - 3.0F * t) * t + 1.0F)
        * t);
      zik += VV[(iy + 26 * (ix + 2)) + 2] * ss * ((t - 1.0F) * t * t);
      Output[k] = zik / 4.0F;
    } else {
      Output[k] = ((real32_T)rtNaN);
    }
  }
}

/*
 * Arguments    : void
 * Return Type  : void
 */
void UpscaleImage_initialize(void)
{
  rt_InitInfAndNaN(8U);
}

/*
 * Arguments    : void
 * Return Type  : void
 */
void UpscaleImage_terminate(void)
{
  /* (no terminate code required) */
}

/*
 * File trailer for UpscaleImage.c
 *
 * [EOF]
 */
