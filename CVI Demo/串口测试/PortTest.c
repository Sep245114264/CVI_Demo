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
	// 打开串行串口
	OpenCom(COM_PORT1, "");
	OpenCom(COM_PORT2, "");
	
	//清空串行口内存
	FlushInQ(COM_PORT1);
	FlushInQ(COM_PORT2);
	FlushOutQ(COM_PORT1);
	FlushOutQ(COM_PORT2);
	
	//提示用户输入关键字母
	PromptPopup("关键字母", "输入关键字母：", gEventChar, 1);
	
	//安装回调函数，确保当关键字母出现在接收区内存时，会被捕获
	InstallComCallback(COM_PORT2, LWRS_RXFLAG, 0, (int)gEventChar[0], Event_Char_Detect_Func, 0);
	
	DisplayPanel (panelHandle);
	RunUserInterface ();
	
	//关闭串行口
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
			// 为该控件设置一个指定的值
			SetCtrlVal(panelHandle, PANEL_OUTPUT_STRING, "\0");
			// 将该控件的值赋值给指定变量
			GetCtrlVal(panelHandle, PANEL_INPUT_STRING, info);
			// 清空该串行口的输入队列字符
			FlushInQ(COM_PORT1);
			// 向该串行口的输入队列写入指定长度的字符
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
			// 获取该串行口输入队列内的字符数
			strLen = GetInQLen(COM_PORT2);
			// 从该串行口读取数据并放入缓冲区内
			ComRd(COM_PORT2, readBuf, strLen);
			// 设置该控件的值为指定值
			SetCtrlVal(panelHandle, PANEL_OUTPUT_STRING, readBuf);
			// 激活该控件使其等待输入
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
		"存在关键字母\"", gEventChar, "\"。");
	MessagePopup("串口测试", outMessage);
	return ;
}
