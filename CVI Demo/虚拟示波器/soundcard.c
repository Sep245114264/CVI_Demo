#include <cvirte.h>		
#include <userint.h>
#include "soundcard.h"
#include <analysis.h>
#include <windows.h>
#include <Mmsystem.h>
static int panelHandle;
double DataT;
UINT Time;
//====��Ƶ���κ���====//
HWAVEIN hwi;																//�豸���
WAVEFORMATEX wfx;															//���θ�ʽ�ṹ
PWAVEHDR pwhrl;																//���λ�����ͷָ��
LPSTR pDataBufferl;															//��������ַָ��
UINT len = 1024;															//����������
	PBYTE pData;															//������ָ�루�ֽ�ָ�룩
	UINT DevNum;															//�豸���
	MMRESULT rult;															//���κ������ؽṹ
	char szText[100];														//���κ������ش����ַ���
	WAVEINCAPS wavecap;														//�����豸���ܽṹ
	void CALLBACK WaveInProc(HWAVEIN hwi, UINT uMsg,DWORD dwUser, DWORD dwl, DWORD dw2);

//====������====//
int main (int argc, char *argv[])
{
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((panelHandle = LoadPanel (0, "soundcard.uir", PANEL)) < 0)
		return -1;
	DisplayPanel (panelHandle);												//��ʾ�������
	RunUserInterface ();													//���п������
	DiscardPanel (panelHandle);												//�˳��������
	return 0;
}
void CALLBACK WaveInProc(HWAVEIN hwi, UINT uMsg,DWORD dwUser, DWORD dwl, DWORD dw2)
{//��������ص�����
	switch(uMsg)
	{
		case WIM_OPEN:														//���������豸�����ɹ����ص���Ϣ
			break;
		case WIM_DATA:														//һ�������������ص���Ϣ
			pData = (PBYTE)pDataBufferl;
			PlotStripChart(panelHandle, PANEL_STRIPCHART, pData, 127, 0, 7, VAL_UNSIGNED_CHAR);
			rult = waveInPrepareHeader(hwi, pwhrl, sizeof(WAVEHDR));		//׼��������
			rult = waveInAddBuffer(hwi, pwhrl, sizeof(WAVEHDR));			//ʹ�û�����
			//��ʼ�µĲɼ�
			++Time;
			SetCtrlVal(panelHandle, PANEL_TIME, Time * 0.1);
			break;
		case WIM_CLOSE:														//���������豸�رճɹ����ص���Ϣ
			break;
	}
}

int CVICALLBACK PanelCall (int panel, int event, void *callbackData,
						   int eventData1, int eventData2)
{//���ص�����
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
			QuitUserInterface (0);											//�˳��������
			break;
	}
	return 0;
}

//�ɼ���ʼ���Ĵ���Ӧ�÷��ڡ���ʼ�ɼ�����ť�Ļص�����������ǳ�ʼ���Ĺ���
int CVICALLBACK Start (int panel, int control, int event,
					   void *callbackData, int eventData1, int eventData2)
{//���������豸��ʼ������ʼ���ݲɼ�
	switch (event)
	{
		case EVENT_COMMIT:
			//****������ʼ��****//
			//�鿴����֧�ֵĸ�ʽ
			//****���θ�ʽ����****//
			//���θ�ʽ����Ϊ����ͨ����8λ�ֱ��ʡ�����Ƶ����11025Hz��
			wfx.wFormatTag = WAVE_FORMAT_1M08;									//ָ����ʽ
			wfx.nChannels = 1;													//��ͨ��
			wfx.wBitsPerSample = 8;												//8λ
			wfx.nSamplesPerSec = 11025;											//����Ƶ�ʣ�Hz��
			wfx.nAvgBytesPerSec = 11025;										//ÿ�����������ֽڣ�
			wfx.nBlockAlign = 1;												//�����ʽ���ֽڣ�
			wfx.cbSize = 0;														//�����ֽ�
			//���趨�Ĳ��θ�ʽ���ṹwfx��ȥ�򿪲��������豸������ָ���˲��������жϺ�����
			//Ϊ��WaveInProce������򿪿����������豸�ɹ����򷵻�ֵΪ0�������0����ʱ�����˳���
			rult = waveInOpen(&hwi, WAVE_MAPPER, &wfx, (DWORD)(&WaveInProc), (DWORD)(&DataT),
							  CALLBACK_FUNCTION);								//�򿪲��������豸
			if( rult > 0 )
			{//�򿪲��������豸�д���
				rult = waveInGetErrorText(rult, (LPSTR)(&szText), 100);			//��ô���
				MessagePopup("�򿪲��������豸����", szText);					//��ʾ����
				return -1;														//����
			}
			//������Ҫ���ٻ�����ͷ�ͻ��������ڴ�ռ䣬����������
			pwhrl = GlobalAlloc(GPTR, sizeof( WAVEHDR));						//���ٻ�����ͷ
			pDataBufferl = GlobalAlloc(GHND, len);								//���ٻ�����
			pDataBufferl = GlobalLock(pDataBufferl);							//����������
			//****�������뻺��������****//
			pwhrl->lpData = pDataBufferl;										//������ָ��
			pwhrl->dwBufferLength = len;										//����������
			pwhrl->dwFlags = 0;													//��־
			pwhrl->dwBytesRecorded = 0;											//��¼����
			//�������������豸�������ݲɼ�
			rult = waveInPrepareHeader(hwi, pwhrl, sizeof(WAVEHDR));			//׼��������
			rult = waveInAddBuffer(hwi, pwhrl, sizeof(WAVEHDR)); 				//ʹ�û�����
			rult = waveInStart(hwi);											//��ʼ���вɼ�
			
			break;
	}
	return 0;
}

//ֹͣ�ɼ��Ĵ���Ӧ�÷��ڡ�ֹͣ�ɼ�����ť�Ļص������ֹͣ�ɼ��Ĺ��̷�Ϊ����5����
//ֹͣ�������룻�رղ��������豸������������������ͷŻ��������ͷŻ�����ͷ
int CVICALLBACK Stop (int panel, int control, int event,
					  void *callbackData, int eventData1, int eventData2)
{//ֹͣ���ݲɼ����رղ��������豸
	switch (event)
	{
		case EVENT_COMMIT:
			//**********�ͷ�����������Դ**********//
			rult = waveInStop(hwi);													//ֹͣ��������
			rult = waveInClose(hwi);												//�رղ��������豸
			GlobalUnlock(pDataBufferl); 											//�������������
			GlobalFree(pDataBufferl);												//�ͷŻ�����
			GlobalFree(pwhrl);														//�ͷŻ�����ͷ
			break;
	}
	return 0;
}
