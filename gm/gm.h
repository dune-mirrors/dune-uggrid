// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:
/****************************************************************************/
/*																			*/
/* File:	  gm.h															*/
/*																			*/
/* Purpose:   grid manager header file (the heart of ug)					*/
/*																			*/
/* Author:	  Peter Bastian, Klaus Johannsen								*/
/*			  Interdisziplinaeres Zentrum fuer Wissenschaftliches Rechnen	*/
/*			  Universitaet Heidelberg										*/
/*			  Im Neuenheimer Feld 368										*/
/*			  6900 Heidelberg												*/
/*			  internet: ug@ica3.uni-stuttgart.de					*/
/*																			*/
/*			  Institut fuer Computeranwendungen III                                                 */
/*			  Universitaet Stuttgart										*/
/*			  Pfaffenwaldring 27											*/
/*			  70569 Stuttgart												*/
/*			  email: ug@ica3.uni-stuttgart.de							*/
/*																			*/
/*			  blockvector data structure:									*/
/*			  Christian Wrobel                                                                              */
/*			  Institut fuer Computeranwendungen III                                                 */
/*			  Universitaet Stuttgart										*/
/*			  Pfaffenwaldring 27											*/
/*			  70569 Stuttgart												*/
/*			  email: ug@ica3.uni-stuttgart.de					    */
/*																			*/
/* History:   09.03.92 begin, ug version 2.0  (as ugtypes2.h)				*/
/*			  13.12.94 begin, ug version 3.0								*/
/*			  27.09.95 blockvector implemented (Christian Wrobel)			*/
/*																			*/
/* Remarks:                                                                                                                             */
/*																			*/
/****************************************************************************/

/****************************************************************************/
/*																			*/
/* auto include mechanism and other include files							*/
/*																			*/
/****************************************************************************/

#ifndef __GM__
#define __GM__

#include <assert.h>
#include <limits.h>

#ifndef __COMPILER__
#include "compiler.h"
#endif

#ifndef __HEAPS__
#include "heaps.h"
#endif

#ifndef __UGENV__
#include "ugenv.h"
#endif

#ifndef __SWITCH__
#include "switch.h"
#endif

#ifndef __MISC__
#include "misc.h"
#endif

/****************************************************************************/
/*																			*/
/* defines in the following order											*/
/*																			*/
/*		  compile time constants defining static data size (i.e. arrays)	*/
/*		  other constants													*/
/*		  macros															*/
/*																			*/
/****************************************************************************/

/* necessary for most C runtime libraries */
#undef DOMAIN

/* some size parameters */
#define MAXLEVEL                                32      /* maximum depth of triangulation		*/
#define MAXOBJECTS                              32      /* use 5 bits for object identification */
#define MAXSELECTION                    100 /* max number of elements in selection	*/
#define TAGS                                    8       /* number of different element types	*/

/* some size macros for allocation purposes */
#define MAX_SIDES_OF_ELEM               4                       /* max number of sides of an elem	*/
#define MAX_EDGES_OF_ELEM               6                       /* max number of edges of an element*/
#define MAX_CORNERS_OF_ELEM             4                       /* max number of corners of an eleme*/
#define MAX_EDGES_OF_SIDE               3           /* max number of edges of a side	*/
#define MAX_CORNERS_OF_SIDE     3           /* max number of corners of a side  */
#define MAX_CORNERS_OF_EDGE             2                       /* an edge has always two corners.. */
#define MAX_SIDES_OF_EDGE               2                       /* two sides have one edge in common*/
#define MAX_SONS                        12          /* max number of sons of an element */

#define MAX_SIDES_TOUCHING              10                      /* max #fine sides touching a coarse*/

/* some useful variables */
typedef COORD COORD_VECTOR[DIM];
typedef COORD COORD_VECTOR_2D[2];
typedef COORD COORD_VECTOR_3D[3];
typedef DOUBLE DOUBLE_VECTOR[DIM];
typedef DOUBLE DOUBLE_VECTOR_2D[2];
typedef DOUBLE DOUBLE_VECTOR_3D[3];

/* boundary segment types */
#define PERIODIC                                1
#define NON_PERIODIC                    2

/* result codes of user supplied functions	0 = OK as usual */
#define OUT_OF_RANGE                    1       /* coordinate out of range				*/
#define CANNOT_INIT_PROBLEM     1       /* configProblem could not init problem */

/* some numbers for algebra */
#define MAXMATRICES             ((MAXVECTORS*(MAXVECTORS+1))/2) /* max number of diff. matrix types */
#define MAXCONNECTIONS  (MAXMATRICES + MAXVECTORS)              /* max number of diff. connections  */

/* constants for blockvector description (BVD) */
#define NO_BLOCKVECTOR ((BLOCKNUMBER) ~0)        /* number for "there is no blockvector"; largest number of type BLOCKNUMBER */
#define MAX_BV_NUMBER (NO_BLOCKVECTOR - 1)      /* largest admissible blockvector number */
#define MAX_BV_LEVEL UCHAR_MAX                          /* largest admissible blockvector level number */
#define BVD_MAX_ENTRIES (sizeof(BVD_ENTRY_TYPE)*CHAR_BIT)       /* maximum number
                                                                   of entries in a BVD; NOTE: the actual available
                                                                   number of entries depends on the range of each entry */

/* constants for BLOCKVECTOR */
#define BVDOWNTYPEVECTOR        0       /* symbolic value for BVDOWNTYPE */
#define BVDOWNTYPEBV            1       /* symbolic value for BVDOWNTYPE */

/* use of GSTATUS (for grids), use power of 2 */
#define GRID_CHANGED                    1
#define GRID_ASSEMBLED                  2
#define GRID_FULLACTIVE                 4

/* selection mode */
#define nodeSelection                   1                       /* objects selected are nodes			*/
#define elementSelection                2                       /* objects selected are elements		*/
#define vectorSelection                 3                       /* objects selected are vectors			*/

/* possible values for rule in MarkForRefinement */
#define NO_REFINEMENT           0
#define COPY                            1
#define RED                             2
#define BLUE                            3
#define BISECTION_1             4
#define BISECTION_2_Q           5
#define BISECTION_2_T1          6
#define BISECTION_2_T2          7
#define BISECTION_3             8
#define UNREFINE                        9

/* values for element class */
#define COPY_CLASS              0
#define IRREGULAR_CLASS 1
#define REGULAR_CLASS   2
#define YELLOW                  0
#define GREEN                   1

/* macros for the control word management									*/
#define MAX_CONTROL_WORDS       20              /* maximum number of control words		*/
#define MAX_CONTROL_ENTRIES 80          /* max number of entries				*/

/* macros for the multigrid user data space management						*/
#define OFFSET_IN_MGUD(id)              (GetMGUDBlockDescriptor(id)->offset)
#define IS_MGUDBLOCK_DEF(id)    (GetMGUDBlockDescriptor(id)!=NULL)

/****************************************************************************/
/*																			*/
/* domain definition data structures										*/
/*																			*/
/****************************************************************************/

/*----------- typedef for functions ----------------------------------------*/

typedef INT (*BndSegFuncPtr)(void *,COORD *,COORD *);


/*----------- definition of structs ----------------------------------------*/

struct domain {

  /* fields for environment directory */
  ENVDIR d;

  /* domain variables */
  COORD MidPoint[DIM];                                  /* point in the middle of domain	*/
  COORD radius;                                                 /* defines sphere around MidPoint	*/
  /* containing the domain			*/
  INT numOfSegments;                                            /* number of boundary segments		*/
  INT numOfCorners;                                             /* number of corner points			*/
  INT domConvex;                                                /* is the domain convex?			*/
} ;

struct boundary_segment {

  /* fields for environment directory */
  ENVVAR v;

  /* fields for boundary segment */
  INT left,right;                                         /* number of left and right subdomain */
  INT id;                                                         /* unique id of that segment			*/
  INT segType;                                            /* segment type, see above			*/
  INT points[CORNERS_OF_BND_SEG];         /* numbers of the vertices (ID)		*/
  INT resolution;                                         /* measure for the curvature			*/
  COORD alpha[DIM_OF_BND],beta[DIM_OF_BND];               /* parameter interval used*/
  BndSegFuncPtr BndSegFunc;                       /* pointer to definition function     */
  void *data;                                             /* can be used by applic to find data */
} ;

/****************************************************************************/
/*																			*/
/* problem data structure													*/
/*																			*/
/****************************************************************************/

/*----------- typedef for functions ----------------------------------------*/

#ifdef __version23__
typedef INT (*BndCondProcPtr)(void *, DOUBLE *, DOUBLE *, INT *);
#else
typedef INT (*BndCondProcPtr)(void *, void *, COORD *, DOUBLE *, INT *);
#endif
typedef INT (*CoeffProcPtr)(COORD *, DOUBLE *);
typedef INT (*UserProcPtr)(DOUBLE *, DOUBLE *);
typedef INT (*ConfigProcPtr)(INT argc, char **argv);


/*----------- definition of structs ----------------------------------------*/

struct problem {

  /* fields for environment directory */
  ENVDIR d;

  /* fields for problem */
  INT problemID;                                /* used to identify problem type			*/
  ConfigProcPtr ConfigProblem;      /* procedure to reinitialize problem		*/
  INT numOfCoeffFct;                            /* # of coefficient functions				*/
  INT numOfUserFct;                             /* # of User functions						*/
  void * CU_ProcPtr[1];                 /* coefficient functions					*/
};

struct bndcond {

  /* fields for environment variable */
  ENVVAR v;

  /* fields for boundary condition */
  INT id;                                               /* corresponds to boundary segment id !         */
  BndCondProcPtr BndCond;               /* function defining boundary condition         */
  void *data;                                   /* additional data for bnd cond                         */
} ;

/****************************************************************************/
/*																			*/
/* format definition data structure                                                                             */
/*																			*/
/****************************************************************************/

/*----------- typedef for functions ----------------------------------------*/

typedef INT (*ConversionProcPtr)(void *, const char *, char *);


/*----------- definition of structs ----------------------------------------*/

struct format {

  /* fields for enironment variable */
  ENVDIR d;

  /* variables of format */
#ifdef __version23__
  INT sVertex;                           /* size of vertex user data structure in bytes */
  INT sNode;                                     /* size of node user data structure in bytes	*/
  INT sDiag;                                     /* size of diagonal user data structure in byt */
  INT sElement;                          /* size of element user data structure in bytes*/
  INT sLink;                                     /* size of link user data structure in bytes	*/
  INT sEdge;                                     /* size of edge user data structure in bytes	*/
  INT sMultiGrid;                        /* size of multigrid user data structure in byt*/

  ConversionProcPtr SaveVertex;                 /* write user data to string		*/
  ConversionProcPtr SaveNode;
  ConversionProcPtr SaveDiag;
  ConversionProcPtr SaveElement;
  ConversionProcPtr SaveLink;
  ConversionProcPtr SaveEdge;
  ConversionProcPtr SaveGrid;
  ConversionProcPtr SaveMultiGrid;

  ConversionProcPtr LoadVertex;                 /* read user data from string		*/
  ConversionProcPtr LoadNode;
  ConversionProcPtr LoadDiag;
  ConversionProcPtr LoadElement;
  ConversionProcPtr LoadLink;
  ConversionProcPtr LoadEdge;
  ConversionProcPtr LoadGrid;
  ConversionProcPtr LoadMultiGrid;

