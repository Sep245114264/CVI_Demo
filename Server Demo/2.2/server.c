#include <utility.h>
#include <formatio.h>
#include <ansi_c.h>
#include <tcpsupp.h>
#include <stdio.h>
#include <cvirte.h>		
#include <userint.h>
#include <string.h>
#include "server.h"

#define SEND_DATA	 1
#define RECEIVE_DATA 2
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
	READ_VOLTAGE = 3,
	READ_TEMPERATURE = 5,
	READ_STATE_PONDING = 4
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
void QuickExe(void);
/**************************************************************************************************/
//										全局变量												  //
/**************************************************************************************************/
static int hconversation;
static int panelHandle;
static int panelHandle_log;
static int inf_index = 0;
static int manual_operate = NO_ACTION;
static int start = 0;
static int count = 0;
static char *DateTimeBuffer = NULL;
static int g_currentlySortedColumn = 0;
static int descending = 1;

int main (int argc, char *argv[])
{
	char temp_buf[256] = {0};
	char server_ip[30] = "192.168.1.3";
	//char server_ip[30] = "192.168.74.185";
	int port_num = 4196;
	int file_temperature, file_ponding;
	
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((panelHandle = LoadPanel (0, "server.uir", PANEL)) < 0)
		return -1;
	if(( panelHandle_log = LoadPanel(0, "server.uir", PANELLOG)) < 0 )
		return -1;
	
	RegisterTCPServerEx(port_num, ServerTCP, NULL, server_ip);
	
	/*file_temperature = OpenFile ("温度.txt", VAL_READ_WRITE, VAL_APPEND, VAL_ASCII);
	CloseFile(file_temperature);
	file_ponding = OpenFile("积水.txt", VAL_READ_WRITE, VAL_APPEND, VAL_ASCII);
	CloseFile(file_ponding);*/
	
	DisplayPanel (panelHandle);
	//data_read();
	RunUserInterface ();
	DiscardPanel (panelHandle);
	return 0;
}

int CVICALLBACK MainPanel (int panel, int event, void *callbackData,
		int eventData1, int eventData2)
{	
	switch (event)
	{
		case EVENT_GOT_FOCUS:
			
			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
			QuickExe();
			QuitUserInterface (0);
			break;
	}
	return 0;
}

int CVICALLBACK Panel_log (int panel, int event, void *callbackData,
		int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
			HidePanel(panelHandle_log);
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
			DisplayPanel(panelHandle_log);
			break;
	}
	return 0;
}

int CVICALLBACK TimerTransmit (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	if( !start )
	{
		return 1;
	}
	
	switch( count )
	{
		case 1:
			Read_Voltage();
			break;
		case 2:
			Read_Temperature();
			break;
		case 3:
			Read_StatePonding();
			count = 0;
			break;
	}
	switch (event)
	{
		case EVENT_TIMER_TICK:
			++count;
			break;
	}
	return 0;
}

int CVICALLBACK StartMonitor (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			start = 1;
			SetCtrlVal(panelHandle, PANEL_STATE_MONITOR, 1);
			break;
	}
	return 0;

}

int CVICALLBACK StopMonitor (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			start = 0;
			SetCtrlVal(panelHandle, PANEL_STATE_MONITOR, 0);
			break;
	}
	return 0;
}

