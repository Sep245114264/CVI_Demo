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
#define POLY 0x07

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

int CVICALLBACK ServerTCP(unsigned int handle, int event, int error, void *callbackData);
long CRC(unsigned char * original_data, int length, unsigned int crc);
PackData unPack(unsigned char * pack_data);
void display_data(unsigned char * data, int kind, int action);

static int hconversation;
static int panelHandle;
static int panelHandle_log;
static int inf_index = 0;

int main (int argc, char *argv[])
{
	char temp_buf[256] = {0};
	char server_ip[30] = "127.0.0.1";
	int port_num = 0;
	
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((panelHandle = LoadPanel (0, "server.uir", PANEL)) < 0)
		return -1;
	if(( panelHandle_log = LoadPanel(0, "server.uir", PANELLOG)) < 0 )
		return -1;
	
	port_num = 1;
	
	RegisterTCPServerEx(port_num, ServerTCP, NULL, server_ip);
	
	//��ʾ�����Ϣ
	SetCtrlVal(panelHandle, PANEL_SERVER_IP, server_ip);
	
	DisplayPanel (panelHandle);
	RunUserInterface ();
	DiscardPanel (panelHandle);
	return 0;
}

int CVICALLBACK ServerTCP(unsigned int handle, int event, int error, void * callbackData)
{
	char addr_buf[256] = {0};
	unsigned char receive_buf[1024] = {0};
	unsigned char info_log[1024] = {0};
	ssize_t data_size = sizeof(receive_buf) - 1;
	int i, opcode, length;
	unsigned char data[5];
	char data_buf[256] = {0};
	PackData unpack_data;
	
	switch(event)
	{
		case TCP_CONNECT:
			hconversation = handle;
			//	��ȡ�ͻ�����Ϣ
			SetCtrlVal(panelHandle, PANEL_CONNECTED, 1);
			GetTCPPeerAddr(handle, addr_buf, 255);
			SetCtrlVal(panelHandle, PANEL_CLIENT_IP, addr_buf);
			
			// ��¼��Ϣ
			sprintf(receive_buf, "----------  %s����������------------\n", addr_buf);
			SetCtrlVal(panelHandle_log, PANELLOG_LOG, receive_buf);
			
			//	�����ж����ӵķ�ʽ
			SetTCPDisconnectMode(handle, TCP_DISCONNECT_AUTO);
			break;
		case TCP_DATAREADY:
			/*****************************************************/
			ServerTCPRead(handle, receive_buf, data_size, 100);
			// ��¼��Ϣ
			display_data(receive_buf, RECEIVE_DATA, NO_ACTION);
			
			// ���������CRCУ��
			unpack_data = unPack(receive_buf);
			// д���ȡ������
			sprintf(data_buf, "%d", unpack_data.data);
			
			switch(inf_index)
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
			inf_index = 0;
			/*****************************************************/
			break;
		case TCP_DISCONNECT:
			SetCtrlVal(panelHandle, PANEL_CONNECTED, 0);
			SetCtrlVal(panelHandle, PANEL_CLIENT_IP, "");
			sprintf(info_log, "-------------�ͻ����ѶϿ�����-------------\n");
			SetCtrlVal(panelHandle_log, PANELLOG_LOG, info_log);
			break;
	}
	return 0;
}

