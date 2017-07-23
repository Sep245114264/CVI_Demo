#include <ansi_c.h>
#include <tcpsupp.h>
#include <stdio.h>
#include <cvirte.h>		
#include <userint.h>
#include <string.h>
#include "server.h"

#define SEND_DATA	 1
#define RECEIVE_DATA 2

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
int CVICALLBACK ServerTCP(unsigned int handle, int event, int error, void *callbackData);
void crc16(unsigned char * original_data, int length, unsigned int * crc);
PackData unPack(unsigned char * pack_data);
void record_data(unsigned char * data, int kind, int action);
void Read_Voltage(void);
void Read_Temperature(void);
void Read_StatePonding(void);

static int hconversation;
static int panelHandle;
static int panelHandle_log;
static int inf_index = 0;
static int manual_operate = NO_ACTION;
static int start = 0;
static int count = 0;

int main (int argc, char *argv[])
{
	char temp_buf[256] = {0};
	char server_ip[30] = "192.168.1.3";
	int port_num = 4196;
	
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((panelHandle = LoadPanel (0, "server.uir", PANEL)) < 0)
		return -1;
	if(( panelHandle_log = LoadPanel(0, "server.uir", PANELLOG)) < 0 )
		return -1;
	
	RegisterTCPServerEx(port_num, ServerTCP, NULL, server_ip);
	
	DisplayPanel (panelHandle);
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

int CVICALLBACK ReadVoltage (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	/********************�豸��ַ ������ ���ݳ��� ��������***************/
	unsigned char transmit_buf[1024] = {0x05, 0x03, 0x0, 0x0, 0x0, 0x02};
	unsigned int crc[2] = {0};
	
	crc16(transmit_buf, 6, crc);
	transmit_buf[6] = crc[0];
	transmit_buf[7] = crc[1];
	
	
	switch (event)
	{
		case EVENT_COMMIT:
			//�ֶ���������
			manual_operate = READ_VOLTAGE;
			
			if( ServerTCPWrite(hconversation, transmit_buf, 7, 100) < 0 )
			{
				MessagePopup("����", "����ʧ��");
				return -1;
			}
			else
			{
				record_data(transmit_buf, SEND_DATA, READ_VOLTAGE);
			}
			inf_index = READ_VOLTAGE;
			break;
	}
	
	return 0;
}

int CVICALLBACK ReadTemperature (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	/********************�豸��ַ ������ ���ݳ��� ��������***************/
	unsigned char transmit_buf[1024] = {0x05, 0x03, 0x0, 0x0, 0x0, 0x02};
	unsigned int crc[2] = {0};
	
	crc16(transmit_buf, 6, crc);
	transmit_buf[6] = crc[0];
	transmit_buf[7] = crc[1];
	
	switch (event)
	{
		case EVENT_COMMIT:
			// �ֶ���������
			manual_operate = READ_TEMPERATURE;
			
			if( ServerTCPWrite(hconversation, transmit_buf, 8, 100) < 0 )
			{
				MessagePopup("����", "����ʧ��");
				return -1;
			}
			else
			{
				// ��¼��Ϣ
				record_data(transmit_buf, SEND_DATA, READ_TEMPERATURE);
			}
			inf_index = READ_TEMPERATURE;
			break;
	}
	return 0;
}

int CVICALLBACK ReadStatePonding (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	/********************�豸��ַ ������ ���ݳ��� ��������***************/
	char transmit_buf[1024] = {0x04, 0x03, 0x00, 0x00, 0x00, 0x02};
	unsigned int crc[2] = {0};
	
	crc16(transmit_buf, 6, crc);
	transmit_buf[6] = crc[0];
	transmit_buf[7] = crc[1];
	
	switch (event)
	{
		case EVENT_COMMIT:
			// �ֶ���������
			manual_operate = READ_STATE_PONDING;
			
			if( ServerTCPWrite(hconversation, transmit_buf, 8, 100) < 0 )
			{
				MessagePopup("����", "��ѯ����ʧ��");
				return -1;
			}
			else
			{
				record_data(transmit_buf, SEND_DATA, READ_STATE_PONDING);
			}
			inf_index = READ_STATE_PONDING;
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
	
	/*switch( count )
	{
		case 1:
			Read_Temperature();
			break;
		case 2:
			Read_StatePonding();
			count = 0;
			break;
	}*/
	switch (event)
	{
		case EVENT_TIMER_TICK:
			Read_Temperature();
			Read_StatePonding();
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
			//	����״̬
			SetCtrlVal(panelHandle, PANEL_CONNECTED, 1);
			
			// ��¼��Ϣ
			sprintf(receive_buf, "----------  %s����������------------\n", addr_buf);
			SetCtrlVal(panelHandle_log, PANELLOG_LOG, receive_buf);
			
			//	�����ж����ӵķ�ʽ
			SetTCPDisconnectMode(handle, TCP_DISCONNECT_AUTO);
			break;
		case TCP_DATAREADY:
			//��λ���������ݣ�֤����ѯ�ɹ�
			manual_operate = 0;
			/*****************************************************/
			ServerTCPRead(handle, receive_buf, 9, 100);
			// ��¼��Ϣ
			record_data(receive_buf, RECEIVE_DATA, NO_ACTION);
									   
			// ���������CRCУ��
			unpack_data = unPack(receive_buf);
			
			if( unpack_data.data == -99 )	// �������
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
			
			// д���ȡ������
			sprintf(data_buf, "%0.1f", (unpack_data.data)/10.0);
			
			switch(unpack_data.target_ip)
			{
				case READ_VOLTAGE:
					sprintf(data_buf, "%0.1f", (unpack_data.data)/10.0);
					SetCtrlVal(panelHandle, PANEL_STRING_VOLTAGE, data_buf);
					break;
				case READ_TEMPERATURE:
					sprintf(data_buf, "%0.1f", (unpack_data.data)/10.0);
					SetCtrlVal(panelHandle, PANEL_STRING_TEMPERATURE, data_buf);
					break;
				case READ_STATE_PONDING:
					sprintf(data_buf, "%d", unpack_data.data);
					if( unpack_data.data )
					{
						SetCtrlVal(panelHandle, PANEL_STATE_PONDING, 1);
					}
					else
					{
						SetCtrlVal(panelHandle, PANEL_STATE_PONDING, 0);
					}
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

// ����λ�����ͻ��������ݰ����н��
PackData unPack(unsigned char * pack_data)
{
	int i;
	unsigned char data[1024];
	unsigned char data_buf[256] = {0};
	unsigned int crc[2] = {0};
	PackData unpack_data;
	
	// ��ȡ�豸IP
	unpack_data.target_ip = pack_data[0];
	// ��ȡ������
	unpack_data.opcode = pack_data[1];
	// ��ȡ���ݳ���
	unpack_data.length = pack_data[2];
			
	// ���ϴ�У�������
	for( i = 0; i < unpack_data.length + 3; ++i )
	{
		data[i] = pack_data[i];
	}
	// CRCУ��
	crc16(data, unpack_data.length + 3, crc);
	if( crc[0] != pack_data[unpack_data.length+3] || crc[1] != pack_data[unpack_data.length+4] )
	{
		sprintf(data_buf, "0x%x 0x%x, %s", crc[0], crc[1], "���ݴ������,�ؽӷ�������...");
		SetCtrlVal(panelHandle_log, PANELLOG_LOG, data_buf);
		unpack_data.data = -99;
		return unpack_data;
	}
	// ����ɹ������ݲ�ͬ���豸��ַѡ��ͬ�Ľ����ʽ
	switch( unpack_data.target_ip )
	{
		case READ_VOLTAGE:
			unpack_data.data = (pack_data[3] * 256 + pack_data[4]); // ����ֵΪint������ΪС��
			break;
		case READ_TEMPERATURE:
			unpack_data.data = pack_data[5] * 256 +pack_data[6];	// ����ͬ��
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
			sprintf(info_data, "\n              ��������ȡ��ѹ\n-------------------------------------------\n");
			SetCtrlVal(panelHandle_log, PANELLOG_LOG, info_data);
			break;
		case READ_TEMPERATURE:
			sprintf(info_data, "\n              ��������ȡ�¶�\n-------------------------------------------\n");
			SetCtrlVal(panelHandle_log, PANELLOG_LOG, info_data);
			break;
		case READ_STATE_PONDING:
			sprintf(info_data, "\n              ��������ѯ��ˮ���\n-------------------------------------------\n");
			SetCtrlVal(panelHandle_log, PANELLOG_LOG, info_data);
			break;
	}
	
	switch( kind )
	{
		case SEND_DATA:
			sprintf(info_data, "�������ݣ�");
			length = 6;
			break;
		case RECEIVE_DATA:
			sprintf(info_data, "�������ݣ�");
			length = 7;
			break;
	}
	SetCtrlVal(panelHandle_log, PANELLOG_LOG, info_data);
	
	for( i = 0; i < length; ++i )
	{
		sprintf(info_data, "%x ", *(data + i));
		SetCtrlVal(panelHandle_log, PANELLOG_LOG, info_data);
	}

	sprintf(info_data, "\nCRCУ���룺%x %x\n", data[length], data[length+1]);
	SetCtrlVal(panelHandle_log, PANELLOG_LOG, info_data);
}

void Read_Voltage(void)
{
	unsigned char transmit_buf[1024] = {0x05, 0x03, 0x0, 0x0, 0x0, 0x02};
	unsigned int crc[2] = {0};
	
	crc16(transmit_buf, 6, crc);
	transmit_buf[6] = crc[0];
	transmit_buf[7] = crc[1];
	
	if( ServerTCPWrite(hconversation, transmit_buf, 8, 100) < 0 )
	{
		MessagePopup("����", "����ʧ��");
		//return -1;
	}
	else
	{
		// ��¼��Ϣ
		record_data(transmit_buf, SEND_DATA, READ_TEMPERATURE);
	}
	inf_index = READ_TEMPERATURE;
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
		MessagePopup("����", "����ʧ��");
		//return -1;
	}
	else
	{
		// ��¼��Ϣ
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
		MessagePopup("����", "��ѯ����ʧ��");
		//return -1;
	}
	else
	{
		record_data(transmit_buf, SEND_DATA, READ_STATE_PONDING);
	}
	inf_index = READ_STATE_PONDING;
}

int CVICALLBACK TimerAccept (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	char addr_buf[256] = {0};
	unsigned char receive_buf[1024] = {0};
	unsigned char info_log[1024] = {0};
	int i, error_action;
	unsigned char data[5];
	char data_buf[256] = {0};
	PackData unpack_data;
	if( !start )
	{
		return 1;
	}
	switch (event)
	{
		case EVENT_TIMER_TICK:
			ServerTCPRead(hconversation, receive_buf, 9, 5000);
			// ��¼��Ϣ
			record_data(receive_buf, RECEIVE_DATA, NO_ACTION);
			break;
	}
	return 0;
}
