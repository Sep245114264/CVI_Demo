#include <windows.h>
#include <utility.h>
#include <formatio.h>
#include <ansi_c.h>
#include <tcpsupp.h>
#include <stdio.h>
#include <cvirte.h>		
#include <userint.h>
#include <string.h>
#include <ExcelReport.h>
#include "server.h"

#define SEND_DATA	 1
#define RECEIVE_DATA 2
#define SAVE_FORMATSTRING "%Y-%m-%d %H-%M-%S"
#define DATETIME_FORMATSTRING "%Y-%m-%d  %H:%M:%S"
#define DATE_FORMATSTRING "%Y-%m-%d" 
#define TIME_FORMATSTRING "%H:%M:%S"

#define POLY 0xA001

typedef struct 
{
	int target_ip;
	int opcode;
	int length;
	int data;
}PackData;

enum InfIndex
{
	NO_ACTION = 0,
	READ_VOLTAGE = 1,
	READ_TEMPERATURE = 2,
	READ_STATE_PONDING = 3
};

enum FileType
{
	TXT = 1,
	EXCEL = 2
};


/**************************************************************************************************/
//										函数原形												  //
/**************************************************************************************************/
int CVICALLBACK ServerTCP(unsigned int handle, int event, int error, void *callbackData);
void crc16(unsigned char * original_data, int length, unsigned int * crc);
PackData unPack(unsigned char * pack_data);
void record_data(unsigned char * data, int kind, int action);
void Read_Voltage(void);
void Read_Temperature(void);
void Read_StatePonding(void);
void display_voltage(int data);
void display_temperature(int data);
void display_ponding(int data);
void data_read(void);
int GetClickedColumnLabel(int panel, int control, int mouseX, int mouseY);
int CVICALLBACK SortByDate (int panel, int control, Point item1, Point item2, void *pcallbackData);
int CVICALLBACK SortByData(int panel, int control, Point item1, Point item2, void *pcallbackData);
int CVICALLBACK SortByWarning(int panel, int control, Point item1, Point item2, void *pcallbackData);
int readServerIP(void);
void SaveVoltage(void);
void SaveTemperature(void);
void SavePonding(void);
void display_date(void);
void saveToTXT(void);
int saveToEXCEL(void);
void QuickExe(void);

/**************************************************************************************************/
//										全局变量												  //
/**************************************************************************************************/
static int hconversation;
static int panelHandle;
static int panelHandle_log;
static int panelHandle_setting;
static int panelHandle_info;
static int inf_index = 0;
static int manual_operate = NO_ACTION;
static int start = 0;
static int count = 0;
static int port_num = 4196;
static char server_ip[30] = {0};
static char *DateTimeBuffer = NULL;
static int g_currentlySortedColumn = 0;
static int descending = 1;
static int interval = 3600;					
static int saveType = TXT;
static int saveTime = 3600;
static int previousData = 0;
static CAObjHandle applicationHandle;		// Excel
static CAObjHandle worksheetHandle;			// worksheet
static CAObjHandle workbookHandle;			// worksheet
