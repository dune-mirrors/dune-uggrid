// SPDX-FileCopyrightText: Copyright © DUNE Project contributors, see file LICENSE.md in module root
// SPDX-License-Identifier: LGPL-2.1-or-later
// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:
/****************************************************************************/
/*																			*/
/* File:	  pgmcheck.c													*/
/*																			*/
/* Purpose:   functions for checking parallel consistency                                       */
/*																			*/
/* Author:	  Stefan Lang                                                                   */
/*			  Institut fuer Computeranwendungen III                                                 */
/*			  Universitaet Stuttgart										*/
/*			  Pfaffenwaldring 27											*/
/*			  70550 Stuttgart												*/
/*			  email: birken@ica3.uni-stuttgart.de							*/
/*			  phone: 0049-(0)711-685-7003									*/
/*			  fax  : 0049-(0)711-685-7000									*/
/*																			*/
/* History:   980204 sl  begin                                                                                          */
/*																			*/
/* Remarks:                                                                                                                             */
/*																			*/
/****************************************************************************/

#ifdef ModelP

/****************************************************************************/
/*																			*/
/* include files															*/
/*			  system include files											*/
/*			  application include files                                                                     */
/*																			*/
/****************************************************************************/

#include <config.h>
#include <cstdlib>

#include "parallel.h"
#include <dune/uggrid/ugdevices.h>
#include <dune/uggrid/gm/evm.h>
#include <dune/uggrid/gm/gm.h>
#include <dune/uggrid/gm/refine.h>
#include <dune/uggrid/gm/shapes.h>
#include <dune/uggrid/gm/ugm.h>
#include <dune/uggrid/low/debug.h>
#include <dune/uggrid/low/namespace.h>

/* UG namespaces: */
USING_UG_NAMESPACES

/* PPIF namespace: */
using namespace PPIF;


/****************************************************************************/
/*																			*/
/* defines in the following order											*/
/*																			*/
/*		  compile time constants defining static data size (i.e. arrays)	*/
/*		  other constants													*/
/*		  macros															*/
/*																			*/
/****************************************************************************/