  ConversionProcPtr PrintVertex;                /* print user data to string		*/
  ConversionProcPtr PrintNode;
  ConversionProcPtr PrintDiag;
  ConversionProcPtr PrintElement;
  ConversionProcPtr PrintLink;
  ConversionProcPtr PrintEdge;
  ConversionProcPtr PrintGrid;
  ConversionProcPtr PrintMultiGrid;
#else
  INT sVertex;                                                  /* size of vertex user data struc. in bytes */
  INT sMultiGrid;                                               /* size of mg user data structure in bytes	*/
  INT VectorSizes[MAXVECTORS];                  /* number of doubles in vectors                         */
  INT MatrixSizes[MAXMATRICES];                 /* number of doubles in matrices			*/
  INT ConnectionDepth[MAXMATRICES];             /* depth of connection for matrices             */
  INT MaxConnectionDepth;                               /* maximal connection depth                             */
  INT NeighborhoodDepth;                                /* geometrical depth corresponding			*/
  /* algebraic con with depth 1				*/

  ConversionProcPtr PrintVertex;                /* print user data to string				*/
  ConversionProcPtr PrintGrid;
  ConversionProcPtr PrintMultigrid;
  ConversionProcPtr PrintVector[MAXVECTORS];
  ConversionProcPtr PrintMatrix[MAXVECTORS][MAXVECTORS];
#endif
} ;

#ifdef __version3__
typedef struct {
  int pos;                                              /* which position is described here             */
  int size;                                             /* data size in bytes						*/
  ConversionProcPtr print;              /* function to print data					*/
} VectorDescriptor ;

typedef struct {
  int from;                                             /* This connection goes from position from	*/
  int to;                                               /* to position to							*/
  int size;                                             /* with size bytes per connection			*/
  int depth;                                            /* connect with depth in dual graph             */
  ConversionProcPtr print;              /* function to print data					*/
} MatrixDescriptor ;
#endif

/****************************************************************************/
/*																			*/
/* matrix/vector/blockvector data structure									*/
/*																			*/
/****************************************************************************/

#ifdef __version23__
typedef int VECTOR;
typedef int MATRIX;
#endif

#ifdef __version3__

/* data structure for BlockvectorDescription */
typedef unsigned INT BVD_ENTRY_TYPE;    /* memory providing storage for level numbers */
typedef unsigned SHORT BLOCKNUMBER;     /* valid numbers are 0..MAX_BV_NUMBER */
typedef unsigned char BLOCKLEVEL;       /* valid levels are 0..MAX_BV_LEVEL */

struct blockvector_description_format           /* describes how a struct of type
                                                                                           blockvector_description is to
                                                                                           be interpreted				*/
{
  INT bits;                                                                       /* bits per blocknumber entry */
  BLOCKLEVEL max_level;                                                   /* max. number of entries	*/
  BVD_ENTRY_TYPE level_mask[BVD_MAX_ENTRIES];
  /* level_mask[i] = mask entries for levels 0..i		*/
  BVD_ENTRY_TYPE neg_digit_mask[BVD_MAX_ENTRIES];
  /* neg_digit_mask[i] = masks out entry for level i	*/
};
typedef struct blockvector_description_format BV_DESC_FORMAT;

struct blockvector_description  /* describes the position of a blockvector	*/
{                                                               /* in a hierarchy of blockvectors			*/
  BVD_ENTRY_TYPE entry;                 /* sequence of block levels	according to	*/
  /* a certain blockvector_description_format	*/
  BLOCKLEVEL current;                           /* levels 0..current-1 currently valid		*/
  BLOCKLEVEL read;                              /* level read is next to be read			*/
};
typedef struct blockvector_description BV_DESC;

struct vector {
  unsigned INT control;                         /* object identification, various flags */
  union geom_object *object;                    /* associated object					*/

  struct vector *pred,*succ;                    /* double linked list of vectors		*/

  unsigned INT index;                           /* ordering of unknowns                                 */
  unsigned INT skip;                                    /* used bitwise to skip unknowns		*/
  struct matrix *start;                         /* implements matrix					*/

  BV_DESC block_descr;                          /* membership to the blockvector levels	*/

  /* user data */
  DOUBLE value[1];                                      /* array of doubles                                     */
};
typedef struct vector VECTOR;

struct matrix {
  unsigned INT control;                         /* object identification, various flags */

  struct matrix *next;                          /* row list                                                     */
  struct vector *vect;                          /* destination vector					*/

  /* user data */
  DOUBLE value[1];                                      /* array of doubles                                     */
};
typedef struct matrix MATRIX;
typedef struct matrix CONNECTION;

struct blockvector
{
  unsigned INT control;                         /* object identification, various flags	  */

  BLOCKNUMBER number;                                   /* logical blockvectornumber			  */
  struct blockvector *pred,*succ;       /* double linked list of blockvectors	  */
  VECTOR *first_vec;                                    /* start vector of this blockvector       */
  VECTOR *end_vec;                                      /* succ. of the last vector of this blockv*/

  struct blockvector *first_son;        /* start of blockvector list on next level*/
  struct blockvector *last_son;         /* end of blockvector list on next level  */
};
typedef struct blockvector BLOCKVECTOR;

#endif

/****************************************************************************/
/*																			*/
/* unstructured grid data structures										*/
/*																			*/
/****************************************************************************/

/*----------- typedef for functions ----------------------------------------*/


/*----------- definition of structs ----------------------------------------*/

struct ivertex {                                        /* inner vertex structure				*/

  /* variables */
  unsigned INT control;                         /* object identification, various flags */
  INT id;                                                       /* unique id used for load/store		*/
  COORD x[DIM];                                         /* vertex position						*/
  COORD xi[DIM];                                        /* local coordinates in father element	*/

  /* pointers */
  union vertex *pred,*succ;                     /* double linked list of vertices		*/
  void *data;                                           /* associated user data structure		*/
  union element *father;                        /* father element						*/
  struct node *topnode;                         /* highest node where defect is valid	*/
} ;


struct vsegment {

  unsigned INT control;                         /* object identification, various flags */
  struct bndsegdesc *segdesc;           /* pointer to boundary segment desc		*/
  COORD lambda[DIM_OF_BND];                     /* position of vertex on boundary segmen*/

        #ifdef __TWODIM__
  COORD zeta;                                           /* local coord. of vertex  in father edg*/
  /* Def.: l = (1-z)*l0 + z*l1			*/
        #endif

  struct vsegment *next;
};


struct bvertex {                                        /* boundary vertex structure			*/

  /* variables */
  unsigned INT control;                         /* object identification, various flags */
  INT id;                                                       /* unique id used for load/store		*/
  COORD x[DIM];                                         /* vertex position						*/
  COORD xi[DIM];                                        /* local coordinates in father element	*/

  /* pointers */
  union vertex *pred,*succ;                     /* double linked list of vertices		*/
  void *data;                                           /* associated user data structure		*/
  union element *father;                        /* father element						*/
  struct node *topnode;                         /* highest node where defect is valid	*/

  struct vsegment *vseg;                        /* pointer to chain of segments                 */
} ;

union vertex {                                          /* only used to define pointer to vertex*/
  struct ivertex iv;
  struct bvertex bv;
} ;


struct node {                                           /* level dependent part of a vertex     */

  /* variables */
  unsigned INT control;                         /* object identification, various flags */
  INT id;                                                       /* unique id used for load/store		*/
  INT index;                                                    /* discrete coordinates for ordering	*/

  /* compatibility mode */
        #ifdef __version23__
  unsigned SHORT vskip;                         /* used bitwise for unkowns in myvertex */
  unsigned SHORT nskip;                         /* used bitwise for unknowns in node	*/
        #endif

  /* pointers */
  struct node *pred,*succ;                      /* double linked list of nodes per level*/
  struct link *start;                           /* list of links						*/
  struct node *father;                          /* node on coarser level (NULL if none) */
  struct node *son;                                     /* node on finer level (NULL if none)	*/
  union vertex *myvertex;                       /* corresponding vertex structure		*/

  /* associated vector if */
        #ifdef __NODEDATA__
  VECTOR *vector;                                       /* associated vector					*/
        #endif

        #ifdef __version23__
  void *data;                                           /* associated user data                                 */
  void *matelem;                                        /* diagonal coefficient (as link data)	*/
        #endif
} ;

struct link {

  /* variables */
  unsigned INT control;                         /* object identification, various flags */
  struct link *next;                                    /* ptr to next link                                     */
  struct node *nbnode;                          /* ptr to neighbor node                                 */

  /* compatibility mode */
        #ifdef __version23__
  void *matelem;                                        /* associated user data structure		*/
        #endif
} ;


struct edge {                                           /* undirected edge of the grid graph	*/

  /* variables */
  struct link links[2];                         /* two links							*/

        #ifdef __MIDNODE__
  struct node *midnode;                         /* pointer to mid node on next finer gri*/
        #endif

  /* associated vector if */
        #ifdef __EDGEDATA__
  VECTOR *vector;                                       /* associated vector					*/
        #endif

  /* compatibility mode */
        #ifdef __version23__
  void *data;                                           /* associated user data structure		*/
        #endif
} ;


struct generic_element {            /* no difference between inner and bndel*/

  /* variables */
  unsigned INT control;             /* object identification, various flags */
  INT id;                           /* unique id used for load/store        */
  unsigned INT flag;                /* additional flags for elements        */
  unsigned INT property;                        /* we need more bits ...				*/

  /* pointers */
  union element *pred, *succ;       /* double linked list of elements       */
  void *refs[1];                                        /* variable length array managed by ug  */
} ;

struct triangle {

  /* variables */
  unsigned INT control;                         /* object identification, various flags */
  INT id;                                                       /* unique id used for load/store		*/
  unsigned INT flag;                            /* additional flags for elements		*/
  unsigned INT property;                        /* we need more bits ...				*/

  /* pointers */
  union element *pred, *succ;           /* double linked list of elements		*/
  struct node *n[3];                                    /* corners of that element				*/
  union element *father;                        /* father element on coarser grid		*/
  union element *sons[4];                       /* element tree                                                 */
  union element *nb[3];                         /* dual graph							*/

  /* associated vector if */
        #ifdef __ELEMDATA__
  VECTOR *vector;                                       /* associated vector					*/
        #endif

  /* compatibility mode */
        #ifdef __version23__
  void *data;                                           /* associated user data structure		*/
        #endif

  struct elementside *side[3];          /* only on bnd, NULL if interior side	*/
} ;

struct quadrilateral {

  /* variables */
  unsigned INT control;                         /* object identification, various flags */
  INT id;                                                       /* unique id used for load/store		*/
  unsigned INT flag;                            /* additional flags for elements		*/
  unsigned INT property;                        /* we need more bits ...				*/

  /* pointers */
  union element *pred, *succ;           /* double linked list of elements		*/
  struct node *n[4];                                    /* corners of that element				*/
  union element *father;                        /* father element on coarser grid		*/
  union element *sons[4];                       /* element tree                                                 */
  union element *nb[4];                         /* dual graph							*/

  /* associated vector if */
        #ifdef __ELEMDATA__
  VECTOR *vector;                                       /* associated vector					*/
        #endif

  /* compatibility mode */
        #ifdef __version23__
  void *data;                                           /* associated user data structure		*/
        #endif

  struct elementside *side[4];          /* only on bnd, NULL if interior side	*/
} ;

struct tetrahedron {

  /* variables */
  unsigned INT control;                         /* object identification, various flags */
  INT id;                                                       /* unique id used for load/store		*/
  unsigned INT flag;                            /* additional flags for elements		*/
  unsigned INT property;                        /* we need more bits ...				*/

  /* pointers */
  union element *pred, *succ;           /* double linked list of elements		*/
  struct node *n[4];                                    /* corners of that element				*/
  union element *father;                        /* father element on coarser grid		*/
  union element *sons[1];                       /* element tree                                                 */
  union element *nb[4];                         /* dual graph							*/

  /* associated vector if */
        #ifdef __ELEMDATA__
  VECTOR *vector;                                       /* associated vector					*/
        #endif

