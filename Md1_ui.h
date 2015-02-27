/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/* Copyright (c) National Instruments 2013. All Rights Reserved.          */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  PANEL                            1       /* callback function: end */
#define  PANEL_NUM_STEPS                  2       /* control type: numeric, callback function: control_changed */
#define  PANEL_NUM_SIZEX                  3       /* control type: numeric, callback function: control_changed */
#define  PANEL_NUM_SIZEY                  4       /* control type: numeric, callback function: control_changed */
#define  PANEL_NUM_DELTAT                 5       /* control type: numeric, callback function: control_changed */
#define  PANEL_NUM_LOGPERIOD              6       /* control type: numeric, callback function: control_changed */
#define  PANEL_NUM_TEMP                   7       /* control type: numeric, callback function: control_changed */
#define  PANEL_NUM_DENSITY                8       /* control type: numeric, callback function: control_changed */
#define  PANEL_TOG_DRAW                   9       /* control type: radioButton, callback function: control_changed */
#define  PANEL_NUM_DRAWPERIOD             10      /* control type: numeric, callback function: control_changed */
#define  PANEL_NUM_DISCSIZE               11      /* control type: numeric, callback function: control_changed */
#define  PANEL_BTN_DRAW                   12      /* control type: command, callback function: button_pressed */
#define  PANEL_BTN_CLEAR                  13      /* control type: command, callback function: button_pressed */
#define  PANEL_BTN_RESET                  14      /* control type: command, callback function: button_pressed */
#define  PANEL_BTN_RUN                    15      /* control type: textButton, callback function: button_pressed */
#define  PANEL_MESSAGES                   16      /* control type: textBox, callback function: (none) */
#define  PANEL_CANVAS                     17      /* control type: canvas, callback function: (none) */
#define  PANEL_SPLITTER_2                 18      /* control type: splitter, callback function: (none) */
#define  PANEL_SPLITTER_3                 19      /* control type: splitter, callback function: (none) */
#define  PANEL_SPLITTER                   20      /* control type: splitter, callback function: (none) */
#define  PANEL_TEXTMSG                    21      /* control type: textMsg, callback function: (none) */
#define  PANEL_TEXTMSG_4                  22      /* control type: textMsg, callback function: (none) */
#define  PANEL_TEXTMSG_2                  23      /* control type: textMsg, callback function: (none) */
#define  PANEL_TEXTMSG_3                  24      /* control type: textMsg, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK button_pressed(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK control_changed(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK end(int panel, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
