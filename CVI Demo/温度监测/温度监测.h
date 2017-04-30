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

#define  PANEL_MAIN                       1       /* callback function: PanelMain */
#define  PANEL_MAIN_NUMERIC_CURRENT       2       /* control type: numeric, callback function: (none) */
#define  PANEL_MAIN_NUMERIC_AVERAGE       3       /* control type: numeric, callback function: (none) */
#define  PANEL_MAIN_NUMERIC_MIN           4       /* control type: numeric, callback function: (none) */
#define  PANEL_MAIN_NUMERIC_MAX           5       /* control type: numeric, callback function: (none) */
#define  PANEL_MAIN_DECORATION            6       /* control type: deco, callback function: (none) */
#define  PANEL_MAIN_TEXTMSG_4             7       /* control type: textMsg, callback function: (none) */
#define  PANEL_MAIN_LED_CEILING           8       /* control type: LED, callback function: (none) */
#define  PANEL_MAIN_LED_FLOOR             9       /* control type: LED, callback function: (none) */
#define  PANEL_MAIN_TEXTMSG_2             10      /* control type: textMsg, callback function: (none) */
#define  PANEL_MAIN_TEXTMSG_3             11      /* control type: textMsg, callback function: (none) */
#define  PANEL_MAIN_NUMERIC_1             12      /* control type: numeric, callback function: (none) */
#define  PANEL_MAIN_NUMERIC_2             13      /* control type: numeric, callback function: (none) */
#define  PANEL_MAIN_TEXTMSG_1             14      /* control type: textMsg, callback function: (none) */
#define  PANEL_MAIN_COMMANDBUTTON         15      /* control type: command, callback function: Quit */
#define  PANEL_MAIN_DECORATION_2          16      /* control type: deco, callback function: (none) */
#define  PANEL_MAIN_TIMER                 17      /* control type: timer, callback function: Timer */
#define  PANEL_MAIN_STRIPCHART            18      /* control type: strip, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK PanelMain(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Quit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Timer(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
