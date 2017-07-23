#include <ansi_c.h>
#include <cvirte.h>	
#include <tcpsupp.h>
#include <userint.h>
#include "client.h"

#define POLY 0xA001

typedef struct 
{
	int target_ip;
	int opcode;
	int length;
}PackData;

int CVICALLBACK ClientTcp(unsigned int handle, int event, int error, void * callbackData);
PackData unPack(unsigned char * pack_data);
void crc16(unsigned char * original_data, int length, unsigned int *crc);



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
	if( ConnectToTCPServer(&hconversation, 4196, "192.168.1.200", ClientTcp, NULL, 100) < 0 )
	{
		MessagePopup("警告", "连接失败。");
	}
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
	unsigned int crc[2] = {0};
	PackData unpack_data;
	//	待发送数据
	unsigned char transmit_buf[4][10] = {
										{0x01, 0x03, 0x04, 0x08, 0xE7, 0x0, 0x0},
										{0x02, 0x03, 0x04, 0x02, 0xA1, 0x0, 0x9F},
										{0x03, 0x03, 0x04, 0x00, 0x0, 0x0, 0x0},
										{0x03, 0x03, 0x04, 0x01, 0x0, 0x0, 0x0}
	};
	int i;
	
	// 添加CRC校验码
	crc16(transmit_buf[0], 7, crc);
	transmit_buf[0][7] = crc[0];
	transmit_buf[0][8] = crc[1];
	crc16(transmit_buf[1], 7, crc);
	transmit_buf[1][7] = crc[0];
	transmit_buf[1][8] = crc[1];
	crc16(transmit_buf[2], 7, crc);
	transmit_buf[2][7] = crc[0];
	transmit_buf[2][8] = crc[1];
	crc16(transmit_buf[3], 7, crc);
	transmit_buf[3][7] = crc[0];
	transmit_buf[3][8] = crc[1];
	
	switch(event)
	{
		case TCP_DATAREADY:
			// 读取指令
			ClientTCPRead(hconversation, receive_buf, 8, 100);
			// 解包并进行CRC校验
			unpack_data = unPack(receive_buf);
			// 响应指令
			switch(unpack_data.target_ip)
			{
				case 1:
					ClientTCPWrite(hconversation, transmit_buf[0], 9, 100);
					break;
				case 2:
					ClientTCPWrite(hconversation, transmit_buf[1], 9, 100);
					break;
				case 3:
					ClientTCPWrite(hconversation, transmit_buf[2], 9, 100);
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

PackData unPack(unsigned char * pack_data)
{
	int i;
	unsigned int crc[2] = {0};
	unsigned char data[1024];
	char data_buf[256] = {0};
	PackData unpack_data;
	
	// 读取设备IP
	unpack_data.target_ip = pack_data[0];
	// 读取操作码					   
	unpack_data.opcode = pack_data[1];
	// 数据长度
	//unpack_data.length = pack_data[2];
	unpack_data.length = 3;
			
	// 对待校验的数据进行整合
	for( i = 0; i < unpack_data.length + 3; ++i )
	{
		data[i] = pack_data[i];
	}
	// CRC校验
	crc16(data, unpack_data.length+3, crc);
	if( crc[0] != pack_data[unpack_data.length + 3] || crc[1] != pack_data[unpack_data.length+4] )
	{
		sprintf(data_buf, "0x%x 0x%x", pack_data[unpack_data.length+3], pack_data[unpack_data.length+4]);
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
