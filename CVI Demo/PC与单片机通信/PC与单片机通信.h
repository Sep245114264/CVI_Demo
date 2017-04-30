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

#define  PANEL                            1
#define  PANEL_TEXTMSG                    2       /* control type: textMsg, callback function: (none) */
#define  PANEL_TEXTMSG_2                  3       /* control type: textMsg, callback function: (none) */
#define  PANEL_COMMANDBUTTON              4       /* control type: command, callback function: Close */
#define  PANEL_NUMERIC_1                  5       /* control type: numeric, callback function: (none) */
#define  PANEL_TIMER                      6       /* control type: timer, callback function: Timer */
#define  PANEL_NUMERIC_2                  7       /* control type: numeric, callback function: (none) */
#define  PANEL_STRIPCHART                 8       /* control type: strip, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK Close(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Timer(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
