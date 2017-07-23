#include <ansi_c.h>
#include <cvirte.h>	
#include <tcpsupp.h>
#include <userint.h>
#include "client.h"

#define POLY 0x07

typedef struct 
{
	int target_ip;
	int opcode;
	int length;
}PackData;

int CVICALLBACK ClientTcp(unsigned int handle, int event, int error, void * callbackData);
PackData unPack(unsigned char * pack_data);
long CRC(unsigned char * original_data, int length, unsigned int crc);



static int panelHandle;
static int hconversation;

int main (int argc, char *argv[])
{
	char temp_buf[256] = {0};
	char port_num_str[31] = {0};
	int port_num;
	
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((panelHandle = LoadPanel (0, "client.uir", PANEL)) < 0)
		return -1;
	
	//PromptPopup("提示", "请输入服务端的IP地址", temp_buf, 255);
	//PromptPopup("提示", "请输入准备接入的端口号", port_num_str, 30);
	//port_num = atoi(port_num_str);
	
	//	请求连接
	ConnectToTCPServer(&hconversation, 1, "127.0.0.1", ClientTcp, NULL, 100);
	//	连接成功
	SetCtrlVal(panelHandle, PANEL_CONNECTED, 1);
	GetTCPHostAddr(temp_buf, 30);
	SetCtrlVal(panelHandle, PANEL_CLIENT_IP, temp_buf);
	
	//	服务端信息
	GetTCPPeerAddr(hconversation, temp_buf, 255);
	SetCtrlVal(panelHandle, PANEL_SERVER_IP, temp_buf);
	
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

int CVICALLBACK ClientTcp(unsigned handle, int event, int error, void * callbackData)
{
	unsigned char receive_buf[1024] = {0};
	unsigned char crc_h, crc_l;
	ssize_t data_size = sizeof(receive_buf) - 1;
	PackData unpack_data;
	//	待发送数据
	unsigned char transmit_buf[3][10] = {
										{0x01, 0x03, 0x04, 0x08, 0xE7, 0x01, 0x02},
										{0x02, 0x01, 0x01, 0x2A},
										{0x03, 0x01, 0x01, 0xB1}
	};
	int i;
	
	// 添加CRC校验码
	transmit_buf[0][7] = CRC(transmit_buf[0], 7, 0x0);
	sprintf(receive_buf, "%x", transmit_buf[0][7]);
	SetCtrlVal(panelHandle, PANEL_TRANSMIT, receive_buf);
	transmit_buf[1][4] = CRC(transmit_buf[1], 4, 0x0);
	transmit_buf[2][4] = CRC(transmit_buf[2], 4, 0x0);
	
	switch(event)
	{
		case TCP_DATAREADY:
			// 读取指令
			ClientTCPRead(hconversation, receive_buf, data_size, 100);
			// 解包并进行CRC校验
			unpack_data = unPack(receive_buf);
			// 响应指令
			switch(unpack_data.target_ip)
			{
				case 1:
					ClientTCPWrite(hconversation, transmit_buf[0], strlen(transmit_buf[0]), 100);
					break;
				case 2:
					ClientTCPWrite(hconversation, transmit_buf[1], strlen(transmit_buf[1]), 100);
					break;
				case 3:
					ClientTCPWrite(hconversation, transmit_buf[2], strlen(transmit_buf[2]), 100);
					break;
			}
			break;
		case TCP_DISCONNECT:
			MessagePopup("提示", "服务端已断开连接");
			SetCtrlVal(panelHandle, PANEL_CONNECTED, 0);
			MainPanel(0, EVENT_CLOSE, NULL, 0, 0);
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
		for(i = 0; i < 8; ++i)
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

PackData unPack(unsigned char * pack_data)
{
	int i;
	unsigned char data[5];
	char data_buf[256] = {0};
	PackData unpack_data;
	
	// 读取设备IP
	unpack_data.target_ip = pack_data[0];
	// 读取操作码					   
	unpack_data.opcode = pack_data[1];
	// 数据长度
	unpack_data.length = pack_data[2];
			
	// CRC校验
	for( i = 0; i < unpack_data.length + 3; ++i )
	{
		data[i] = pack_data[i];
	}
	if( CRC(data, unpack_data.length + 3, 0x0) != pack_data[unpack_data.length + 3] )
	{
		sprintf(data_buf, "0x%x", pack_data[unpack_data.length + 3]);
		MessagePopup("警告", data_buf);
		MessagePopup("警告", "数据传输出错");
	} 
			
	// 数据内容
	for( i = 0; i < unpack_data.length; ++i )
	{
		data[i] = pack_data[3+i];
	}
	return unpack_data;
}
