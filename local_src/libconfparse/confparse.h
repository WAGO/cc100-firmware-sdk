//------------------------------------------------------------------------------
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2019-2022 WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
///------------------------------------------------------------------------------
/// \file confparse.h
///
/// \brief library for parsing config files and comandline 
///
/// \author Ralf Gliese, elrest Automationssysteme GmbH
///------------------------------------------------------------------------------


#define MAXBUFLEN 		255
#define MAX_SIGN_CNT  200

/* finds a pattern in a xml File
   parameters:
   FILE *pFd_Initdata   : points to a configuration file descriptor 
   char *cStringBufComp : pointer to search string
*/
int XmlFindPattern(FILE *pFd_Initdata, char *cStringBufComp);

/* get a string which contains the configuration data, before using this
   function you have to use XmlFindPattern() to get the correct file
   position of the configuration data
   parameters:
   FILE *pFd_Initdata   : points to a configuration file descriptor 
   char *cStringBufComp : pointer to search string
   char cValueBuf[20]   : contains to string with the configuration data
*/
int XmlGetValue(FILE *pFd_Initdata, char *cStringBufComp, char cValueBuf[20]);

/* finds a pattern in a linux like configuration File,
   # defines a comment, blanks are not allowed in configuration key or
   configuration data,
   the data format is defined as <configuration key>=<configuration data>
   After user using this function *pFd_Initdata points
   to the "configuration data"
   parameters:
   FILE *pFd_Initdata   : points to a configuration file descriptor 
   char *cStringBufComp : pointer to search string
*/
int FindConfKey(FILE *pFd_Initdata, char *cStringBufComp);

/* get the string which contains the configuration data, before using this
   function you have to use FindConfKey() to get the correct file
   position of the configuration data
   parameters:
   FILE *pFd_Initdata   : points to a configuration file descriptor 
   char cValueBuf[20]   : contains to string with the configuration data
*/
int GetConfValue(FILE *pFd_Initdata, char *cValueBuf);

/* search in the kernel command line via /proc/cmdline for a string,
   given by *cStringBufComp. If found *pFd_Initdata points to data
   and the return value is 0 
   parameters:
   FILE *pFd_Initdata   : points to a /proc/cmdline configuration file descriptor 
   char *cStringBufComp : contains the searchstring
*/
int SearchEnvVar(FILE *pFd_Initdata, char *cStringBufComp);

/* reads the displaytype from kernel command.
   The seach string is given by *cStringBufComp. If found the return value 
   is the displaytype, in case of erroe this value is 0.
   parameters:
   FILE *pFd_Initdata   : points to a /proc/cmdline configuration file descriptor 
   char *cStringBufComp : contains the searchstring = "displaytype"
*/
int GetCmdValue(FILE *pFd_Initdata, char *cStringBufComp, char *cValueBuf);

