#include <cvirte.h>		
#include <userint.h>
#include "soundcard.h"
#include <analysis.h>
#include <windows.h>
#include <Mmsystem.h>
static int panelHandle;
double DataT;
UINT Time;
//====音频波形函数====//
HWAVEIN hwi;																//设备句柄
WAVEFORMATEX wfx;															//波形格式结构
PWAVEHDR pwhrl;																//波形缓冲区头指针
LPSTR pDataBufferl;															//缓冲区地址指针
UINT len = 1024;															//缓冲区长度
	PBYTE pData;															//数据区指针（字节指针）
	UINT DevNum;															//设备编号
	MMRESULT rult;															//波形函数返回结构
	char szText[100];														//波形函数返回代码字符串
	WAVEINCAPS wavecap;														//波形设备性能结构
	void CALLBACK WaveInProc(HWAVEIN hwi, UINT uMsg,DWORD dwUser, DWORD dwl, DWORD dw2);

//====主函数====//
int main (int argc, char *argv[])
{
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((panelHandle = LoadPanel (0, "soundcard.uir", PANEL)) < 0)
		return -1;
	DisplayPanel (panelHandle);												//显示控制面板
	RunUserInterface ();													//运行控制面板
	DiscardPanel (panelHandle);												//退出控制面板
	return 0;
}
void CALLBACK WaveInProc(HWAVEIN hwi, UINT uMsg,DWORD dwUser, DWORD dwl, DWORD dw2)
{//波形输入回调函数
	switch(uMsg)
	{
		case WIM_OPEN:														//波形输入设备开启成功发回的消息
			break;
		case WIM_DATA:														//一个缓冲区满发回的消息
			pData = (PBYTE)pDataBufferl;
			PlotStripChart(panelHandle, PANEL_STRIPCHART, pData, 127, 0, 7, VAL_UNSIGNED_CHAR);
			rult = waveInPrepareHeader(hwi, pwhrl, sizeof(WAVEHDR));		//准备缓冲区
			rult = waveInAddBuffer(hwi, pwhrl, sizeof(WAVEHDR));			//使用缓冲区
			//开始新的采集
			++Time;
			SetCtrlVal(panelHandle, PANEL_TIME, Time * 0.1);
			break;
		case WIM_CLOSE:														//波形输入设备关闭成功发回的消息
			break;
	}
}

int CVICALLBACK PanelCall (int panel, int event, void *callbackData,
						   int eventData1, int eventData2)
{//面板回调函数
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
			QuitUserInterface (0);											//退出控制面板
			break;
	}
	return 0;
}

//采集初始化的代码应该放在“开始采集”按钮的回调函数里，以下是初始化的过程
int CVICALLBACK Start (int panel, int control, int event,
					   void *callbackData, int eventData1, int eventData2)
{//波形输入设备初始化，开始数据采集
	switch (event)
	{
		case EVENT_COMMIT:
			//****声卡初始化****//
			//查看声卡支持的格式
			//****波形格式设置****//
			//波形格式设置为：单通道、8位分辨率、采样频率是11025Hz。
			wfx.wFormatTag = WAVE_FORMAT_1M08;									//指定格式
			wfx.nChannels = 1;													//单通道
			wfx.wBitsPerSample = 8;												//8位
			wfx.nSamplesPerSec = 11025;											//采样频率（Hz）
			wfx.nAvgBytesPerSec = 11025;										//每秒吞吐量（字节）
			wfx.nBlockAlign = 1;												//对齐格式（字节）
			wfx.cbSize = 0;														//附加字节
			//用设定的波形格式（结构wfx）去打开波形输入设备，其中指定了波形输入中断函数名
			//为“WaveInProce”。如打开开波形输入设备成功，则返回值为0，否则非0，此时程序退出。
			rult = waveInOpen(&hwi, WAVE_MAPPER, &wfx, (DWORD)(&WaveInProc), (DWORD)(&DataT),
							  CALLBACK_FUNCTION);								//打开波形输入设备
			if( rult > 0 )
			{//打开波形输入设备有错误
				rult = waveInGetErrorText(rult, (LPSTR)(&szText), 100);			//获得错误
				MessagePopup("打开波形输入设备错误！", szText);					//提示错误
				return -1;														//返回
			}
			//根据需要开辟缓冲区头和缓冲区的内存空间，锁定缓冲区
			pwhrl = GlobalAlloc(GPTR, sizeof( WAVEHDR));						//开辟缓冲区头
			pDataBufferl = GlobalAlloc(GHND, len);								//开辟缓冲区
			pDataBufferl = GlobalLock(pDataBufferl);							//锁定缓冲区
			//****波形输入缓冲区设置****//
			pwhrl->lpData = pDataBufferl;										//缓冲区指针
			pwhrl->dwBufferLength = len;										//缓冲区长度
			pwhrl->dwFlags = 0;													//标志
			pwhrl->dwBytesRecorded = 0;											//记录长度
			//启动波形输入设备进行数据采集
			rult = waveInPrepareHeader(hwi, pwhrl, sizeof(WAVEHDR));			//准备缓冲区
			rult = waveInAddBuffer(hwi, pwhrl, sizeof(WAVEHDR)); 				//使用缓冲区
			rult = waveInStart(hwi);											//开始进行采集
			
			break;
	}
	return 0;
}

//停止采集的代码应该放在“停止采集”按钮的回调函数里。停止采集的过程分为如下5步：
//停止波形输入；关闭波形输入设备；解除缓冲区锁定；释放缓冲区；释放缓冲区头
int CVICALLBACK Stop (int panel, int control, int event,
					  void *callbackData, int eventData1, int eventData2)
{//停止数据采集；关闭波形输入设备
	switch (event)
	{
		case EVENT_COMMIT:
			//**********释放声卡输入资源**********//
			rult = waveInStop(hwi);													//停止波形输入
			rult = waveInClose(hwi);												//关闭波形输入设备
			GlobalUnlock(pDataBufferl); 											//解除缓冲区锁定
			GlobalFree(pDataBufferl);												//释放缓冲区
			GlobalFree(pwhrl);														//释放缓冲区头
			break;
	}
	return 0;
}
