// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:
/****************************************************************************/
/*                                                                          */
/* File:      sock.h                                                        */
/*                                                                          */
/* Purpose:   header file for using TCP and UDP sockets                     */
/*                                                                          */
/* Author:    Klaus Birken                                                  */
/*            Institut fuer Computeranwendungen III                         */
/*            Universitaet Stuttgart                                        */
/*            Pfaffenwaldring 27                                            */
/*            70569 Stuttgart                                               */
/*            internet: birken@ica3.uni-stuttgart.de                        */
/*                                                                          */
/* History:   960820 kb  begin                                              */
/*                                                                          */
/* Remarks:                                                                 */
/*                                                                          */
/****************************************************************************/

#ifndef __KB_SOCK_H__
#define __KB_SOCK_H__


/****************************************************************************/
/*                                                                          */
/* auto include mechanism and other include files                           */
/*                                                                          */
/****************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>


#ifndef __COMPILER__
#include "compiler.h"
#endif


#ifndef INADDR_NONE
#define INADDR_NONE  0xffffffff   /* should be in <netinet/in.h> */
#endif


/****************************************************************************/

/*
        settings for TCP protocol
 */

#define SERV_TCP_PORT_DEFAULT  6666
#define SERV_TCP_PORT_MIN      6000
#define SERV_TCP_PORT_MAX      6999



/****************************************************************************/

enum DevCmds
{
  DC_NN = 0,
  DC_InitScreen,
  DC_WriteString,
  DC_GetNextUGEvent,
  DC_MousePosition,
  DC_MouseStillDown,

  DC_InitRemotePort,

  DC_OpenOutput,
  DC_CloseOutput,
  DC_ActivateOutput,
  DC_UpdateOutput,

  DC_Move,
  DC_Draw,
  DC_Polyline,
  DC_InversePolyline,
  DC_Polygon,
  DC_InversePolygon,
  DC_ErasePolygon,
  DC_Polymark,
  DC_Text,
  DC_CenteredText,
  DC_ClearViewPort,
  DC_SetLineWidth,
  DC_SetTextSize,
  DC_SetMarkerSize,
  DC_SetMarker,
  DC_SetColor,
  DC_SetPaletteEntry,
  DC_SetNewPalette,
  DC_GetPaletteEntry,
  DC_Flush
};

extern char *cmd_text[];


/****************************************************************************/

int SocketRead (int, char *, int);
int SocketWrite (int, char *, int);
int SocketReadString (int, char *, int);


void SocketWriteCmd (int, int);
void SocketWriteInt (int, int);
void SocketWriteIntN (int, int *, int);
void SocketWriteLong (int, long);
void SocketWriteString (int, char *);
void SocketWriteData (int, const char *, int);

INT SocketReadINT (int);
void SocketReadIntN (int, int *, int);
long SocketReadLong (int);


/****************************************************************************/

#endif