int CVICALLBACK ServerTCP(unsigned int handle, int event, int error, void * callbackData)
{
	char addr_buf[256] = {0};
	unsigned char receive_buf[1024] = {0};
	unsigned char info_log[1024] = {0};
	int i, error_action, j;
	unsigned char data[5];
	char data_buf[256] = {0};
	PackData unpack_data;
	
	switch(event)
	{
		case TCP_CONNECT:
			hconversation = handle;
			//	连接状态
			SetCtrlVal(panelHandle, PANEL_CONNECTED, 1);
			
			// 记录信息
			sprintf(receive_buf, "----------  %s与服务端连接------------\n", addr_buf);
			SetCtrlVal(panelHandle_log, PANELLOG_LOG, receive_buf);
			
			//	设置中断连接的方式
			SetTCPDisconnectMode(handle, TCP_DISCONNECT_AUTO);
			break;
		case TCP_DATAREADY:
			//下位机返回数据，证明查询成功
			manual_operate = 0;
			/*****************************************************/
			ServerTCPRead(handle, receive_buf, 9, 5000);
			
			// 记录信息
			record_data(receive_buf, RECEIVE_DATA, NO_ACTION);
			
			/*for( i = 0; i < 5000; ++i )
			{
				for( j = 0; j < 2000; ++j )
				{
				}
			}*/				
			// 解包并进行CRC校验
			unpack_data = unPack(receive_buf);
			
			if( unpack_data.data == -99 )	// 传输出错
			{	
				return 0;
			}
			
			// 写入读取的数据
			sprintf(data_buf, "%0.1f", (unpack_data.data)/10.0);
			
			switch(unpack_data.target_ip)
			{
				case READ_VOLTAGE:
					/*sprintf(data_buf, "%0.1f", (unpack_data.data)/10.0);
					SetCtrlVal(panelHandle, PANEL_STRING_VOLTAGE, data_buf);*/
					display_voltage(unpack_data.data);
					break;
				case READ_TEMPERATURE:
					/*sprintf(data_buf, "%0.1f", (unpack_data.data)/10.0);
					SetCtrlVal(panelHandle, PANEL_STRING_TEMPERATURE, data_buf);*/
					display_temperature(unpack_data.data);
					break;
				case READ_STATE_PONDING:
					/*sprintf(data_buf, "%d", unpack_data.data);
					if( unpack_data.data )
					{
						SetCtrlVal(panelHandle, PANEL_STATE_PONDING, 1);
					}
					else
					{
						SetCtrlVal(panelHandle, PANEL_STATE_PONDING, 0);
					}*/
					
					display_ponding(unpack_data.data);
					break;
			}
			inf_index = NO_ACTION;
			/*****************************************************/
			break;
		case TCP_DISCONNECT:
			SetCtrlVal(panelHandle, PANEL_CONNECTED, 0);
			sprintf(info_log, "-------------客户端已断开连接-------------\n");
			SetCtrlVal(panelHandle_log, PANELLOG_LOG, info_log);
			break;
	}
	return 0;
}

void crc16(unsigned char * original_data, int length, unsigned int *crc)
{
	unsigned char * ptr = original_data;
	unsigned int check = 0;
	unsigned int temp_check;
	int i, j;
	
	check = 0xFFFF;
	for( i = 0; i < length; ++i )
	{
		temp_check = *(ptr+i) & 0x00FF;
		check ^= temp_check;
		for( j = 0; j < 8; ++j )
		{
			if( check & 1 )
			{
				check >>= 1;
				check ^= POLY;
			}
			else
			{
				check >>= 1;
			}
		}
	}
	crc[0] = check & 0xFF;
	crc[1] = check >> 8;
}

// 将下位机发送回来的数据包进行解包
PackData unPack(unsigned char * pack_data)
{
	int i;
	unsigned char data[1024];
	unsigned char data_buf[256] = {0};
	unsigned int crc[2] = {0};
	PackData unpack_data;
	
	// 读取设备IP
	unpack_data.target_ip = pack_data[0];
	// 读取操作码
	unpack_data.opcode = pack_data[1];
	// 读取数据长度
	unpack_data.length = pack_data[2];
			
	// 整合待校验的数据
	for( i = 0; i < unpack_data.length + 3; ++i )
	{
		data[i] = pack_data[i];
	}
	// CRC校验
	crc16(data, unpack_data.length + 3, crc);
	if( crc[0] != pack_data[unpack_data.length+3] || crc[1] != pack_data[unpack_data.length+4] )
	{
		sprintf(data_buf, "0x%x 0x%x, %s", crc[0], crc[1], "数据传输出错,重接发送请求...");
		SetCtrlVal(panelHandle_log, PANELLOG_LOG, data_buf);
		unpack_data.data = -99;
		return unpack_data;
	}
	// 检验成功，根据不同的设备地址选择不同的解包方式
	switch( unpack_data.target_ip )
	{
		case READ_VOLTAGE:
			unpack_data.data = (pack_data[3] * 256 + pack_data[4]); // 返回值为int，不能为小数
			break;
		case READ_TEMPERATURE:
			unpack_data.data = pack_data[5] * 256 +pack_data[6];	// 理由同上
			if( unpack_data.data > 1000 )
			{
				unpack_data.data = -99;
				return unpack_data;
			}
			break;
		case READ_STATE_PONDING:
			unpack_data.data = pack_data[4];
			break;
	}
	return unpack_data;
}

