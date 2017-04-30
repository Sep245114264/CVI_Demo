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

#define  PANEL                            1       /* callback function: Panel */
#define  PANEL_TABLE                      2       /* control type: table, callback function: (none) */
#define  PANEL_COMMANDBUTTON              3       /* control type: command, callback function: Button */
#define  PANEL_COMMANDBUTTON_2            4       /* control type: command, callback function: Export */
#define  PANEL_COMMANDBUTTON_3            5       /* control type: command, callback function: directory */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK Button(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK directory(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Export(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Panel(int panel, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