  /* associated vector if */
        #ifdef __SIDEDATA__
  VECTOR *sidevector[4];                        /* associated vectors for sides			*/
        #endif

  struct elementside *side[4];          /* only on bnd, NULL if interior side	*/
} ;

union element {
  struct generic_element ge;
  struct triangle tr;
  struct quadrilateral qu;
  struct tetrahedron te;
} ;

struct elementside {

  /* variables */
  unsigned INT control;                                         /* object identification, various flags */
  struct bndsegdesc *segdesc;                           /* pointer to boundary segment desc		*/
  COORD lambda[MAX_CORNERS_OF_SIDE]                     /* parameter of side corners			*/
  [DIM_OF_BND];
  /* pointers */
  struct elementside *pred,*succ;                       /* double linked list					*/
} ;

struct bndsegdesc {                                             /* descriptor for one boundary segment	*/
  unsigned INT control;                                         /* object identification, various flags */
  INT id;                                                                       /* unique id used for load/store		*/
  struct boundary_segment *theSegment;          /* (1) coordinate definition			*/
  struct bndcond *theBoundaryCondition;         /* (2) boundary condition definition	*/
} ;

union geom_object {                                             /* objects that can hold a vector		*/
  struct node nd;
  struct edge ed;
  union element el;
} ;

union selection_object {                                        /* objects than can be selected			*/
  struct node nd;
  union element el;
  struct vector ve;
} ;

struct grid {

  /* variables */
  unsigned INT control;                         /* object identification, various flags */
  INT status;                                           /* possible values see defines above	*/
  INT level;                                                    /* level of that grid					*/
  INT nVert;                                                    /* number of vertices					*/
  INT nNode;                                                    /* number of nodes						*/
  INT nElem;                                                    /* number of elements					*/
  INT nEdge;                                                    /* number of edges						*/
  INT nVector;                                          /* number of vectors					*/
  INT nCon;                                                     /* number of Connections				*/
  INT nSide;                                                    /* number of element sides				*/

  /* pointers */
  union  element *elements;                     /* pointer to first element                     */
  union  element *lastelement;          /* pointer to last element				*/
  union  vertex *vertices;                      /* pointer to first vertex				*/
  union  vertex *lastvertex;                    /* pointer to last vertex				*/
  struct elementside *sides;                    /* pointer to first boundary side		*/
  struct node *firstNode;                       /* pointer to first node				*/
  struct node *lastNode;                        /* pointer to last node                                 */
  VECTOR *firstVector;                          /* pointer to first vector				*/
  VECTOR *lastVector;                           /* pointer to last vector				*/
  BLOCKVECTOR *firstblockvector;        /* pointer to the first blockvector		*/
  BLOCKVECTOR *lastblockvector;         /* pointer to the last blockvector		*/
  struct grid *coarser, *finer;         /* coarser and finer grids				*/
  struct multigrid *mg;                         /* corresponding multigrid structure	*/
} ;

struct multigrid {

  /* env item */
  ENVVAR v;

  /* variables */
  INT status;                                           /* possible values, see above			*/
  INT vertIdCounter;                                    /* count objects in that multigrid		*/
  INT nodeIdCounter;                                    /* count objects in that multigrid		*/
  INT elemIdCounter;                                    /* count objects in that multigrid		*/
  INT topLevel;                                         /* depth of the element tree			*/
  INT currentLevel;                                     /* level we are working on				*/
  struct domain *theDomain;                     /* pointer to domain definition                 */
  struct format *theFormat;                     /* pointer to format definition                 */
  struct problem *theProblem;           /* pointer to problem definition		*/
  struct bndsegdesc *segments;          /* array of combined boundary descriptio*/
  INT numOfSegments;                                    /* number of entries in the array above */
  INT numOfSubdomains;                          /* number of subdomains		            */
  union vertex **corners;                       /* pointer to array of pointers to corne*/
  INT numOfCorners;                                     /* number of entries in the array above */
  HEAP *theHeap;                                        /* associated heap structure			*/

  /* pointers */
  struct grid *grids[MAXLEVEL];         /* pointers to the grids				*/
  void *freeObjects[MAXOBJECTS];        /* pointer to allocated but unused objs */
  void *freeVectors[MAXVECTORS];        /* pointer to allocated but unused objs */
  void *freeConnections[MAXCONNECTIONS];      /* ptr to alloc. but unused objs	*/

  /* selection */
  INT NbOfSelections;                           /* number of selected objects			*/
  INT SelectionMode;                                    /* selectionmode (see above)			*/
  union selection_object *Selection[MAXSELECTION];       /* pointer to selec obj*/

  /* user data */
  void *GenData;                                        /* general user data space				*/
  HEAP *UserHeap;                                       /* user heap							*/
} ;

/****************************************************************************/
/*																			*/
/*					typedef for structs                                                                     */
/*																			*/
/****************************************************************************/

/* geometrical part */
typedef struct domain DOMAIN;
typedef struct boundary_segment BOUNDARY_SEGMENT;

typedef struct problem PROBLEM ;
typedef struct bndcond BOUNDARY_CONDITION ;

typedef struct format FORMAT;

typedef union  vertex VERTEX;
typedef struct vsegment VSEGMENT;
typedef struct node NODE;
typedef union  element ELEMENT;
typedef struct elementside ELEMENTSIDE;
typedef struct bndsegdesc BNDSEGDESC;
typedef struct link LINK;
typedef struct edge EDGE;
typedef union  geom_object GEOM_OBJECT;
typedef union  selection_object SELECTION_OBJECT;
typedef struct grid GRID;
typedef struct multigrid MULTIGRID;

/****************************************************************************/
/*																			*/
/*					structs for evaluation functions						*/
/*																			*/
/****************************************************************************/

/*----------- typedef for functions ----------------------------------------*/

typedef INT (*PreprocessingProcPtr)(const char *, MULTIGRID *);
typedef DOUBLE (*ElementEvalProcPtr)(const ELEMENT *,const COORD **,COORD *);
typedef void (*ElementVectorProcPtr)(const ELEMENT *,const COORD **,COORD *,DOUBLE *);
typedef DOUBLE (*MatrixEvalProcPtr)(const MATRIX *);

/*----------- definition of structs ----------------------------------------*/

struct elementvalues {

  /* fields for enironment list variable */
  ENVVAR v;

  PreprocessingProcPtr PreprocessProc;                  /* prepare eval values					*/
  ElementEvalProcPtr EvalProc;                                  /* pointer to corresponding function	*/
} ;

struct elementvector {

  /* fields for enironment list variable */
  ENVVAR v;

  PreprocessingProcPtr PreprocessProc;                  /* prepare eval values					*/
  ElementVectorProcPtr EvalProc;                                /* pointer to corresponding function	*/
  int dimension;                                                                /* dimension of result vector			*/
} ;

struct matrixvalues {

  /* fields for enironment list variable */
  ENVVAR v;

  PreprocessingProcPtr PreprocessProc;                  /* prepare eval values					*/
  MatrixEvalProcPtr EvalProc;                                   /* pointer to corresponding function	*/
} ;

typedef struct elementvalues EVALUES ;
typedef struct elementvector EVECTOR ;
typedef struct matrixvalues MVALUES ;

/****************************************************************************/
/*																			*/
/* algebraic dependency for vector ordering                                                             */
/*																			*/
/****************************************************************************/

typedef INT (*DependencyProcPtr)(GRID *, const char *);

struct AlgebraicDependency {

  /* fields for enironment list variable */
  ENVVAR v;

  DependencyProcPtr DependencyProc;             /* pointer to dependency function			*/
} ;

typedef struct AlgebraicDependency ALG_DEP;

/****************************************************************************/
/*																			*/
/* find cut for vector ordering				                                                                */
/*																			*/
/****************************************************************************/

typedef VECTOR *(*FindCutProcPtr)(GRID *, VECTOR *, INT *);

typedef struct {

  /* fields for enironment list variable */
  ENVVAR v;

  FindCutProcPtr FindCutProc;           /* pointer to find cut function				*/

} FIND_CUT;

/****************************************************************************/
/*																			*/
/*					dynamic management of control words                                     */
/*																			*/
/****************************************************************************/

/* description of a control word */
typedef struct {
  unsigned INT offset_in_object ;               /* where in object is it ?			*/
  unsigned INT used_mask ;                              /* used bits						*/
} CONTROL_WORD ;

/* manage part of a control word */
typedef struct {
  INT used;                                                             /* this struct is used				*/
  INT control_word ;                                            /* pointer to corresponding controlw*/
  INT offset_in_word;                                   /* shift in control word			*/
  INT length;                                                   /* number of bits used				*/
  unsigned INT offset_in_object;                /* copy from control word (faster)	*/
  unsigned INT mask;                                            /* 1 where bits are used			*/
  unsigned INT xor_mask;                                /* 0 where bits are used			*/
} CONTROL_ENTRY ;

#ifndef __COMPILE_CW__
extern CONTROL_WORD
  control_words[MAX_CONTROL_WORDS];             /* global array with descriptions	*/
extern CONTROL_ENTRY
  control_entries[MAX_CONTROL_ENTRIES];      /* predefined control words		*/
#endif

/* general query macros */

#define ControlWord(p,ce)  (((unsigned INT *)(p))[control_entries[ce].offset_in_object])
#define CW_READ(p,ce)      ((ControlWord(p,ce) & control_entries[ce].mask)>>control_entries[ce].offset_in_word)
#define CW_WRITE(p,ce,n)   ControlWord(p,ce) = (ControlWord(p,ce)&control_entries[ce].xor_mask)|(((n)<<control_entries[ce].offset_in_word)&control_entries[ce].mask)


/****************************************************************************/
/*																			*/
/* Macro definitions for algebra structures                                                             */
/*																			*/
/*																			*/
/* Use of the control word:                                                                                             */
/*																			*/
/* macro name|bits	|V|M|use												*/
/*																			*/
/* all objects:                                                                                                                         */
/*																			*/
/* vectors:                                                                                                                             */
/* VTYPE	 |0 - 1 |*| | node-,edge-,side- or elemvector					*/
/* VECTORSIDE|7 - 8 |*| | nb of side the side vect corr. to (in object elem)*/
/* VCLASS	 |11-12 |*| | class of v. (3: if corr. to red/green elem)		*/
/*					  (2: if corr. to first algebraic nb.)					*/
/*					  (1: if corr. to second algebraic nb.)                                 */
/* VDATATYPE |13-16 |*| | data type used bitwise							*/
/* VNCLASS	 |17-18 |*| | type of elem on finer grid the v. lies geom. in:	*/
/*							0: no elem on finer grid						*/
/*							1: elem of 'second alg. nbhood' only			*/
/*							2: elem of 'first alg. nbhood' only                     */
/*							3: red or green elem							*/
/* VNEW          |19	|*| | 1 if vector is new								*/
/* VCNEW	 |20	|*| | 1 if vector has a new connection					*/
/*																			*/
/* matrices:																*/
/* MOFFSET	 |0     | |*| 0 if first matrix in connection else 1			*/
/* MROOTTYPE |1 - 2 | |*| VTYPE of root vector								*/
/* MDESTTYPE |3 - 4 | |*| VTYPE of destination vector						*/
/* MDIAG	 |5     | |*| 1 if diagonal matrix element						*/
/* MTYPE	 |6 -11 | |*| one of the (different sized) 10+4 matricees		*/
/* MUSED	 |12	| |*| general purpose flag								*/
/* MSIZE	 |13-27 | |*| size of the matrix in bytes						*/
/* MNEW          |28	| |*| 1 if matrix/connection is new                                     */
/* CEXTRA	 |29	| |*| 1 if is extra connection							*/
/*																			*/
/* Use of the control word in 'BLOCKVECTOR':									*/
/* BVDOWNTYPE 0	 BVDOWNTYPEVECTOR if the down component points to a vector,	*/
/*				 BVDOWNTYPEBV if it points to a further blockvector (son)	*/
/*																			*/
/****************************************************************************/

