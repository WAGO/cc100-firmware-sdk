//------------------------------------------------------------------------------
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2019-2022 WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
///------------------------------------------------------------------------------
/// \file    libmsgtool.c
///
/// \version $Id: libmsgtool.c 65689 2022-03-11 14:37:43Z falk.werner@wago.com $
///
/// \brief   lib tools, list management, messages
///
/// \author  Wolfgang Rückl, elrest Automationssysteme GmbH
///------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h> // for exit()
#include <string.h> //for memset
#include <ctype.h>
 
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <arpa/inet.h> // for inet_ntoa()
#include <net/ethernet.h>
#include <net/if.h>

#include <netinet/ip_icmp.h>   //Provides declarations for icmp header
#include <netinet/udp.h>       //Provides declarations for udp header
#include <netinet/tcp.h>       //Provides declarations for tcp header
#include <netinet/ip.h>        //Provides declarations for ip header
#include <netinet/in.h>

#include <fcntl.h>
#include <netdb.h>

#include <spawn.h>
#include <sys/wait.h>

#include <errno.h>
#include <time.h>   
#include "msgtool.h"

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------
#ifndef ERROR
#define ERROR           -1
#endif

#ifndef SUCCESS
#define SUCCESS          0
#endif

#define CMD_LED_GREEN_STATIC "/usr/bin/dbus-send --system --type=signal /wago/diagnose/ID wago.diagnose.ID10001001 int32:$(date +%%s) int32:0 string:\"%s\" boolean:true"
#define CMD_LED_GREEN_BLINK  "/usr/bin/dbus-send --system --type=signal /wago/diagnose/ID wago.diagnose.ID10001002 int32:$(date +%%s) int32:0 string:\"%s\" boolean:true"
#define CMD_LED_RED_STATIC   "/usr/bin/dbus-send --system --type=signal /wago/diagnose/ID wago.diagnose.ID10001003 int32:$(date +%%s) int32:0 string:\"%s\" boolean:true"
#define CMD_LED_RED_BLINK    "/usr/bin/dbus-send --system --type=signal /wago/diagnose/ID wago.diagnose.ID10001004 int32:$(date +%%s) int32:0 string:\"%s\" boolean:true"
#define CMD_LED_BLUE_STATIC  "/usr/bin/dbus-send --system --type=signal /wago/diagnose/ID wago.diagnose.ID10001005 int32:$(date +%%s) int32:0 string:\"%s\" boolean:true"
#define CMD_LED_BLUE_BLINK   "/usr/bin/dbus-send --system --type=signal /wago/diagnose/ID wago.diagnose.ID10001006 int32:$(date +%%s) int32:0 string:\"%s\" boolean:true"


//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// variables’ and constants’ definitions
//------------------------------------------------------------------------------

static int iRgbLedState = RGB_LED_STATE_GN;

//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------

/// \brief msg create server queue
/// \param[in]  key ident number
/// \retval server id
int MsgToolCreateServerQueue(key_t key)
{
  int res;  
  res = MsgToolGetServerId(key);  
  if (res >= 0)
  {    
    //remove queue  
    MsgToolRemoveQueue(res);    
  }
  
  //create
  res = msgget(key, MSG_TOOL_PERM | IPC_CREAT);  
  if (res < 0)
  {
    //init message queue failed
    return ERROR;
  }
  //id value
  return res;
}

/// \brief msg create client queue
/// \param[in]  key ident number
/// \retval client id
int MsgToolCreateClientQueue(key_t key)
{
  int res;
  res = msgget(IPC_PRIVATE, MSG_TOOL_PERM | IPC_CREAT);
  if (res < 0)
  {
    //init message queue failed
    return ERROR;
  }
  //id value
  return res;
}

/// \brief msg get server id
/// \param[in]  key ident number
/// \retval -1 ERROR
/// \retval server id
int MsgToolGetServerId(key_t key)
{
  int res;
  res = msgget(key, 0);
  if (res < 0)
  {
    //init message queue failed
    return ERROR;
  }
  //id value
  return res;
}

/// \brief delete queue
/// \param[in]  key ident number
/// \retval -1 ERROR
/// \retval 0 SUCCESS
int MsgToolRemoveQueue(int id)
{
  return (msgctl(id, IPC_RMID, NULL));
}

/// \brief msg receive (wait)
/// \param[in]  id ident number
/// \param[in]  pMsgData ptr to msg type / value
/// \retval -1 ERROR
/// \retval 0 SUCCESS
int MsgToolReceive(int id, tMsg2Server * pMsgData)
{
  // receive message, waiting 
  int res = msgrcv(id, pMsgData, MSG_TOOL_LEN, 0, 0);
  if (res < 0)
  {
    //error
    return ERROR;
  }
  //success
  return SUCCESS;
}

/// \brief msg receive
/// \param[in]  id ident number
/// \param[in]  pMsgData ptr to msg type / value
/// \retval -1 ERROR
/// \retval 0 SUCCESS
int MsgToolReceiveNonBlocking(int id, tMsg2Server * pMsgData)
{
  // receive messages non blocking
  int res = msgrcv(id, pMsgData, MSG_TOOL_LEN, 0, IPC_NOWAIT);
  if (res != -1)
  {
    //msg received
    return SUCCESS;
  }
  return ERROR;
}

/// \brief msg send
/// \param[in]  id ident number
/// \param[in]  pMsgData ptr to msg type / value
/// \retval -1 ERROR
/// \retval 0 SUCCESS
int MsgToolSend(int id, tMsg2Server * pMsgData)
{
  int res;
  
  if (id < 0)
    return ERROR;
  
  // send message 
  res = msgsnd(id, pMsgData, MSG_TOOL_LEN, 0);
  if (res == -1)
  {
    //error
    return ERROR;
  }
  //success
  return SUCCESS;
}