long CRC(unsigned char * original_data, int length, unsigned int crc)
{
	unsigned char * ptr = original_data;
	unsigned char i = 0;
	
	for(; length > 0; --length)
	{
		crc = crc ^ (*ptr++);
		for( i = 0; i < 8; ++i )
		{
			crc <<= 1;
			if( crc & 0x100 )
			{
				crc = (crc ^ POLY) & 0xFF;
			}
		}
	}
	return crc;
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

int CVICALLBACK ReadVoltage (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	/********************�豸��ַ ������ ���ݳ��� ��������***************/
	unsigned char transmit_buf[1024] = {0x01, 0x01, 0x01, 0x03};
	unsigned char info_log[1024] = {0};
	unsigned char crc_h, crc_l;
	
	transmit_buf[4] = CRC(transmit_buf, 4, 0x0);
	
	
	switch (event)
	{
		case EVENT_COMMIT:
			if( ServerTCPWrite(hconversation, transmit_buf, strlen(transmit_buf), 100) < 0 )
			{
				MessagePopup("����", "��ȡʧ��"); 
				return -1;
			}
			else
			{
				display_data(transmit_buf, SEND_DATA, READ_VOLTAGE);
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
	unsigned char transmit_buf[1024] = {0x02, 0x01, 0x01, 0x03};
	unsigned char info_log[1024] = {0};
	unsigned char crc_h, crc_l;
	
	transmit_buf[4] = CRC(transmit_buf, 4, 0x0);
	
	switch (event)
	{
		case EVENT_COMMIT:
			if( ServerTCPWrite(hconversation, transmit_buf, strlen(transmit_buf), 100) < 0 )
			{
				MessagePopup("����", "��ȡʧ��");
				return -1;
			}
			else
			{
				// ��¼��Ϣ
				display_data(transmit_buf, SEND_DATA, READ_TEMPERATURE);
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
	char transmit_buf[1024] = {0x03, 0x01, 0x01, 0x03};
	unsigned char crc_h, crc_l;
	
	transmit_buf[4] = CRC(transmit_buf, 4, 0x0);
	
	switch (event)
	{
		case EVENT_COMMIT:
			if( ServerTCPWrite(hconversation, transmit_buf, strlen(transmit_buf), 100) < 0 )
			{
				MessagePopup("����", "��ѯʧ��");
				return -1;
			}
			else
			{
				display_data(transmit_buf, SEND_DATA, READ_STATE_PONDING);
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

// ����λ�����ͻ��������ݰ����н��
PackData unPack(unsigned char * pack_data)
{
	int i;
	unsigned char data[5];
	unsigned char data_buf[256] = {0};
	unsigned char test[] = {0x01};
	PackData unpack_data;
	
	// ��ȡ�豸IP
	unpack_data.target_ip = pack_data[0];
	// ��ȡ������
	unpack_data.opcode = pack_data[1];
	// ��ȡ���ݳ���
	unpack_data.length = pack_data[2];
			
	// CRCУ��
	for( i = 0; i < unpack_data.length + 3; ++i )
	{
		data[i] = pack_data[i];
	}
	if( CRC(data, unpack_data.length + 3, 0x0) != pack_data[unpack_data.length + 3] )
	{
		sprintf(data_buf, "0x%x", CRC(data, unpack_data.length + 3, 0x0));
		MessagePopup("����", data_buf);
		MessagePopup("����", "���ݴ������");
	}
	// ʮ������תʮ���ƣ�8λ���ϣ�
	if( unpack_data.length > 1 )
	{
		unpack_data.data = pack_data[2 + unpack_data.length - 1] * 256 + pack_data[2 + unpack_data.length];
	}
	else
	{
		unpack_data.data = pack_data[unpack_data.length + 2];
	}
	return unpack_data;
}

void display_data(unsigned char * data, int kind, int action)
{
	int length = strlen(data) - 1;				// ȥ��CRCУ����
	int i;
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
			break;
		case RECEIVE_DATA:
			sprintf(info_data, "�������ݣ�");
			break;
	}
	SetCtrlVal(panelHandle_log, PANELLOG_LOG, info_data);
	for( i = 0; i < length; ++i )
	{
		sprintf(info_data, "%x ", *(data + i));
		SetCtrlVal(panelHandle_log, PANELLOG_LOG, info_data);
	}
	sprintf(info_data, "\nCRCУ���룺%x\n", data[length]);
	SetCtrlVal(panelHandle_log, PANELLOG_LOG, info_data);
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
