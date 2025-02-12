/*
 * Copyright (c) 2000 - 2022 WAGO GmbH & Co. KG
 * 
 * PROPRIETARY RIGHTS of WAGO GmbH & Co. KG are involved in
 * the subject matter of this material. All manufacturing, reproduction,
 * use, and sales rights pertaining to this subject matter are governed
 * by the license agreement. The recipient of this software implicitly
 * accepts the terms of the license.
 * 
 * Filename:
 *  $Workfile: linux_time.c $
 *
 * Last Modification:
 *  $Author: ea $
 *  $Modtime: 29.03.2010 $
 *  $Revision$
 */


/****************************************************************************************/
/* includes */

#include "os_api.h"                         /* operating system */

#include <pthread.h>
#include <sys/signal.h>
#include <sys/time.h>
#include <errno.h>


/****************************************************************************************/
/* functionality */

void
os_time_get_time (uint32_t* timeSec, uint32_t* timeUsec)
{
  struct timeval val;

  gettimeofday(&val, NULL);

  *timeSec = (uint32_t)val.tv_sec;
  *timeUsec = (uint32_t)val.tv_usec;
}


/****************************************************************************************/
/* end of source */
