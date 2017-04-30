#include <formatio.h>
#include <utility.h>
#include <ansi_c.h>
#include <rs232.h>
#include <cvirte.h>		
#include <userint.h>
#include "UI.h"
#define COM_PORT1	1
#define COM_PORT2	2

int order[10] = 
{
	0x01, 0x11, 0x02, 0x11,
	0x02, 0x11, 0x02, 0x87,
	0x02, 0x9A
};

enum LiftIndex
{
	first = 1,
	second,
	third,
	forth
};

static int panelMain;
static int panelLog;
static int record_num = 0;
int ComIndex = 0;
int Error = 0;
int StartMonitor = 0;

void AddRecordToTable(int panel, int control, int record_num, float temperature);
//void DisplayErrorMessage(int Error);
void DisplayErrorMessage(void);

int main (int argc, char *argv[])
{
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((panelMain = LoadPanel (0, "UI.uir", PANEL_MAIN)) < 0)
		return -1;
	if ((panelLog = LoadPanel (0, "UI.uir", PANEL_LOG)) < 0)
		return -1;
	//MessagePopup("warning", ReturnRS232Err());
	
	//OpenCom(COM_PORT1, "");
	//FlushInQ(COM_PORT1);
	//FlushOutQ(COM_PORT1);
	
	DisplayPanel (panelMain);
	RunUserInterface ();
	DiscardPanel (panelMain);
	CloseCom(COM_PORT1);
	return 0;
}

int CVICALLBACK InfoRead (int panel, int control, int event,
					   void *callbackData, int eventData1, int eventData2)
{
	int length = 0;
	int i = 0;
	int data_temperature = 0;
	unsigned char buf[1000];
	if( !StartMonitor )
	{
		return 1;
	}
	switch (event)
	{
		case EVENT_TIMER_TICK:
			//FlushInQ(COM_PORT1);
			//FlushOutQ(COM_PORT1);
			if( StartMonitor )
			{
				length = GetInQLen(COM_PORT1); 
			}
			else
			{
				length = 0;
			}
			if( length > 0)
			{
				ComRd(COM_PORT1, buf, length);
				//存储温度值
				data_temperature = buf[length-2] * 256 + buf[length-1];
				//当前温度
				SetCtrlVal(panelMain, PANEL_MAIN_NUMERIC_TEMPERATURE, 			(float)(data_temperature)/10			);
				//绘制图线
				for( i = 0; i < length; ++i )
				{
					//温度折线图
					PlotStripChartPoint(panelMain, PANEL_MAIN_STRIPCHART, 			(float)(data_temperature)/10			);
				}
				
				if( buf[length-3] )
				{
					SetCtrlAttribute(panelMain, PANEL_MAIN_LED_WARNING, ATTR_CTRL_VAL, 0);
				}
				else
				{
					SetCtrlAttribute(panelMain, PANEL_MAIN_LED_WARNING, ATTR_CTRL_VAL, 1); 
				}
				++record_num;
				AddRecordToTable(panelLog, PANEL_LOG_TABLE, record_num, data_temperature);
				//重置缓冲区
				for( i = 0; i < length; ++i )
				{
					buf[i] = 0;
				}
			}
			FlushInQ(COM_PORT1);
			FlushOutQ(COM_PORT1);
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

int CVICALLBACK Log (int panel, int control, int event,
					 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			InstallPopup(panelLog);
			break;
	}
	return 0;
}

int CVICALLBACK QUIT_LOG (int panel, int control, int event,
						  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			RemovePopup(panelLog);
			break;
	}
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
			QuitUserInterface(0);
			break;
	}
	return 0;
}

int CVICALLBACK PanelLog (int panel, int event, void *callbackData,
						  int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
			RemovePopup(panelLog);
			break;
	}
	return 0;
}

//
int CVICALLBACK Monitor (int panel, int control, int event,
						 void *callbackData, int eventData1, int eventData2)
{
	char *buf = {0};  
	int *num = {0};
	int i;
	int ErrorNum = 0;
	//char *ErrorString;
	//ErrorString = (char*)malloc(sizeof(char) * 250);
	//char *ErrorString;
	
	switch (event)
	{
		case EVENT_COMMIT:
			DisableBreakOnLibraryErrors();
			Error = OpenComConfig (COM_PORT1, "", 9600, 0, 8, 1, 512, 512);
	
			if( Error )
			{
				//ErrorNum = ReturnRS232Err();
				//ErrorString = GetRS232ErrorString(ErrorNum);
				//MessagePopup("Warning", ErrorString);
				//free(ErrorString);
				DisplayErrorMessage();
				
			} 
			
			//FlushInQ(COM_PORT1);
			//FlushOutQ(COM_PORT1);
			
			
			GetCtrlIndex(panelMain, PANEL_MAIN_RING, &ComIndex);
			switch( ComIndex )
			{
				case first:
					num = order;
					StartMonitor = 1;
					break;
				case second:
					buf = "2";
					break;
				case third:
					buf = "3";
					break;
				case forth:
					buf = "4";
					break;
				//default:
				//	MessagePopup("警告", "不是有效的监测对象");
					
			}
			FlushInQ(COM_PORT1);
			FlushOutQ(COM_PORT1);
			if( ComIndex )
			{
				for( i = 0; i < 10; ++i )
				{
					ComWrtByte(COM_PORT1, num[i]);
				}
			}
			break;
	}
	
	return 0;
}


//添加历史记录
void AddRecordToTable(int panel, int control, int record_num, float temperature)
{
	char current_time[30] = {0};
	int day, year, month;
	int minutes, hours, seconds;

	GetSystemDate(&month, &day, &year);
	GetSystemTime(&hours, &minutes, &seconds);
	sprintf(current_time, "%d/%02d/%02d %02d:%02d:%02d", year, month, day, hours, minutes, seconds);
	
	InsertTableRows(panel, control, 1, 1, VAL_USE_MASTER_CELL_TYPE);
	SetTableCellVal(panel, control, MakePoint(1, 1), current_time);
	SetTableCellVal(panel, control, MakePoint(2, 1), temperature/10);
}

int CVICALLBACK Export (int panel, int control, int event,
						void *callbackData, int eventData1, int eventData2)
{
	char fileName[20] = "HistoryLog.txt";
	switch (event)
	{
		case EVENT_COMMIT:
			OpenFile(fileName, VAL_READ_WRITE, VAL_TRUNCATE, VAL_ASCII);
			
			break;
	}
	return 0;
}

/*void DisplayErrorMessage(int Error)
{
	switch( Error )
	{
		case -6:
			MessagePopup("Warning", "Error Code: -3\nPlot not open");
	}
}*/

void DisplayErrorMessage(void)
{
	int ErrorIndex = 0;
	char *ErrorString;
	char ErrorMessage[250] = {0};
	
	ErrorIndex = ReturnRS232Err();
	ErrorString = GetRS232ErrorString(ErrorIndex);
	Fmt(ErrorMessage, "%s%d%s%s", "错误代码: ", ErrorIndex, "\n错误信息: ", ErrorString);
	MessagePopup("警告", ErrorMessage);
}
