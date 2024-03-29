// SPDX-FileCopyrightText: Copyright © DUNE Project contributors, see file LICENSE.md in module root
// SPDX-License-Identifier: LGPL-2.1-or-later
// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:
/****************************************************************************/
/*																			*/
/* File:	  ugtimer.c														*/
/*																			*/
/* Purpose:   implements a simple timing facility			                */
/*																			*/
/* Author:	  Peter Bastian/Klaus Johannsen                                                                 */
/*			  Institut fuer Computeranwendungen III                                                 */
/*			  Universitaet Stuttgart										*/
/*			  Pfaffenwaldring 27											*/
/*			  70550 Stuttgart												*/
/*			  email: ug@ica3.uni-stuttgart.de								*/
/*																			*/
/* History:   29.01.92 begin, ug version 2.0								*/
/*																			*/
/* Remarks:                                                                                                                             */
/*																			*/
/****************************************************************************/

/****************************************************************************/
/*																			*/
/* include files															*/
/*			  system include files											*/
/*			  application include files                                                                     */
/*																			*/
/****************************************************************************/

#include <config.h>
#include <cstdio>
#include <cassert>

#include "ugtimer.h"

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
/* data structures used in this source file (exported data structures are	*/
/*		  in the corresponding include file!)								*/
/*																			*/
/****************************************************************************/


/****************************************************************************/
/*																			*/
/* definition of exported global variables									*/
/*																			*/
/****************************************************************************/

NS_PREFIX UG_TIMER NS_PREFIX ug_timer[MAX_TIMER];


/****************************************************************************/
/*																			*/
/* definition of variables global to this source file only (static!)		*/
/*																			*/
/****************************************************************************/


/****************************************************************************/
/*																			*/
/* forward declarations of functions used before they are defined			*/
/*																			*/
/****************************************************************************/


/****************************************************************************/
/** \brief Allocate a new timer
 */
/****************************************************************************/

void NS_PREFIX new_timer (int *n)
{
  int i;

  *n = -1;

  for (i=0; i<MAX_TIMER; i++)
    if (!ug_timer[i].used)
    {
      *n = i;
      ug_timer[i].used = true;
      ug_timer[i].start = 0.0;
      ug_timer[i].stop = 0.0;
      ug_timer[i].sum = 0.0;
      break;
    }
  if (*n == -1)
  {
    printf("NEW_TIMER(): couldn't allocate new timer!\n");
    fflush(stdout);
    assert(0);
  }
}
