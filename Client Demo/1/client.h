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
#define  PANEL_TRANSMIT                   2       /* control type: textBox, callback function: (none) */
#define  PANEL_RECEIVE                    3       /* control type: textBox, callback function: (none) */
#define  PANEL_CONNECTED                  4       /* control type: LED, callback function: (none) */
#define  PANEL_CLIENT_IP                  5       /* control type: string, callback function: (none) */
#define  PANEL_SERVER_IP                  6       /* control type: string, callback function: (none) */
#define  PANEL_DECORATION                 7       /* control type: deco, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK MainPanel(int panel, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
