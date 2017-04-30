/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  PANEL_LOG                        1       /* callback function: PanelLog */
#define  PANEL_LOG_BUTTON_QUIT_LOG        2       /* control type: command, callback function: QUIT_LOG */
#define  PANEL_LOG_TABLE                  3       /* control type: table, callback function: (none) */
#define  PANEL_LOG_BUTTON_EXPORT          4       /* control type: command, callback function: Export */

#define  PANEL_MAIN                       2       /* callback function: PanelMain */
#define  PANEL_MAIN_BUTTON_LOG            2       /* control type: command, callback function: Log */
#define  PANEL_MAIN_BUTTON_QUIT           3       /* control type: command, callback function: Quit */
#define  PANEL_MAIN_NUMERIC_TEMPERATURE   4       /* control type: numeric, callback function: (none) */
#define  PANEL_MAIN_LED_WARNING           5       /* control type: LED, callback function: (none) */
#define  PANEL_MAIN_RING                  6       /* control type: ring, callback function: (none) */
#define  PANEL_MAIN_STRIPCHART            7       /* control type: strip, callback function: (none) */
#define  PANEL_MAIN_COMMANDBUTTON         8       /* control type: command, callback function: Monitor */
#define  PANEL_MAIN_TIMER                 9       /* control type: timer, callback function: InfoRead */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK Export(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK InfoRead(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Log(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Monitor(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK PanelLog(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK PanelMain(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Quit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK QUIT_LOG(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
