// NOTE: The current revision of this file was left untouched when the DUNE source files were reindented!
// NOTE: It contained invalid syntax that could not be processed by uncrustify.

/* This software was developed by Bruce Hendrickson and Robert Leland   *
 * at Sandia National Laboratories under US Department of Energy        *
 * contract DE-AC04-76DP00789 and is copyrighted by Sandia Corporation. */

#include <stdio.h>
#include "../main/structs.h"
#include "../main/defs.h"

static struct ipairs *pedges;	/* perturbed edges */
static double *pvals;		/* perturbed values */

/* Inititialize the perturbation */
void perturb_init(n)
int n;		/* graph size at this level */
{
   extern Heap   *heap;     /* pointer to heap of multigrid */
   extern double *MEM_OK;   /* variable for memory overflow exeception */
   extern int NPERTURB;		/* number of edges to perturb */
   extern double PERTURB_MAX;	/* maximum perturbation */
   int i, j;			/* loop counter */
   double drandom();

   /* Initialize the diagonal perturbation weights */
   pedges = (struct ipairs *) (MEM_OK = smalloc((unsigned) NPERTURB*sizeof(struct ipairs));
   if (!MEM_OK) return;
   pvals = (double *) (MEM_OK = smalloc((unsigned) NPERTURB*sizeof(double));
   if (!MEM_OK) return;

   if (n <= 1) {
      for (i=0; i<NPERTURB; i++) {
         pedges[i].val1 = pedges[i].val2 = 0;
         pvals[i] = 0;
      }
      return;
   }

   for (i=0; i<NPERTURB; i++) {
      pedges[i].val1 = 1 + (n*drandom());

      /* Find another vertex to define an edge. */
      j = 1 + (n*drandom());
      while (j == i) j = 1 + (n*drandom());
      pedges[i].val2 = 1 + (n*drandom());

      pvals[i] = PERTURB_MAX * drandom();
   }
}

void perturb_clear()
{
   extern Heap   *heap;     /* pointer to heap of multigrid */
   

   sfree((char *) pedges);
   sfree((char *) pvals);
   pedges = NULL;
   pvals = NULL;
}


/* Modify the result of splarax to break any graph symmetry */
void perturb(result, vec)
double *result;			/* result of matrix-vector multiply */
double *vec;			/* vector matrix multiplies */
{
   extern int NPERTURB;		/* number of edges to perturb */
   int i;			/* loop counter */

   for (i=0; i<NPERTURB; i++) {
      result[pedges[i].val1] +=
	 pvals[i]*(vec[pedges[i].val2] - vec[pedges[i].val1]);
      result[pedges[i].val2] +=
	 pvals[i]*(vec[pedges[i].val1] - vec[pedges[i].val2]);
   }
}