#define CHECK_OBJECT_PRIO(o,prio,master,ghost,id,s,_nerr_)                   \
  do { \
  if (USED(o)==1 && ! master (o))                                          \
  {                                                                        \
    UserWriteF("MASTER %s=" id ## _FMTX " has WRONG prio=%d\n",      \
               s, id ## _PRTX(o),prio(o));                                  \
    _nerr_++;                                                        \
  }                                                                        \
  if (USED( o )==0 && ! ghost ( o ))                                       \
  {                                                                        \
    UserWriteF("GHOST %s=" id ## _FMTX " has WRONG prio=%d\n",       \
               s, id ## _PRTX( o ),prio(o));                                \
    _nerr_++;                                                        \
  } \
  } while (0)

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


/****************************************************************************/
/*																			*/
/* definition of variables global to this source file only (static!)		*/
/*																			*/
/****************************************************************************/

/* count for errors to report from communication scatter functions */
static INT check_distributed_objects_errors = 0;


/****************************************************************************/
/*																			*/
/* forward declarations of functions used before they are defined			*/
/*																			*/
/****************************************************************************/


/****************************************************************************/
/*
   CheckProcListCons -

   SYNOPSIS:
   INT CheckProcListCons (const DDD_InfoProcListRange& proclist, DDD_PRIO uniqueTag);

   PARAMETERS:
   .  proclist
   .  uniqueTag

   DESCRIPTION:

   RETURN VALUE:
   INT
 */
/****************************************************************************/

INT NS_DIM_PREFIX CheckProcListCons (const DDD_InfoProcListRange& proclist, DDD_PRIO uniqueTag)
{
  int nunique = 0;

  /* check uniqueness */
  for (auto&& [proc, prio] : proclist)
  {
    if (prio == uniqueTag)
      ++nunique;
  }

  /* nunique must be 1 for master elements   */
  /* nunique can  be 0/1 for (inner) nodes   */
  /*   with PrioBorder/PrioMaster            */
  return nunique;
}



/****************************************************************************/
/*
   ListProcList -

   SYNOPSIS:
   INT ListProcList (const DDD_InfoProcListRange& proclist, DDD_PRIO uniqueTag);

   PARAMETERS:
   .  proclist
   .  uniqueTag

   DESCRIPTION:

   RETURN VALUE:
   INT
 */
/****************************************************************************/

static INT ListProcList (const DDD_InfoProcListRange& proclist, DDD_PRIO uniqueTag)
{
  for (auto&& [proc, prio] : proclist)
  {
    if (prio == uniqueTag)
      UserWriteF(" proc=%d", proc);
  }
  return 0;
}


/****************************************************************************/
/*
   CheckVectorPrio -

   SYNOPSIS:
   INT CheckVectorPrio (ELEMENT *theElement, VECTOR *theVector);

   PARAMETERS:
   .  theElement
   .  theVector

   DESCRIPTION:

   RETURN VALUE:
   INT
 */
/****************************************************************************/

static INT CheckVectorPrio (DDD::DDDContext& context, ELEMENT *theElement, VECTOR *theVector)
{
  INT nmaster;
  INT nerrors = 0;

  /* check vector prio */
  CHECK_OBJECT_PRIO(theVector,PRIO,MASTER,GHOST,VINDEX,"Vector",nerrors);

  /* master copy has to be unique */
  const auto& proclist = DDD_InfoProcListRange(context, PARHDR(theVector));
  if ((nmaster = CheckProcListCons(proclist, PrioMaster)) > 1)
  {
    UserWriteF("VECTOR=" ID_FMTX " ERROR: master copy not unique, nmaster=%d:",
               ID_PRTX(theVector),nmaster);
    ListProcList(proclist, PrioMaster);
    UserWriteF("\n");
    nerrors++;
  }

  return nerrors;
}


/****************************************************************************/
/*
   CheckNodePrio -

   SYNOPSIS:
   INT CheckNodePrio (ELEMENT *theElement, NODE *theNode);

   PARAMETERS:
   .  theElement
   .  theNode

   DESCRIPTION:

   RETURN VALUE:
   INT
 */
/****************************************************************************/

static INT CheckNodePrio (DDD::DDDContext& context, ELEMENT *theElement, NODE *theNode)
{
  INT nmaster;
  INT nerrors = 0;

  /* check node prio */
  CHECK_OBJECT_PRIO(theNode,PRIO,MASTER,GHOST,ID,"NODE",nerrors);

  /* master copy has to be unique */
  const auto& proclist = DDD_InfoProcListRange(context, PARHDR(theNode));
  if ((nmaster = CheckProcListCons(proclist, PrioMaster)) > 1)
  {
    UserWriteF("NODE=" ID_FMTX " ERROR: master copy not unique, nmaster=%d:",
               ID_PRTX(theNode),nmaster);
    ListProcList(proclist, PrioMaster);
    UserWriteF("\n");
    nerrors++;
  }

  return(nerrors);
}


/****************************************************************************/
/*
   CheckEdgePrio -

   SYNOPSIS:
   INT CheckEdgePrio (ELEMENT *theElement, EDGE *theEdge);

   PARAMETERS:
   .  theElement
   .  theEdge

   DESCRIPTION:

   RETURN VALUE:
   INT
 */
/****************************************************************************/

static INT CheckEdgePrio (DDD::DDDContext& context, ELEMENT *theElement, EDGE *theEdge)
{
  INT nerrors = 0;

        #ifdef UG_DIM_3
  INT nmaster;

  /* check edge prio */
  CHECK_OBJECT_PRIO(theEdge,PRIO,MASTER,GHOST,ID,"EDGE",nerrors);

  /* master copy has to be unique */
  const auto& proclist = DDD_InfoProcListRange(context, PARHDR(theEdge));
  if ((nmaster = CheckProcListCons(proclist, PrioMaster)) > 1)
  {
    UserWriteF("EDGE=" EDID_FMTX " ERROR: master copy not unique, nmaster=%d:",
               EDID_PRTX(theEdge),nmaster);
    ListProcList(proclist, PrioMaster);
    UserWriteF("\n");
    nerrors++;
  }
        #endif

  if (ddd_ctrl(context).edgeData)
    if (EDVECTOR(theEdge) != NULL)
      nerrors += CheckVectorPrio(context, theElement,EDVECTOR(theEdge));

  return(nerrors);
}


/****************************************************************************/
/*
   CheckElementPrio -

   SYNOPSIS:
   INT CheckElementPrio (ELEMENT *theElement);

   PARAMETERS:
   .  theElement

   DESCRIPTION:

   RETURN VALUE:
   INT
 */
/****************************************************************************/

static INT CheckElementPrio (DDD::DDDContext& context, ELEMENT *theElement)
{
  INT i,nmaster,prio,valid_copy;
  INT nerrors = 0;
  NODE    *theNode;
  EDGE    *theEdge;
  ELEMENT *SonList[MAX_SONS];

  const auto& me = context.me();

  if (PARTITION(theElement)==me && !EMASTER(theElement))
  {
    UserWriteF(PFMT "#FATAL# MASTER ELEM=" EID_FMTX " has WRONG part=%d prio=%d\n",
               me,EID_PRTX(theElement),PARTITION(theElement),EPRIO(theElement));
    nerrors++;
  }
  if (PARTITION(theElement)!=me && !EGHOST(theElement))
  {
    UserWriteF(PFMT "#FATAL# GHOST ELEM=" EID_FMTX " has WRONG part=%d prio=%d\n",
               me,EID_PRTX(theElement),PARTITION(theElement),EPRIO(theElement));
    nerrors++;

    /* test ghost prio */
    prio = 0;
    for (i=0; i<SIDES_OF_ELEM(theElement); i++)
    {
      if (EMASTER(NBELEM(theElement,i))) prio = PrioHGhost;
    }
    if (GetSons(theElement,SonList) != 0) RETURN(1);
    if (SonList[0] != NULL) prio += PrioVGhost;

    if (EPRIO(theElement) != prio)
    {
      UserWriteF(PFMT "ERROR GHOST ELEM=" EID_FMTX
                 " has WRONG prio=%d should be prio=%d\n",
                 me,EID_PRTX(theElement),EPRIO(theElement),prio);
      nerrors++;
    }
  }

  /* check element prio */
  CHECK_OBJECT_PRIO(theElement,EPRIO,EMASTER,EGHOST,EID,"ELEM",nerrors);

  /* master copy has to be unique */
  const auto& proclist = DDD_InfoProcListRange(context, PARHDRE(theElement));
  if ((nmaster = CheckProcListCons(proclist, PrioMaster)) != 1)
  {
    UserWriteF("ELEM=" EID_FMTX " ERROR: master copy not unique, ",
               EID_PRTX(theElement),nmaster);
    if (EFATHER(theElement) != NULL)
      UserWriteF("Father=" EID_FMTX, EID_PRTX(EFATHER(theElement)));
    else
      UserWrite("Father=NULL");
    UserWriteF(" nmaster=%d:",nmaster);
    ListProcList(proclist, PrioMaster);
    UserWriteF("\n");
    nerrors++;
  }

  /* hghost copy needs to a master neighbor */
  if (EHGHOST(theElement))
  {
    valid_copy = 0;
    for (i=0; i<SIDES_OF_ELEM(theElement); i++)
    {
      if (NBELEM(theElement,i)!=NULL && EMASTER(NBELEM(theElement,i)))
        valid_copy = 1;
    }
    if (!valid_copy)
    {
      UserWriteF("ELEM=" EID_FMTX " ERROR: hghost copy with no master neighbor!\n",
                 EID_PRTX(theElement));
      nerrors++;
    }
  }

  /* vghost copy needs to a master Son */
  if (EVGHOST(theElement))
  {
    if (GetSons(theElement,SonList) != 0) RETURN(1);
    if (SonList[0] == NULL) valid_copy = 0;
    else valid_copy = 1;
    if (!valid_copy)
    {
      UserWriteF("ELEM=" EID_FMTX " ERROR: vghost copy with no master son!\n",
                 EID_PRTX(theElement));
      nerrors++;
    }
  }

  if (ddd_ctrl(context).elemData)
    if (EVECTOR(theElement) != NULL)
      nerrors += CheckVectorPrio(context, theElement,EVECTOR(theElement));

  if (ddd_ctrl(context).sideData)
  {
    for (i=0; i<SIDES_OF_ELEM(theElement); i++)
      if (SVECTOR(theElement,i) != NULL)
        nerrors += CheckVectorPrio(context, theElement,SVECTOR(theElement,i));
  }

  for (i=0; i<CORNERS_OF_ELEM(theElement); i++)
  {
    theNode = CORNER(theElement,i);
    nerrors += CheckNodePrio(context, theElement,theNode);
  }

  for (i=0; i<EDGES_OF_ELEM(theElement); i++)
  {
    theEdge = GetEdge(CORNER(theElement,CORNER_OF_EDGE(theElement,i,0)),
                      CORNER(theElement,CORNER_OF_EDGE(theElement,i,1)));
    ASSERT(theEdge != NULL);
    nerrors += CheckEdgePrio(context, theElement,theEdge);
  }

  return (nerrors);
}


/****************************************************************************/
/*
   CheckDistributedObjects - Check the uniqueness of global ids

   SYNOPSIS:
   INT CheckDistributedObjects (GRID *theGrid);

   PARAMETERS:
   .  theGrid

   DESCRIPTION:
   Compare the global ids of distributed objects which are identified.
   This is done for nodes and edges (3D).

   RETURN VALUE:
   INT
 */
/****************************************************************************/

static int Gather_ElemObjectGids (DDD::DDDContext&, DDD_OBJ obj, void *data, DDD_PROC proc, DDD_PRIO prio)
{
  ELEMENT *theElement = (ELEMENT *)obj;

  /* copy node gids into buffer */
  for (INT i=0; i<CORNERS_OF_ELEM(theElement); i++)
  {
    ((DDD_GID *)data)[i] = GID(CORNER(theElement,i));
  }

        #ifdef UG_DIM_3
  /* copy edge gids into buffer */
  for (INT i=CORNERS_OF_ELEM(theElement),j=0; i<EDGES_OF_ELEM(theElement); i++,j++)
  {
    EDGE *theEdge = GetEdge(CORNER_OF_EDGE_PTR(theElement,j,0),
                            CORNER_OF_EDGE_PTR(theElement,j,1));
    assert(theEdge!=NULL);
    ((DDD_GID *)data)[i] = GID(theEdge);
  }
        #endif

  return 0;
}

static int Scatter_ElemObjectGids (DDD::DDDContext&, DDD_OBJ obj, void *data, DDD_PROC proc, DDD_PRIO prio)
{
  ELEMENT *theElement = (ELEMENT *)obj;

  /* compare node gids with buffer gids */
  for (INT i=0; i<CORNERS_OF_ELEM(theElement); i++)
  {
    NODE* theNode = CORNER(theElement,i);
    if (((DDD_GID *)data)[i] != GID(theNode))
    {
      UserWriteF("ELEM=" EID_FMTX " #ERROR#: NODE=" ID_FMTX " gids don't match "
                 "local=%08x remote=%08x remoteproc/prio=%d/%d\n",EID_PRTX(theElement),ID_PRTX(theNode),
                 GID(theNode),((DDD_GID *)data)[i],proc,prio);
      check_distributed_objects_errors++;
      assert(0);
    }
  }

        #ifdef UG_DIM_3
  /* compare edge gids with buffer gids */
  for (INT i=CORNERS_OF_ELEM(theElement),j=0; i<EDGES_OF_ELEM(theElement); i++,j++)
  {
    EDGE* theEdge = GetEdge(CORNER_OF_EDGE_PTR(theElement,j,0),
                            CORNER_OF_EDGE_PTR(theElement,j,1));
    assert(theEdge!=NULL);
    if (((DDD_GID *)data)[i] != GID(theEdge))
    {
      UserWriteF("ELEM=" EID_FMTX " #ERROR#: EDGE=" ID_FMTX " gids don't match "
                 "local=%08x remote=%08x remoteproc/prio=%d/%d\n",EID_PRTX(theElement),ID_PRTX(theEdge),
                 GID(theEdge),((DDD_GID *)data)[i],proc,prio);
      check_distributed_objects_errors++;
      assert(0);
    }
  }
        #endif

  return 0;
}

#ifdef UG_DIM_3
static int Gather_EdgeObjectGids (DDD::DDDContext&, DDD_OBJ obj, void *data, DDD_PROC proc, DDD_PRIO prio)
{
  EDGE *theEdge = (EDGE *)obj;

  INT i = 0;

  NODE* theNode0 = NBNODE(LINK0(theEdge));
  NODE* theNode1 = NBNODE(LINK1(theEdge));
  NODE* MidNode  = MIDNODE(theEdge);

  /* copy node gids into buffer */
  ((DDD_GID *)data)[i++] = GID(theNode0);
  ((DDD_GID *)data)[i++] = GID(theNode1);
  if (MidNode != NULL)
    ((DDD_GID *)data)[i++] = GID(MidNode)+1;
  else
    ((DDD_GID *)data)[i++] = 0;

  return 0;
}

static int Scatter_EdgeObjectGids (DDD::DDDContext& context, DDD_OBJ obj, void *data, DDD_PROC proc, DDD_PRIO prio)
{
  DDD_GID remotegid;
  EDGE *theEdge = (EDGE *)obj;

  /* this check allows no edges copies of type VGHOST */
  /* since then midnode might be NULL due to local    */
  /* load balancing situation                         */
  const auto& proclist = DDD_InfoProcListRange(context, PARHDR(theEdge));
  for (auto&& [proc, prio] : proclist)
  {
    if (VGHOSTPRIO(prio))
      return 0;
  }

  INT i = 0;

  NODE* theNode0 = NBNODE(LINK0(theEdge));
  NODE* theNode1 = NBNODE(LINK1(theEdge));
  NODE* MidNode  = MIDNODE(theEdge);

  /* compare node0 gids with buffer gids */
  if (((DDD_GID *)data)[i] != GID(theNode0))
  {
    UserWriteF("EDGE=" ID_FMTX " #ERROR#: NODE0=" ID_FMTX " gids don't match "
               "local=%08x remote=%08x remoteproc/prio=%d/%d\n",
               ID_PRTX(theEdge),ID_PRTX(theNode0),
               GID(theNode0),((DDD_GID *)data)[i],proc,prio);
    check_distributed_objects_errors++;
    assert(0);
  }
  i++;

  /* compare node1 gids with buffer gids */
  if (((DDD_GID *)data)[i] != GID(theNode1))
  {
    UserWriteF("EDGE=" ID_FMTX " #ERROR#: NODE1=" ID_FMTX " gids don't match "
               "local=%08x remote=%08x remoteproc/prio=%d/%d\n",
               ID_PRTX(theEdge),ID_PRTX(theNode1),
               GID(theNode1),((DDD_GID *)data)[i],proc,prio);
    check_distributed_objects_errors++;
    assert(0);
  }
  i++;

  /* compare node0 gids with buffer gids */
  if (((DDD_GID *)data)[i]>0)
    remotegid = ((DDD_GID *)data)[i]-1;

  if (MidNode != NULL)
  {
    if (remotegid != GID(MidNode))
    {
      UserWriteF("EDGE=" ID_FMTX " #ERROR#: MIDNODE=" ID_FMTX " gids don't match "
                 "local=%08x remote=%08x remoteproc/prio=%d/%d\n",
                 ID_PRTX(theEdge),ID_PRTX(MidNode),
                 GID(MidNode),remotegid,proc,prio);
      check_distributed_objects_errors++;
      assert(0);
    }
  }
  else
  {

    if (remotegid != 0)
    {
      UserWriteF("EDGE=" ID_FMTX " #ERROR#: MIDNODE=NULL gids don't match "
                 "local=%08x remote=%08x remoteproc/prio=%d/%d\n",
                 ID_PRTX(theEdge),0,remotegid,proc,prio);
      check_distributed_objects_errors++;
      assert(0);
    }
  }
  i++;

  return 0;
}
#endif

static INT CheckDistributedObjects (GRID *theGrid)
{
  auto& context = theGrid->dddContext();
  const auto& dddctrl = ddd_ctrl(context);

  INT nerrors;
        #ifdef UG_DIM_2
  INT size = MAX_CORNERS_OF_ELEM;       /* compare the 3/4 node ids */
        #endif
        #ifdef UG_DIM_3
  INT size = MAX_CORNERS_OF_ELEM+MAX_EDGES_OF_ELEM;             /* compare 8 nodes + 12 edges */
        #endif

  check_distributed_objects_errors = 0;

  // void DDD_IFAOnewayX (DDD::DDDContext&, DDD_IF, DDD_ATTR, DDD_IF_DIR, size_t, ComProcXPtr, ComProcXPtr);
  DDD_IFAOnewayX(context,
                 dddctrl.ElementSymmVHIF,GRID_ATTR(theGrid),IF_BACKWARD,size*sizeof(DDD_GID),
                 Gather_ElemObjectGids, Scatter_ElemObjectGids);

        #ifdef UG_DIM_3
  if (0)
    DDD_IFAOnewayX(context,
                   dddctrl.BorderEdgeSymmIF,GRID_ATTR(theGrid),IF_BACKWARD,3*sizeof(DDD_GID),
                   Gather_EdgeObjectGids, Scatter_EdgeObjectGids);
        #endif

  nerrors = check_distributed_objects_errors;
  return(nerrors);
}

/****************************************************************************/
/*
   CheckInterfaces -

   SYNOPSIS:
   INT CheckInterfaces (GRID *theGrid);

   PARAMETERS:
   .  theGrid

   DESCRIPTION:

   RETURN VALUE:
   INT
 */
/****************************************************************************/

INT NS_DIM_PREFIX CheckInterfaces (GRID *theGrid)
{
  auto& context = theGrid->dddContext();
  auto& dddctrl = ddd_ctrl(context);

  INT i,j;
  ELEMENT *theElement;
  NODE    *theNode;
  EDGE    *theEdge;
  int nerrors = 0;

  /* reset USED flag of all grid objects  */
  /* set USED flag of master grid objects */
  for (j=0; j<2; j++)
  {
    for (theElement =(j==0 ? PFIRSTELEMENT(theGrid) : FIRSTELEMENT(theGrid));
         theElement!=NULL;
         theElement=SUCCE(theElement))
    {
      SETUSED(theElement,j);
      if (dddctrl.elemData)
        if (EVECTOR(theElement) != NULL)
          SETUSED(EVECTOR(theElement),j);
      if (dddctrl.sideData)
      {
        for (i=0; i<SIDES_OF_ELEM(theElement); i++)
          if (SVECTOR(theElement,i) != NULL)
            SETUSED(SVECTOR(theElement,i),j);
      }

      for (i=0; i<CORNERS_OF_ELEM(theElement); i++)
      {
        theNode = CORNER(theElement,i);
        SETUSED(theNode,j);
        SETUSED(MYVERTEX(theNode),j);
      }

      for (i=0; i<EDGES_OF_ELEM(theElement); i++)
      {
        theEdge = GetEdge(CORNER(theElement,CORNER_OF_EDGE(theElement,i,0)),
                          CORNER(theElement,CORNER_OF_EDGE(theElement,i,1)));
        ASSERT(theEdge != NULL);
        SETUSED(theEdge,j);
        if (dddctrl.edgeData)
          if (EDVECTOR(theEdge) != NULL)
            SETUSED(EDVECTOR(theEdge),j);
      }
    }
  }

  /* check validity of priorities */
  for (theElement=PFIRSTELEMENT(theGrid); theElement!=NULL;
       theElement=SUCCE(theElement))
  {
    nerrors += CheckElementPrio(theGrid->dddContext(), theElement);
  }

  /* check uniqueness of global ids for distributed nodes and edges */
  nerrors += CheckDistributedObjects(theGrid);

  /* check ddd interface consistency */
  DDD_SetOption(theGrid->dddContext(), OPT_QUIET_CONSCHECK, OPT_ON);
  nerrors += DDD_ConsCheck(theGrid->dddContext());
  DDD_SetOption(theGrid->dddContext(), OPT_QUIET_CONSCHECK, OPT_OFF);

  return(nerrors);
}

#endif
