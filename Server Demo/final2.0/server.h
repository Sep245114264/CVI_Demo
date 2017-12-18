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
#define  PANEL_CONNECTED                  2       /* control type: LED, callback function: (none) */
#define  PANEL_STRING_VOLTAGE             3       /* control type: string, callback function: (none) */
#define  PANEL_STRING_TEMPERATURE         4       /* control type: string, callback function: (none) */
#define  PANEL_STATE_PONDING              5       /* control type: LED, callback function: (none) */
#define  PANEL_START_MONITOR              6       /* control type: command, callback function: StartMonitor */
#define  PANEL_DECORATION_3               7       /* control type: deco, callback function: (none) */
#define  PANEL_STOPMONITOR                8       /* control type: command, callback function: StopMonitor */
#define  PANEL_MAX_VOLTAGE                9       /* control type: numeric, callback function: (none) */
#define  PANEL_STATE_VOLTAGE              10      /* control type: LED, callback function: (none) */
#define  PANEL_MIN_VOLTAGE                11      /* control type: numeric, callback function: (none) */
#define  PANEL_MAX_TEMPERATURE            12      /* control type: numeric, callback function: (none) */
#define  PANEL_MIN_TEMPERATURE            13      /* control type: numeric, callback function: (none) */
#define  PANEL_STATE_TEMPERATURE          14      /* control type: LED, callback function: (none) */
#define  PANEL_STATE_MONITOR              15      /* control type: LED, callback function: (none) */
#define  PANEL_DATE                       16      /* control type: string, callback function: (none) */
#define  PANEL_TIME                       17      /* control type: string, callback function: (none) */
#define  PANEL_DECORATION                 18      /* control type: deco, callback function: (none) */
#define  PANEL_TABLE_TEMPERATURE          19      /* control type: table, callback function: ResultTemperature */
#define  PANEL_TABLE_PONDING              20      /* control type: table, callback function: ResultPonding */
#define  PANEL_TABLE_VOLTAGE              21      /* control type: table, callback function: ResultVoltage */
#define  PANEL_BUTTON_LOG                 22      /* control type: command, callback function: Log */
#define  PANEL_DECORATION_2               23      /* control type: deco, callback function: (none) */
#define  PANEL_BUTTON_CLOSE               24      /* control type: command, callback function: ButtonClose */
#define  PANEL_CHART_VOLTAGE              25      /* control type: strip, callback function: (none) */
#define  PANEL_CHART_PONDING              26      /* control type: strip, callback function: (none) */
#define  PANEL_CHART_TEMPERATURE          27      /* control type: strip, callback function: (none) */
#define  PANEL_DECORATION_5               28      /* control type: deco, callback function: (none) */
#define  PANEL_TIMER_SAVE                 29      /* control type: timer, callback function: TimerSave */
#define  PANEL_BUTTONSETTING              30      /* control type: command, callback function: ButtonSetting */
#define  PANEL_SAVEPONDING                31      /* control type: command, callback function: CallSavePonding */
#define  PANEL_SAVETEMPERATURE            32      /* control type: command, callback function: CallSaveTemperature */
#define  PANEL_SAVEVOLTAGE                33      /* control type: command, callback function: CallSaveVoltage */
#define  PANEL_TIMER_DISPLAY              34      /* control type: timer, callback function: TimerDisplay */
#define  PANEL_EXPORTEXCEL                35      /* control type: command, callback function: ExportExcel */
#define  PANEL_TIMERTRANSMIT              36      /* control type: timer, callback function: TimerTransmit */
#define  PANEL_SERVERIP                   37      /* control type: string, callback function: (none) */
#define  PANEL_REGISTER                   38      /* control type: command, callback function: Register */
#define  PANEL_LISTENPORT                 39      /* control type: string, callback function: (none) */

#define  PANEL_INFO                       2       /* callback function: PanelInfo */
#define  PANEL_INFO_TEXTMSG_2             2       /* control type: textMsg, callback function: (none) */
#define  PANEL_INFO_TEXTMSG               3       /* control type: textMsg, callback function: (none) */

#define  PANELLOG                         3       /* callback function: Panel_log */
#define  PANELLOG_LOG                     2       /* control type: textBox, callback function: (none) */

#define  PANELSET                         4       /* callback function: PanelSetting */
#define  PANELSET_SAVETIME                2       /* control type: numeric, callback function: (none) */
#define  PANELSET_SAVETYPE                3       /* control type: ring, callback function: (none) */
#define  PANELSET_TEXTMSG                 4       /* control type: textMsg, callback function: (none) */
#define  PANELSET_SAVESETTING             5       /* control type: command, callback function: SaveSetting */
#define  PANELSET_SERVERIP1               6       /* control type: string, callback function: (none) */
#define  PANELSET_TEXTMSG_2               7       /* control type: textMsg, callback function: (none) */
#define  PANELSET_SERVERIP2               8       /* control type: string, callback function: (none) */
#define  PANELSET_TEXTMSG_3               9       /* control type: textMsg, callback function: (none) */
#define  PANELSET_SERVERIP3               10      /* control type: string, callback function: (none) */
#define  PANELSET_SERVERIP4               11      /* control type: string, callback function: (none) */
#define  PANELSET_TEXTMSG_4               12      /* control type: textMsg, callback function: (none) */
#define  PANELSET_TEXTMSG_5               13      /* control type: textMsg, callback function: (none) */
#define  PANELSET_PORTNUM                 14      /* control type: string, callback function: (none) */
#define  PANELSET_TEXTMSG_6               15      /* control type: textMsg, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK ButtonClose(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ButtonSetting(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CallSavePonding(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CallSaveTemperature(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CallSaveVoltage(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ExportExcel(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Log(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK MainPanel(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Panel_log(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK PanelInfo(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK PanelSetting(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Register(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ResultPonding(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ResultTemperature(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ResultVoltage(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SaveSetting(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK StartMonitor(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK StopMonitor(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK TimerDisplay(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK TimerSave(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK TimerTransmit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
