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
#define  PANEL_STRING                     2       /* control type: string, callback function: Transmit */
#define  PANEL_TRANSMIT                   3       /* control type: textBox, callback function: (none) */
#define  PANEL_RECEIVE                    4       /* control type: textBox, callback function: (none) */
#define  PANEL_ONLINE                     5       /* control type: LED, callback function: (none) */
#define  PANEL_CONNECTED                  6       /* control type: LED, callback function: (none) */
#define  PANEL_SERVER_IP                  7       /* control type: string, callback function: (none) */
#define  PANEL_CLIENT_IP                  8       /* control type: string, callback function: (none) */
#define  PANEL_TEXTMSG                    9       /* control type: textMsg, callback function: (none) */
#define  PANEL_DECORATION                 10      /* control type: deco, callback function: (none) */
#define  PANEL_DECORATION_2               11      /* control type: deco, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK MainPanel(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Transmit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