void record_data(unsigned char * data, int kind, int action)
{
	int i, length;
	unsigned char info_data[1024] = {0};
	
	switch( action )
	{
		case READ_VOLTAGE:
			sprintf(info_data, "\n              操作：读取电压\n-------------------------------------------\n");
			SetCtrlVal(panelHandle_log, PANELLOG_LOG, info_data);
			break;
		case READ_TEMPERATURE:
			sprintf(info_data, "\n              操作：读取温度\n-------------------------------------------\n");
			SetCtrlVal(panelHandle_log, PANELLOG_LOG, info_data);
			break;
		case READ_STATE_PONDING:
			sprintf(info_data, "\n              操作：查询积水深度\n-------------------------------------------\n");
			SetCtrlVal(panelHandle_log, PANELLOG_LOG, info_data);
			break;
	}
	
	switch( kind )
	{
		case SEND_DATA:
			sprintf(info_data, "发送数据：");
			length = 6;
			break;
		case RECEIVE_DATA:
			sprintf(info_data, "接受数据：");
			length = 7;
			break;
	}
	SetCtrlVal(panelHandle_log, PANELLOG_LOG, info_data);
	
	for( i = 0; i < length; ++i )
	{
		sprintf(info_data, "%x ", *(data + i));
		SetCtrlVal(panelHandle_log, PANELLOG_LOG, info_data);
	}

	sprintf(info_data, "\nCRC校检码：%x %x\n", data[length], data[length+1]);
	SetCtrlVal(panelHandle_log, PANELLOG_LOG, info_data);
}

void Read_Voltage(void)
{
	unsigned char transmit_buf[1024] = {0x03, 0x03, 0x0, 0x0, 0x0, 0x02};
	unsigned int crc[2] = {0};
	
	crc16(transmit_buf, 6, crc);
	transmit_buf[6] = crc[0];
	transmit_buf[7] = crc[1];
	
	if( ServerTCPWrite(hconversation, transmit_buf, 8, 100) < 0 )
	{
		MessagePopup("警告", "发送失败,请等待下位机连接......");
		SetCtrlVal(panelHandle, PANEL_STATE_MONITOR, 0);
		start = 0;
		//return -1;
	}
	else
	{
		// 记录信息
		record_data(transmit_buf, SEND_DATA, READ_VOLTAGE);
	}
	inf_index = READ_VOLTAGE;
}
	
void Read_Temperature(void)
{
	unsigned char transmit_buf[1024] = {0x05, 0x03, 0x0, 0x0, 0x0, 0x02};
	unsigned int crc[2] = {0};
	
	crc16(transmit_buf, 6, crc);
	transmit_buf[6] = crc[0];
	transmit_buf[7] = crc[1];
	
	if( ServerTCPWrite(hconversation, transmit_buf, 8, 100) < 0 )
	{
		MessagePopup("警告", "发送失败, 请等待下位机连接......");
		start = 0;
		SetCtrlVal(panelHandle, PANEL_STATE_MONITOR, 0);
		//return -1;
	}
	else
	{
		// 记录信息
		record_data(transmit_buf, SEND_DATA, READ_TEMPERATURE);
	}
	inf_index = READ_TEMPERATURE;
}

void Read_StatePonding(void)
{
	char transmit_buf[1024] = {0x04, 0x03, 0x00, 0x00, 0x00, 0x02};
	unsigned int crc[2] = {0};
	
	crc16(transmit_buf, 6, crc);
	transmit_buf[6] = crc[0];
	transmit_buf[7] = crc[1];

	if( ServerTCPWrite(hconversation, transmit_buf, 8, 5000) < 0 )
	{
		MessagePopup("警告", "查询发送失败，请等待下位机连接......");
		start = 0;
		SetCtrlVal(panelHandle, PANEL_STATE_MONITOR, 0);
		//return -1;
	}
	else
	{
		record_data(transmit_buf, SEND_DATA, READ_STATE_PONDING);
	}
	inf_index = READ_STATE_PONDING;
}