/// \brief Read configuration data from a *.conf file
///
/// read from file in form of Name=Value
/// 
/// \param[in]  pHead   List pointer to save data into
/// \param[in]  szFile  Filename with complete path
/// \retval 0  SUCCESS
/// \retval -1 ERROR
int ConfReadValues(tConfList * pHead, char * szFile)
{
  FILE * fp;
  int i, len;
  int pos = -1;
  char szLine[1024];
  char * pStr1;
  char * pStr2;
  tConfList * pList;

  pList = pHead;
  
  if (!pHead)
  {
    return ERROR;  
  }
  
  fp = fopen(szFile, "r+");
  if (fp == NULL)
  {
    //todo errortext
    return ERROR;
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

      pList->pStrRight = (char*) malloc(strlen(pStr2) + 1);
      if (pList->pStrRight)
      {
        strcpy(pList->pStrRight, pStr2);
        pStr1[pos] = '\0';
        while (pStr1[strlen(pStr1) - 1] == ' ')
        {
          pStr1[strlen(pStr1) - 1] = '\0';
        }

        pList->pStrLeft = (char*) malloc(strlen(pStr1) + 1);
        if (pList->pStrLeft)
        {
          strcpy(pList->pStrLeft, pStr1);
        }
        else
        {
          //Failure: clean up
          free(pList->pStrRight);
          pList->pStrRight = NULL;
        }
      }

      //debug printf("new 0x%x 0x%x: %s | %s\n", pList, g_pList, pList->pStrLeft, pList->pStrRight);

      //next
      pList->pNext = (tConfList*) malloc(sizeof(tConfList));
      memset(pList->pNext, 0x00, sizeof(tConfList));
      pList = pList->pNext;
    }
  }

  fclose(fp);
  return SUCCESS;
}

/// \brief Save configuration data to a *.conf file
///
/// save data to file in form of name=value
/// 
/// \param[in]  pHead   List pointer to read data from
/// \param[in]  szFile  Filename with complete path
/// \retval 0  SUCCESS
/// \retval -1 ERROR
int ConfSaveValues(tConfList * pHead, char * szFile)
{
  FILE * fp;  
  tConfList * pList = pHead;

  if (!pHead)
  {
    return ERROR;  
  }

  fp = fopen(szFile, "w+");
  if (fp == NULL)
  {
    return ERROR;
  }  
  
  //fprintf(fp, "# %s\n", szFile);

  while (pList)
  {
    if (pList->pStrLeft)
    {
       fprintf(fp,"%s=%s\n", pList->pStrLeft, pList->pStrRight); 
    }

    pList = pList->pNext;
  }  
    
  fclose(fp);
  
  system("sync");
  
  return SUCCESS;
}

/// \brief Get count of list entries
///
/// \param[in]  pHead   List pointer to read data from
/// \param[in]  szFile  Filename with complete path
/// \retval number of list entries
int ConfGetCount(tConfList * pHead)
{
  int ret = 0;
  tConfList * pList = pHead;
  
  while (pList)
  {
    if (pList->pStrLeft)
    {      
      ret++;
    }
    pList = pList->pNext;
  }

  return ret;
}

/// \brief Change a value in the list
///
/// change list entries in form of name=value
/// 
/// \param[in]  pArray   parameter array
/// \param[in]  pHead    List pointer to save data to
/// \param[in]  pszIdent identifier (left value)
/// \param[in]  pszValue new value (right value)
/// \retval 0  SUCCESS
/// \retval -1 ERROR
int ConfSetValue(tConfValues * pArray, tConfList * pHead, char * szIdent, char * szValue)
{
  int ret = ERROR;
  tConfList * pList = pHead;
  
  if ( (pArray == NULL) || (ConfIsValueValid(pArray, szIdent, szValue) == SUCCESS) )
  {
 
    while (pList)
    {
      if ((pList->pStrLeft) && (stricmp(szIdent, pList->pStrLeft) == 0))
      {
        if (pList->pStrRight)
        {
          free(pList->pStrRight);
        }

        if ((!szValue) || (szValue[0] == '\0'))
        {
          pList->pStrRight = (char*) malloc(1);
          if (pList->pStrRight)
            pList->pStrRight[0] = '\0';
        }
        else
        {          
          pList->pStrRight = (char*) malloc(strlen(szValue) + 1);
          strcpy(pList->pStrRight, szValue);
        }

        ret = SUCCESS;
        break;
      }

      pList = pList->pNext;
    }
    
    if (ret != SUCCESS)
    {
      //not found, add value
      if (ConfAddValue(pHead, szIdent, szValue) == SUCCESS)
        ret = SUCCESS;
    }
  
  }

  return ret;
}

/// \brief Add a new value to the list
///
/// append a new list entry in form of name=value
/// 
/// \param[in]  pHead   List pointer to append data to
/// \param[in]  szIdent identifier (left value)
/// \param[in]  szValue value (right value)
/// \retval 0  SUCCESS
/// \retval -1 ERROR
int ConfAddValue(tConfList * pHead, char * szIdent, char * szValue)
{
  int ret = ERROR;
  tConfList * pList = pHead;  
  tConfList * pSave = pHead;
  
  //append
  while (pList)
  {
    pSave = pList;    
    pList = pList->pNext;
  }
  
  //next
  pList = (tConfList*) malloc(sizeof(tConfList));
  if (pList)
  {
    memset(pList, 0x00, sizeof(tConfList));
    
    pList->pStrRight = (char*) malloc(strlen(szValue) + 1);
    if (pList->pStrRight)
    {
      strcpy(pList->pStrRight, szValue);

      pList->pStrLeft = (char*) malloc(strlen(szIdent) + 1);
      if (pList->pStrLeft)
      {
        strcpy(pList->pStrLeft, szIdent);
        pSave->pNext = pList;
        ret = SUCCESS;
      }
    } 
  }
  
  if (ret != SUCCESS)
  {
    //error: clean up
    if (pList)
    {
       if (pList->pStrRight)
         free(pList->pStrRight);  
       
       if (pList->pStrLeft)
         free(pList->pStrLeft);  
            
       free(pList);  
       pList = NULL;    
     }
  }

  return ret;
}

