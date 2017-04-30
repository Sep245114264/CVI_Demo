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

#define  PANEL                            1       /* callback function: PanelCall */
#define  PANEL_TIME                       2       /* control type: numeric, callback function: (none) */
#define  PANEL_START                      3       /* control type: command, callback function: Start */
#define  PANEL_TEXTMSG                    4       /* control type: textMsg, callback function: (none) */
#define  PANEL_STOP                       5       /* control type: command, callback function: Stop */
#define  PANEL_STRIPCHART                 6       /* control type: strip, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK PanelCall(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Start(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Stop(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
