//------------------------------------------------------------------------------
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2019 WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
///------------------------------------------------------------------------------
///
///  \file    getconfvalue.c
///
///  \version $Id: getconfvalue.c 43946 2019-10-23 11:10:18Z wrueckl_elrest $
///
///  \brief   get value from *.conf file 
///
///  \author  Wolfgang Rückl, elrest Automationssysteme GmbH
///------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>

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

int ConfReadValue(char * szFile, char * szIn, char * szOut, int iMaxLen);
void ShowHelpText();

/// \brief main function
/// \param[in]  argc number of arguments
/// \param[in]  argv arguments
/// \retval 0  SUCCESS
int main(int argc, char *argv[])
{
  int state = -1;
  int i, k;
  char szOut[256] = "";  
  char * pStr;
  int lwr = 0;
  int upr = 0;
  int iPosFile = 0;
  int iPosIdent = 0;

  
  if (argc >= 2)
  {
    if ((strcmp(argv[1], "-h") == 0) || (strcmp(argv[1], "--help") == 0))
    {
      ShowHelpText();
      state = 0;
    }
        
    for (k=1; k < argc; k++)
    {
      pStr = argv[k];
      
      if (*pStr == '-')
      {
        if (strlen(pStr) > 1)
        {
          pStr++;
          if (tolower(*pStr) == 'l')
          {
            lwr = 1;
          }
          else if (tolower(*pStr) == 'u')
          {
            upr = 1;
          }                      
        }             
      }
      else
      {
        if (iPosFile == 0)
        {
          iPosFile = k;
        }
        else if (iPosIdent == 0)
        {
          iPosIdent = k;
        }                
      }
    }
    
    if ((iPosFile > 0) && (iPosIdent > 0))
    {
      if (ConfReadValue(argv[iPosFile], argv[iPosIdent], &szOut[0], sizeof(szOut)) == 0)
      {
        if (lwr)
        {
          for (i=0; i < strlen(szOut); i++)
          {
            szOut[i] = tolower(szOut[i]);          
          }
        }
        else if (upr)
        {
          for (i=0; i < strlen(szOut); i++)
          {
            szOut[i] = toupper(szOut[i]);          
          }
        }
        printf("%s", szOut);  
        state = 0;
      }
    }    
  }

  return state;
}

//------------------------------------------------------------------------------
/// Read value from *.conf file
///
/// \param szFile   full configuration filename
/// \param szIn     pointer to identifier
/// \param szOut    pointer to output buffer
/// \param iMaxLen  szOut buffer length
///
/// \return  0 SUCCESS
/// \return -1 ERROR
//------------------------------------------------------------------------------
int ConfReadValue(char * szFile, char * szIn, char * szOut, int iMaxLen)
{
  FILE * fp;
  int i, len;
  int pos = -1;
  char szLine[1024];
  char * pStr1;
  char * pStr2;

  int state = -1;

  fp = fopen(szFile, "r+");
  if (fp == NULL)
  {
    //todo errortext
    return state;
  }

  // read lines
  while (fgets(szLine, sizeof(szLine), fp) != NULL)
  {
    pos = -1;

    pStr1 = &szLine[0];
    //trim left
    while ((*pStr1 == ' ') || (*pStr1 == '\t'))
    {
      pStr1++;
    }

    //remove comments
    len = strlen(pStr1);
    for (i = 0; i < len; i++)
    {
      if (pStr1[i] == '=')
      {
        pos = i;
        break;
      }
      else if (pStr1[i] == '#')
      {
        pStr1[i] = '\0';
        break;
      }
    }

    if (pos > 0)
    {
      //found
      pStr2 = &pStr1[pos];
      pStr2++;

      //trim left
      while ((*pStr2 == ' ') || (*pStr2 == '\t'))
      {
        pStr2++;
      }
      //trim right
      while ((pStr2[strlen(pStr2) - 1] == ' ')
          || (pStr2[strlen(pStr2) - 1] == '\t')
          || (pStr2[strlen(pStr2) - 1] == '\r')
          || (pStr2[strlen(pStr2) - 1] == '\n'))
      {
        pStr2[strlen(pStr2) - 1] = '\0';
      }

      //right pStr2


      pStr1[pos] = '\0';
      while (pStr1[strlen(pStr1) - 1] == ' ')
      {
        pStr1[strlen(pStr1) - 1] = '\0';
      }

      if (strcmp(szIn, pStr1) == 0)
      {
        //left pStr1
        strncpy(szOut, pStr2, iMaxLen);
        state = 0;
        break;
      }


    }
  }

  fclose(fp);
  return state;
}

/// \brief Print help text / usage to stdout
void ShowHelpText()
{
  printf("\n* get value from *.conf file * \n\n");
  printf("identifier=value\n");
  printf("Usage: getconfvalue <file> <identifier>\n\n");
  printf("options:\n");
  printf("-h print this help \n");
  printf("-l value as lowercase \n");
  printf("-u value as uppercase \n\n");
  printf("file:       complete file path \n");
  printf("identifier: variable name \n\n");  
}


