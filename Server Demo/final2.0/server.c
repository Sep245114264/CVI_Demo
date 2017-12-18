#include <userint.h>
#include "server.h"
#include "main.h"		
#include "communication.c"
#include "dataProcess.c"
#include "dataDisplay.c"
#include "dataSave.c"
#include "control.c"
#include "dataSort.c"

int main (int argc, char *argv[])
{	
	char buffer[32] = {0};
	
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((panelHandle = LoadPanel (0, "server.uir", PANEL)) < 0)
		return -1;
	if(( panelHandle_log = LoadPanel(0, "server.uir", PANELLOG)) < 0 )
		return -1;
	if(( panelHandle_setting = LoadPanel(0, "server.uir", PANELSET)) < 0 )
		return -1;
	if(( panelHandle_info = LoadPanel(0, "server.uir", PANEL_INFO)) < 0 )
		return -1;
										
	readServerIP();
	SetCtrlVal(panelHandle, PANEL_SERVERIP, server_ip);
	
	GetCtrlVal(panelHandle_setting, PANELSET_PORTNUM, buffer);
	port_num = atoi(buffer);
	SetCtrlVal(panelHandle, PANEL_LISTENPORT, buffer);
	
	DisplayPanel (panelHandle);
	//data_read();
	RunUserInterface ();
	DiscardPanel (panelHandle);
	return 0;
}

/****************************** 程序主界面 *********************************/

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
			//QuickExe();
			QuitUserInterface (0);
			break;
	}
	return 0;
}

int CVICALLBACK ButtonSetting (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			DisplayPanel(panelHandle_setting);
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
			HidePanel(panelHandle_info);
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
			// 下位机返回数据，证明查询成功
			manual_operate = 0;
			/*****************************************************/
			ServerTCPRead(handle, receive_buf, 9, 5000);
			
			// 记录信息
			record_data(receive_buf, RECEIVE_DATA, NO_ACTION);
									   
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
					display_voltage(unpack_data.data);
					break;
				case READ_TEMPERATURE:
					display_temperature(unpack_data.data);
					break;
				case READ_STATE_PONDING:
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

int CVICALLBACK TimerDisplay (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_TIMER_TICK:
			display_date();
			break;
	}
	return 0;
}

int CVICALLBACK TimerSave (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	if( !start )
	{
		return 1;
	}
	while( !interval-- )
	{
		//interval = saveTime;
		switch (event)
		{
			case EVENT_TIMER_TICK:
				switch( saveType )
				{
					case TXT:
						saveToTXT();
						break;
					case EXCEL:
						saveToEXCEL();
						break;
				}
				break;
		}				
	}
	return 0;
}

int CVICALLBACK CallSaveVoltage (int panel, int control, int event,  
		void *callbackData, int eventData1, int eventData2)
{
	if( !start )
	{
		return 1;
	}
	start = 0;
	switch (event)
	{
		case EVENT_COMMIT:
			SaveVoltage();
			break;
	}
	start = 1;
	return 0;
}

int CVICALLBACK CallSaveTemperature (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	if( !start )
	{
		return 1;
	}
	start = 0;
	switch (event)
	{
		case EVENT_COMMIT:
			SaveTemperature();
			break;
	}
	start = 1;
	return 0;
}

int CVICALLBACK CallSavePonding (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	if( !start )
	{
		return 1;
	}
	start = 0;
	switch (event)
	{
		case EVENT_COMMIT:
			SavePonding();
			break;
	}
	start = 1;
	return 0;
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
						assert(0);
						break;
				}
			}
		}
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

int CVICALLBACK ButtonClose (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			QuitUserInterface (0); 
			break;
	}
	return 0;
}

/************************* 通信日志  ***********************************/

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

/*************************  高级设置  *************************************/

int CVICALLBACK PanelSetting (int panel, int event, void *callbackData,
		int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
			HidePanel(panelHandle_setting);
			break;
	}
	return 0;
}

/*************************************************************************/
/*							程序控制									 */
/*************************************************************************/

int CVICALLBACK SaveSetting (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	char buffer[32] = {0};
	
	switch (event)
	{
		case EVENT_COMMIT:
			
			GetCtrlVal(panelHandle_setting, PANELSET_SAVETIME, &saveTime);
			saveTime *= 60;
			interval = saveTime;
			if( readServerIP() < 0 )
			{
				return -1;
			}
			
			GetCtrlVal(panelHandle_setting, PANELSET_SAVETYPE, &saveType);
			SetCtrlVal(panelHandle, PANEL_SERVERIP, server_ip);
			
			GetCtrlVal(panelHandle_setting, PANELSET_PORTNUM, buffer);
			port_num = atoi(buffer);
			SetCtrlVal(panelHandle, PANEL_LISTENPORT, buffer);
			
			MessagePopup("提示", "设置更改成功");
			HidePanel(panelHandle_setting);
			
			break;
	}
	return 0;
}

int CVICALLBACK ExportExcel (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			saveToEXCEL();
			MessagePopup("提示", "导出成功");
			break;
	}
	return 0;
}

int CVICALLBACK Register (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int errorCode;
	char buffer[64] = {0};
	
	switch (event)
	{
		case EVENT_COMMIT:
			errorCode = RegisterTCPServerEx(port_num, ServerTCP, NULL, server_ip);
			switch( errorCode )
			{
				case -1:
					MessagePopup("警告", "创建主机失败，请检查网络设置...");
					break;
				case -2:
					MessagePopup("警告", "无法建立连接。");
					break;
				case -3:
					MessagePopup("警告", "该主机已被创建。");
					break;
				case 0:
					DisplayPanel(panelHandle_info);
					break;
				default:
					sprintf(buffer, "%s%d", "未知错误，错误代码：", errorCode);
					MessagePopup("警告", buffer);
					break;
			}
			break;
	}
	return 0;
}

int CVICALLBACK PanelInfo (int panel, int event, void *callbackData,
		int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:

			break;
	}
	return 0;
}
