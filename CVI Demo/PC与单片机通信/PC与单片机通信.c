#include <rs232.h>
#include <cvirte.h>		
#include <userint.h>
#include "PC�뵥Ƭ��ͨ��.h"

#define COM_PORT1	1

static int panelHandle;
unsigned char datatemp[1000] = {0};
int num = 0;

int main (int argc, char *argv[])
{
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((panelHandle = LoadPanel (0, "PC�뵥Ƭ��ͨ��.uir", PANEL)) < 0)
		return -1;
	
	//�򿪴���
	OpenCom(COM_PORT1, "");
	//��ʼ������
	FlushInQ(COM_PORT1);  
	FlushOutQ(COM_PORT1);
	
	DisplayPanel (panelHandle);
	RunUserInterface ();
	DiscardPanel (panelHandle);
	
	//�رմ���
	CloseCom(COM_PORT1);
	return 0;
}

int CVICALLBACK Close (int panel, int control, int event,
					   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			QuitUserInterface (0);
			CloseCom(COM_PORT1);
			break;
	}
	return 0;
}

//�����Զ�ȡ�������뻺����������
int CVICALLBACK Timer (int panel, int control, int event,
					   void *callbackData, int eventData1, int eventData2)
{
	int i = 0;
	int len = 0;
	char buf[100] = {0};
	
	switch (event)
	{
		case EVENT_TIMER_TICK:
			len = GetInQLen(COM_PORT1);
			if( len )
			{
				//��ȡģ����
				ComRd(COM_PORT1, buf, len);
				
				//��ʾ��ǰֵ
				SetCtrlVal(panelHandle, PANEL_NUMERIC_1, buf[len-1]);
				SetCtrlVal(panelHandle, PANEL_NUMERIC_2, (float)buf[len-1] / 10);
				
				//��������
				for( i = 0; i < len; ++i )
				{
					PlotStripChartPoint(panelHandle, PANEL_STRIPCHART, (float)buf[len-1] / 10);
				}
				for( i = 0; i < len; ++i )
				{
					buf[i] = 0;
				}
			}
			FlushInQ(COM_PORT1);
			FlushOutQ(COM_PORT1);
			break;
	}
	return 0;
}