/// \brief Get value from list
///
/// get list entry in form of name=value
/// 
/// \param[in]  pHead   List pointer to read data from
/// \param[in]  szIdent identifier (left value)
/// \param[out] szOut   value needed (right value)
/// \param[in]  iMaxLen max buffer size of szOut
/// \retval 0  SUCCESS
/// \retval -1 ERROR
int ConfGetValue(tConfList * pHead, char * szIdent, char * szOut, int iMaxLen)
{
  int ret = ERROR;
  tConfList * pList = pHead;
  
  while (pList)
  {
    if ((pList->pStrLeft) && (stricmp(szIdent, pList->pStrLeft) == 0))
    {
      strncpy(szOut, pList->pStrRight , iMaxLen);      
      ret = SUCCESS;
      break;
    }

    pList = pList->pNext;
  }
  return ret;
}

/// \brief Destroy list
///
/// free list memory and remove list
/// 
/// \param[in]  pHead   List pointer to remove all data
void ConfDestroyList(tConfList * pHead)
{
  tConfList * pList = pHead;
  tConfList * pNext = NULL;
  //clean up
  while (pList)
  {
    //debug printf("clean 0x%x: %s | %s\n", pList, pList->pStrLeft, pList->pStrRight);

    if (pList->pStrLeft)
      free(pList->pStrLeft);

    if (pList->pStrRight)
      free(pList->pStrRight);

    pNext = pList->pNext;
    free(pList);
    pList = pNext;
  }
  pHead = NULL;
}

/// \brief Print all values
///
/// print values to stdout in form of name=value
/// 
/// \param[in]  pHead   List pointer to read data
void ConfPrintAllValues(tConfList * pHead)
{
  tConfList * pList = pHead;
  
  while (pList)
  {
    if (pList->pStrLeft)
    {
      printf("%s=%s\n", pList->pStrLeft, pList->pStrRight);
    }
    pList = pList->pNext;
  }  
}

/// \brief Print all values
///
/// print values to stdout in JSON format
/// 
/// \param[in]  pHead   List pointer to read data
void ConfPrintAllValuesJson(tConfList * pHead, char * szOut)
{
  tConfList * pList = pHead;
  char szLine[524] = "";
  char szConverted[512] = "";
  while (pList)
  {
    if (pList->pStrLeft)
    {
      ConfExpandEscapes(pList->pStrRight, szConverted);
      snprintf(szLine, sizeof(szLine), "\"%s\": \"%s\",", pList->pStrLeft, szConverted);
      strcat(szOut, szLine);
    }
    pList = pList->pNext;
  }  
}

/// \brief Remove separator f.e. comma
///
/// remove separator from the end of the JSON string
/// 
/// \param[in]  pszJson pointer to JSON string
void ConfRemoveJsonSeparatorFromEnd(char * pszJson)
{
  int iLen;
  if (pszJson)
  {
    iLen = strlen(pszJson);
    if (iLen > 3)
    {
      if (pszJson[iLen-1] == ',')
        pszJson[iLen-1] = '\0';
        
      if (pszJson[iLen-2] == ',')
        pszJson[iLen-2] = '\0';
    }
  }
}


/// \brief Check for number
///
/// check if the string is a whole number
/// 
/// \param[in]  szIn input string
/// \retval 0  input string is a whole number
/// \retval -1 input string is not a whole number
int ConfIsNumber(char * pszIn)
{
  int ret = SUCCESS;
  int i, len;
  char * pStr = pszIn;
  
  // is empty
  if ((!pStr) || (pStr[0] == '\0'))
    ret = ERROR;
  
  //negative number
  if (*pStr == '-')
    pStr++;
  
  len = strlen(pStr);
  if (len == 0)
    ret = ERROR;
  
  for (i=0; i < len; i++)
  {
    char c = pStr[i];
    if ((c < 0x30) || (c > 0x39))
    {
       ret = ERROR;
       break;  
    }
  }  
  
  return ret;
}

/// \brief Reset configuration data to its default values
///
/// save default config-data to file in form of name=value
/// 
/// \param[in]  pArray  Pointer to read data from (tConfValues)
/// \param[in]  szFile  Filename with complete path
/// \retval 0  SUCCESS
/// \retval -1 ERROR
int ConfResetToDefaultValues(tConfValues * pArray, char * szFile)
{
  FILE * fp = NULL;  
  int fd, k;
  char szCmd[256] = "";

  if (!pArray)
  {
    return ERROR;  
  }
  
  fd = open(szFile, O_CREAT | O_TRUNC | O_RDWR, 0666 ); 
  if (fd >= 0)
  {
    fp = fdopen(fd, "r+");
  } 

  //old fp = fopen(szFile, "w+");
  if (fp == NULL)
  {
    return ERROR;
  }  
  
  //fprintf(fp, "# %s\n", szFile);
 
  k = 0;
  while (pArray[k].nameStr[0])
  {          
    fprintf(fp,"%s=%s\n", pArray[k].nameStr, pArray[k].defaultStr); 
    k++;
  }       
    
  fclose(fp);
      
  if (k > 0)
  {
    sprintf(szCmd, "chmod a+rw %s", szFile);
    system(szCmd);
    system("sync");
    return SUCCESS;
  }
  else
  {
    return ERROR;
  }
}

/// \brief Create list
///
/// create head pointer of a list
/// call ConfDestroyList to free memory
/// 
/// \retval pointer to a new list
tConfList * ConfCreateList()
{
  tConfList * ptr = (tConfList*) malloc(sizeof(tConfList));
  if (ptr)
  {
    memset(ptr, 0x00, sizeof(tConfList));
  }
  return ptr;
}


/// \brief Check if the value to be set is valid
///
/// validation of inputs, look in comma separated list
/// 
/// \param[in]  pArray   parameter array (tConfValues)
/// \param[in]  pName    Name of the value to be set
/// \param[in]  pValue   Value to be set
/// \retval  0  Valid (allowed)
/// \retval -1  ERROR (not allowed)
int ConfIsValueValid(tConfValues * pArray, char * pName, char * pValue)
{
  int k;
  int status = ERROR;
  //tConfList * pList;
  char * pStr;
  char * pStr1;
  char buf[256];

  if (!pArray)
  {
    return ERROR;  
  }  
 
  k = 0;
  while ((pArray[k].nameStr[0]) && (status != SUCCESS))
  {       
    if (stricmp(pArray[k].nameStr, pName) == 0)
    {      
      if (pArray[k].valueList[0])
      {       
        strncpy(buf, pArray[k].valueList, sizeof(buf));        
        pStr = &buf[0];        
        
        pStr1 = strrchr(pStr, ',');
        while (pStr1)
        {
          *pStr1 = 0;
          pStr1++;
          while (*pStr1 == ' ')
            pStr1++;
          
          if (stricmp(pValue, pStr1) == 0)
          {
            status = SUCCESS;
            break;
          }         
            
          pStr1 = strrchr(pStr, ',');
        }
        
        if (status != SUCCESS)
        {
          if (stricmp(pValue, pStr) == 0)
          {
            status = SUCCESS;
          }
        }
                
      }
      else
      {
        //no value list
        status = SUCCESS;
      }
      break;
    }    
    k++;
  }          
    
  return status;
}


