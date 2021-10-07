//------------------------------------------------------------------------------
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2019 WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
///------------------------------------------------------------------------------
/// \file    msgtool.h
///
/// \version $Id: msgtool.h 51697 2020-09-02 13:59:51Z wrueckl_elrest $
///
/// \brief   interprocess communication using messages
///
/// \author  Wolfgang Rückl, elrest Automationssysteme GmbH
///------------------------------------------------------------------------------

#ifndef HEADER_MSG_TOOL
#define HEADER_MSG_TOOL

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------

#include <stdlib.h>

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

#define BACKLIGHT_CONF_FILE          "/etc/specific/backlight.conf"
#define PLCSELECT_CONF_FILE          "/etc/specific/plcselect.conf"
#define AUTOSTART_CONF_FILE          "/etc/specific/autostart.conf"
#define DISPLAY_CONF_FILE            "/etc/specific/display.conf"
#define MOTION_CONF_FILE             "/etc/specific/motionsensor.conf"
#define SCREEN_CONF_FILE             "/etc/specific/screen.conf"
#define TOUCHBEEPER_CONF_FILE        "/etc/specific/touchbeeper.conf"
#define AUDIO_CONF_FILE              "/etc/specific/audio.conf"
#define BOOT_CONF_FILE               "/etc/specific/bootsettings.conf"
#define GESTURE_CONF_FILE            "/etc/specific/gesture.conf"
#define ISCONFIGURED_CONF_FILE       "/etc/specific/isconfigured.conf"
#define BROWSER_SECURITY_CONF_FILE   "/etc/specific/browsersecurity.conf"
#define FONTS_CONF_FILE              "/etc/specific/fonts.conf"
#define TESTABILITY_CONF_FILE        "/etc/specific/testability.conf"
#define VIRTUALKEYBOARD_CONF_FILE    "/etc/specific/virtualkeyboard.conf"
#define ERUNTIME_CONF_FILE           "/etc/specific/eruntime.conf"
#define MICROBROWSER_CONF_FILE       "/usr/local/bin/microbrowser/mb_config.ini"

//Piped fifo names
#define DEV_VIRTUALKEYBOARD          "/dev/virtualkeyboard"
#define DEV_WEBENGINEBROWSER         "/dev/webenginebrowser"

//Ident numbers
#define MSG_TOOL_IDENT_NUMBER_BACKLIGHT 1500L
#define MSG_TOOL_IDENT_WEBKITBROWSER    1501L
#define MSG_TOOL_IDENT_STATEMONITOR     1502L

//DBUS name / path
#define DBUS_WEBKIT_NAME         "webkitbrowser.CMD"
#define DBUS_WEBKIT_PATH         "/webkitbrowser/CMD"
#define DBUS_MONITOR_NAME        "statemonitor.CMD"
#define DBUS_MONITOR_PATH        "/statemonitor/CMD"

// String size using messages
#define MSG_TOOL_LEN                     256

// message permission 
#define MSG_TOOL_PERM                   0660

//#define EEPROM_DEVICE            "/sys/class/i2c-adapter/i2c-1/1-0054/eeprom" // WTP 
#define EEPROM_DEVICE              "/sys/bus/i2c/devices/1-0054/eeprom"         // CTP/VTP
//#define EEPROM_DEVICE            "/sys/bus/i2c/devices/0-0052/eeprom"         //imx6 PTE hw