void display_voltage(int data)
{
	char data_buf[1024] = {0};
	double dis_data = 0.0;
	int max_voltage, min_voltage, row, flag, i;
	sprintf(data_buf, "%0.1f", data / 10.0);
	dis_data = data/10.0;
	
	//绘制电压变化图
	PlotStripChart(panelHandle, PANEL_CHART_VOLTAGE, &dis_data, 1, 0, 0, VAL_DOUBLE);
	
	GetCtrlVal(panelHandle, PANEL_MAX_VOLTAGE, &max_voltage);
	GetCtrlVal(panelHandle, PANEL_MIN_VOLTAGE, &min_voltage);
	if( data/10.0 > (double)max_voltage || data/10.0 < (double)min_voltage )
	{
		SetCtrlVal(panelHandle, PANEL_STATE_VOLTAGE, 1);
		flag = 1;
	}
	else
	{
		SetCtrlVal(panelHandle, PANEL_STATE_VOLTAGE, 0);
		flag = 0;
	}
	SetCtrlVal(panelHandle, PANEL_STRING_VOLTAGE, data_buf);
	
	//新加一行
	InsertTableRows (panelHandle, PANEL_TABLE_VOLTAGE, 1, 1, VAL_USE_MASTER_CELL_TYPE);
	// 获取行数
	GetNumTableRows(panelHandle, PANEL_TABLE_VOLTAGE, &row);
	// 设置数据
	SetTableCellVal(panelHandle, PANEL_TABLE_VOLTAGE, MakePoint(1, 1), DateTimeBuffer);
	SetTableCellVal(panelHandle, PANEL_TABLE_VOLTAGE, MakePoint(2, 1), data_buf);
	if( flag )
	{	
		SetTableCellVal(panelHandle, PANEL_TABLE_VOLTAGE, MakePoint(3, 1), "是");
		//flag = "是";
	}
	else
	{
		SetTableCellVal(panelHandle, PANEL_TABLE_VOLTAGE, MakePoint(3, 1), "否");
		//flag = "否";
	}
	//fprintf(file, "%s	%s	%s\n", DateTimeBuffer, data_buf, flag);
	// 标注颜色
	if( flag )
	{
		for( i = 1; i <= 3; ++i )
		{
			SetTableCellAttribute(panelHandle, PANEL_TABLE_VOLTAGE, MakePoint(i, 1), ATTR_TEXT_COLOR, VAL_RED);
		}
	}
	else
	{
		for( i = 1; i <= 3; ++i )
		{
			SetTableCellAttribute(panelHandle, PANEL_TABLE_VOLTAGE, MakePoint(i, 1), ATTR_TEXT_COLOR, VAL_BLACK);
		}
	}
}
	
void display_temperature(int data)
{
	char data_buf[1024] = {0};
	double dis_data = 0.0;
	int max_temperature, min_temperature, row, flag, i;
	max_temperature = 30;
	min_temperature = 15;
	sprintf(data_buf, "%0.1f", data/10.0);
	dis_data = data/10.0;
	
	//绘制温度变化图
	PlotStripChart (panelHandle, PANEL_CHART_TEMPERATURE, &dis_data, 1, 0, 0, VAL_DOUBLE);
	
	GetCtrlVal(panelHandle, PANEL_MAX_TEMPERATURE, &max_temperature);
	GetCtrlVal(panelHandle, PANEL_MIN_TEMPERATURE, &min_temperature);
	if( data/10.0 > (double)(max_temperature) || data/10.0 < (double)(min_temperature) )
	{
		SetCtrlVal(panelHandle, PANEL_STATE_TEMPERATURE, 1);
		flag = 1;
	}
	else
	{
		SetCtrlVal(panelHandle, PANEL_STATE_TEMPERATURE, 0);
		flag = 0;
	}
	SetCtrlVal(panelHandle, PANEL_STRING_TEMPERATURE, data_buf);
	
	//新加一行
	InsertTableRows (panelHandle, PANEL_TABLE_TEMPERATURE, 1, 1, VAL_USE_MASTER_CELL_TYPE);
	// 获取行数
	GetNumTableRows(panelHandle, PANEL_TABLE_TEMPERATURE, &row);
	// 设置数据
	SetTableCellVal(panelHandle, PANEL_TABLE_TEMPERATURE, MakePoint(1, 1), DateTimeBuffer);
	SetTableCellVal(panelHandle, PANEL_TABLE_TEMPERATURE, MakePoint(2, 1), data_buf);
	if( flag )
	{	
		SetTableCellVal(panelHandle, PANEL_TABLE_TEMPERATURE, MakePoint(3, 1), "是");
		//flag = "是";
	}
	else
	{
		SetTableCellVal(panelHandle, PANEL_TABLE_TEMPERATURE, MakePoint(3, 1), "否");
		//flag = "否";
	}
	//fprintf(file, "%s	%s	%s\n", DateTimeBuffer, data_buf, flag);
	// 标注颜色
	if( flag )
	{
		for( i = 1; i <= 3; ++i )
		{
			SetTableCellAttribute(panelHandle, PANEL_TABLE_TEMPERATURE, MakePoint(i, 1), ATTR_TEXT_COLOR, VAL_RED);
		}
	}
	else
	{
		for( i = 1; i <= 3; ++i )
		{
			SetTableCellAttribute(panelHandle, PANEL_TABLE_TEMPERATURE, MakePoint(i, 1), ATTR_TEXT_COLOR, VAL_BLACK);
		}
	}
}
	