/// \brief compare strings ignoring case
///
/// The function compares pStr1 and pStr2
///
/// \param[in] 
/// \param[in] 
/// \retval  0 strings are equal
/// \retval <0 strings are not equal
/// \retval >0 strings are not equal
int stricmp(const char * pStr1, const char *pStr2)
{
    char c1, c2;
    int  v;

    do 
    {
      c1 = *pStr1++;
      c2 = *pStr2++;
      // The casts are necessary when pStr1 is shorter & char is signed 
      v = (unsigned int) tolower(c1) - (unsigned int) tolower(c2);
    } while ((v == 0) && (c1 != '\0') && (c2 != '\0') );

    return v;
}

/// \brief Write sysfs driver settings 
///
/// The function configures the sysfs driver
/// if an error occurs the function returns -1
///
/// \param[in] sysfs_path  path to the sysfs (/sys/devices/platform/omap_i2c.3/i2c-3/3-005a/si114x/)
/// \param[in] sysfs_node  identifier to be set
/// \param[in] cValue      new value to be set
/// \retval  0 SUCCESS
/// \retval -1 ERROR
int WriteSysFs(char *sysfs_path, char *sysfs_node, char *cValue)
{
  int fd;
  char cValueBuf[255];
  //char cSysfsValue[20];
  //char szError[256] = "";
  
  if ((strlen(sysfs_path) + strlen(sysfs_node)) < 255)
  {
    strcpy(cValueBuf, sysfs_path);
    strcat(cValueBuf, sysfs_node);
    //printf("cValueBuf=%s \n", cValueBuf);
  }  
  else
  {
    //SetLastError("WriteSysFs: strlen error");
    return ERROR;
  }
  fd = open(cValueBuf, O_WRONLY);
  if (fd < 0)
  {
    //sprintf(szError, "WriteSysFs: error open sysfs %s", cValueBuf);
    //SetLastError(szError);
    return ERROR;
  }
    
  if (write(fd, cValue, strlen(cValue)) <=0)
  {
    //sprintf(szError, "WriteSysFs: error write to sysfs %s", cValueBuf);
    close(fd);
    //SetLastError(szError);
    return ERROR;
  }

  if (close(fd) < 0)
  {
    //sprintf(szError, "WriteSysFs: error closing sysfs fd");
    //SetLastError(szError);
    return ERROR;
  }
  return SUCCESS;
}


/// \brief Read sysfs driver settings 
///
/// The function reads driver settings
/// if an error occurs the function returns -1
///
/// \param[in] sysfs_path  path to the sysfs (/sys/devices/platform/omap_i2c.3/i2c-3/3-005a/si114x/)
/// \param[in] sysfs_node  identifier to be set
/// \param[in] cValue      new value to be set
/// \retval  0 SUCCESS
/// \retval -1 ERROR
int ReadSysFs(char *sysfs_path, char *sysfs_node, char * szOut, int iMaxLen)
{
  int fd;
  char cValueBuf[256];
  
  if ((strlen(sysfs_path) + strlen(sysfs_node)) < sizeof(cValueBuf))
  {
    strcpy(cValueBuf, sysfs_path);
    strcat(cValueBuf, sysfs_node);
    //printf("cValueBuf=%s \n", cValueBuf);
  }  
  else
  {
    //SetLastError("ReadSysFs: strlen error");
    return ERROR;
  }
  fd = open(cValueBuf, O_RDONLY);
  if (fd < 0)
  {
    //sprintf(szError, "ReadSysFs: error open sysfs %s", cValueBuf);
    //SetLastError(szError);
    return ERROR;
  }
    
  if (read(fd, szOut, iMaxLen) <=0)
  {
    //sprintf(szError, "ReadSysFs: error write to sysfs %s", cValueBuf);
    close(fd);
    //SetLastError(szError);
    return ERROR;
  }

  if (close(fd) < 0)
  {
    //sprintf(szError, "ReadSysFs: error closing sysfs fd");
    //SetLastError(szError);
    return ERROR;
  }
  return SUCCESS;
}

/// \brief Read lines from a text file
///
/// read lines from file
/// 
/// \param[in]  pHead   List pointer to save data into
/// \param[in]  szFile  Filename with complete path
/// \retval 0  SUCCESS
/// \retval -1 ERROR
int ConfReadLines(tConfList * pHead, char * szFile)
{
  FILE * fp;
  char szLine[1024];
  char * pStr1;
  tConfList * pList;

  pList = pHead;
  
  if (!pHead)
  {
    return ERROR;  
  }
  
  fp = fopen(szFile, "r+");
  if (fp == NULL)
  {
    //todo errortext
    return ERROR;
  }

  // read lines
  while (fgets(szLine, sizeof(szLine), fp) != NULL)
  {
    pStr1 = &szLine[0];
    //trim left
    while ((*pStr1 == ' ') || (*pStr1 == '\t'))
    {
      pStr1++;
    }
    
    //trim right
    while ( (strlen(pStr1) > 0) && ((pStr1[strlen(pStr1) - 1] == ' ')            
        || (pStr1[strlen(pStr1) - 1] == '\t')
        || (pStr1[strlen(pStr1) - 1] == '\r')
        || (pStr1[strlen(pStr1) - 1] == '\n')))
    {
      pStr1[strlen(pStr1) - 1] = '\0';
    }

    if (strlen(pStr1) > 0)
    {
       pList->pStrLeft = (char*) malloc(strlen(pStr1) + 1);  
       if (pList->pStrLeft)
       {
         strcpy(pList->pStrLeft, pStr1);
         
         //next
         pList->pNext = (tConfList*) malloc(sizeof(tConfList));
         memset(pList->pNext, 0x00, sizeof(tConfList));
         pList = pList->pNext;
       }
    }        
  }

  fclose(fp);
  return SUCCESS;
}