#ifdef __version3__

/****************************************************************************/
/*																			*/
/* general macros															*/
/*																			*/
/****************************************************************************/

#define NEW_DEFECT(v)    (VCLASS(v)>=2)
#define FINE_GRID_DOF(v) ((VCLASS(v)>=2)&&(VNCLASS(v)<=1))

/****************************************************************************/
/*																			*/
/* macros for VECTORs														*/
/*																			*/
/****************************************************************************/

/* control word identifier */
#define VECTOR_CW                                       0
#define VECTOR_OFFSET                           0

/* predefined control word entries */
#define VTYPE_CE                                        0
#define VTYPE_SHIFT                             0
#define VTYPE_LEN                                       2
#define VTYPE(p)                                        CW_READ(p,VTYPE_CE)
#define SETVTYPE(p,n)                           CW_WRITE(p,VTYPE_CE,n)

#define VCUSED_CE                                       64
#define VCUSED_SHIFT                            4
#define VCUSED_LEN                                      1
#define VCUSED(p)                                       CW_READ(p,VCUSED_CE)
#define SETVCUSED(p,n)                          CW_WRITE(p,VCUSED_CE,n)

#define VCOUNT_CE                                       2
#define VCOUNT_SHIFT                            5
#define VCOUNT_LEN                                      2
#define VCOUNT(p)                                       CW_READ(p,VCOUNT_CE)
#define SETVCOUNT(p,n)                          CW_WRITE(p,VCOUNT_CE,n)

#define VECTORSIDE_CE                           3
#define VECTORSIDE_SHIFT                        7
#define VECTORSIDE_LEN                          2
#define VECTORSIDE(p)                           CW_READ(p,VECTORSIDE_CE)
#define SETVECTORSIDE(p,n)                      CW_WRITE(p,VECTORSIDE_CE,n)

#define VCLASS_CE                                       4
#define VCLASS_SHIFT                            11
#define VCLASS_LEN                                      2
#define VCLASS(p)                                       CW_READ(p,VCLASS_CE)
#define SETVCLASS(p,n)                          CW_WRITE(p,VCLASS_CE,n)

#define VDATATYPE_CE                            5
#define VDATATYPE_SHIFT                         13
#define VDATATYPE_LEN                           4
#define VDATATYPE(p)                            CW_READ(p,VDATATYPE_CE)
#define SETVDATATYPE(p,n)                       CW_WRITE(p,VDATATYPE_CE,n)

#define VNCLASS_CE                                      6
#define VNCLASS_SHIFT                           17
#define VNCLASS_LEN                             2
#define VNCLASS(p)                                      CW_READ(p,VNCLASS_CE)
#define SETVNCLASS(p,n)                         CW_WRITE(p,VNCLASS_CE,n)

#define VNEW_CE                                         7
#define VNEW_SHIFT                                      19
#define VNEW_LEN                                        1
#define VNEW(p)                                         CW_READ(p,VNEW_CE)
#define SETVNEW(p,n)                            CW_WRITE(p,VNEW_CE,n)

#define VCNEW_CE                                        8
#define VCNEW_SHIFT                             20
#define VCNEW_LEN                                       1
#define VCNEW(p)                                        CW_READ(p,VCNEW_CE)
#define SETVCNEW(p,n)                           CW_WRITE(p,VCNEW_CE,n)

#define VCNB_CE                                         65
#define VCNB_SHIFT                                      21
#define VCNB_LEN                                        5
#define VCNB(p)                                         CW_READ(p,VCNB_CE)
#define SETVCNB(p,n)                            CW_WRITE(p,VCNB_CE,n)

#define VOBJECT(v)                                      ((v)->object)
#define PREDVC(v)                                       ((v)->pred)
#define SUCCVC(v)                                       ((v)->succ)
#define VINDEX(v)                                       ((v)->index)
#define VSKIPME(v,n)                            ((((v)->skip)>>n) & 1)
#define VVECSKIP(v,n)                           ((((v)->skip)>>n) & 15)
#define VFULLSKIP(v,n)                          (VVECSKIP(v,n)==15)
#define SETVSKIPME(v,n)                         (((v)->skip=n))
#define VECSKIP(v)                                      ((v)->skip)
#define VECSKIPBIT(v,n)                         (((v)->skip) & (1<<n))
#define SETVECSKIPBIT(v,n)                      (v)->skip = ((v)->skip & (~(1<<n))) | (1<<n)
#define VSTART(v)                                       ((v)->start)
#define VVALUE(v,n)                             ((v)->value[n])
#define VVALUEPTR(v,n)                          (&((v)->value[n]))
#define VMYNODE(v)                                      ((NODE*)((v)->object))
#define VMYEDGE(v)                                      ((EDGE*)((v)->object))
#define VMYELEMENT(v)                           ((ELEMENT*)((v)->object))
#define VUP(p)                                          LoWrd(VINDEX(p))
#define SETVUP(p,n)                             SetLoWrd(VINDEX(p),n)
#define VDOWN(p)                                        HiWrd(VINDEX(p))
#define SETVDOWN(p,n)                           SetHiWrd(VINDEX(p),n)
#define VBVD(v)                                         ((v)->block_descr)
#define VMATCH(v,bvd,bvdf)                      BVD_IS_SUB_BLOCK( &(v)->block_descr, bvd, bvdf )

/* user for nodes, edges and elements */
#define CAST_NVECTOR(p)                         NVECTOR(p)
#define CAST_EDVECTOR(p)                        EDVECTOR(p)
#define CAST_SVECTOR(p,i)                       SVECTOR(p,i)
#define CAST_EVECTOR(p)                         EVECTOR(p)


/****************************************************************************/
/*																			*/
/* macros for MATRIXs														*/
/*																			*/
/****************************************************************************/

/* control word identifier */
#define MATRIX_CW                                       1
#define MATRIX_OFFSET                           0

#define MOFFSET_CE                                      9
#define MOFFSET_SHIFT                           0
#define MOFFSET_LEN                             1
#define MOFFSET(p)                                      CW_READ(p,MOFFSET_CE)
#define SETMOFFSET(p,n)                         CW_WRITE(p,MOFFSET_CE,n)

#define MROOTTYPE_CE                            10
#define MROOTTYPE_SHIFT                         1
#define MROOTTYPE_LEN                           2
#define MROOTTYPE(p)                            CW_READ(p,MROOTTYPE_CE)
#define SETMROOTTYPE(p,n)                       CW_WRITE(p,MROOTTYPE_CE,n)

#define MDESTTYPE_CE                            11
#define MDESTTYPE_SHIFT                         3
#define MDESTTYPE_LEN                           2
#define MDESTTYPE(p)                            CW_READ(p,MDESTTYPE_CE)
#define SETMDESTTYPE(p,n)                       CW_WRITE(p,MDESTTYPE_CE,n)

#define MDIAG_CE                                        12
#define MDIAG_SHIFT                             5
#define MDIAG_LEN                                       1
#define MDIAG(p)                                        CW_READ(p,MDIAG_CE)
#define SETMDIAG(p,n)                           CW_WRITE(p,MDIAG_CE,n)

#define MTYPE_CE                                        13
#define MTYPE_SHIFT                             6
#define MTYPE_LEN                                       6
#define MTYPE(p)                                        CW_READ(p,MTYPE_CE)
#define SETMTYPE(p,n)                           CW_WRITE(p,MTYPE_CE,n)

#define MUSED_CE                                        14
#define MUSED_SHIFT                             12
#define MUSED_LEN                                       1
#define MUSED(p)                                        CW_READ(p,MUSED_CE)
#define SETMUSED(p,n)                           CW_WRITE(p,MUSED_CE,n)

#define MSIZE_CE                                        15
#define MSIZE_SHIFT                             13
#define MSIZE_LEN                                       15
#define MSIZEMAX                                        (POW2(MSIZE_LEN)-1)
#define MSIZE(p)                                        CW_READ(p,MSIZE_CE)
#define SETMSIZE(p,n)                           CW_WRITE(p,MSIZE_CE,n)

#define MNEW_CE                                         16
#define MNEW_SHIFT                                      28
#define MNEW_LEN                                        1
#define MNEW(p)                                         CW_READ(p,MNEW_CE)
#define SETMNEW(p,n)                            CW_WRITE(p,MNEW_CE,n)

#define CEXTRA_CE                                       17
#define CEXTRA_SHIFT                            29
#define CEXTRA_LEN                                      1
#define CEXTRA(p)                                       CW_READ(p,CEXTRA_CE)
#define SETCEXTRA(p,n)                          CW_WRITE(p,CEXTRA_CE,n)

#define MDOWN_CE                                        62
#define MDOWN_SHIFT                             30
#define MDOWN_LEN                                       1
#define MDOWN(p)                                        CW_READ(p,MDOWN_CE)
#define SETMDOWN(p,n)                           CW_WRITE(p,MDOWN_CE,n)

#define MUP_CE                                          63
#define MUP_SHIFT                                       31
#define MUP_LEN                                         1
#define MUP(p)                                          CW_READ(p,MUP_CE)
#define SETMUP(p,n)                             CW_WRITE(p,MUP_CE,n)

#define MINC(m)                                         ((MATRIX*)(((unsigned int)(m))+MSIZE(m)))
#define MDEC(m)                                         ((MATRIX*)(((unsigned int)(m))-MSIZE(m)))
#define MNEXT(m)                                        ((m)->next)
#define MDEST(m)                                        ((m)->vect)
#define MADJ(m)                                         ((MDIAG(m)) ? (m) : ((MOFFSET(m)) ? (MDEC(m)) : (MINC(m))))
#define MMYCON(m)                                       ((MOFFSET(m)) ? (MDEC(m)) : (m))
#define MVALUE(m,n)                             ((m)->value[n])
#define MVALUEPTR(m,n)                          (&((m)->value[n]))
#define MDESTINDEX(m)                           ((m)->vect->index)

/****************************************************************************/
/*																			*/
/* macros for CONNECTIONs													*/
/*																			*/
/****************************************************************************/

#define CMATRIX0(m)                             (m)
#define CMATRIX1(m)                             ((MDIAG(m)) ? (NULL) : (MINC(m)))
#define SETCUSED(c,n)                           {SETMUSED(CMATRIX0(c),n); SETMUSED(MADJ(CMATRIX0(c)),n);}

/****************************************************************************/
/*																			*/
/* macros for struct blockvector_description (BV_DESC)						*/
/*																			*/
/****************************************************************************/

/* access to members of struct blockvector_description (BV_DESC) */
#define BVD_NR_ENTRIES(bvd)                                     ((bvd)->current)

/* macros for blockvectordescription */

#define BVD_INIT(bvd)                                           ((bvd)->current=0)

/* sequential access operations on struct blockvector_description  (BV_DESC) */
#define BVD_PUSH_ENTRY(bvd,bnr,bvdf)            PushEntry( (bvd), (bnr), (bvdf) )
#define BVD_DISCARD_LAST_ENTRY(bvd)                     {assert(BVD_NR_ENTRIES(bvd)>0);BVD_NR_ENTRIES(bvd)--;}
#define BVD_INIT_SEQ_READ(bvd)                          ((bvd)->read = 0)
#define BVD_READ_NEXT_ENTRY(bvd,bvdf)           ( ((bvd)->read<BVD_NR_ENTRIES(bvd)) ? BVD_GET_ENTRY((bvd),(bvd)->read++,(bvdf)) : NO_BLOCKVECTOR )

