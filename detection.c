/**********************************************/
/*���ã�	�����ʷ��¼					
/*panel: 	�����
/*control: 	�ؼ���
/*...: 		��Ҫ��¼����Ϣ 
/**********************************************/
void AddRecordToTable(int panel, int control, float temperature)
{
	char current_time[30] = {0};
	int day, year, month;
	int minutes, hours, seconds;

	GetSystemDate(&month, &day, &year);
	GetSystemTime(&hours, &minutes, &seconds);
	sprintf(current_time, "%d/%02d/%02d %02d:%02d:%02d", year, month, day, hours, minutes, seconds);
	
	InsertTableRows(panel, control, 1, 1, VAL_USE_MASTER_CELL_TYPE);
	SetTableCellVal(panel, control, MakePoint(1, 1), current_time);
	SetTableCellVal(panel, control, MakePoint(2, 1), temperature);
}

/***********************************************/
/*���ã�	������ʷ��¼					
/*panel: 	�����
/*control: 	�ؼ���
/*...: 		��Ҫ��¼����Ϣ 
/**********************************************/
void ExportHistory(int panel, int control, int record_num, int other)
{
	int FileHandle = 0;			// �����ļ��ľ��
	int i = 0;
	char date[50] = {0};		// ��¼��ǰ����ʱ��
	double temperature = 0;		
	double ponding = 0;
	char histroy[100] = {0};
	char fileName[500] = "��ʷ��¼.txt";
	
	if( other )
	{
		FileSelectPopupEx("", "*.txt", "", "������ָ��Ŀ¼", VAL_OK_BUTTON, 0, 0, fileName);
	}
	FileHandle = OpenFile(fileName, VAL_READ_WRITE, VAL_TRUNCATE, VAL_ASCII);
	// �ӿؼ��ж�ȡ��ʷ��¼���洢����Ӧ�ı�����
	for( i = 0; i < record_num; ++i )
	{
		GetTableCellVal(panel, control, MakePoint(1, i), date);
		GetTableCellVal(panel, control, MakePoint(2, i), &ponding);
		GetTableCellVal(panel, control, MakePoint(3, i), &temperature);
		sprintf(histroy, "%s %0.2f %0.2f\n", date, ponding, temperature);
		WriteFile(FileHandle, histroy, strlen(histroy));
	}
		
	CloseFile(FileHandle);
}

/**********************************************/
/*���ã�	����������Ϣ					
/*panel: 	�����
/*control: 	�ؼ���
/*...: 		��Ҫ��¼����Ϣ 
/**********************************************/
void DisplayErrorMessage(void)
{
	int ErrorIndex = 0;
	char *ErrorString;
	char ErrorMessage[250] = {0};
	
	ErrorIndex = ReturnRS232Err();\
	ErrorString = GetRS232ErrorString(ErrorIndex);
	Fmt(ErrorMessage, "%s%d%s%s", "�������: ", ErrorIndex, "\n������Ϣ: ", ErrorString);
	MessagePopup("����", ErrorMessage);
}