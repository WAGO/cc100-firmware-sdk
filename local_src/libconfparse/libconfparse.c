//------------------------------------------------------------------------------
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2019 WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
///------------------------------------------------------------------------------
/// \file libconfparse.c
///
/// \version $Id: libconfparse.c 43946 2019-10-23 11:10:18Z wrueckl_elrest $
///
/// \brief library for parsing config files and comandline 
///
/// \author Ralf Gliese, elrest Automationssysteme GmbH
///------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "confparse.h"

//------------------------------------------------------------------------------
// Local macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// External variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Local typedefs
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Local variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// external functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Local functions
//------------------------------------------------------------------------------

//++++++++++++++++++++++++++++++++ XmlFindPattern +++++++++++++++++++++++++++//
int XmlFindPattern(FILE *pFd_Initdata, char *cStringBufComp)
{
  char c;
  int  iGetValue;
  char cStringBuf[200];
  unsigned short usReadSigns;
  int iStrCmp;

  iGetValue = fgetc(pFd_Initdata);
  c = (char)iGetValue;
  while (iGetValue != EOF)
  {
    if(c == '<')
    {
      iGetValue = fgetc(pFd_Initdata);
      c = (char)iGetValue;
      if( iGetValue == EOF)
        return -1;

      if((c == '?') || (c == '!'))  // it`s a comment
      {
        usReadSigns = 0;
        //while((c= fgetc(pFd_Initdata)) != '>' )
        while(c != '>' )
        {
          //cStringBuf[usReadSusSlvCntigns] = c;
          //usReadSigns++;
          iGetValue = fgetc(pFd_Initdata);
          c = (char)iGetValue;
        }
        //cStringBuf[usReadSigns] = '\0';
        //printf("comment or no data \n");
        //printf("%s \n", cStringBuf);
        //c = fgetc(pFd_Initdata);
        if( iGetValue == EOF)
          return -1;
      }
      else                          // data found
      {
        usReadSigns = 0;
        cStringBuf[usReadSigns] = c;
        while( (c != ' ') && (c != '>') )
        {
          usReadSigns++;
          iGetValue = fgetc(pFd_Initdata);
          c = (char)iGetValue;
          cStringBuf[usReadSigns] = c;
          //printf("%c", c);
        }
        cStringBuf[usReadSigns] = '\0';
        iStrCmp = strcmp(cStringBuf, cStringBufComp);
        if(iStrCmp == 0)
        {
          //printf ("expression found\n");
          return 0;
        }
        //printf ("data found\n");
        //printf("%s \n", cStringBuf);
        //c = fgetc(pFd_Initdata);
        if( iGetValue == EOF)
          return -1;
      }
    }
    else
    {
      iGetValue = fgetc(pFd_Initdata);
      c = (char)iGetValue;
      if( iGetValue == EOF)
        return -1;
    }
  }
  return 0;
}
//++++++++++++++++++++++++++++++++ XmlFindPattern end +++++++++++++++++++++++//


//++++++++++++++++++++++++++++++++ XmlGetValue ++++++++++++++++++++++++++++++//
int XmlGetValue(FILE *pFd_Initdata, char *cStringBufComp, char cValueBuf[MAXBUFLEN])
{
  char c;
  int i = 0, iStrCmp;


  iStrCmp = XmlFindPattern(pFd_Initdata, cStringBufComp);
  if(iStrCmp == 0)
  {
    i=0;
    c = fgetc(pFd_Initdata);
    //printf("%c \n", c);
    while((c != '<') && (i < MAXBUFLEN))
    {
      cValueBuf[i] = c;
      c = fgetc(pFd_Initdata);
      //printf("%c \n", c);
      i++;
    }

    if(i == MAXBUFLEN)
    {
      printf ("value error\n");
      return -1;
    }
    else
    {
      cValueBuf[i] = '\0';
      return 0;
    }
  }
  else
  {
    printf ("expression not found\n");
    return -1;
  }
}
//++++++++++++++++++++++++++++++++ XmlGetValue end ++++++++++++++++++++++++++//

//++++++++++++++++++++++++++++++++ FindConfKey +++++++++++++++++++++++++++//
int FindConfKey(FILE *pFd_Initdata, char *cStringBufComp)
{
  char c;
  int  iGetValue;
  char cStringBuf[200];
  unsigned short usReadSigns;
  int iStrCmp;

  if(pFd_Initdata == NULL)
  {
    printf("error FindConfKey pFd_Initdata = NULL \n");
    return -1;
  }
  iGetValue = fgetc(pFd_Initdata);
  c = (char)iGetValue;
  while (iGetValue != EOF)
  {
    if (c == '#')
    {
	  //printf("#### found a comment \n");
      usReadSigns = 0;
      iGetValue = fgetc(pFd_Initdata);
      c = (char)iGetValue;
      if( iGetValue == EOF)
        return -1;
      //printf("-%c", c);
   
      while((c != '\n') && (iGetValue != EOF))
      {
        iGetValue = fgetc(pFd_Initdata);
        c = (char)iGetValue;
        if( iGetValue == EOF)
		  return -1;
        //printf("-%c", c);
      }

      iGetValue = fgetc(pFd_Initdata);
      c = (char)iGetValue;

      if( iGetValue == EOF)
        return -1;
    }
    else                          // data found
    {
//	  printf("#### found data \n");
      usReadSigns = 0;
//      if(c != ' ')
//		cStringBuf[usReadSigns] = c;
      while((c != '=') && (iGetValue != EOF) && (c != ' ') && (c != '\n') && (c != '\r'))
      {
        if ((c != '=') && (iGetValue != EOF) && (c != ' ') && (c != '\n') && (c != '\r'))
        {
          cStringBuf[usReadSigns] = c;
          usReadSigns++;
          //printf("*%c", c);
          iGetValue = fgetc(pFd_Initdata);
          c = (char)iGetValue;
        }

      }
      //printf("\n");
      //printf("usReadSigns = %i \n", usReadSigns);
      cStringBuf[usReadSigns] = '\0';
      //printf("cStringBuf=%s \n", cStringBuf);
      //printf("cStringBufComp=%s \n", cStringBufComp);
      
      iStrCmp = strcmp(cStringBuf, cStringBufComp);
      if(iStrCmp == 0)
      {
        //printf ("expression found\n");
        return 0;
      }
      //printf ("data found\n");
      //printf("%s \n", cStringBuf);
      //c = fgetc(pFd_Initdata);
      iGetValue = fgetc(pFd_Initdata);
      c = (char)iGetValue;      
      if( iGetValue == EOF)
        return -1;
    }
  }
  if( iGetValue == EOF)
    return -1;
        
  return 0;
}
//++++++++++++++++++++++++++++++++ FindConfKey end +++++++++++++++++++++++//