void display_ponding(int data)
{
	char data_buf[1024] = {0};
	double dis_data = 0.0;
	int row, flag, i;
	
	dis_data = data;
	sprintf(data_buf, "%d", data); 
	// 绘制积水通断状况
	PlotStripChart(panelHandle, PANEL_CHART_PONDING, &dis_data, 1, 0, 0, VAL_DOUBLE);
	if( data )
	{
		SetCtrlVal(panelHandle, PANEL_STATE_PONDING, 1);
		flag = 1;
	}
	else
	{
		SetCtrlVal(panelHandle, PANEL_STATE_PONDING, 0);
		flag = 0;
	}
	//新加一行
	InsertTableRows (panelHandle, PANEL_TABLE_PONDING, 1, 1, VAL_USE_MASTER_CELL_TYPE);
	// 获取行数
	GetNumTableRows(panelHandle, PANEL_TABLE_PONDING, &row);
	// 设置数据
	SetTableCellVal(panelHandle, PANEL_TABLE_PONDING, MakePoint(1, 1), DateTimeBuffer);
	if( flag )
	{	
		SetTableCellVal(panelHandle, PANEL_TABLE_PONDING, MakePoint(2, 1), "是");
		//flag = "是";
	}
	else
	{
		SetTableCellVal(panelHandle, PANEL_TABLE_PONDING, MakePoint(2, 1), "否");
		//flag = "否";
	}
	//fprintf(file, "%s	%s	%s\n", DateTimeBuffer, data_buf, flag);
	// 标注颜色
	if( flag )
	{
		for( i = 1; i <= 2; ++i )
		{
			SetTableCellAttribute(panelHandle, PANEL_TABLE_PONDING, MakePoint(i, 1), ATTR_TEXT_COLOR, VAL_RED);
		}
	}
	else
	{
		for( i = 1; i <= 2; ++i )
		{
			SetTableCellAttribute(panelHandle, PANEL_TABLE_PONDING, MakePoint(i, 1), ATTR_TEXT_COLOR, VAL_BLACK);
		}
	}
}

int CVICALLBACK TimerSave (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	FILE *file_voltage; 
	FILE *file_temperature;
	FILE *file_ponding;
	int row, i;
	char date[1024] = {0};
	char data_buf[1024] = {0};
	char flag[16] = {0};
	
	if( !start )
	{
		return 1;
	}
	switch (event)
	{
		case EVENT_TIMER_TICK:
			file_voltage = fopen("电压.txt", "a+");
			file_temperature = fopen("温度.txt", "a+");
			file_ponding = fopen("积水.txt", "a+");
			
			GetNumTableRows(panelHandle, PANEL_TABLE_VOLTAGE, &row);
			while( --row )
			{
				if( row < 0 )
				{
					break;
				}
				GetTableCellVal(panelHandle, PANEL_TABLE_VOLTAGE, MakePoint(1, row), date);
				GetTableCellVal(panelHandle, PANEL_TABLE_VOLTAGE, MakePoint(2, row), data_buf);
				GetTableCellVal(panelHandle, PANEL_TABLE_VOLTAGE, MakePoint(3, row), flag);
				fprintf(file_voltage, "%s	   %s		%s\n", date, data_buf, flag);
			}
			
			GetNumTableRows(panelHandle, PANEL_TABLE_TEMPERATURE, &row);
			while( --row )
			{
				if( row < 0 )
				{
					break;
				}
				GetTableCellVal(panelHandle, PANEL_TABLE_TEMPERATURE, MakePoint(1, row), date);
				GetTableCellVal(panelHandle, PANEL_TABLE_TEMPERATURE, MakePoint(2, row), data_buf);
				GetTableCellVal(panelHandle, PANEL_TABLE_TEMPERATURE, MakePoint(3, row), flag);
				fprintf(file_temperature, "%s	   %s		%s\n", date, data_buf, flag);
			}
			
			GetNumTableRows(panelHandle, PANEL_TABLE_PONDING, &row);
			while( --row )
			{
				if( row < 0 )
				{
					break;
				}
				GetTableCellVal(panelHandle, PANEL_TABLE_PONDING, MakePoint(1, row), date);
				GetTableCellVal(panelHandle, PANEL_TABLE_PONDING, MakePoint(2, row), flag);
				fprintf(file_ponding, "%s	 	%s\n", date, flag);
			}
			fclose(file_temperature);
			fclose(file_ponding);
			DeleteTableRows (panelHandle, PANEL_TABLE_TEMPERATURE, 1, -1);
			DeleteTableRows(panelHandle, PANEL_TABLE_PONDING, 1, -1);
			break;			
	}
	return 0;
}