/* random access operations on struct blockvector_description  (BV_DESC) */
#define BVD_SET_ENTRY(bvd,level,bnr,bvdf)       ( (bvd)->entry = ( ((bvd)->entry & (bvdf)->neg_digit_mask[(level)]) | ( (bnr) << ( (bvdf)->bits*(level) ) ) ) )
#define BVD_GET_ENTRY(bvd,level,bvdf)           ( ((bvd)->entry >> ((bvdf)->bits * (level))) & (bvdf)->level_mask[0] )

#define BVD_IS_SUB_BLOCK(bvd_a,bvd_b,bvdf)      ( (BVD_NR_ENTRIES(bvd_a) >= BVD_NR_ENTRIES(bvd_b)) && (((bvd_a)->entry & (((bvdf)->level_mask[BVD_NR_ENTRIES(bvd_b)-1]))) == ((((bvd_b)->entry & (bvdf)->level_mask[BVD_NR_ENTRIES(bvd_b)-1])))))

/****************************************************************************/
/*																			*/
/* macros for BLOCKVECTOR													*/
/*																			*/
/****************************************************************************/

/* control word identifier */
#define BLOCKVECTOR_CW                                  12
#define BLOCKVECTOR_OFFSET                              0

#define BVDOWNTYPE_CE                                   67
#define BVDOWNTYPE_SHIFT                                0
#define BVDOWNTYPE_LEN                                  1
#define BVDOWNTYPE(bv)                                  CW_READ(bv,BVDOWNTYPE_CE)
#define SETBVDOWNTYPE(bv,n)                     CW_WRITE(bv,BVDOWNTYPE_CE,n)

/* access to members of struct blockvector */
#define BVNUMBER(bv)                                    ((bv)->number)
#define BVPRED(bv)                                              ((bv)->pred)
#define BVSUCC(bv)                                              ((bv)->succ)
#define BVFIRSTVECTOR(bv)                               ((bv)->first_vec)
#define BVENDVECTOR(bv)                                 ((bv)->end_vec)
#define BVDOWNVECTOR(bv)                                ((bv)->first_vec)
#define BVDOWNBV(bv)                                    ((bv)->first_son)
#define BVDOWNBVEND(bv)                                 ((bv)->last_son)

/* operations on struct block */
#define BV_IS_LEAF_BV(bv)                               (BVDOWNTYPE(bv)==BVDOWNTYPEVECTOR)

#endif

/****************************************************************************/
/*																			*/
/* Macro definitions for geometric objects									*/
/*																			*/
/*																			*/
/* Use of the control word:                                                                                             */
/*																			*/
/* macro name|bits	|V|N|L|E|G|M|use										*/
/*																			*/
/* all objects:                                                                                                                         */
/* OBJT          |27-31 |*|*|*|*|*|*|object type identification                                 */
/* TAG		 |24-26 |*|*|*|*|*|*|general purpose tag field					*/
/* USED          |23	|*|*|*|*|*|*|object visited, leave them as you found 'em*/
/* LEVEL	 |17-22 | |*| |*| | |level of a node/element (imp. for copies)	*/
/* THEFLAG	 |16	|*|*|*|*|*|*|general purp.,  leave them as you found 'em*/
/*																			*/
/* vertices:																*/
/* MOVED	 |0     |*| | | | | |boundary vertex not lying on edge midpoint */
/* ONEDGE	 |1 - 3 |*| | | | | |no. of edge in father element				*/
/* MOVE          |4-5	|*| | | | | |vertex can be moved on a 0(1,2,3) dim subsp*/
/*																			*/
/* nodes:																	*/
/* CLASS	 |0-2	| |*| | | | |class of node on current level                     */
/* NCLASS	 |3-5	| |*| | | | |class of node on next level				*/
/* MODIFIED  |6         | |*| | | | |1 if node must be assembled				*/
/* N_OUTFLOW |0-7	|														*/
/* N_INFLOW  |8-15	|														*/
/*																			*/
/* links and edges:                                                                                                             */
/* LOFFSET	 |0     | | |*| | | |position of link in links array			*/
/* EXTRA	 |1     | | |*| | | |set to 1 if edge is no triangulation edge	*/
/* NOOFELEM  |2-9	| | |*| | | |nb. of elem. the edge is part of			*/
/* AUXEDGE	 |10	|														*/
/*																			*/
/* elements:																*/
/* ECLASS	 |8-9	| | | |*| | |element class from enumeration type		*/
/* NSONS	 |10-13 | | | |*| | |number of sons                                                     */
/* NEWEL	 |14	| | | |*| | |element just created						*/
/* VSIDES	 |11-14 | | | |*| | |viewable sides                                                     */
/* NORDER	 |15-19 | | | |*| | |view position order of the nodes			*/
/* CUTMODE	 |26-27 | | | |*| | |elem intersects cutplane or...                     */
/*																			*/
/****************************************************************************/

/* object identification */
#define IVOBJ 0                                                 /* inner vertex                                         */
#define BVOBJ 1                                                 /* boundary vertex					*/
#define IEOBJ 2                                                 /* inner element					*/
#define BEOBJ 3                                                 /* boundary element                             */
#define EDOBJ 4                                                 /* edge object						*/
#define NDOBJ 5                                                 /* node object						*/
#define ESOBJ 6                                                 /* element side object				*/
#define GROBJ 7                                                 /* grid object						*/
#define MGOBJ 8                                                 /* multigrid object                             */
#define VSOBJ 9                                                 /* vertex segment object			*/

#ifdef __version23__

#define NPREDEFOBJ 10                                   /* no of predefined objects             */
#endif

#ifdef __version3__
/* object numbers for algebra */
#define VEOBJ 10                                                /* vector object					*/
#define MAOBJ 11                                                /* matrix object					*/
#define BLOCKVOBJ 12                                            /* blockvector object                           */
#define NPREDEFOBJ 13                                   /* no of predefined objects             */
#endif

/****************************************************************************/
/*																			*/
/* general macros															*/
/*																			*/
/****************************************************************************/

/* macros for handling of flags, not recommended to use anymore ! */
#define SET_FLAG(flag,bitpattern)               (flag |=  (bitpattern))
#define CLEAR_FLAG(flag,bitpattern)     (flag &= ~(bitpattern))
#define READ_FLAG(flag,bitpattern)              ((flag & (bitpattern))>0)

/* control word identifier */
#define GENERAL_CW                                      2
#define GENERAL_OFFSET                          0

#define OBJ_CE                                          20
#define OBJ_SHIFT                                       27
#define OBJ_LEN                                         5
#define OBJT(p)                                         CW_READ(p,OBJ_CE)
#define SETOBJT(p,n)                            CW_WRITE(p,OBJ_CE,n)

#define USED_CE                                         21
#define USED_SHIFT                                      23
#define USED_LEN                                        1
#define USED(p)                                         CW_READ(p,USED_CE)
#define SETUSED(p,n)                            CW_WRITE(p,USED_CE,n)

#define TAG_CE                                          42
#define TAG_SHIFT                                       24
#define TAG_LEN                                         3
#define TAG(p)                                          CW_READ(p,TAG_CE)
#define SETTAG(p,n)                             CW_WRITE(p,TAG_CE,n)

#define LEVEL_CE                                        22
#define LEVEL_SHIFT                             17
#define LEVEL_LEN                                       5
#define LEVEL(p)                                        CW_READ(p,LEVEL_CE)
#define SETLEVEL(p,n)                           CW_WRITE(p,LEVEL_CE,n)

#define THEFLAG_CE                                      23
#define THEFLAG_SHIFT                           16
#define THEFLAG_LEN                             1
#define THEFLAG(p)                                      CW_READ(p,THEFLAG_CE)
#define SETTHEFLAG(p,n)                         CW_WRITE(p,THEFLAG_CE,n)

/* use this to allocate space for general flags in all control words */
/* of geometric objects                                                                                          */
#define GENERAL_SHIFT                           16
#define GENERAL_LEN                             15

#define CTRL(p)         (*((unsigned INT *)(p)))
#define ID(p)           (((INT *)(p))[1])

/****************************************************************************/
/*																			*/
/* macros for vertices														*/
/*																			*/
/****************************************************************************/

/* control word identifier */
#define VERTEX_CW                                       3
#define VERTEX_OFFSET                           0

#define VERTEX_GEN                                      24

#define MOVE_CE                                         25
#define MOVE_SHIFT                                      4
#define MOVE_LEN                                        2
#define MOVE(p)                                         CW_READ(p,MOVE_CE)
#define SETMOVE(p,n)                            CW_WRITE(p,MOVE_CE,n)

#define MOVED_CE                                        26
#define MOVED_SHIFT                             0
#define MOVED_LEN                                       1
#define MOVED(p)                                        CW_READ(p,MOVED_CE)
#define SETMOVED(p,n)                           CW_WRITE(p,MOVED_CE,n)

#define ONEDGE_CE                                       27
#define ONEDGE_SHIFT                            1
#define ONEDGE_LEN                                      3
#define ONEDGE(p)                                       CW_READ(p,ONEDGE_CE)
#define SETONEDGE(p,n)                          CW_WRITE(p,ONEDGE_CE,n)


#define PREDV(p)                (p)->iv.pred
#define SUCCV(p)                (p)->iv.succ
#define CVECT(p)                (p)->iv.x
#define XC(p)                   (p)->iv.x[0]
#define YC(p)                   (p)->iv.x[1]
#define ZC(p)                   (p)->iv.x[2]
#define LCVECT(p)               (p)->iv.xi
#define XI(p)                   (p)->iv.xi[0]
#define ETA(p)                  (p)->iv.xi[1]
#define NU(p)                   (p)->iv.xi[2]
#define VDATA(p)                (p)->iv.data
#define VFATHER(p)              (p)->iv.father
#define TOPNODE(p)              (p)->iv.topnode

/* for boundary vertices */
#define VSEG(p)                 (p)->bv.vseg
#define FIRSTSEG(p)     (BSEGDESC(VSEG(p)))             /* compatibility purposes	*/
#define FIRSTSEGDESC(p) (BSEGDESC(VSEG(p)))
#define BVLAMBDA(p)     (VSEG(p)->lambda[0])
#define FIRSTLAMBDA(p)  (VSEG(p)->lambda[0])
#define FIRSTPVECT(p)   (VSEG(p)->lambda)

/****************************************************************************/
/*																			*/
/* macros for vertex segments												*/
/*																			*/
/****************************************************************************/

#define NEXTSEG(p)              (p)->next
#define BSEG(p)                 (p)->segdesc                    /* compatibility purposes	*/
#define BSEGDESC(p)             (p)->segdesc
#define PVECT(p)                (p)->lambda
#define LAMBDA(p,i)     (p)->lambda[i]
#define ZETA(p)                 (p)->zeta


/****************************************************************************/
/*																			*/
/* macros for nodes                                                                                                             */
/*																			*/
/****************************************************************************/

/* control word identifier */
#define NODE_CW                                         4
#define NODE_OFFSET                             0

#define NODE_GEN                                        28

#define CLASS_CE                                        29
#define CLASS_SHIFT                             0
#define CLASS_LEN                                       3
#define CLASS(p)                                        CW_READ(p,CLASS_CE)
#define SETCLASS(p,n)                           CW_WRITE(p,CLASS_CE,n)

#define NCLASS_CE                                       30
#define NCLASS_SHIFT                            3
#define NCLASS_LEN                                      3
#define NCLASS(p)                                       CW_READ(p,NCLASS_CE)
#define SETNCLASS(p,n)                          CW_WRITE(p,NCLASS_CE,n)

