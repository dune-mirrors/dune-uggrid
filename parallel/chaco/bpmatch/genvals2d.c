// NOTE: The current revision of this file was left untouched when the DUNE source files were reindented!
// NOTE: It contained invalid syntax that could not be processed by uncrustify.

/* This software was developed by Bruce Hendrickson and Robert Leland   *
 * at Sandia National Laboratories under US Department of Energy        *
 * contract DE-AC04-76DP00789 and is copyrighted by Sandia Corporation. */

#include	<stdio.h>
#include	"../main/defs.h"
#include	"../main/params.h"
   

void genvals2d(xvecs, vals, nvtxs)
/* Create lists of sets of values to be sorted. */
double **xvecs;			/* vectors to partition */
double *vals[4][MAXSETS];	/* ptrs to lists of values */
int nvtxs;			/* number of values */
{
   extern Heap   *heap;     /* pointer to heap of multigrid */
   extern double *MEM_OK;   /* variable for memory overflow exeception */
   int nlists = 4;		/* number of lists to generate */
   double *temp[4];		/* place holders for vals */
   int i;			/* loop counter */

   for (i=0; i<nlists; i++) {
      temp[i] = (double *) (MEM_OK = smalloc((unsigned) nvtxs*sizeof(double));
      if (!MEM_OK) return;
   }

   for (i=1; i<=nvtxs; i++) {
      temp[0][i-1] = 4*xvecs[1][i];
      temp[1][i-1] = 4*xvecs[2][i];
      temp[2][i-1] = 4*(xvecs[1][i] + xvecs[2][i]);
      temp[3][i-1] = 4*(xvecs[2][i] - xvecs[1][i]);
   }

   vals[0][1] = vals[1][0] = vals[2][3] = vals[3][2] = temp[0];
   vals[0][2] = vals[2][0] = vals[1][3] = vals[3][1] = temp[1];
   vals[0][3] = vals[3][0] = temp[2];
   vals[1][2] = vals[2][1] = temp[3];
}