int CVICALLBACK TimerDisplay (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	double currDateTime;
	int bufferLen;
	char *dateTimeBuffer = NULL;
	int row;
	
	switch (event)
	{
		case EVENT_TIMER_TICK:
			GetCurrentDateTime (&currDateTime);
			// 显示日期
			bufferLen = FormatDateTimeString (currDateTime, DATE_FORMATSTRING, NULL, 0);
			dateTimeBuffer = malloc (bufferLen + 1);
			FormatDateTimeString (currDateTime, DATE_FORMATSTRING, dateTimeBuffer, bufferLen + 1 );
			SetCtrlVal(panelHandle, PANEL_DATE, dateTimeBuffer);
			// 显示时间
			bufferLen = FormatDateTimeString(currDateTime, TIME_FORMATSTRING, NULL, 0);
			dateTimeBuffer = malloc (bufferLen + 1);
			FormatDateTimeString (currDateTime, TIME_FORMATSTRING, dateTimeBuffer, bufferLen + 1 );
			SetCtrlVal(panelHandle, PANEL_TIME, dateTimeBuffer);
			//整合日期时间
			bufferLen = FormatDateTimeString(currDateTime, DATETIME_FORMATSTRING, NULL, 0);
			DateTimeBuffer = malloc(bufferLen + 1);
			FormatDateTimeString(currDateTime, DATETIME_FORMATSTRING, DateTimeBuffer, bufferLen + 1);
			break;
	}
	return 0;
}

void data_read(void)
{
	FILE * file_temperature;
	int i;
	int count = 0;
	char ch;
	char dateTime[1024] = {0};
	char date[1024] = {0};
	char time[1024] = {0};
	char data[1024] = {0};
	char flag[4] = {0};
	file_temperature = fopen("温度.txt", "r");
	while( (ch = fgetc(file_temperature)) != EOF )
	{
		if( ch == '\n' )
		{
			++count;
		}
	}
	fseek(file_temperature, 0, SEEK_SET);
	for( i = 0; i < count; ++i )
	{
		fscanf(file_temperature, "%s  %s		%s		%s", date, time, flag);
		sprintf(dateTime, "%s  %s", date, time);
		//新加一行
		InsertTableRows (panelHandle, PANEL_TABLE_TEMPERATURE, 1, 1, VAL_USE_MASTER_CELL_TYPE);
		// 设置数据
		SetTableCellVal(panelHandle, PANEL_TABLE_TEMPERATURE, MakePoint(1, 1), dateTime);
		SetTableCellVal(panelHandle, PANEL_TABLE_TEMPERATURE, MakePoint(2, 1), data);
		SetTableCellVal(panelHandle, PANEL_TABLE_TEMPERATURE, MakePoint(3, 1), flag);
	}
	fclose(file_temperature);
}

int CVICALLBACK ResultVoltage (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int colIndex;
	
	if( event == EVENT_LEFT_CLICK )
	{
		// 判断是否点击标签
		if( (colIndex = GetClickedColumnLabel(panelHandle, PANEL_TABLE_VOLTAGE,
											 eventData2, eventData1)) != 0 )
		{
			if( colIndex == g_currentlySortedColumn )
			{
				descending = !descending;
				switch( colIndex )
				{
					case 1:
						SortTableCells(panel, control, VAL_TABLE_ENTIRE_RANGE,
									   VAL_COLUMN_MAJOR, colIndex, !descending,
									   SortByDate, 0);
						break;
					case 2:
						SortTableCells(panel, control, VAL_TABLE_ENTIRE_RANGE,
									   VAL_COLUMN_MAJOR, colIndex, !descending,
									   SortByData, 0);
					case 3:
						SortTableCells(panel, control, VAL_TABLE_ENTIRE_RANGE,
									   VAL_COLUMN_MAJOR, colIndex, !descending,
									   SortByWarning, 0);
						break;
				}						
			}
			else
			{
				g_currentlySortedColumn = colIndex;
				switch( colIndex)
				{
					// 按日期排序
					case 1:
						SortTableCells(panel, control, VAL_TABLE_ENTIRE_RANGE,
									   VAL_COLUMN_MAJOR, colIndex, !descending,
									   SortByDate, 0);
						break;
					// 按数值排序
					case 2:
						SortTableCells(panel, control, VAL_TABLE_ENTIRE_RANGE,
									   VAL_COLUMN_MAJOR, colIndex, !descending,
									   SortByData, 0);
						break;
					// 按报警动作排序
					case 3:
						SortTableCells(panel, control, VAL_TABLE_ENTIRE_RANGE,
									   VAL_COLUMN_MAJOR, colIndex, !descending,
									   SortByWarning, 0);
						break;
					default:
						assert(0);
						break;
				}
			}
		}
	}
	return 0;
}

