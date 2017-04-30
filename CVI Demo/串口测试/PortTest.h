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

#define  PANEL                            1       /* callback function: PanelCB */
#define  PANEL_INPUT_STRING               2       /* control type: string, callback function: (none) */
#define  PANEL_OUTPUT_STRING              3       /* control type: string, callback function: (none) */
#define  PANEL_SEND_DATA                  4       /* control type: command, callback function: SendData */
#define  PANEL_RECV_DATA                  5       /* control type: command, callback function: ReceiveData */
#define  PANEL_AUITBUTTON                 6       /* control type: command, callback function: Quit */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK PanelCB(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Quit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ReceiveData(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SendData(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
