/**************************************************************************************************/
/*									数据保存												  */
/**************************************************************************************************/
void SaveVoltage(void)
{
	
	FILE * fVoltage;
	int row, i;
	char date[1024] = {0};
	char data_buf[1024] = {0};
	char flag[16] = {0};
	
	fVoltage = fopen("电压.txt", "a+");

	GetNumTableRows(panelHandle, PANEL_TABLE_VOLTAGE, &row);
	while( --row )
	{
		if( row < 0 )
		{
			break;
		}
		GetTableCellVal(panelHandle, PANEL_TABLE_VOLTAGE, MakePoint(1, row), date);
		GetTableCellVal(panelHandle, PANEL_TABLE_VOLTAGE, MakePoint(2, row), data_buf);
		GetTableCellVal(panelHandle, PANEL_TABLE_VOLTAGE, MakePoint(3, row), flag);
		fprintf(fVoltage, "%s	   %s		%s\n", date, data_buf, flag);
	}
	fclose(fVoltage);
	DeleteTableRows (panelHandle, PANEL_TABLE_VOLTAGE, 1, -1);
}

void SaveTemperature(void)
{
	FILE * fTemperature;
	int row, i;
	char date[1024] = {0};
	char data_buf[1024] = {0};
	char flag[16] = {0};
	fTemperature = fopen("温度.txt", "a+");
	GetNumTableRows(panelHandle, PANEL_TABLE_TEMPERATURE, &row);
	while( --row )
	{
		if( row < 0 )
		{
			break;
		}
		GetTableCellVal(panelHandle, PANEL_TABLE_TEMPERATURE, MakePoint(1, row), date);
		GetTableCellVal(panelHandle, PANEL_TABLE_TEMPERATURE, MakePoint(2, row), data_buf);
		GetTableCellVal(panelHandle, PANEL_TABLE_TEMPERATURE, MakePoint(3, row), flag);
		fprintf(fTemperature, "%s	   %s		%s\n", date, data_buf, flag);
	}
	fclose(fTemperature);
	DeleteTableRows (panelHandle, PANEL_TABLE_TEMPERATURE, 1, -1);
}

void SavePonding(void)
{
	FILE * fPonding;
	int row, i;
	char date[1024] = {0};
	char data_buf[1024] = {0};
	char flag[16] = {0};
	fPonding = fopen("积水.txt", "a+");
	GetNumTableRows(panelHandle, PANEL_TABLE_PONDING, &row);
	while( --row )
	{
		if( row < 0 )
		{
			break;
		}
		GetTableCellVal(panelHandle, PANEL_TABLE_PONDING, MakePoint(1, row), date);
		GetTableCellVal(panelHandle, PANEL_TABLE_PONDING, MakePoint(2, row), flag);
		fprintf(fPonding, "%s	 	%s\n", date, flag);
	}
	fclose(fPonding);
	DeleteTableRows(panelHandle, PANEL_TABLE_PONDING, 1, -1);
}

void saveToTXT(void)
{
	SaveVoltage();
	SaveTemperature();
	SavePonding();
	interval = saveTime;
}

int saveToEXCEL(void)
{
	int i,row;
	double currDateTime;
	int bufferLen;
	char colWidth[16] = {0};
	char infoBuf[1024] ={0};
	char excelFileName[1024] = {0};
	char saveFileName[1024] = {0};
	
	char excelCellNum[16] = {0};
	
	GetNumTableRows(panelHandle, PANEL_TABLE_TEMPERATURE, &row);
	if( row < 0 )
	{
		return 1;
	}
	GetProjectDir(excelFileName);
	strcat(excelFileName, "\\监控日志.xlsx");
	ExcelRpt_ApplicationNew(0, &applicationHandle);
	if( ExcelRpt_WorkbookOpen (applicationHandle, excelFileName, &workbookHandle) < 0 )
	{
		MessagePopup("", "error");
		ExcelRpt_ApplicationQuit(applicationHandle);
	}
	ExcelRpt_GetWorksheetFromIndex(workbookHandle, 1, &worksheetHandle);
	ExcelRpt_ActivateWorksheet(worksheetHandle);
			
	// 存储电压记录
	sprintf(excelCellNum, "%s%d:%s%d", "A", 3, "C", row);
	ExcelRpt_WriteDataFromTableControl(worksheetHandle, excelCellNum, panelHandle, PANEL_TABLE_VOLTAGE);
	// 存储温度记录
	sprintf(excelCellNum, "%s%d:%s%d", "D", 3, "F", row);
	ExcelRpt_WriteDataFromTableControl(worksheetHandle, excelCellNum, panelHandle, PANEL_TABLE_TEMPERATURE);
	//存储积水记录
	sprintf(excelCellNum, "%s%d:%s%d", "G", 3, "H", row);
	ExcelRpt_WriteDataFromTableControl(worksheetHandle, excelCellNum, panelHandle, PANEL_TABLE_PONDING);
	
	// 保存记录
	GetCurrentDateTime (&currDateTime);
	bufferLen = FormatDateTimeString(currDateTime, DATETIME_FORMATSTRING, NULL, 0);
	DateTimeBuffer = malloc(bufferLen + 1);
	FormatDateTimeString(currDateTime, SAVE_FORMATSTRING, DateTimeBuffer, bufferLen + 1);
	GetProjectDir(saveFileName);
	sprintf(infoBuf, "%s%s%s","\\log\\", DateTimeBuffer, ".xlsx");
	strcat(saveFileName, infoBuf);
	if( ExcelRpt_WorkbookSave(workbookHandle, saveFileName, ExRConst_DefaultFileFormat) < 0 )
	{
		//GetProjectDir(infoBuf);
		//strcat(infoBuf, "\\log");
		MakeDir("log");
		ExcelRpt_WorkbookSave(workbookHandle, saveFileName, ExRConst_DefaultFileFormat);
	}
	//ExcelRpt_WorkbookClose(workbookHandle,0);
	ExcelRpt_ApplicationQuit(applicationHandle);
	CA_DiscardObjHandle(applicationHandle);
	CA_DiscardObjHandle(workbookHandle);
	CA_DiscardObjHandle(worksheetHandle);
	//Excel_AppQuit (, NULL);
	DeleteTableRows(panelHandle, PANEL_TABLE_VOLTAGE, 1, -1);
	DeleteTableRows(panelHandle, PANEL_TABLE_TEMPERATURE, 1, -1);
	DeleteTableRows(panelHandle, PANEL_TABLE_PONDING, 1, -1);
	interval = saveTime;
	
	return 0;
}
