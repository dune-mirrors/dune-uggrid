// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:
/****************************************************************************/
/*																			*/
/* File:	  transgrid.h													*/
/*																			*/
/* Purpose:   header for trnsgrid.c			                                                                */
/*																			*/
/* Author:	  Peter Bastian/Klaus Johannsen                                                                 */
/*			  Institut fuer Computeranwendungen III                                                 */
/*			  Universitaet Stuttgart										*/
/*			  Pfaffenwaldring 27											*/
/*			  70550 Stuttgart												*/
/*			  email: ug@ica3.uni-stuttgart.de				                        */
/*																			*/
/* History:   29.01.92 begin, ug version 2.0								*/
/*																			*/
/* Remarks:                                                                                                                             */
/*																			*/
/****************************************************************************/


/* RCS_ID
   $Header$
 */

/****************************************************************************/
/*																			*/
/* auto include mechanism and other include files							*/
/*																			*/
/****************************************************************************/

#ifndef __TRANSGRID__
#define __TRANSGRID__

#include "np.h"

#define CRIT_OFFSET                     (20)
#define CRITBIT(vec, n)                 VECSKIPBIT((vec), (CRIT_OFFSET+(n)))
#define SETCRITBIT(vec, n)              SETVECSKIPBIT((vec), (CRIT_OFFSET+(n)))
#define CLEARCRITBIT(vec, n)            CLEAR_FLAG(VECSKIP(vec),(1 << (CRIT_OFFSET + (n))))
#define CHANGEBIT(vec)                  VECSKIPBIT((vec), (CRIT_OFFSET-1))
#define SETCHANGEBIT(vec)               SETVECSKIPBIT((vec), (CRIT_OFFSET-1))
#define CLEARCHANGEBIT(vec)             CLEAR_FLAG(VECSKIP(vec),(1 << (CRIT_OFFSET -1)));

/****************************************************************************/
/*																			*/
/* defines in the following order											*/
/*																			*/
/*		  compile time constants defining static data size (i.e. arrays)	*/
/*		  other constants													*/
/*		  macros															*/
/*																			*/
/****************************************************************************/

/****************************************************************************/
/*																			*/
/* data structures exported by the corresponding source file				*/
/*																			*/
/****************************************************************************/



/****************************************************************************/
/*																			*/
/* definition of exported global variables									*/
/*																			*/
/****************************************************************************/



/****************************************************************************/
/*																			*/
/* function declarations													*/
/*																			*/
/****************************************************************************/

#endif
