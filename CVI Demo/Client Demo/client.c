#include <ansi_c.h>
#include <cvirte.h>	
#include <tcpsupp.h>
#include <userint.h>
#include "client.h"

int CVICALLBACK ClientTcp(unsigned int handle, int event, int error, void * callbackData);

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
	
	PromptPopup("��ʾ", "���������˵�IP��ַ", temp_buf, 255);
	PromptPopup("��ʾ", "������׼������Ķ˿ں�", port_num_str, 30);
	port_num = atoi(port_num_str);
	
	//	��������
	ConnectToTCPServer(&hconversation, port_num, temp_buf, ClientTcp, NULL, 100);
	//	���ӳɹ�
	SetCtrlVal(panelHandle, PANEL_CONNECTED, 1);
	GetTCPHostAddr(temp_buf, 30);
	SetCtrlVal(panelHandle, PANEL_CLIENT_IP, temp_buf);
	
	//	�������Ϣ
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

int CVICALLBACK Transmit (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	char transmit_buf[1024] = {0};
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal(panelHandle, PANEL_STRING, transmit_buf);
			strcat(transmit_buf, "\n");
			SetCtrlVal(panelHandle, PANEL_TRANSMIT, transmit_buf);
			SetCtrlVal(panelHandle, PANEL_STRING, "");
			//	������Ϣ
			ClientTCPWrite(hconversation, transmit_buf, strlen(transmit_buf), 100);
			break;
	}
	return 0;
}

int CVICALLBACK ClientTcp(unsigned handle, int event, int error, void * callbackData)
{
	char receive_buf[1024] = {0};
	ssize_t data_size = sizeof(receive_buf) - 1;
	
	switch(event)
	{
		case TCP_DATAREADY:
			ClientTCPRead(hconversation, receive_buf, data_size, 100);
			SetCtrlVal(panelHandle, PANEL_RECEIVE, receive_buf);
			break;
		case TCP_DISCONNECT:
			MessagePopup("��ʾ", "������ѶϿ�����");
			SetCtrlVal(panelHandle, PANEL_CONNECTED, 0);
			MainPanel(0, EVENT_CLOSE, NULL, 0, 0);
			break;
	}
	return 0;
}
