#include <rs232.h>
#include <analysis.h>
#include <cvirte.h>		
#include <userint.h>
#include "ÎÂ¶È¼à²â.h"

#define COM_PORT	1

double datatemp[1000] = {0};
int num = 0;
static int panelMain;

int main (int argc, char *argv[])
{
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((panelMain = LoadPanel (0, "ÎÂ¶È¼à²â.uir", PANEL_MAIN)) < 0)
		return -1;
	
	OpenCom(COM_PORT, "");
	FlushInQ(COM_PORT);
	FlushOutQ(COM_PORT);
	
	DisplayPanel (panelMain);
	RunUserInterface ();
	CloseCom(COM_PORT);
	DiscardPanel (panelMain);
	return 0;
}

int CVICALLBACK PanelMain (int panel, int event, void *callbackData,
						   int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
			QuitUserInterface (0);
			break;
	}
	return 0;
}

int CVICALLBACK Timer (int panel, int control, int event,
					   void *callbackData, int eventData1, int eventData2)
{
	int i = 0;
	int len = 0;
	int data = 0;
	double temp = 0;
	double max = 0;
	int maxid = 0;
	double min = 0;
	int minid = 0;
	unsigned char buf[100] = {00};
	
	switch (event)
	{
		case EVENT_TIMER_TICK:
			len = GetInQLen(COM_PORT);
			if( len )
			{
				ComRd(COM_PORT,buf, len);
				
				if( len < 2 )
				{
					data = buf[len-1];
				}
				else
				{
					data = buf[len-2] *256 + buf[len-1];
				}
				if( num < 100 )
				{
					datatemp[num] = (float)(data)/10;
					++num;
				}
				else
				{
					for( i =0; i < 100; ++i )
					{
						datatemp[i] = 0;
					}
					num = 0;
				}
				for( i = 0; i < len; ++i )
				{
					buf[i] = 0;++i;
				}
				SetCtrlVal(panelMain, PANEL_MAIN_NUMERIC_CURRENT, (float)(data)/10);
				
				Sum1D(datatemp, num, &temp);
				SetCtrlVal(panelMain, PANEL_MAIN_NUMERIC_AVERAGE, (float)(temp)/num);
				
				MaxMin1D(datatemp, num, &max, &maxid, &min, &minid);
				SetCtrlVal(panelMain, PANEL_MAIN_NUMERIC_MIN, min);
				SetCtrlVal(panelMain, PANEL_MAIN_NUMERIC_MAX, max);
				
				if( (float)(data)/10 >= 50.0 )
				{
					SetCtrlAttribute(panelMain, PANEL_MAIN_LED_CEILING, ATTR_CTRL_VAL, 1);
				}
				else
				{
					SetCtrlAttribute(panelMain, PANEL_MAIN_LED_CEILING, ATTR_CTRL_VAL, 0);
				}
				if( (float)(data)/10 <= 30.0 )
				{
					SetCtrlAttribute(panelMain, PANEL_MAIN_LED_FLOOR, ATTR_CTRL_VAL, 1);
				}
				else
				{
					SetCtrlAttribute(panelMain, PANEL_MAIN_LED_FLOOR, ATTR_CTRL_VAL, 0); 
				} 
				
				SetCtrlVal(panelMain, PANEL_MAIN_NUMERIC_1, data);
				SetCtrlVal(panelMain, PANEL_MAIN_NUMERIC_2, (float)(data)/10);
				
				for( i = 0; i < num; ++i )
				{
					PlotStripChartPoint(panelMain, PANEL_MAIN_STRIPCHART, (float)(data)/10);
				}
			}
			FlushInQ(COM_PORT);
			FlushOutQ(COM_PORT);
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
			FlushInQ(COM_PORT);
			FlushOutQ(COM_PORT);
			CloseCom(COM_PORT);
			QuitUserInterface(0);
			break;
	}
	return 0;
}
