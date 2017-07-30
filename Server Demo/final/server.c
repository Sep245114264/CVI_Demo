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
	if(( panelHandle_setting = LoadPanel(0, "server.uir", PANELSET)) < 0 )
		return -1;
	
	RegisterTCPServerEx(port_num, ServerTCP, NULL, server_ip);
	
	DisplayPanel (panelHandle);
	//data_read();
	RunUserInterface ();
	DiscardPanel (panelHandle);
	return 0;
}

/****************************** ���������� *********************************/

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
			hconversation = handle;
			//	����״̬
			SetCtrlVal(panelHandle, PANEL_CONNECTED, 1);
			
			// ��¼��Ϣ
			sprintf(receive_buf, "----------  %s����������------------\n", addr_buf);
			SetCtrlVal(panelHandle_log, PANELLOG_LOG, receive_buf);
			
			//	�����ж����ӵķ�ʽ
			SetTCPDisconnectMode(handle, TCP_DISCONNECT_AUTO);
			break;
		case TCP_DATAREADY:
			// ��λ���������ݣ�֤����ѯ�ɹ�
			manual_operate = 0;
			/*****************************************************/
			ServerTCPRead(handle, receive_buf, 9, 5000);
			
			// ��¼��Ϣ
			record_data(receive_buf, RECEIVE_DATA, NO_ACTION);
									   
			// ���������CRCУ��
			unpack_data = unPack(receive_buf);
			
			if( unpack_data.data == -99 )	// �������
			{	
				return 0;
			}
			
			// д���ȡ������
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
			sprintf(info_log, "-------------�ͻ����ѶϿ�����-------------\n");
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
		interval = saveTime;
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
		// �ж��Ƿ�����ǩ
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
					// ����������
					case 1:
						SortTableCells(panel, control, VAL_TABLE_ENTIRE_RANGE,
									   VAL_COLUMN_MAJOR, colIndex, !descending,
									   SortByDate, 0);
						break;
					// ����ֵ����
					case 2:
						SortTableCells(panel, control, VAL_TABLE_ENTIRE_RANGE,
									   VAL_COLUMN_MAJOR, colIndex, !descending,
									   SortByData, 0);
						break;
					// ��������������
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
		// �ж��Ƿ�����ǩ
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
					// ����������
					case 1:
						SortTableCells(panel, control, VAL_TABLE_ENTIRE_RANGE,
									   VAL_COLUMN_MAJOR, colIndex, !descending,
									   SortByDate, 0);
						break;
					// ����ֵ����
					case 2:
						SortTableCells(panel, control, VAL_TABLE_ENTIRE_RANGE,
									   VAL_COLUMN_MAJOR, colIndex, !descending,
									   SortByData, 0);
						break;
					// ��������������
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
		// �ж��Ƿ�����ǩ
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
					// ����������
					case 1:
						SortTableCells(panel, control, VAL_TABLE_ENTIRE_RANGE,
									   VAL_COLUMN_MAJOR, colIndex, !descending,
									   SortByDate, 0);
						break;
					// ��������������
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

/************************* ͨ����־  ***********************************/

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

/*************************  �߼�����  *************************************/

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
/*							�������									 */
/*************************************************************************/

int CVICALLBACK SaveSetting (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			
			GetCtrlVal(panelHandle_setting, PANELSET_SAVETIME, &saveTime);
			saveTime *= 60;
			interval = saveTime;
			
			GetCtrlVal(panelHandle_setting, PANELSET_SAVETYPE, &saveType);
			
			MessagePopup("��ʾ", "���ø��ĳɹ�");
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
			MessagePopup("��ʾ", "�����ɹ�");
			break;
	}
	return 0;
}