/// \brief Save lines to a text file
///
/// save lines to text file
/// 
/// \param[in]  pHead   List pointer to read data from
/// \param[in]  szFile  Filename with complete path
/// \retval 0  SUCCESS
/// \retval -1 ERROR
int ConfSaveLines(tConfList * pHead, char * szFile)
{
  FILE * fp;  
  tConfList * pList = pHead;

  if (!pHead)
  {
    return ERROR;  
  }

  fp = fopen(szFile, "w+");
  if (fp == NULL)
  {
    return ERROR;
  }  
  
  while (pList)
  {
    if (pList->pStrLeft)
    {
       fprintf(fp,"%s\n", pList->pStrLeft); 
    }

    pList = pList->pNext;
  }  
    
  fclose(fp);
  
  system("sync");
  
  return SUCCESS;
}

/// \brief Add a new line
///
/// append a new line
/// 
/// \param[in]  pHead   List pointer to append data to
/// \param[in]  szLine  text
/// \retval 0  SUCCESS
/// \retval -1 ERROR
int ConfAddLine(tConfList * pHead, char * szLine)
{
  int ret = ERROR;
  tConfList * pList = pHead;  
  tConfList * pSave = pHead;
  
  //append
  while (pList)
  {
    pSave = pList;    
    pList = pList->pNext;
  }
  
  //next
  pList = (tConfList*) malloc(sizeof(tConfList));
  if (pList)
  {
    memset(pList, 0x00, sizeof(tConfList));
    
    pList->pStrRight = NULL;
    
    pList->pStrLeft = (char*) malloc(strlen(szLine) + 1);
    if (pList->pStrLeft)
    {
      strcpy(pList->pStrLeft, szLine);
      pSave->pNext = pList;
      ret = SUCCESS;
    }
     
  }
  
  if (ret != SUCCESS)
  {
    //error: clean up
    if (pList)
    {
       if (pList->pStrRight)
         free(pList->pStrRight);  
       
       if (pList->pStrLeft)
         free(pList->pStrLeft);  
            
       free(pList);  
       pList = NULL;    
     }
  }

  return ret;
}


/// \brief Add a new line at the beginning
///
/// insert first new line
/// 
/// \param[in]  pHead   List pointer to append data to
/// \param[in]  szLine  text
/// \retval 0  SUCCESS
/// \retval -1 ERROR
int ConfInsertStartLine(tConfList * pHead, char * szLine)
{
  int ret = ERROR;
  tConfList * pList = pHead;      
  tConfList * pNew = (tConfList*) malloc(sizeof(tConfList));

  //f.e. /home/.pekwm/start 
  //line one is a comment line, insert into next line

  if ((pList)&&(pNew))
  {
    memset(pNew, 0x00, sizeof(tConfList));            
    pNew->pStrRight = NULL;        
    pNew->pStrLeft = (char*) malloc(strlen(szLine) + 1);
    if (pNew->pStrLeft)
    {
      strcpy(pNew->pStrLeft, szLine);
      //insert
      pNew->pNext = pList->pNext;
      pList->pNext = pNew;            
      ret = SUCCESS;
    }     
  }
  
  if (ret != SUCCESS)
  {
    //error: clean up
    if (pNew)
    {
       if (pNew->pStrRight)
         free(pNew->pStrRight);  
       
       if (pNew->pStrLeft)
         free(pNew->pStrLeft);  
            
       free(pNew);  
       pNew = NULL;    
     }
  }

  return ret;
}

/// \brief check if file exists
/// \param[in]  pFilename absolute file name
/// \retval -1 ERROR
/// \retval 0 SUCCESS
int FileExists(char * pFilename)
// Check if file is available
{
  struct stat sts;
  if (stat(pFilename, &sts) == -1 && errno == ENOENT)
  {
    return ERROR;
  }
  else
  {   
    //check size ( not empty )
    if (sts.st_size > 1)
      return SUCCESS;
    else
      return ERROR;
  }
}

/// \brief check if file exists without size check
/// \param[in]  pFilename absolute file name
/// \retval -1 ERROR
/// \retval 0 SUCCESS
int FileExistsWithoutSizeCheck(char * pFilename)
// Check if file is available
{
  struct stat sts;
  if (stat(pFilename, &sts) == -1 && errno == ENOENT)
  {
    return ERROR;
  }
  else
  {   
    return SUCCESS;
  }
}

/// \brief get eth link state
/// \retval 0  ethernet cable is not connected / linked
/// \retval 1  ethernet cable is connected / linked
int get_link_state()
{
  char buffer[256];
  char * pLine = NULL;
  int iRet = -1;
  char szCmd1[128] = "/sbin/ethtool ethX1 | awk '/Link detected:/ {print $NF}'";
  char szCmd2[128] = "/sbin/ethtool ethX2 | awk '/Link detected:/ {print $NF}'";

  //ethtool ethX2 | awk '/Link detected:/ {print $NF}'
  //no
  //yes
  
  //ethX1
  FILE * pFile = popen(szCmd1, "r");

  if (!pFile)
  {
    return -1;
  }  
  pLine = fgets(buffer, sizeof(buffer), pFile);
  
  if (pLine)
  {
    TrimRight(pLine);
    //printf("pLineX1: %s\n", pLine);
    
    if (strcmp(pLine, "yes")==0)
    {
      iRet = 1;
    }
    else
    {
      iRet = 0;
    }
  }
  
  pclose(pFile);
  
  //ethX2
  if (iRet != 1)
  {
    pFile = popen(szCmd2, "r");
    if (!pFile)
    {
      return -1;
    }  
    pLine = fgets(buffer, sizeof(buffer), pFile);
    
    if (pLine)
    {
      TrimRight(pLine);
      //printf("pLineX2: %s\n", pLine);
      
      if (strcmp(pLine, "yes")==0)
      {
        iRet = 1;
      }
      else
      {
        iRet = 0;
      }
    }
    
    pclose(pFile);
  }
  
  return iRet;
}