int CVICALLBACK ResultTemperature (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	
	int colIndex;
	
	if( event == EVENT_LEFT_CLICK )
	{
		// 判断是否点击标签
		if( (colIndex = GetClickedColumnLabel(panelHandle, PANEL_TABLE_TEMPERATURE,
											 eventData2, eventData1)) != 0 )
		{
			if( colIndex == g_currentlySortedColumn )
			{
				descending = !descending;
				switch( colIndex )
				{
					case 1:
						SortTableCells(panel, control, VAL_TABLE_ENTIRE_RANGE,
									   VAL_COLUMN_MAJOR, colIndex, !descending,
									   SortByDate, 0);
						break;
					case 2:
						SortTableCells(panel, control, VAL_TABLE_ENTIRE_RANGE,
									   VAL_COLUMN_MAJOR, colIndex, !descending,
									   SortByData, 0);
					case 3:
						SortTableCells(panel, control, VAL_TABLE_ENTIRE_RANGE,
									   VAL_COLUMN_MAJOR, colIndex, !descending,
									   SortByWarning, 0);
						break;
				}						
			}
			else
			{
				g_currentlySortedColumn = colIndex;
				switch( colIndex)
				{
					// 按日期排序
					case 1:
						SortTableCells(panel, control, VAL_TABLE_ENTIRE_RANGE,
									   VAL_COLUMN_MAJOR, colIndex, !descending,
									   SortByDate, 0);
						break;
					// 按数值排序
					case 2:
						SortTableCells(panel, control, VAL_TABLE_ENTIRE_RANGE,
									   VAL_COLUMN_MAJOR, colIndex, !descending,
									   SortByData, 0);
						break;
					// 按报警动作排序
					case 3:
						SortTableCells(panel, control, VAL_TABLE_ENTIRE_RANGE,
									   VAL_COLUMN_MAJOR, colIndex, !descending,
									   SortByWarning, 0);
						break;
					default:
						assert(0);
						break;
				}
			}
		}
	}
	return 0;
}

int CVICALLBACK ResultPonding (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int colIndex;
	
	if( event == EVENT_LEFT_CLICK )
	{
		// 判断是否点击标签
		if( (colIndex = GetClickedColumnLabel(panelHandle, PANEL_TABLE_PONDING,
											 eventData2, eventData1)) != 0 )
		{
			if( colIndex == g_currentlySortedColumn )
			{
				descending = !descending;
				switch( colIndex )
				{
					case 1:
						SortTableCells(panel, control, VAL_TABLE_ENTIRE_RANGE,
									   VAL_COLUMN_MAJOR, colIndex, !descending,
									   SortByDate, 0);
						break;
					case 2:
						SortTableCells(panel, control, VAL_TABLE_ENTIRE_RANGE,
									   VAL_COLUMN_MAJOR, colIndex, !descending,
									   SortByWarning, 0);
						break;
				}						
			}
			else
			{
				g_currentlySortedColumn = colIndex;
				switch( colIndex)
				{
					// 按日期排序
					case 1:
						SortTableCells(panel, control, VAL_TABLE_ENTIRE_RANGE,
									   VAL_COLUMN_MAJOR, colIndex, !descending,
									   SortByDate, 0);
						break;
					// 按报警动作排序
					case 2:
						SortTableCells(panel, control, VAL_TABLE_ENTIRE_RANGE,
									   VAL_COLUMN_MAJOR, colIndex, !descending,
									   SortByWarning, 0);
						break;
					default:
						//assert(0);
						break;
				}
			}
		}
	}
	return 0;
}


int GetClickedColumnLabel(int panel, int control, int mouseX, int mouseY)
{
	int colWidth, colIndex, xLabelTraverse, gridTop, tableTop;
	Rect labelBarCoords;
	
	GetTableCellRangeRect(panel, control, VAL_TABLE_ENTIRE_RANGE,
						  &labelBarCoords);
	
	// 获取数值
	GetCtrlAttribute(panel, control, ATTR_COLUMN_LABELS_HEIGHT,
					 &labelBarCoords.height);
	GetCtrlAttribute(panel, control, ATTR_GRID_AREA_TOP, &gridTop);
	GetCtrlAttribute(panel, control, ATTR_TOP, &tableTop);
	labelBarCoords.top = tableTop + gridTop - labelBarCoords.height;
	
	// 判断鼠标是否停留在标签上
	if( RectContainsPoint(labelBarCoords, MakePoint(mouseX, mouseY)) )
	{
		xLabelTraverse = labelBarCoords.left;
		for( colIndex = 1; colIndex <= 3; ++colIndex )
		{
			GetTableColumnAttribute(panel, control, colIndex,
								   ATTR_COLUMN_ACTUAL_WIDTH, &colWidth);
			if( (mouseX >= xLabelTraverse) && (mouseX <= (xLabelTraverse + colWidth)) )
			{
				return colIndex;
			}
			xLabelTraverse += colWidth;
		}
	}
	return 0;
}

