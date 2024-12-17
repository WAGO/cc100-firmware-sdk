//------------------------------------------------------------------------------
/// Copyright (c) WAGO GmbH & Co. KG
///
/// PROPRIETARY RIGHTS are involved in the subject matter of this material.
/// All manufacturing, reproduction, use and sales rights pertaining to this
/// subject matter are governed by the license agreement. The recipient of this
/// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///
///  \file     get_possible_runtimes.c
///
///  \version  $Rev$
///
///  \brief    <Insert description here>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define CUSTOM   0x01
#define CODESYS2 0x02
#define ERUNTIME 0x04
#define CODESYS3 0x08

static const char * custom = "0";
static const char * codesys3 = "1";
static const char * codesys2 = "2";
static const char * eruntime = "3";
static const char * true = "true";
static const char * false = "false";

typedef struct {
  uint8_t json;
  uint8_t help;
  uint32_t runtime;
}tOptions;

static const char optlist[] = "e3c2u1n0jh";

//------------------------------------------------------------------------------
static tOptions _ParseOptions(int argc, char ** argv)
{
  tOptions ret;
  int c;
  memset(&ret,0,sizeof(ret));

  while ((c = getopt(argc, argv, optlist)) != -1)
  {
    switch(c)
    {
      case 'u':
      case '0':
        if(ret.runtime)
        {
          ret.help = 1;
        }
        ret.runtime = CUSTOM;
        break;
      case 'n':
      case '1':
          if(ret.runtime)
          {
            ret.help = 1;
          }
          ret.runtime = CODESYS3;
          break;
      case 'c':
      case '2':
          if(ret.runtime)
          {
            ret.help = 1;
          }
          ret.runtime = CODESYS2;
          break;
      case 'e':
      case '3':
          if(ret.runtime)
          {
            ret.help = 1;
          }
          ret.runtime = ERUNTIME;
          break;
      case 'j':
          ret.json = 1;
          break;
      case 'h':
          ret.help = 1;
          break;
      case '?':
      default:
          fprintf(stderr,"unknown arg %c\n", optopt);
          ret.help =1;
          break;
    }
  }

  return ret;
}

static void _Usage(void)
{
  puts("\n* get Possible Runtimes for this device*\n");
  puts("Usage: get_possible_runtimes [options]\n");
  puts("Calling without options returns all possible Runtimes");
  puts("In this case return values are:\n0 (custom); 1 (codesys3) 2 (codesys2) 3 (eruntime)\n");
  puts("The Options -u, -0, -n ,-1, -c, -2, -e and -3 are exclusive and result in true or false.\n");
  puts("options:");
  puts("  -j       print in json format");
  puts("  -u or -0 is custom runtime possible");
  puts("  -n or -1 is codesys3 runtime possible"); // n for native
  puts("  -c or -2 is codesys2 possible");
  puts("  -e or -3 is eruntime possible\n");
}

void jsonTrueFalse(const char * start,const char * key,uint32_t trueFalse,const char * end)
{
  printf("%s\"%s\":\"",start,key);
  if(trueFalse)
  {
    printf(true);
  }
  else
  {
    printf(false);
  }
  printf("\"%s",end);
}

static void printTrueFalse(uint32_t runtime,uint32_t runtimeBits,uint8_t json)
{
  if(json)
  {
    char const * pRuntime = NULL;
    switch(runtime)
    {
      case CUSTOM:
        pRuntime = custom;
        break;
      case CODESYS3:
        pRuntime = codesys3;
        break;
      case CODESYS2:
        pRuntime = codesys2;
        break;
      case ERUNTIME:
        pRuntime = eruntime;
        break;
    }
    jsonTrueFalse("{",pRuntime,(runtimeBits & runtime),"}");
  }
  else
  {
    if((runtimeBits & runtime))
    {
      printf(true);
    }
    else
    {
      printf(false);
    }
    printf("\n");
  }
}

static void printRuntimes(uint32_t runtimeBits,uint8_t json)
{
  char   space[]=" ";
  char   komma[]=",";
  char * pDelimitter = space;

  int havePrinted=0;
  if(json)
  {
    printf("{\"availableVersions\":[");
    pDelimitter=komma;
  }

    if(runtimeBits & CUSTOM)
    {
      printf("%s",custom);
      havePrinted=1;
    }

    if(runtimeBits & CODESYS3)
    {
      if(havePrinted)
      {
        printf(pDelimitter);
      }
      printf("%s",codesys3);
      havePrinted=1;
    }

    if(runtimeBits & CODESYS2)
    {
      if(havePrinted)
      {
        printf(pDelimitter);
      }
      printf("%s",codesys2);
      havePrinted=1;
    }

    if(runtimeBits & ERUNTIME)
    {
      if(havePrinted)
      {
        printf(pDelimitter);
      }
      printf("%s",eruntime);
      havePrinted=1;
    }

    if(json)
    {
      printf("]}");
    }
    else if(havePrinted)
    {
      printf("\n");
    }
}

static uint32_t checkCodesys3(void)
{
  const char * codesys3Path = "/usr/bin/codesys3";
  return (access(codesys3Path, F_OK|X_OK) == 0);
}

int main(int argc,char *argv[])
{
  tOptions opt;
  opt = _ParseOptions(argc, argv);
  if(opt.help)
  {
    _Usage();
    exit(EXIT_FAILURE);
  }
  
  uint32_t runtimeBits=CUSTOM;
  if (checkCodesys3())
  {
    runtimeBits |= CODESYS3;
  }

  if(opt.runtime)
  {
    printTrueFalse(opt.runtime,runtimeBits,opt.json);
  }
  else
  {
    printRuntimes(runtimeBits,opt.json);
  }

  return 0;
}

//---- End of source file ------------------------------------------------------