#ifdef __cplusplus
extern "C" {
#endif


// fix structure used in msg.h (do not edit)
typedef struct
{
  long msgtype; 
  char szValue[MSG_TOOL_LEN];
} tMsg2Server;

/// Configuration data list in form of Name=Value
typedef struct ConfList
{
  char * pStrLeft;             ///< left string - identifier
  char * pStrRight;            ///< right string - value
  struct ConfList * pNext;   ///< pointer to next data 
  
} tConfList;


/// Specify each parameter value
typedef struct
{
  char nameStr[64];      ///< parameter name
  char defaultStr[64];   ///< parameter default value
  char valueList[256];   ///< parameter value list comma separated
} tConfValues;

enum eMsgToolType
{
  MSG_TOOL_TYPE_VALUE_CHANGED = 1000,
  MSG_TOOL_TYPE_SERVICE_RESTART,
  MSG_TOOL_START_WEBKIT_BROWSER,
  MSG_TOOL_STOP_WEBKIT_BROWSER,
  MSG_TOOL_RELOAD_WEBKIT_BROWSER,
  MSG_TOOL_DISCONNECTED_WEBKIT_BROWSER,
  MSG_TOOL_VKEYB_WEBKIT_BROWSER,
  MSG_TOOL_CLOSE_WEBKIT_BROWSER,
  MSG_TOOL_START_WEBKIT_INSPECTOR,
  MSG_TOOL_STOP_WEBKIT_INSPECTOR,
  MSG_TOOL_START_STATEMONITOR,
  MSG_TOOL_START_STATEMONITOR_DISCONNECTED,
  MSG_TOOL_STOP_STATEMONITOR,
  MSG_TOOL_RETRY_CONNECTION,  
  MSG_TOOL_PLCLIST_WEBKIT_BROWSER,
  MSG_TOOL_SWIPE_GESTURE_WEBKIT_BROWSER,
    
  MSG_TOOL_TYPE_END
};

enum eRgbLedState
{
  RGB_LED_STATE_OFF,
  RGB_LED_STATE_GN,
  RGB_LED_STATE_GN_BLINK,
  RGB_LED_STATE_RE,
  RGB_LED_STATE_RE_BLINK,
  RGB_LED_STATE_BL,
  RGB_LED_STATE_BL_BLINK,
  
  RGB_LED_STATE_END
};


//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------

int MsgToolCreateServerQueue(key_t key);
int MsgToolCreateClientQueue(key_t key);
int MsgToolGetServerId(key_t key);
int MsgToolRemoveQueue(int id);
int MsgToolReceive(int id, tMsg2Server * pMsgData);
int MsgToolReceiveNonBlocking(int id, tMsg2Server * pMsgData);
int MsgToolSend(int id, tMsg2Server * pMsgData);


int ConfReadValues(tConfList * pHead, char * szFile);
int ConfSaveValues(tConfList * pHead, char * szFile);
tConfList * ConfCreateList();
void ConfDestroyList(tConfList * pHead);
int ConfSetValue(tConfValues * pArray, tConfList * pHead, char * szIdent, char * szValue);
int ConfAddValue(tConfList * pHead, char * szIdent, char * szValue);
int ConfGetValue(tConfList * pHead, char * szIdent, char * szOut, int iMaxLen);
int ConfGetCount(tConfList * pHead);
void ConfPrintAllValues(tConfList * pHead);
void ConfPrintAllValuesJson(tConfList * pHead, char * szOut);
int ConfIsNumber(char * pszIn);
int ConfResetToDefaultValues(tConfValues * pArray, char * szFile);
int ConfIsValueValid(tConfValues * pArray, char * pName, char * pValue);
int ConfReadLines(tConfList * pHead, char * szFile);
int ConfSaveLines(tConfList * pHead, char * szFile);
int ConfAddLine(tConfList * pHead, char * szLine);
int ConfInsertStartLine(tConfList * pHead, char * szLine);
void ConfRemoveJsonSeparatorFromEnd(char * pszJson);
void ConfExpandEscapes(const char* src, char* dest);

int stricmp(const char * pStr1, const char *pStr2);
int WriteSysFs(char *sysfs_path, char *sysfs_node, char *cValue);
int ReadSysFs(char *sysfs_path, char *sysfs_node, char * szOut, int iMaxLen);
int FileExists(char * pFilename);
int FileExistsWithoutSizeCheck(char * pFilename);
int Write2PipedFifo(char * pszDev, char * pszCmd);

int check_network_connection(char *host, int port);
int get_link_state();

unsigned int GetTimeStampMs(void);

int setRgbLed(int eState, const char * pErrorText);
int getRgbLed();

char * TrimRight(char *pStr);
void AppendSlash(char *pStr);

int IsUsbKeyboardPresent();
int IsUsbMousePresent();

#ifdef __cplusplus
}
#endif

#endif   //EOF
