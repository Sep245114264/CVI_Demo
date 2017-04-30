#include <formatio.h>
#include <utility.h>
#include <ansi_c.h>
#include <cvirte.h>		
#include <userint.h>
#include "test.h"

#define OTHER 1

void ExportHistory(int panel, int control, int record_num, int other);
void AddRecordToTable(int panel, int control, float temperature, float ponding);

static int panelHandle;
static int record_num = 0;
char *str;

int main (int argc, char *argv[])
{
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((panelHandle = LoadPanel (0, "test.uir", PANEL)) < 0)
		return -1;
	DisplayPanel (panelHandle);
	RunUserInterface ();
	DiscardPanel (panelHandle);
	return 0;
}

int CVICALLBACK Panel (int panel, int event, void *callbackData,
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

int CVICALLBACK Button (int panel, int control, int event,
						void *callbackData, int eventData1, int eventData2)
{
	/*char current_time[100] = {0};
	
	int month, day, year;
	int minutes, hours, seconds; */
	
	switch (event)
	{
		case EVENT_COMMIT:
			++record_num;
			/*InsertTableRows(panel, PANEL_TABLE, -1, 1, VAL_USE_MASTER_CELL_TYPE);
			GetSystemDate(&month, &day, &year);
			GetSystemTime(&hours, &minutes, &seconds);
			sprintf(current_time, "%02d/%02d/%d %02d:%02d:%02d", month, day, year, hours, minutes, seconds);
			SetTableCellVal(panel, PANEL_TABLE, MakePoint(1, record_num), current_time);
			SetTableCellVal(panel, PANEL_TABLE, MakePoint(2, record_num),  (double)(seconds));
			SetTableCellVal(panel, PANEL_TABLE, MakePoint(3, record_num),  (double)(minutes));  */
			
			
			AddRecordToTable(panel, PANEL_TABLE, 13, record_num);
			
			break;
	}
	return 0;
}

int CVICALLBACK Export (int panel, int control, int event,
						void *callbackData, int eventData1, int eventData2)
{
	/*int FileHandle = 0;
	int i = 0;
	char date[100] = {0};
	double temperature = 0;
	double ponding = 0;
	str = (char*)malloc(sizeof(char*)*record_num*50); */
	
	switch (event)
	{
		case EVENT_COMMIT:
			/*FileHandle = OpenFile ("text.txt", VAL_READ_WRITE, VAL_TRUNCATE, VAL_ASCII);
			for( i = 0; i < record_num; ++i )
			{
				GetTableCellVal(panel, PANEL_TABLE, MakePoint(1, record_num), date);
				GetTableCellVal(panel, PANEL_TABLE, MakePoint(2, record_num), &ponding);
				GetTableCellVal(panel, PANEL_TABLE, MakePoint(3, record_num), &temperature);
				sprintf(&str[record_num], "%s %0.2f %0.2f\n", date, ponding, temperature);
				WriteFile(FileHandle, &str[record_num], strlen(&str[record_num]));
			}
			
			CloseFile(FileHandle);
			free(str); */
			
			
			ExportHistory(panel, PANEL_TABLE, record_num, 0);
			break;
	}
	return 0;
}

int CVICALLBACK directory (int panel, int control, int event,
						   void *callbackData, int eventData1, int eventData2)
{
	/*char fileName[500] = {0};
	
	int FileHandle = 0;
	int i = 0;
	char date[100] = {0};
	double temperature = 0;
	double ponding = 0;
	str = (char*)malloc(sizeof(char*)*record_num*50); */
	
	switch (event)
	{
		case EVENT_COMMIT:
			/*FileSelectPopupEx("", "*.txt", "", "导出到指定目录", VAL_OK_BUTTON, 0, 0, fileName);
			FileHandle = OpenFile (fileName, VAL_READ_WRITE, VAL_TRUNCATE, VAL_ASCII);
			for( i = 0; i < record_num; ++i )
			{
				GetTableCellVal(panel, PANEL_TABLE, MakePoint(1, record_num), date);
				GetTableCellVal(panel, PANEL_TABLE, MakePoint(2, record_num), &ponding);
				GetTableCellVal(panel, PANEL_TABLE, MakePoint(3, record_num), &temperature);
				sprintf(&str[record_num], "%s %0.2f %0.2f\n", date, ponding, temperature);
				WriteFile(FileHandle, &str[record_num], strlen(&str[record_num]));
			}
			
			CloseFile(FileHandle);
			free(str); */
			
			
			
			ExportHistory(panel, PANEL_TABLE, record_num, 1);
			break;
	}
	return 0;
}