#define MODIFIED_CE                             31
#define MODIFIED_SHIFT                          6
#define MODIFIED_LEN                            1
#define MODIFIED(p)                             CW_READ(p,MODIFIED_CE)
#define SETMODIFIED(p,n)                        CW_WRITE(p,MODIFIED_CE,n)

#define NPROP_CE                                        66
#define NPROP_SHIFT                                     7
#define NPROP_LEN                                       4
#define NPROP(p)                                        CW_READ(p,NPROP_CE)
#define SETNPROP(p,n)                           CW_WRITE(p,NPROP_CE,n)


#define PREDN(p)        (p)->pred
#define SUCCN(p)        (p)->succ
#define INDEX(p)        (p)->index
#define START(p)        (p)->start
#define NFATHER(p)      (p)->father
#define SONNODE(p)      (p)->son
#define MYVERTEX(p) (p)->myvertex
#define NDATA(p)        (p)->data
#define NDIAG(p)        (p)->matelem
#define NVECTOR(p)      (p)->vector

#ifdef __version23__
#define VSKIP(p)        (p)->vskip
#define NSKIP(p)        (p)->nskip
#endif

/****************************************************************************/
/*																			*/
/* macros for links                                                                                                             */
/*																			*/
/****************************************************************************/

/* CAUTION: the controlword of LINK0 and its edge are identical (AVOID overlapping of flags) */

/* control word identifier */
#define LINK_CW                                         5
#define LINK_OFFSET                             0

#define LINK_GEN                                        32

#define LOFFSET_CE                                      33
#define LOFFSET_SHIFT                           0
#define LOFFSET_LEN                             1
#define LOFFSET(p)                                      CW_READ(p,LOFFSET_CE)
#define SETLOFFSET(p,n)                         CW_WRITE(p,LOFFSET_CE,n)

#define NBNODE(p)       (p)->nbnode
#define NEXT(p)         (p)->next
#define LDATA(p)        (p)->matelem
#define MATELEM(p)      (p)->matelem  /* can be used for node and link */

#define MYEDGE(p)       ((EDGE *)((p)-LOFFSET(p)))
#define REVERSE(p)      ((p)+(1-LOFFSET(p)*2))

/****************************************************************************/
/*																			*/
/* macros for edges                                                                                                             */
/*																			*/
/****************************************************************************/

/* control word identifier */
#define EDGE_CW                                         6
#define EDGE_OFFSET                             0

#define EDGE_GEN                                        34

#define EOFFSET_CE                                      35 /* same as in link ! */

#define EXTRA_CE                                        36
#define EXTRA_SHIFT                             1
#define EXTRA_LEN                                       1
#define EXTRA(p)                                        CW_READ(p,EXTRA_CE)
#define SETEXTRA(p,n)                           CW_WRITE(p,EXTRA_CE,n)

#define NOOFELEM_CE                             37
#define NOOFELEM_SHIFT                          2
#define NOOFELEM_LEN                            7
#define NO_OF_ELEM_MAX                          128
#define NO_OF_ELEM(p)                           CW_READ(p,NOOFELEM_CE)
#define SET_NO_OF_ELEM(p,n)             CW_WRITE(p,NOOFELEM_CE,n)
#define INC_NO_OF_ELEM(p)                       SET_NO_OF_ELEM(p,NO_OF_ELEM(p)+1)
#define DEC_NO_OF_ELEM(p)                       SET_NO_OF_ELEM(p,NO_OF_ELEM(p)-1)

#define AUXEDGE_CE                                      38
#define AUXEDGE_SHIFT                           9
#define AUXEDGE_LEN                             1
#define AUXEDGE(p)                                      CW_READ(p,AUXEDGE_CE)
#define SETAUXEDGE(p,n)                         CW_WRITE(p,AUXEDGE_CE,n)

#define EDGENEW_CE                                      41
#define EDGENEW_SHIFT                           16
#define EDGENEW_LEN                             1
#define EDGENEW(p)                                      CW_READ(p,EDGENEW_CE)
#define SETEDGENEW(p,n)                         CW_WRITE(p,EDGENEW_CE,n)

#define LINK0(p)        (&((p)->links[0]))
#define LINK1(p)        (&((p)->links[1]))
#define MIDNODE(p)      ((p)->midnode)
#define EDDATA(p)       ((p)->data)
#define EDVECTOR(p) ((p)->vector)

/****************************************************************************/
/*																			*/
/* macros for elements														*/
/*																			*/
/****************************************************************************/

/* TAG values */
#define TRIANGLE                3
#define QUADRILATERAL   4
#define TETRAHEDRON     4

/* control word identifier */
#define ELEMENT_CW                                              7
#define ELEMENT_OFFSET                                  0
#define FLAG_CW                                                 8
#define FLAG_OFFSET                                     2

#define ELEMENT_GEN                                     43
#define FLAG_GEN                                                44

/* macros for control word */
#define ECLASS_CE                                               46
#define ECLASS_SHIFT                                    8
#define ECLASS_LEN                                              2
#define ECLASS(p)                                               CW_READ(p,ECLASS_CE)
#define SETECLASS(p,n)                                  CW_WRITE(p,ECLASS_CE,n)

#define NSONS_CE                                                47
#define NSONS_SHIFT                                     10
#define NSONS_LEN                                               4
#define NSONS(p)                                                CW_READ(p,NSONS_CE)
#define SETNSONS(p,n)                                   CW_WRITE(p,NSONS_CE,n)

#define NEWEL_CE                                                48
#define NEWEL_SHIFT                                     14
#define NEWEL_LEN                                               1
#define NEWEL(p)                                                CW_READ(p,NEWEL_CE)
#define SETNEWEL(p,n)                                   CW_WRITE(p,NEWEL_CE,n)

/* macros for flag word                           */
/* are obviously all for internal use */

/*******************************/
/* the general element concept */
/*******************************/

/* this structure contains all topological properties of an element and more .. */
typedef struct {
  INT tag;                                                                      /* element type to be defined       */

  /* the following parameters determine size of refs array in element */
  INT max_sons_of_elem;                                         /* max number of sons for this type */
  INT sides_of_elem;                                                    /* how many sides ?					*/
  INT corners_of_elem;                                          /* how many corners ?				*/

  /* more size parameters */
  INT edges_of_elem;                                                    /* how many edges ?					*/
  INT edges_of_side[MAX_SIDES_OF_ELEM];         /* number of edges for each side	*/
  INT corners_of_side[MAX_SIDES_OF_ELEM];       /* number of corners for each side  */
  INT corners_of_edge;                                          /* is always 2 !					*/

  /* index computations */
  /* Within each element sides, edges, corners are numbered in some way.      */
  /* Within each side the edges and corners are numbered, within the edge the */
  /* corners are numbered. The following arrays map the local numbers within  */
  /* the side or edge to the numbering within the element.					*/
  INT edge_of_side[MAX_SIDES_OF_ELEM][MAX_EDGES_OF_SIDE];
  INT corner_of_side[MAX_SIDES_OF_ELEM][MAX_CORNERS_OF_SIDE];
  INT corner_of_edge[MAX_EDGES_OF_ELEM][MAX_CORNERS_OF_EDGE];

  /* the following parameters are derived from data above */
  INT mapped_inner_objt;                                        /* tag to objt mapping for free list*/
  INT mapped_bnd_objt;                                          /* tag to objt mapping for free list*/
  INT inner_size, bnd_size;                                     /* size in bytes used for alloc     */
  INT edge_with_corners[MAX_CORNERS_OF_ELEM][MAX_CORNERS_OF_ELEM];
  INT side_with_edge[MAX_EDGES_OF_ELEM][MAX_SIDES_OF_EDGE];
  INT corner_of_side_inv[MAX_SIDES_OF_ELEM][MAX_CORNERS_OF_ELEM];
  INT edges_of_corner[MAX_CORNERS_OF_ELEM][MAX_EDGES_OF_ELEM];
  INT corner_of_oppedge[MAX_EDGES_OF_ELEM][MAX_CORNERS_OF_EDGE];

  /* ... the refinement rules should be placed here later */
} GENERAL_ELEMENT;

#ifndef __COMPILE_EL__
/* these are the offsets into the variable length pointer array of the element */
extern INT n_offset[TAGS];
extern INT father_offset[TAGS];
extern INT sons_offset[TAGS];
extern INT nb_offset[TAGS];
extern INT evector_offset[TAGS];
extern INT svector_offset[TAGS];
extern INT data_offset[TAGS];
extern INT side_offset[TAGS];

/* the element descriptions are also globally available, these are pointers ! */
extern GENERAL_ELEMENT *element_descriptors[TAGS];
#endif


#define SIDES_OF_ELEM(p)                (element_descriptors[TAG(p)]->sides_of_elem)
#define EDGES_OF_ELEM(p)                (element_descriptors[TAG(p)]->edges_of_elem)
#define CORNERS_OF_ELEM(p)              (element_descriptors[TAG(p)]->corners_of_elem)
#define SONS_OF_ELEM(p)                         (element_descriptors[TAG(p)]->max_sons_of_elem) /* this is the number of pointers ! */

#define EDGES_OF_SIDE(p,i)              (element_descriptors[TAG(p)]->edges_of_side[(i)])
#define CORNERS_OF_SIDE(p,i)            (element_descriptors[TAG(p)]->corners_of_side[(i)])

#define CORNERS_OF_EDGE                         2

#define EDGE_OF_SIDE(p,s,e)             (element_descriptors[TAG(p)]->edge_of_side[(s)][(e)])
#define CORNER_OF_SIDE(p,s,c)           (element_descriptors[TAG(p)]->corner_of_side[(s)][(c)])
#define CORNER_OF_EDGE(p,e,c)           (element_descriptors[TAG(p)]->corner_of_edge[(e)][(c)])
#define CORNER_OF_OPPEDGE(p,e,c)        (element_descriptors[TAG(p)]->corner_of_oppedge[(e)][(c)])

#define INNER_SIZE(tag)                 (element_descriptors[tag]->inner_size)
#define BND_SIZE(tag)                   (element_descriptors[tag]->bnd_size)
#define MAPPED_INNER_OBJT(tag)      (element_descriptors[tag]->mapped_inner_objt)
#define MAPPED_BND_OBJT(tag)        (element_descriptors[tag]->mapped_bnd_objt)
#define EDGE_WITH_CORNERS(p,c0,c1)      (element_descriptors[TAG(p)]->edge_with_corners[(c0)][(c1)])
#define SIDE_WITH_EDGE(p,e,k)           (element_descriptors[TAG(p)]->side_with_edge[(e)][(k)])
#define CORNER_OF_SIDE_INV(p,s,c)       (element_descriptors[TAG(p)]->corner_of_side_inv[(s)][(c)])
#define EDGES_OF_CORNER(p,c,k)          (element_descriptors[TAG(p)]->edges_of_corner[(c)][(k)])

#define CTRL2(p)        ((p)->ge.flag)
#define FLAG(p)                 (p)->ge.flag
#define PROP(p)                 (p)->ge.property
#define SETPROP(p,n)    (p)->ge.property = n
#define SUCCE(p)                (p)->ge.succ
#define PREDE(p)                (p)->ge.pred

#define CORNER(p,i)     ((NODE *) (p)->ge.refs[n_offset[TAG(p)]+(i)])
#define EFATHER(p)              ((ELEMENT *) (p)->ge.refs[father_offset[TAG(p)]])
#define SON(p,i)                ((ELEMENT *) (p)->ge.refs[sons_offset[TAG(p)]+(i)])
#define NBELEM(p,i)     ((ELEMENT *) (p)->ge.refs[nb_offset[TAG(p)]+(i)])
#define EDATA(p)                ((void *) (p)->ge.refs[data_offset[TAG(p)]])
#define SIDE(p,i)               ((ELEMENTSIDE *) (p)->ge.refs[side_offset[TAG(p)]+(i)])
#define EVECTOR(p)              ((VECTOR *) (p)->ge.refs[evector_offset[TAG(p)]])
#define SVECTOR(p,i)    ((VECTOR *) (p)->ge.refs[svector_offset[TAG(p)]+(i)])