int CVICALLBACK SortByDate (int panel, int control, Point item1, Point item2, 
                            void *pcallbackData)
{
    int  month1;
    int  month2;
    int  day1;
    int  day2;
    int  year1;
    int  year2;
    int  hour1;
    int  hour2;
    int  min1;
    int  min2;
    int  sec1;
    int  sec2;
    char firstDateStr[35];
    char secondDateStr[35];
    
    // 读取待比较的数据
    GetTableCellVal (panel, control, item1, firstDateStr);
    GetTableCellVal (panel, control, item2, secondDateStr);				  
    sscanf (firstDateStr, "%d-%d-%d  %d:%d:%d", &year1, &month1, &day1,
            &hour1, &min1, &sec1);
    sscanf (secondDateStr, "%d-%d-%d  %d:%d:%d", &year2, &month2, &day2,
            &hour2, &min2, &sec2);
    
	if( year2 == year1 )
	{
		if( month2 == month1 )
		{
			if( day2 == day1 )
			{
				if( hour2 == hour1 )
				{
					if( min2 == min1 )
					{
						if( sec2 == sec1 )
						{
							return 0;
						}
						else
						{
							return (sec2 < sec1 ) ? -1 : 1;
						}
					}
					else
					{
						return (min2 < min1) ? -1 : 1;
					}
				}
				else
				{
					return (hour2 < hour1) ? -1 : 1;
				}
			}
			else
			{
				return (day2 < day1) ? -1 : 1;
			}
		}
		else
		{
			return (month2 < month1) ? -1 : 1;
		}
	}
	else
	{
		return (year2 < year1) ? -1 : 1;
	}
}

int CVICALLBACK SortByData(int panel, int control, Point item1, Point item2, void *pcallbackData)
{
	double data1, data2;
	char firstDataStr[1024] = {0};
	char secondDataStr[1024] = {0};
	
	GetTableCellVal(panel, control, item1, firstDataStr);
	GetTableCellVal(panel, control, item2, secondDataStr);
	sscanf(firstDataStr, "%lf", &data1);
	sscanf(secondDataStr, "%lf", &data2);
	
	if( data1 == data2 )
	{
		return 0;
	}
	else
	{
		return (data2 < data1) ? -1 : 1;
	}
}

int CVICALLBACK SortByWarning(int panel, int control, Point item1, Point item2, void *pcallbackData)
{
	char warning1[10] = {0};
	char warning2[10] = {0};
	
	GetTableCellVal(panel, control, item1, warning1);
	GetTableCellVal(panel, control, item2, warning2);
	
	if( strcmp(warning1, warning2) == 0 )
	{
		return 0;
	}
	else
	{
		return ( strcmp(warning1, warning2) > 0 ) ? -1 : 1;
	}
}

int CVICALLBACK ButtonClose (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			QuickExe();
			QuitUserInterface (0); 
			break;
	}
	return 0;
}

void QuickExe(void)
{
	FILE *file_temperature;
	FILE *file_ponding;
	FILE *file_voltage;
	char fv_name[50] = "电压.txt";
	char ft_name[50] = "温度.txt";
	char fp_name[50] = "积水.txt";
	
	file_voltage = fopen(fv_name, "a+");
	fprintf(file_voltage, "%s\n", "---------------------------------");
	fclose(file_voltage);
	
	file_temperature = fopen(ft_name, "a+");
	fprintf(file_temperature, "%s\n", "----------------------------------");
	fclose(file_temperature);
	
	file_ponding = fopen(fp_name, "a+");
	fprintf(file_ponding, "%s\n", "------------------------------------");
	fclose(file_ponding);
}

int CVICALLBACK SaveVoltage (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:

			break;
	}
	return 0;
}

int CVICALLBACK SaveTemperature (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:

			break;
	}
	return 0;
}

int CVICALLBACK SavePonding (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:

			break;
	}
	return 0;
}
