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
#define  PANEL_BUTTON_LOG                 9       /* control type: command, callback function: Log */
#define  PANEL_START_MONITOR              10      /* control type: command, callback function: StartMonitor */
#define  PANEL_CHART_TEMPERATURE          11      /* control type: strip, callback function: (none) */
#define  PANEL_DECORATION                 12      /* control type: deco, callback function: (none) */
#define  PANEL_DECORATION_3               13      /* control type: deco, callback function: (none) */
#define  PANEL_CHART_PONDING              14      /* control type: strip, callback function: (none) */
#define  PANEL_DECORATION_2               15      /* control type: deco, callback function: (none) */
#define  PANEL_STOPMONITOR                16      /* control type: command, callback function: StopMonitor */
#define  PANEL_TIMERTRANSMIT              17      /* control type: timer, callback function: TimerTransmit */
#define  PANEL_CHART_VOLTAGE              18      /* control type: strip, callback function: (none) */

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
int  CVICALLBACK StartMonitor(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK StopMonitor(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK TimerTransmit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
