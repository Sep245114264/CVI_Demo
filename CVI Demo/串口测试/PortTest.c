#include <formatio.h>
#include <rs232.h>
#include <ansi_c.h>
#include <cvirte.h>		
#include <userint.h>
#include "PortTest.h"

#define COM_PORT1	1
#define COM_PORT2	2

static char gEventChar[2];
static int panelHandle;

void CVICALLBACK Event_Char_Detect_Func(int portNo, int eventMask, void *callbackData);


int main (int argc, char *argv[])
{
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((panelHandle = LoadPanel (0, "PortTest.uir", PANEL)) < 0)
		return -1;
	// �򿪴��д���
	OpenCom(COM_PORT1, "");
	OpenCom(COM_PORT2, "");
	
	//��մ��п��ڴ�
	FlushInQ(COM_PORT1);
	FlushInQ(COM_PORT2);
	FlushOutQ(COM_PORT1);
	FlushOutQ(COM_PORT2);
	
	//��ʾ�û�����ؼ���ĸ
	PromptPopup("�ؼ���ĸ", "����ؼ���ĸ��", gEventChar, 1);
	
	//��װ�ص�������ȷ�����ؼ���ĸ�����ڽ������ڴ�ʱ���ᱻ����
	InstallComCallback(COM_PORT2, LWRS_RXFLAG, 0, (int)gEventChar[0], Event_Char_Detect_Func, 0);
	
	DisplayPanel (panelHandle);
	RunUserInterface ();
	
	//�رմ��п�
	CloseCom(COM_PORT1);
	CloseCom(COM_PORT2);
	DiscardPanel (panelHandle);
	return 0;
}

int CVICALLBACK PanelCB (int panel, int event, void *callbackData,
						 int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
			QuitUserInterface(0);
			break;
	}
	return 0;
}

int CVICALLBACK SendData (int panel, int control, int event,
						  void *callbackData, int eventData1, int eventData2)
{
	char info[20];
	
	switch (event)
	{
		case EVENT_COMMIT:
			// Ϊ�ÿؼ�����һ��ָ����ֵ
			SetCtrlVal(panelHandle, PANEL_OUTPUT_STRING, "\0");
			// ���ÿؼ���ֵ��ֵ��ָ������
			GetCtrlVal(panelHandle, PANEL_INPUT_STRING, info);
			// ��ոô��пڵ���������ַ�
			FlushInQ(COM_PORT1);
			// ��ô��пڵ��������д��ָ�����ȵ��ַ�
			ComWrt(COM_PORT1, info, strlen(info));
			break;
	}
	return 0;
}

int CVICALLBACK ReceiveData (int panel, int control, int event,
							 void *callbackData, int eventData1, int eventData2)
{
	char readBuf[20] = {0};
	int strLen = 0;
	
	switch (event)
	{
		case EVENT_COMMIT:
			// ��ȡ�ô��п���������ڵ��ַ���
			strLen = GetInQLen(COM_PORT2);
			// �Ӹô��пڶ�ȡ���ݲ����뻺������
			ComRd(COM_PORT2, readBuf, strLen);
			// ���øÿؼ���ֵΪָ��ֵ
			SetCtrlVal(panelHandle, PANEL_OUTPUT_STRING, readBuf);
			// ����ÿؼ�ʹ��ȴ�����
			SetActiveCtrl(panelHandle, PANEL_INPUT_STRING);
			break;
	}
	return 0;
}

int CVICALLBACK Quit (int panel, int control, int event,
					  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			QuitUserInterface(0);
			break;
	}
	return 0;
}

void CVICALLBACK Event_Char_Detect_Func(int portNo,int eventMask,void *callbackData)
{
	char outMessage[256];;
	
	Fmt(outMessage, "%s%s%s",
		"���ڹؼ���ĸ\"", gEventChar, "\"��");
	MessagePopup("���ڲ���", outMessage);
	return ;
}
