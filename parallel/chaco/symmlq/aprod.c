// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:
/* This software was developed by Bruce Hendrickson and Robert Leland   *
* at Sandia National Laboratories under US Department of Energy        *
* contract DE-AC04-76DP00789 and is copyrighted by Sandia Corporation. */

#include <stdio.h>
#include "../main/structs.h"
#include "../main/defs.h"

int aprod_(lnvtxs, x, y, dA, vwsqrt, work, dorthlist)
long *lnvtxs;
double *x;
double *y;
double *dA;
double *vwsqrt;
double *work;
double *dorthlist;      /* vectors to orthogonalize against */
{
  extern Heap   *heap;       /* pointer to heap of multigrid */
  extern double *MEM_OK;     /* variable for memory overflow exeception */
  int nvtxs;                    /* int copy of long_nvtxs */
  struct vtx_data **A;
  struct orthlink *orthlist;            /* vectors to orthogonalize against */
  void splarax(), orthog1(), orthogvec(), orthogonalize();

  nvtxs = *lnvtxs;
  A = (struct vtx_data **) dA;
  orthlist = (struct orthlink *) dorthlist;

  /* The offset on x and y is because the arrays come
     originally from Fortran declarations which index from 1 */
  splarax(y-1, A, nvtxs, x-1, vwsqrt, work-1);
  if (!MEM_OK) return;

  /* Now orthogonalize against lower eigenvectors. */
  if (vwsqrt == NULL) orthog1(y-1, 1, nvtxs);
  else orthogvec(y-1, 1, nvtxs, vwsqrt);
  orthogonalize(y-1, nvtxs, orthlist);

  return(0);
}