int get_link_state_old()
{
  int iRet = -1;
  char szOut[32] = "";
  int n;
  
  //   /sys/class/net/eth0/carrier:1
  //   /sys/class/net/eth0/carrier:0
  
  if (ReadSysFs("/sys/class/net/br0/","carrier", szOut, sizeof(szOut)) == SUCCESS)
  {
    //success
  }
  else if (ReadSysFs("/sys/class/net/eth0/","carrier", szOut, sizeof(szOut)) == SUCCESS)
  {
    //success
  }
  else
  {
    //error
    //printf("get_link_state read error\n");
    return iRet;  
  }
  
  //remove lf cr
  n = strlen(szOut);
  if (n > 0)
  {
    while ((szOut[n-1] == 10)||(szOut[n-1] == 13))
    {
      szOut[n-1] = 0;
      n = strlen(szOut);
    }
  }
   
  if (strcmp(szOut, "1") == 0)
  {
    //link state connected  
    //printf("link state connected \n");
    iRet = 1;
  }
  else if (strcmp(szOut, "0") == 0)
  {
    //link state disconnected
    //printf("link state disconnected \n");
    iRet = 0;
  }
  else
  {
    printf("link state: %s \n", szOut);
  }
  
  return iRet;
}



/// \brief check if PLC is available
///
/// check if network connection is possible
/// 
/// \param[in]  host   hostname or ip-address
/// \param[in]  port   port number
/// \retval  0   PLC is available
/// \retval -1   PLC is not available
///
int check_network_connection(char *host, int port)
{
   struct hostent *he = NULL;
   //int one = 1;
   struct sockaddr_in s;
   //int iRet = 0;
   int iSock;

   fd_set fdset;
   struct timeval tv;

   int iDuration;
   int iTime = GetTimeStampMs();
   
   if (get_link_state() == 0)
   {
     //link state (carrier) not connected, no eth cable plugged
     return ERROR;
   }

   if ((iSock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
   {
      return ERROR;
   }

   // if the server wasnt specified as an ip address, look it up 
   if (!inet_aton(host, &s.sin_addr))
   {
      if ((he = gethostbyname(host)))
      {
         memcpy(&s.sin_addr.s_addr, he->h_addr, he->h_length);
      }
      else
      {
         close(iSock);
         return ERROR;
      }
   }

   s.sin_port = htons(port);
   s.sin_family = AF_INET;

   fcntl(iSock, F_SETFL, O_NONBLOCK);
   connect(iSock, (struct sockaddr*) &s, sizeof(s));

   FD_ZERO(&fdset);
   FD_SET(iSock, &fdset);
   tv.tv_sec = 1; // 1 second timeout 
   tv.tv_usec = 0;

   if (select(iSock + 1, NULL, &fdset, NULL, &tv) == 1)
   {
      int so_error;
      socklen_t len = sizeof so_error;

      getsockopt(iSock, SOL_SOCKET, SO_ERROR, &so_error, &len);

      if (so_error == 0)
      {
         close(iSock);
         iDuration = GetTimeStampMs() - iTime;
         if (iDuration >= (tv.tv_sec * 1000 + tv.tv_usec / 1000))
         {
            return ERROR;
         }
         return SUCCESS;
         //printf("%s:%d is open\n", addr, port);
      }
      //printf("so_error: %d \n", so_error);

      close(iSock);
      return ERROR;
   }

   close(iSock);
   iDuration = GetTimeStampMs() - iTime;
   if (iDuration >= (tv.tv_sec * 1000 + tv.tv_usec / 1000))
   {
      return ERROR;
   }
   return SUCCESS;
}


/// \brief get timestamp [ms]
///
/// getting a CLOCK_MONOTONIC timestamp
/// 
/// \retval timestamp [ms]
/// 
unsigned int GetTimeStampMs(void)
{
   struct timespec tv;
   clock_gettime(CLOCK_MONOTONIC,&tv);
   return tv.tv_sec * 1000 + tv.tv_nsec / 1000000;
} 

/// \brief set RGB LED Color and optional an error text
///
/// set color, blink mode, error text
/// 
/// \param[in]  eState     rgb led status
/// \param[in]  pErrorText error text (optional)
///
/// \retval  0   SUCCESS
/// \retval -1   ERROR
///
int setRgbLed(int eState, const char * pErrorText)
{
  int iRet = ERROR;
  char szCmd[512] = "";  
  
  return 0; //wr_test //todo //remove //iMX6
  
  //call led server to change LED color and blink status
  //set error text to diagnose logger
  switch (eState)
  {
    case RGB_LED_STATE_OFF:          
      //printf("set RGB-LED off\n");
    break;
        
    case RGB_LED_STATE_GN:
      snprintf(szCmd, sizeof(szCmd), CMD_LED_GREEN_STATIC, pErrorText);
      iRet = system(szCmd);
      //printf("set RGB-LED green\n");
    break;
    
    case RGB_LED_STATE_GN_BLINK:
      snprintf(szCmd, sizeof(szCmd), CMD_LED_GREEN_BLINK, pErrorText);
      iRet = system(szCmd);
      //printf("set RGB-LED green blinking\n");
    break;
    
    case RGB_LED_STATE_RE:
      snprintf(szCmd, sizeof(szCmd), CMD_LED_RED_STATIC, pErrorText);
      iRet = system(szCmd);
      //printf("set RGB-LED red\n");
    break;
    
    case RGB_LED_STATE_RE_BLINK:
      snprintf(szCmd, sizeof(szCmd), CMD_LED_RED_BLINK, pErrorText);
      iRet = system(szCmd); 
      //printf("set RGB-LED red blinking\n");
    break;
    
    case RGB_LED_STATE_BL:
      snprintf(szCmd, sizeof(szCmd), CMD_LED_BLUE_STATIC, pErrorText);
      iRet = system(szCmd); 
      //printf("set RGB-LED blue\n");
    break;
    
    case RGB_LED_STATE_BL_BLINK:
      snprintf(szCmd, sizeof(szCmd), CMD_LED_BLUE_BLINK, pErrorText);
      iRet = system(szCmd);
      //printf("set RGB-LED blue blinking\n");
    break;
    
    default:
      iRet = ERROR;    
  }
  
  if (iRet == SUCCESS)
  {
    iRgbLedState = eState;
    //if ((pErrorText) && (strlen(pErrorText) > 0))
    //{
    //  printf("%s\n", pErrorText);
    //}
  }
  
  return (iRet);  
}

/// \brief get RGB LED state
///
/// get color, blink mode, current state
/// 
/// \retval state
///
int getRgbLed()
{
  int ret = iRgbLedState;
  //todo
  //RGB_LED_STATE_GN,
  //RGB_LED_STATE_GN_BLINK,
  //RGB_LED_STATE_RE,
  //RGB_LED_STATE_RE_BLINK,
  //RGB_LED_STATE_BL,
  //RGB_LED_STATE_BL_BLINK,
  
  return ret; 
}

/// \brief trim string on the right side
///
/// remove line feed, carriage return, spaces from the right
/// 
/// \param[in] pStr  pointer to string  
///
/// \retval pointer to string
///
char * TrimRight(char *pStr)
{
  if (pStr)
  {  
    int n = strlen(pStr);
    if (n > 0)
    {
      //remove lf cr sp
      while ((pStr[n-1] == 10)||(pStr[n-1] == 13)||(pStr[n-1] == 32))
      {
        pStr[n-1] = 0;
        n = strlen(pStr);
      }
    }
  }
  return pStr;
}

/// \brief append slash
///
/// append slash to the end of a string
/// 
/// \param[in] pStr  pointer to string  
///
void AppendSlash(char *pStr)
{
  if (pStr)
  {
    int len = strlen(pStr);
    if (pStr[len-1] != '/')
      strcat(pStr, "/");
  }
}


/// \brief usb keyboard available
///
/// is a usb keyboard plugged in
/// 
/// \retval  0   usb keyboard present
/// \retval -1   no usb keyboard present
///
int IsUsbKeyboardPresent()
{
  
/*  

I: Bus=0003 Vendor=413c Product=2106 Version=0110
N: Name="DELL Dell QuietKey Keyboard"
P: Phys=usb-ehci-omap.0-1.3/input0
S: Sysfs=/devices/platform/usbhs_omap/ehci-omap.0/usb1/1-1/1-1.3/1-1.3:1.0/input/input4
U: Uniq=
H: Handlers=sysrq kbd event4 led_event_handler
B: PROP=0
B: EV=120013
B: KEY=10000 7 ff9f207a c14057ff febeffdf ffefffff ffffffff fffffffe
B: MSC=10
B: LED=7

*/  
  
/*

Decoding 120013

The value 120013 is a hexadecimal. As binary it gives us:

0x120013 == 0001 0010 0000 0000 0001 0011 binary
               1    2    0    0    1    3

Numbered from right they are:

   2            1               <= offset (10's)
3210 9876 5432 1098 7654 3210   <= offset (counted from right)
0001 0010 0000 0000 0001 0011   <= binary

Set bits are:
   0, 1, 4, 17, 20

Then check input.h you find that they correspond to:

   0  EV_SYN (0x00)
   1  EV_KEY (0x01)
   4  EV_MSC (0x04)
  17  EV_LED (0x11)
  20  EV_REP (0x14)

To check what they mean a quick introduction is given by kernel Documentation.

* EV_SYN:
  - Used as markers to separate events. Events may be separated in time or in
    space, such as with the multitouch protocol.

* EV_KEY:
  - Used to describe state changes of keyboards, buttons, or other key-like
    devices.

* EV_MSC:
  - Used to describe miscellaneous input data that do not fit into other types.

* EV_LED:
  - Used to turn LEDs on devices on and off.

* EV_REP:
  - Used for autorepeating devices.

 
*/  
  
 
  FILE * fp;
  int ret = ERROR;
  char szLine[1024];
  char * pStr;

  fp = fopen("/proc/bus/input/devices", "r");
  if (fp == NULL)
  {
    //printf("could not open file: /proc/bus/input/devices\n");
    return ret;
  }

  // read lines
  while (fgets(szLine, sizeof(szLine), fp) != NULL)
  {
    //B: EV=120013
    pStr = &szLine[3];
    if (*pStr == 'E') 
    {
      pStr++;
      if (*pStr == 'V')
      {
        pStr++;
        if (*pStr == '=')
        {
          pStr++;
          TrimRight(pStr);
          if (strcmp(pStr, "120013")==0)
          {            
            //printf("DEBUG: %s\n", pStr);
            ret = SUCCESS;
            break;
          }
        }
      } 
    }
  }

  fclose(fp);
  //printf("IsUsbKeyboardPresent done: %d\n", ret);
  return ret;
}


/// \brief usb mouse available
///
/// is a usb mouse plugged in
/// 
/// \retval  0   usb mouse present
/// \retval -1   no usb mouse present
///
int IsUsbMousePresent()
{
  
/*  

I: Bus=0003 Vendor=0461 Product=4d22 Version=0111
N: Name="USB Optical Mouse"
P: Phys=usb-ci_hdrc.1-1.3/input0
S: Sysfs=/devices/soc0/soc/2100000.aips-bus/2184200.usb/ci_hdrc.1/usb2/2-1/2-1.3/2-1.3:1.0/0003:0461:4D22.0004/input/input7
U: Uniq=
H: Handlers=mouse1 event4
B: PROP=0
B: EV=17
B: KEY=70000 0 0 0 0 0 0 0 0
B: REL=103
B: MSC=10


I: Bus=0003 Vendor=045e Product=0084 Version=0110
N: Name="Microsoft Basic Optical Mouse"
P: Phys=usb-ci_hdrc.1-1.3/input0
S: Sysfs=/devices/soc0/soc/2100000.aips-bus/2184200.usb/ci_hdrc.1/usb2/2-1/2-1.3/2-1.3:1.0/0003:045E:0084.0005/input/input8
U: Uniq=
H: Handlers=mouse1 event4
B: PROP=0
B: EV=17
B: KEY=70000 0 0 0 0 0 0 0 0
B: REL=103
B: MSC=10


I: Bus=0003 Vendor=046d Product=c050 Version=0110
N: Name="Logitech USB-PS/2 Optical Mouse"
P: Phys=usb-ci_hdrc.1-1.3/input0
S: Sysfs=/devices/soc0/soc/2100000.aips-bus/2184200.usb/ci_hdrc.1/usb2/2-1/2-1.3/2-1.3:1.0/0003:046D:C050.0007/input/input10
U: Uniq=
H: Handlers=mouse1 event4
B: PROP=0
B: EV=17
B: KEY=ff0000 0 0 0 0 0 0 0 0
B: REL=143
B: MSC=10

*/  
 
  FILE * fp;
  int ret = ERROR;
  char szLine[1024];
  char * pStr;

  fp = fopen("/proc/bus/input/devices", "r");
  if (fp == NULL)
  {
    //printf("could not open file: /proc/bus/input/devices\n");
    return ret;
  }

  // read lines
  while (fgets(szLine, sizeof(szLine), fp) != NULL)
  {
    //B: EV=17
    pStr = &szLine[3];
    if (*pStr == 'E') 
    {
      pStr++;
      if (*pStr == 'V')
      {
        pStr++;
        if (*pStr == '=')
        {
          pStr++;
          TrimRight(pStr);
          if (strcmp(pStr, "17")==0)
          {            
            //printf("DEBUG: %s\n", pStr);
            ret = SUCCESS;
            break;
          }
        }
      } 
    }
  }

  fclose(fp);
  //printf("IsUsbMousePresent done: %d\n", ret);
  return ret;
}

/// \brief expand escapes
///
/// expand escapes converting string to JSON
/// 
/// \param[in] src  pointer to source string  
/// \param[in] dest  pointer to destination string  
///
void ConfExpandEscapes(const char* src, char* dest) 
{
  char c;

  while ((c = *(src++))) {
    switch(c) {
      case '\a': 
        *(dest++) = '\\';
        *(dest++) = 'a';
        break;
      case '\b': 
        *(dest++) = '\\';
        *(dest++) = 'b';
        break;
      case '\t': 
        *(dest++) = '\\';
        *(dest++) = 't';
        break;
      case '\n': 
        *(dest++) = '\\';
        *(dest++) = 'n';
        break;
      case '\v': 
        *(dest++) = '\\';
        *(dest++) = 'v';
        break;
      case '\f': 
        *(dest++) = '\\';
        *(dest++) = 'f';
        break;
      case '\r': 
        *(dest++) = '\\';
        *(dest++) = 'r';
        break;
      case '\\': 
        *(dest++) = '\\';
        *(dest++) = '\\';
        break;
      case '\"': 
        *(dest++) = '\\';
        *(dest++) = '\"';
        break;
      default:
        *(dest++) = c;
     }
  }

  *dest = '\0'; // Ensure nul terminator 
}

/// \brief Write into piped fifo file
///
/// The function writes text into a piped fifo file
/// Make sure the command ends with a newline \n
/// if an error occurs the function returns -1
///
/// \param[in] pszDev  path to the piped fifo (f.e. /dev/webenginebrowser)
/// \param[in] pszCmd  command to send (f.e. "close\n")
/// \retval  0 SUCCESS
/// \retval -1 ERROR
int Write2PipedFifo(char * pszDev, char * pszCmd)
{
  int ret = ERROR;
  if ((pszDev) && (pszCmd))
  {
    if (FileExistsWithoutSizeCheck(pszDev) == SUCCESS)
    {
      int fd = open(pszDev, O_RDWR | O_NONBLOCK);
      if (fd >= 0)
      {
        //char * pCmd = "close\n";
        if (write(fd, pszCmd, strlen(pszCmd)) > 0)
          ret = SUCCESS;
          
        close(fd);
      }
    }
  }
  return ret;
}

//howto char *argv[] = { "addgroup", "users", NULL };
//howto char *pCmd = "/bin/addgroup";
//howto int ret = SystemCall(pCmd, argv);
extern char **environ;
int SystemCall(char * pCmd, char * argv[])
{
  int iRet = 0;
  pid_t pid;
  int status;
  fflush(NULL);
  status = posix_spawn(&pid, pCmd, NULL, NULL, argv, environ);
  if (status == 0)
  {
    fflush(NULL);
    if (waitpid(pid, &status, 0) != -1)
    {
      iRet = WEXITSTATUS(status);
    }
    else
    {
      iRet = -1;
    }
  }
  else
  {
    iRet = -1;
  }
  return iRet;
}

int SystemCallExt(char * pCmd, char * argv[], char * pOut, int iOutSize)
{
  int iRet = 0;
  int pipe_file_descriptors[2];
  posix_spawn_file_actions_t actions;
  pid_t pid;
  int status;
  
  //sanity check
  if (pOut == NULL)
  {
    return -1;
  }
  if (iOutSize <= 0)
  {
    return -1;
  }
  //init spawn and pipe
  if (pipe(pipe_file_descriptors) == -1) 
  {
    return -1;
  }
  if (posix_spawn_file_actions_init(&actions) != 0) 
  {
    return -1;
  }
  if (posix_spawn_file_actions_addclose(&actions, pipe_file_descriptors[0]) != 0) 
  {
    return -1;
  }
  if (posix_spawn_file_actions_adddup2(&actions, pipe_file_descriptors[1], 1) != 0) 
  {
    return -1;
  }
  //call prg
  status = posix_spawn(&pid, pCmd, &actions, NULL, argv, environ);
  if (status == 0)
  {
    fflush(NULL);
    close(pipe_file_descriptors[1]);
    FILE *f = fdopen(pipe_file_descriptors[0], "r");
    if (f == NULL)
    {
      return -1;
    }
    fflush(NULL);
    //get stdout
    fgets(pOut, iOutSize, f);
    fclose(f);
    if (waitpid(pid, &status, 0) != -1)
    {
      iRet = WEXITSTATUS(status);
    }
    else
    {
      iRet = -1;
    }
  }
  else
  {
    iRet = -1;
  }
  return iRet;
}