//++++++++++++++++++++++++++++++++ GetConfValue ++++++++++++++++++++++++++//
int GetConfValue(FILE *pFd_Initdata, char *cValueBuf)
{
  char c;
  int  iGetValue;
  int i = 0;

  if(pFd_Initdata == NULL)
  {
    printf("error GetConfValue pFd_Initdata = NULL \n");
    return -1;
  }

  memset(cValueBuf, 0, MAXBUFLEN);
  iGetValue = fgetc(pFd_Initdata);
  c = (char)iGetValue;
  i=0;

  while((c != '=') && (iGetValue != EOF) && (c != ' ') && (c != '\n') && (c != '\r') && (i < MAXBUFLEN))
  {
    if((c != '=') && (iGetValue != EOF) && (c != ' ') && (c != '\n') && (c != '\r') && (i < MAXBUFLEN))
	  {
      //printf(" ------> c=%c\n", c);
      *cValueBuf = c;
      cValueBuf++;
	    i++;
      iGetValue = fgetc(pFd_Initdata);
      c = (char)iGetValue;
    }
  }
  cValueBuf = 0;

  if(i >= MAXBUFLEN)
  {
    printf ("value error\n");
    return -1;
  }
  else
  {
    //cValueBuf = '\0';
    return 0;
  }

  if( iGetValue == EOF)
    return -1;

  return 0;
}
//++++++++++++++++++++++++++++++++ GetConfValue end ++++++++++++++++++++++//

//++++++++++++++++++++++++++++++++ SearchEnvVar begin +++++++++++++++++++++++//
int SearchEnvVar(FILE *pFd_Initdata, char *cStringBufComp)
{
  int i, k, iEnvVarCnt, iRetValue = 0;
  char c, cValFound;
  char cValueBuf[MAX_SIGN_CNT];
  int iStrCmp;
   
  //c = fgetc(pFd_Initdata);
  cValFound = 0;
  iEnvVarCnt = 0;
  i = 0;
  k = 0;
  c = '#';
  while(cValFound == 0)
  {
    k = 0;
    while((c != '=') && (i < MAX_SIGN_CNT)  && (cValFound == 0) && (k < MAX_SIGN_CNT))
    {
      c = fgetc(pFd_Initdata);
      //printf("%c \n", c);
      if (c == ' ')
      {
        k = 0;
      }
      else if (c != '=')
      {        
        cValueBuf[k] = c;

        k++;
        if(k >= MAX_SIGN_CNT)
        {
          cValFound = -1;
        }
      }
    }
    cValueBuf[k] = '\0';
    //printf("cValueBuf = |%s| \n", cValueBuf);

    iStrCmp = strcmp(cValueBuf, cStringBufComp);
    if(iStrCmp == 0)
    {
      //printf ("expression found\n");
      cValFound = 1;
      iEnvVarCnt++;
      break;
    }
    else
    {
      //printf ("expression not found\n");
      iEnvVarCnt++;
    }

    while((c != ' ') && (i < MAX_SIGN_CNT))
    {
      c = fgetc(pFd_Initdata);
      //printf("BLANK %c \n", c);
      i++;
      if(i >= MAX_SIGN_CNT)
      {
        cValFound = -1;
      }
    }
  }

  if (cValFound == -1)
    iRetValue = -1;
  else
    iRetValue = 0;
   
  return  iRetValue;
}
//++++++++++++++++++++++++++++++++ SearchEnvVar end +++++++++++++++++++++++++//

//++++++++++++++++++++++++++++++++ GetDisplayType begin +++++++++++++++++++++//
int GetCmdValue(FILE *pFd_Initdata, char *cStringBufComp, char *cValueBuf)
{
  char c;
  int i;

  c = fgetc(pFd_Initdata);
  i = 0;
  while((c != ' ') && (i < 20))
  {
    cValueBuf[i] = c;
    c = fgetc(pFd_Initdata);
    //printf("%c \n", c);
    i++;
  }
  if (i >= 20)
      return -1;

  cValueBuf[i] = '\0';
  //printf("cValueBuf = %s \n", cValueBuf);

  return 0;
}
//++++++++++++++++++++++++++++++++ GetDisplayType end +++++++++++++++++++++++//
