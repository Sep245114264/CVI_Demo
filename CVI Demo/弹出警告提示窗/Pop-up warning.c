#include <formatio.h>
#include <ansi_c.h>
#include <rs232.h>
#include <cvirte.h>		
#include <userint.h>
#include "Pop-up warning.h"

#define COM_PORT	1

static int panelHandle;
int Err = 0;

void DisplayComPortErr(void);

int main (int argc, char *argv[])
{
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((panelHandle = LoadPanel (0, "Pop-up warning.uir", PANEL)) < 0)
		return -1;
	DisplayPanel (panelHandle);
	//OpenCom(COM_PORT, "");
	RunUserInterface ();
	DiscardPanel (panelHandle);
	//CloseCom(COM_PORT);
	return 0;
}

int CVICALLBACK Monitor (int panel, int control, int event,
						 void *callbackData, int eventData1, int eventData2)
{
	int command = 0;
	char *buf = {0};
	char ComErr[100] = {0};
	int Err = 0;
	
	switch (event)
	{
		case EVENT_COMMIT:
			Err = ReturnRS232Err();
			MessagePopup("Warning", Err);
				//GetComStat(COM_PORT);
			DisplayComPortErr();
			//sprintf(ComErr, "%d", );
			//MessagePopup("Warning", ComErr);
			GetCtrlIndex(1, PANEL_RING, &command);
			switch( command )
			{
				case 1:
					buf = "1";
					break;
				case 2:
					buf = "2";
					break;
				case 3:
					buf = "3";
					break;
				default:
					MessagePopup("Warning", "Cannot connect the right comb");
			}
			break;
	}
	return 0;
}

int CVICALLBACK Quit (int panel, int control, int event,
					  void *callbackData, int eventData1, int eventData2)
{
	
	switch (event)
	{
		case EVENT_COMMIT:
			QuitUserInterface(0);
			break;
	}
	return 0;
}

void DisplayComPortErr(void)
{
	char ErrorMessage[200]; 
	switch( Err )
	{
		case 0:
			MessagePopup("串口警告", "没有错误");
			break;
		case -2:
			Fmt(ErrorMessage, "%s", "无效的串口号");
			MessagePopup("串口警告", ErrorMessage);
			break;
		case -3:
			Fmt(ErrorMessage, "%s", "串口未打开");
			MessagePopup("串口警告", ErrorMessage);
			break;
		case -99:
			Fmt(ErrorMessage, "%s", "打开串口超时");
			MessagePopup("串口警告", ErrorMessage);
			break;
		}
}
				
