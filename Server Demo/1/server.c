#include <ansi_c.h>
#include <cvirte.h>	
#include <tcpsupp.h>
#include <userint.h>
#include "server.h"

int CVICALLBACK ServerTcp(unsigned int handle, int event, int error, void * callbackData);

static int hconversation;		//	handle_conversation
static int panelHandle;

int main (int argc, char *argv[])
{
	char temp_buf[256] = {0};
	int port_num;
	
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((panelHandle = LoadPanel (0, "server.uir", PANEL)) < 0)
		return -1;
	
	//	��ȡ��������˵���Ϣ
	PromptPopup("��ʾ", "���������˵Ŀ��Ŷ˿ڣ�", temp_buf, 30);
	port_num = atoi(temp_buf);
	
	//	ע���Ϊ����˳���
	RegisterTCPServer(port_num, ServerTcp, NULL);
	
	//	��ʾ�����Ϣ
	SetCtrlVal(panelHandle, PANEL_ONLINE, 1);
	
	GetTCPHostAddr(temp_buf, 256);
	SetCtrlVal(panelHandle, PANEL_SERVER_IP, temp_buf);
	
	SetCtrlAttribute(panelHandle, PANEL_STRING, ATTR_DIMMED, 1);
	
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

int CVICALLBACK ServerTcp(unsigned int handle, int event, int error, void * callbackData)
{
	char addr_buf[256] = {0};
	char receive_buf[1024] = {0};
	int data_size = sizeof(receive_buf) - 1;
	
	switch(event)
	{
		case TCP_CONNECT:
			hconversation = handle;
			//	��ȡ�ͻ�����Ϣ
			SetCtrlVal(panelHandle, PANEL_CONNECTED, 1);
			GetTCPPeerAddr(handle, addr_buf, 255);
			SetCtrlVal(panelHandle, PANEL_CLIENT_IP, addr_buf);
			
			sprintf(receive_buf, "%s -- ���ӳɹ�\n", addr_buf);
			SetCtrlVal(panelHandle, PANEL_TRANSMIT, receive_buf);
			
			// ����������Ϣ
			SetCtrlAttribute(panelHandle, PANEL_STRING, ATTR_DIMMED, 0);
			
			//	�����ж����ӵķ�ʽ
			SetTCPDisconnectMode(handle, TCP_DISCONNECT_AUTO);
			break;
		case TCP_DATAREADY:
			ServerTCPRead(handle, receive_buf, data_size, 100);
			SetCtrlVal(panelHandle, PANEL_RECEIVE, receive_buf);
			break;
		case TCP_DISCONNECT:
			SetCtrlVal(panelHandle, PANEL_CONNECTED, 0);
			SetCtrlVal(panelHandle, PANEL_CLIENT_IP, "");
			sprintf(receive_buf, "--�ͻ����ѶϿ�����--\n");
			SetCtrlVal(panelHandle, PANEL_RECEIVE, receive_buf);
			//	��ֹ������Ϣ
			SetCtrlAttribute(panelHandle, PANEL_STRING, ATTR_DIMMED, 1);
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
			ServerTCPWrite(hconversation, transmit_buf, strlen(transmit_buf), 100);
			break;
	}
	return 0;
}