/* use the following macros to assign values, since definition  */
/* above is no proper lvalue.									*/
#define SET_CORNER(p,i,q)       (p)->ge.refs[n_offset[TAG(p)]+(i)] = q
#define SET_EFATHER(p,q)        (p)->ge.refs[father_offset[TAG(p)]] = q
#define SET_SON(p,i,q)          (p)->ge.refs[sons_offset[TAG(p)]+(i)] = q
#define SET_NBELEM(p,i,q)       (p)->ge.refs[nb_offset[TAG(p)]+(i)] = q
#define VOID_NBELEM(p,i)        (p)->ge.refs[nb_offset[TAG(p)]+(i)]
#define SET_EDATA(p,q)          (p)->ge.refs[data_offset[TAG(p)]] = q
#define SET_SIDE(p,i,q)         (p)->ge.refs[side_offset[TAG(p)]+(i)] = q
#define SET_EVECTOR(p,q)        (p)->ge.refs[evector_offset[TAG(p)]] = q
#define SET_SVECTOR(p,i,q)      (p)->ge.refs[svector_offset[TAG(p)]+(i)] = q

/****************************************************************************/
/*																			*/
/* macros for element sides                                                                                             */
/*																			*/
/****************************************************************************/

#define SUCCS(p)                (p)->succ
#define PREDS(p)                (p)->pred
#define SEGDESC(p)              (p)->segdesc
#define PARAM(p,i,j)    (p)->lambda[i][j]
#define PARAMPTR(p,i)   (p)->lambda[i]

/****************************************************************************/
/*																			*/
/* macros for boundary segment descriptors									*/
/*																			*/
/****************************************************************************/

#define SEGID(p)                (p)->theSegment->id
#define LEFT(p)                 (p)->theSegment->left
#define RIGHT(p)                (p)->theSegment->right
#define SEGTYPE(p)              (p)->theSegment->type
#define POINT(p,i)              (p)->theSegment->points[i]
#define RES(p)                  (p)->theSegment->resolution
#define BNDSEGFUNC(p)   ((p)->theSegment->BndSegFunc)
#define SEGDATA(p)              ((p)->theSegment->data)
#define BNDDATA(p)              ((p)->theSegment->data)
#define BNDCONDFUNC(p)  ((p)->theBoundaryCondition->BndCond)
#define CONDDATA(p)     ((p)->theBoundaryCondition->data)
#define ALPHA(p,i)              (p)->theSegment->alpha[i]
#define BETA(p,i)               (p)->theSegment->beta[i]

/****************************************************************************/
/*																			*/
/* macros for grids                                                                                                             */
/*																			*/
/****************************************************************************/

/* control word identifier */
#define GRID_CW                                                 9
#define GRID_CW_OFFSET                                  0
#define GRID_STATUS                                     10
#define GRID_STATUS_OFFSET                              1

#define GLEVEL(p)                       ((p)->level)
#define GSTATUS(p)                      ((p)->status)
#define SETGSTATUS(p,n)         ((p)->status|=n)
#define RESETGSTATUS(p,n)       ((p)->status&=(~n))
#define FIRSTELEMENT(p)         ((p)->elements)
#define LASTELEMENT(p)          ((p)->lastelement)
#define FIRSTVERTEX(p)          ((p)->vertices)
#define LASTVERTEX(p)           ((p)->lastvertex)
#define FIRSTELEMSIDE(p)        ((p)->sides)
#define FIRSTNODE(p)            ((p)->firstNode)
#define LASTNODE(p)             ((p)->lastNode)
#define FIRSTVECTOR(p)          ((p)->firstVector)
#define LASTVECTOR(p)           ((p)->lastVector)
#define GFIRSTBV(p)             ((p)->firstblockvector)
#define GLASTBV(p)                      ((p)->lastblockvector)
#define UPGRID(p)                       ((p)->finer)
#define DOWNGRID(p)             ((p)->coarser)
#define MYMG(p)                         ((p)->mg)
#define NV(p)                           ((p)->nVert)
#define NN(p)                           ((p)->nNode)
#define NT(p)                           ((p)->nElem)
#define NE(p)                           ((p)->nEdge)
#define NS(p)                           ((p)->nSide)
#define NVEC(p)                         ((p)->nVector)
#define NC(p)                           ((p)->nCon)


/****************************************************************************/
/*																			*/
/* macros for multigrids													*/
/*																			*/
/****************************************************************************/

/* control word identifier */
#define MULTIGRID_STATUS                                11
#define MULTIGRID_STATUS_OFFSET                 (sizeof(ENVVAR))

#define MGSTATUS(p)                     ((p)->status)
#define VIDCNT(p)                               ((p)->vertIdCounter)
#define NIDCNT(p)                               ((p)->nodeIdCounter)
#define EIDCNT(p)                               ((p)->elemIdCounter)
#define TOPLEVEL(p)                     ((p)->topLevel)
#define CURRENTLEVEL(p)                 ((p)->currentLevel)
#define MGDOMAIN(p)                     ((p)->theDomain)
#define MGFORMAT(p)                     ((p)->theFormat)
#define DATAFORMAT(p)                   ((p)->theFormat)
#define MGPROBLEM(p)                    ((p)->theProblem)
#define MG_GEOM_SEGMENT(p,i)    (((p)->segments[i]).theSegment)
#define MGBNDSEGDESC(p,i)               (&((p)->segments[i]))
#define MGNOOFSEG(p)                    ((p)->numOfSegments)
#define MGVERTEX(p,k)                   ((p)->corners[k])
#define MGNOOFSUBDOMAINS(p)             ((p)->numOfSubdomains)
#define MGNOOFCORNERS(p)                ((p)->numOfCorners)
#define MGHEAP(p)                               ((p)->theHeap)
#define GRID_ON_LEVEL(p,i)              ((p)->grids[i])
#define SELECTIONSIZE(p)                ((p)->NbOfSelections)
#define SELECTIONMODE(p)                ((p)->SelectionMode)
#define SELECTIONOBJECT(p,i)    ((p)->Selection[(((i)<MAXSELECTION) ? (i) : (MAXSELECTION-1))])
#define MGNAME(p)                               ((p)->v.name)
#define MG_USER_HEAP(p)                 ((p)->UserHeap)
#define GEN_MGUD(p)                     ((p)->GenData)
#define GEN_MGUD_ADR(p,o)               ((void *)(((char *)((p)->GenData))+(o)))

/****************************************************************************/
/*																			*/
/* macros for problems														*/
/*																			*/
/****************************************************************************/

#define PROBLEMID(p)   (p)->problemID
#define CONFIGFUNC(p)  (p)->ConfigProblem
#define NUMCOEFF(p)    (p)->numOfCoeffFct
#define NUMUSERFCT(p)  (p)->numOfUserFct
#define COEFFFUNC(p,i) (CoeffProcPtr)((p)->CU_ProcPtr[i])
#define USERFUNC(p,i)  (UserProcPtr)((p)->CU_ProcPtr[i+(p)->numOfCoeffFct])

/****************************************************************************/
/*																			*/
/* declaration of exported global variables									*/
/*																			*/
/****************************************************************************/

/* predefined blockvector description formats */
extern const BV_DESC_FORMAT DH_bvdf;                    /* bvdf for domain halfening	*/
extern const BV_DESC_FORMAT one_level_bvdf;     /* bvdf for only 1 blocklevel	*/

/****************************************************************************/
/*																			*/
/* interface functions for module grid manager								*/
/*																			*/
/****************************************************************************/

/* return values for functions returning an INT. The usual rule is: 0 ok, >0 error */
#define GM_OK                                           0
#define GM_ERROR                                        1
#define GM_FILEOPEN_ERROR                       2
#define GM_RULE_WITH_ORIENTATION        3
#define GM_RULE_WITHOUT_ORIENTATION 4
#define GM_OUT_OF_MEM                           5
#define GM_OUT_OF_RANGE                         6
#define GM_NOT_FOUND                            7
#define GM_INCONSISTANCY                        8
#define GM_FATAL                                        999

/* some constants passed as parameters */
#define GM_KEEP_BOUNDARY_NODES          0
#define GM_MOVE_BOUNDARY_NODES          1
#define GM_REFINE_TRULY_LOCAL           2
#define GM_COPY_ALL                             3
#define GM_FCFCLL                                       1
#define GM_FFCCLL                                       2
#define GM_ALL_LEVELS                           1
#define GM_CURRENT_LEVEL                        2
#define GM_ORDER_IN_COLS                        0
#define GM_ORDER_IN_ROWS                        1
#define GM_PUT_AT_BEGIN                         1               /* put skip vectors at begin of the list	*/
#define GM_PUT_AT_END                           2               /* put skip vectors at end of the list		*/

/* get/set current multigrid, loop through multigrids */
MULTIGRID               *MakeMGItem                             (const char *name);
MULTIGRID               *GetMultigrid                           (const char *name);
MULTIGRID               *GetFirstMultigrid                      (void);
MULTIGRID               *GetNextMultigrid                       (const MULTIGRID *theMG);

/* domain definition */
DOMAIN                   *CreateDomain                          (char *name, COORD *MidPoint, COORD radius, INT segments, INT corners, INT Convex);
DOMAIN                   *GetDomain                             (char *name);
BOUNDARY_SEGMENT *CreateBoundarySegment         (char *name, INT left, INT right,INT id,INT type,INT res,INT *point, COORD *alpha,COORD *beta, BndSegFuncPtr BndSegFunc, void *data);
BOUNDARY_SEGMENT *CreateBoundarySegment2D       (char *name, int left, int right, int id, int from, int to, int res, COORD alpha, COORD beta, BndSegFuncPtr BndSegFunc, void *data);
BOUNDARY_SEGMENT *GetFirstBoundarySegment       (DOMAIN *theDomain);
BOUNDARY_SEGMENT *GetNextBoundarySegment        (BOUNDARY_SEGMENT *theBSeg);
BOUNDARY_CONDITION *GetFirstBoundaryCondition (PROBLEM *theProblem);
BOUNDARY_CONDITION *GetNextBoundaryCondition (BOUNDARY_CONDITION *theBCond);

/* problem definition */
PROBLEM                  *CreateProblem                         (char *domain,char *name, int id, ConfigProcPtr config, int numOfCoefficients, CoeffProcPtr coeffs[], int numOfUserFct, UserProcPtr userfct[]);
PROBLEM                  *GetProblem                            (const char * domain, const char *name);
#ifdef __version23__
BOUNDARY_CONDITION *CreateBoundaryCondition (char *name, INT id, BndCondProcPtr theBndCond);
#endif
#ifdef __version3__
BOUNDARY_CONDITION *CreateBoundaryCondition (char *name, INT id, BndCondProcPtr theBndCond, void *Data);
#endif

/* format definition */
FORMAT                   *GetFormat                             (const char *name);
#ifdef __version23__
FORMAT                   *CreateFormat                          (char *name,int sVertex,int sNode,int sDiag,int sElement,int sLink,int sEdge, int sMultiGrid,
                                                                 ConversionProcPtr SaveVertex,ConversionProcPtr SaveNode,ConversionProcPtr SaveDiag,
                                                                 ConversionProcPtr SaveElement,ConversionProcPtr SaveLink,ConversionProcPtr SaveEdge,
                                                                 ConversionProcPtr SaveGrid,ConversionProcPtr SaveMultiGrid,ConversionProcPtr LoadVertex,
                                                                 ConversionProcPtr LoadNode,ConversionProcPtr LoadDiag,ConversionProcPtr LoadElement,
                                                                 ConversionProcPtr LoadLink,ConversionProcPtr LoadEdge,ConversionProcPtr LoadGrid,
                                                                 ConversionProcPtr LoadMultiGrid,ConversionProcPtr PrintVertex,ConversionProcPtr PrintNode,
                                                                 ConversionProcPtr PrintDiag,ConversionProcPtr PrintElement,ConversionProcPtr PrintLink,
                                                                 ConversionProcPtr PrintEdge,ConversionProcPtr PrintGrid,ConversionProcPtr PrintMultiGrid);
