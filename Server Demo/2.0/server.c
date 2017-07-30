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

#define LIMIT_PONDING 180
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
	READ_TEMPERATURE = 5,
	READ_STATE_PONDING = 4
};

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

static int hconversation;
static int panelHandle;
static int panelHandle_log;
static int inf_index = 0;
static int manual_operate = NO_ACTION;
static int start = 0;
static int count = 0;
static char *DateTimeBuffer = NULL;

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
	RunUserInterface ();
	DiscardPanel (panelHandle);
	return 0;
}

int CVICALLBACK MainPanel (int panel, int event, void *callbackData,
		int eventData1, int eventData2)
{
	FILE *file_temperature;
	FILE *file_ponding;
	
	switch (event)
	{
		case EVENT_GOT_FOCUS:
			
			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
			file_temperature = fopen("温度.txt","a+");
			file_ponding = fopen("积水.txt", "a+");
			fprintf(file_temperature, "%s\n", "-------------------------");
			fprintf(file_ponding, "%s\n", "--------------------------");
			fclose(file_temperature);
			fclose(file_ponding);
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
	int i, error_action;
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
			ServerTCPRead(handle, receive_buf, 9, 100);
			// 记录信息
			record_data(receive_buf, RECEIVE_DATA, NO_ACTION);
									   
			// 解包并进行CRC校验
			unpack_data = unPack(receive_buf);
			
			if( unpack_data.data == -99 )	// 传输出错
			{	
				error_action = unpack_data.target_ip;
				switch(error_action)
				{
					case READ_VOLTAGE:
						SetCtrlVal(panelHandle, PANEL_STRING_VOLTAGE, data_buf);
						break;
					case READ_TEMPERATURE:
						SetCtrlVal(panelHandle, PANEL_STRING_TEMPERATURE, data_buf);
						break;
					case READ_STATE_PONDING:
						if( *data_buf > LIMIT_PONDING )
						{
							SetCtrlVal(panelHandle, PANEL_STATE_PONDING, 1);
						}
						break;
				}
				inf_index = NO_ACTION;
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
		MessagePopup("警告", "发送失败");
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
		MessagePopup("警告", "发送失败");
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
		MessagePopup("警告", "查询发送失败");
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
			fclose(file_voltage);
			fclose(file_temperature);
			fclose(file_ponding);
			DeleteTableRows(panelHandle, PANEL_TABLE_VOLTAGE, 1, -1);
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
