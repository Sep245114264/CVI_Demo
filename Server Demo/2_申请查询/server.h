/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/* Copyright (c) National Instruments 2017. All Rights Reserved.          */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  PANEL                            1       /* callback function: MainPanel */
#define  PANEL_READ_VOLTAGE               2       /* control type: command, callback function: ReadVoltage */
#define  PANEL_READ_TEMPERATURE           3       /* control type: command, callback function: ReadTemperature */
#define  PANEL_READ_STATE_PONDING         4       /* control type: command, callback function: ReadStatePonding */
#define  PANEL_CONNECTED                  5       /* control type: LED, callback function: (none) */
#define  PANEL_STRING_VOLTAGE             6       /* control type: string, callback function: (none) */
#define  PANEL_STRING_TEMPERATURE         7       /* control type: string, callback function: (none) */
#define  PANEL_STATE_PONDING              8       /* control type: LED, callback function: (none) */
#define  PANEL_CLIENT_IP                  9       /* control type: string, callback function: (none) */
#define  PANEL_SERVER_IP                  10      /* control type: string, callback function: (none) */
#define  PANEL_DECORATION                 11      /* control type: deco, callback function: (none) */
#define  PANEL_DECORATION_2               12      /* control type: deco, callback function: (none) */
#define  PANEL_BUTTON_LOG                 13      /* control type: command, callback function: Log */

#define  PANELLOG                         2       /* callback function: Panel_log */
#define  PANELLOG_LOG                     2       /* control type: textBox, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK Log(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK MainPanel(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Panel_log(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ReadStatePonding(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ReadTemperature(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ReadVoltage(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
