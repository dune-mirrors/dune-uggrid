// SPDX-FileCopyrightText: Copyright © DUNE Project contributors, see file LICENSE.md in module root
// SPDX-License-Identifier: LGPL-2.1-or-later
// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:
/****************************************************************************/
/*                                                                          */
/* File:      stat.c                                                        */
/*                                                                          */
/* Purpose:   DDD statistical evaluation                                    */
/*                                                                          */
/* Author:    Klaus Birken                                                  */
/*            Rechenzentrum Uni Stuttgart                                   */
/*            Universitaet Stuttgart                                        */
/*            Allmandring 30                                                */
/*            70550 Stuttgart                                               */
/*            internet: birken@rus.uni-stuttgart.de                         */
/*                                                                          */
/* History:   95/01/16 kb  begin                                            */
/*                                                                          */
/* Remarks:                                                                 */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*                                                                          */
/* include files                                                            */
/*            system include files                                          */
/*            application include files                                     */
/*                                                                          */
/****************************************************************************/

/* standard C library */
#include <config.h>
#include <cstdlib>
#include <cstdio>
#include <stdarg.h>

#include <dune/uggrid/parallel/ddd/dddi.h>


/****************************************************************************/
/*                                                                          */
/* defines in the following order                                           */
/*                                                                          */
/*        compile time constants defining static data size (i.e. arrays)    */
/*        other constants                                                   */
/*        macros                                                            */
/*                                                                          */
/****************************************************************************/



/****************************************************************************/
/*                                                                          */
/* definition of exported global variables                                  */
/*                                                                          */
/****************************************************************************/

/* Storage for statistical data (compile with option -DStatistics)
   Caution: statistics should be used via macros in dddi.h! */

#ifdef Statistics
STAT_DATA stat_data;
#endif



/****************************************************************************/
/*                                                                          */
/* definition of variables global to this source file only (static!)        */
/*                                                                          */
/****************************************************************************/




/****************************************************************************/
/*                                                                          */
/* routines                                                                 */
/*                                                                          */
/****************************************************************************/
START_UGDIM_NAMESPACE

void ddd_StatInit (void)
{
#ifdef Statistics
  stat_data.curr_module = 0;
#endif
}


void ddd_StatExit (void)
{}


/****************************************************************************/


double DDD_StatClock (int module, int index)
{
  STAT_SET_MODULE(module);
  return(STAT_GETTIMER(index));
}

long DDD_StatCount (int module, int index)
{
  STAT_SET_MODULE(module);
  return(STAT_GETCOUNT(index));
}



const char *DDD_StatClockDesc (int module, int index)
{
  STAT_SET_MODULE(module);
  return("<NIY>");
}

const char *DDD_StatCountDesc (int module, int index)
{
  STAT_SET_MODULE(module);
  return("<NIY>");
}


/****************************************************************************/

END_UGDIM_NAMESPACE