#endif
#ifdef __version3__
FORMAT                  *CreateFormat           (char *name, INT sVertex, INT sMultiGrid,
                                                 ConversionProcPtr PrintVertex,ConversionProcPtr PrintGrid,ConversionProcPtr PrintMultigrid,
                                                 INT nvDesc, VectorDescriptor *vDesc,INT nmDesc, MatrixDescriptor *mDesc);
FORMAT                  *Ugly_CreateFormat (char *name,INT sVertex, INT sMultiGrid, INT *VectorSizes,
                                            INT *FromType, INT *ToType, INT *MatrixSizes, INT *ConnectionDepth,
                                            ConversionProcPtr PrintVertex, ConversionProcPtr PrintGrid,ConversionProcPtr PrintMultigrid,
                                            ConversionProcPtr PrintVector[MAXVECTORS], ConversionProcPtr PrintMatrix[MAXVECTORS][MAXVECTORS] );
#endif

/* create, saving and disposing a multigrid structure */
MULTIGRID       *CreateMultiGrid                (char *MultigridName, char *domain, char *problem, char *format, unsigned long heapSize);
MULTIGRID       *LoadMultiGrid                  (char *MultigridName, char *FileName, char *domain, char *problem, char *format, unsigned long heapSize);
INT             SaveMultiGrid                   (MULTIGRID *theMG, char *FileName, char *comment);
INT             DisposeMultiGrid                (MULTIGRID *theMG);

/* coarse grid manipulations */
INT             InsertInnerNode                 (MULTIGRID *theMG, COORD *pos);
INT             InsertBoundaryNode              (MULTIGRID *theMG, INT bnd_seg_id, COORD *pos);
INT             DeleteNodeWithID                (MULTIGRID *theMG, INT id);
INT             DeleteNode                              (MULTIGRID *theMG, NODE *theNode);
INT             InsertElementFromIDs    (MULTIGRID *theMG, INT n, INT  *idList);
INT             InsertElement                   (MULTIGRID *theMG, INT n, NODE **NodeList);
INT             DeleteElementWithID     (MULTIGRID *theMG, INT id);
INT             DeleteElement                   (MULTIGRID *theMG, ELEMENT *theElement);

/* refinement */
INT             EstimateHere                    (ELEMENT *theElement);
INT             MarkForRefinement               (ELEMENT *theElement, INT rule, INT side);
INT             GetRefinementMark               (const ELEMENT *theElement, INT *rule, INT *side);
INT             RefineMultiGrid                 (MULTIGRID *theMG, INT flag, EVECTOR *direction);
NODE            *GetFineNodeOnEdge              (const ELEMENT *theElement, INT side);
/*INT			GetFineSidesTouchingCoarseSide (const ELEMENT *theElement, INT side, INT *nfine, ELEMENT *Elements[MAX_SIDES_TOUCHING], INT Sides[MAX_SIDES_TOUCHING]);*/

/* moving nodes */
INT             MoveInnerNode                   (MULTIGRID *theMG, NODE *theNode, COORD *newPos);
INT             MoveBoundaryNode                (MULTIGRID *theMG, NODE *theNode, INT segid, COORD *newPos);
INT             SmoothMultiGrid                 (MULTIGRID *theMG, INT niter, INT bdryFlag);

/* handling struct blockvector_description_format (BV_DESC_FORMAT) */
INT InitBVDF                                            ( BV_DESC_FORMAT *bvdf, BLOCKNUMBER max_blocks );

/* handling struct blockvector_description (BV_DESC) */
INT PushEntry                                           ( BV_DESC *bvd, BLOCKNUMBER bnr, const BV_DESC_FORMAT *bvdf );

/* functions to create a BLOCKVECTOR structure for a regular rectangular grid */
INT CreateBVStripe                                      ( GRID *grid, INT points, INT points_per_stripe );
INT CreateBVDomainHalfening                     ( GRID *grid, INT side );

/* general functions for BLOCKVECTOR */
INT CreateBlockvector                                   ( GRID *theGrid, BLOCKVECTOR **BVHandle );
INT DisposeBlockvector                                  ( GRID *theGrid, BLOCKVECTOR *bv );
void FreeAllBV                                          ( GRID *grid );
void FreeBVList                                         ( GRID *grid, BLOCKVECTOR *bv );

/* algebraic connections */
#ifdef __version3__
CONNECTION      *CreateExtraConnection  (GRID *theGrid, VECTOR *from, VECTOR *to);
INT             DisposeExtraConnections (GRID *theGrid);
MATRIX          *GetMatrix                              (const VECTOR *FromVector, const VECTOR *ToVector);
CONNECTION      *GetConnection                  (const VECTOR *FromVector, const VECTOR *ToVector);
#endif
#ifdef __version23__
EDGE            *CreateAuxEdge                  (GRID *theGrid, NODE *from, NODE *to);
INT             DisposeAuxEdges                 (GRID *theGrid);
#endif

/* searching */
NODE            *FindNodeFromId                 (GRID *theGrid, INT id);
NODE            *FindNodeFromPosition   (GRID *theGrid, COORD *pos, COORD *tol);
VECTOR          *FindVectorFromPosition (GRID *theGrid, COORD *pos, COORD *tol);
ELEMENT         *FindElementFromId              (GRID *theGrid, INT id);
ELEMENT         *FindElementFromPosition(GRID *theGrid, COORD *pos);
BLOCKVECTOR *FindBV                                     (const GRID *grid, BV_DESC *bvd, const BV_DESC_FORMAT *bvdf );

/* list */
void            ListMultiGridHeader     (const INT longformat);
void            ListMultiGrid                   (MULTIGRID *theMG, const INT isCurrent, const INT longformat);
void            ListGrids                               (const MULTIGRID *theMG);
void            ListNode                                (MULTIGRID *theMG, NODE *theNode,               INT dataopt, INT bopt, INT nbopt, INT vopt);
void            ListNodeSelection               (MULTIGRID *theMG,                                              INT dataopt, INT bopt, INT nbopt, INT vopt);
void            ListNodeRange                   (MULTIGRID *theMG, INT from, INT to,    INT dataopt, INT bopt, INT nbopt, INT vopt);
void            ListElement                     (MULTIGRID *theMG, ELEMENT *theElement, INT dataopt, INT bopt, INT nbopt, INT vopt);
void            ListElementSelection    (MULTIGRID *theMG,                                              INT dataopt, INT bopt, INT nbopt, INT vopt);
void            ListElementRange                (MULTIGRID *theMG, INT from, INT to,    INT dataopt, INT bopt, INT nbopt, INT vopt, INT lopt);
void            ListVector                              (MULTIGRID *theMG, VECTOR *theVector,   INT matrixopt, INT dataopt);
void            ListVectorSelection     (MULTIGRID *theMG,                                              INT matrixopt, INT dataopt);
void            ListVectorOfElementSelection(MULTIGRID *theMG,                                  INT matrixopt, INT dataopt);
void            ListVectorRange                 (MULTIGRID *theMG,                      INT fl, INT tl, INT fromV, INT toV, INT matrixopt, INT dataopt);

/* query */
LINK            *GetLink                                (NODE *from, NODE *to);
EDGE            *GetEdge                                (NODE *from, NODE *to);
INT             GetSons                                 (ELEMENT *theElement, ELEMENT *SonList[MAX_SONS]);
INT             VectorPosition                  (VECTOR *theVector, COORD *position);
INT             VectorInElement                 (ELEMENT *theElement, VECTOR *theVector);
INT             MinMaxAngle                     (ELEMENT *theElement, DOUBLE *amin, DOUBLE *amax);

/* check */
INT             CheckGrid                               (GRID *theGrid);
INT             CheckConnections                (GRID *theGrid);

/* selection */
void            ClearSelection                  (MULTIGRID *theMG);
INT             AddNodeToSelection              (MULTIGRID *theMG, NODE *theNode);
INT             IsNodeSelected                  (MULTIGRID *theMG, NODE *theNode);
INT             AddElementToSelection   (MULTIGRID *theMG, ELEMENT *theElement);
INT             IsElementSelected               (MULTIGRID *theMG, ELEMENT *theElement);
INT             AddVectorToSelection    (MULTIGRID *theMG, VECTOR *theVector);
INT             IsVectorSelected                (MULTIGRID *theMG, VECTOR *theVector);
INT             RemoveNodeFromSelection (MULTIGRID *theMG, NODE *theNode);
INT             RemoveElementFromSelection(MULTIGRID *theMG, ELEMENT *theElement);
INT             RemoveVectorFromSelection(MULTIGRID *theMG, VECTOR *theVector);

/* multigrid user data space management (using the heaps.c block heap management) */
INT             AllocateControlEntry    (INT cw_id, INT length, INT *ce_id);
INT             FreeControlEntry                (INT ce_id);
INT             DefineMGUDBlock                 (BLOCK_ID id, MEM size);
INT             FreeMGUDBlock                   (BLOCK_ID id);
BLOCK_DESC      *GetMGUDBlockDescriptor (BLOCK_ID id);

/* ordering of degrees of freedom */
ALG_DEP         *CreateAlgebraicDependency (char *name, DependencyProcPtr DependencyProc);
FIND_CUT        *CreateFindCutProc              (char *name, FindCutProcPtr FindCutProc);
INT                     LexOrderVectorsInGrid   (GRID *theGrid, const INT *order, const INT *sign, INT SpecSkipVecs, INT AlsoOrderMatrices);
INT             OrderVectors                    (MULTIGRID *theMG, INT levels, INT mode, INT PutSkipFirst, INT SkipPat, const char *dependency, const char *dep_options, const char *findcut);
INT                     ShellOrderVectors               (GRID *theGrid, VECTOR *seed);

/* functions for evaluation-fct management */
INT              InitEvalProc                                                           ();
EVALUES         *CreateElementValueEvalProc                             (const char *name, PreprocessingProcPtr PreProc, ElementEvalProcPtr EvalProc);
EVECTOR         *CreateElementVectorEvalProc                            (const char *name, PreprocessingProcPtr PreProc, ElementVectorProcPtr EvalProc, INT d);
MVALUES         *CreateMatrixValueEvalProc                                      (const char *name, PreprocessingProcPtr PreProc, MatrixEvalProcPtr EvalProc);
EVALUES         *CreateElementValueEvalProcFromCoeffProc        (const char *name, CoeffProcPtr CoeffProc);
EVECTOR         *CreateElementVectorEvalProcFromCoeffProc       (const char *name, CoeffProcPtr CoeffProc, INT d);
EVALUES         *GetElementValueEvalProc                                        (const char *name);
EVECTOR         *GetElementVectorEvalProc                                       (const char *name);
MVALUES         *GetMatrixValueEvalProc                                         (const char *name);

/* miscellaneous */
INT             RenumberMultiGrid               (MULTIGRID *theMG);
INT                     OrderNodesInGrid                (GRID *theGrid, const INT *order, const INT *sign, INT AlsoOrderLinks);
INT             PutAtStartOfList                (GRID *theGrid, INT cnt, ELEMENT **elemList);
INT         MGSetVectorClasses      (MULTIGRID *theMG);

#endif
