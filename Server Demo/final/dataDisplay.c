/********************************************************************************/
/*								数据显示										*/
/********************************************************************************/
// 显示日期
void display_date(void)
{
	double currDateTime;
	int bufferLen;
	char *dateTimeBuffer = NULL;
	int row;
	
	GetCurrentDateTime (&currDateTime);
	// 显示日期
	bufferLen = FormatDateTimeString (currDateTime, DATE_FORMATSTRING, NULL, 0);
	dateTimeBuffer = malloc (bufferLen + 1);
	FormatDateTimeString (currDateTime, DATE_FORMATSTRING, dateTimeBuffer, bufferLen + 1 );
	SetCtrlVal(panelHandle, PANEL_DATE, dateTimeBuffer);
	// 显示时间
	bufferLen = FormatDateTimeString(currDateTime, TIME_FORMATSTRING, NULL, 0);
	dateTimeBuffer = malloc (bufferLen + 1);
	FormatDateTimeString (currDateTime, TIME_FORMATSTRING, dateTimeBuffer, bufferLen + 1 );
	SetCtrlVal(panelHandle, PANEL_TIME, dateTimeBuffer);
	// 整合日期时间
	bufferLen = FormatDateTimeString(currDateTime, DATETIME_FORMATSTRING, NULL, 0);
	DateTimeBuffer = malloc(bufferLen + 1);
	FormatDateTimeString(currDateTime, DATETIME_FORMATSTRING, DateTimeBuffer, bufferLen + 1);
}

void display_voltage(int data)
{
	char data_buf[1024] = {0};
	double dis_data = 0.0;
	int max_voltage, min_voltage, flag, i;
	sprintf(data_buf, "%0.1f", data/1.0);
	dis_data = data/1.0;
	
	// 绘制电压变化图
	PlotStripChart(panelHandle, PANEL_CHART_VOLTAGE, &dis_data, 1, 0, 0, VAL_DOUBLE);
	
	GetCtrlVal(panelHandle, PANEL_MAX_VOLTAGE, &max_voltage);
	GetCtrlVal(panelHandle, PANEL_MIN_VOLTAGE, &min_voltage);
	if( data/1.0 > (double)max_voltage || data/1.0 < (double)min_voltage )
	{
		SetCtrlVal(panelHandle, PANEL_STATE_VOLTAGE, 1);
		flag = 1;
	}
	else
	{
		SetCtrlVal(panelHandle, PANEL_STATE_VOLTAGE, 0);
		flag = 0;
	}
	SetCtrlVal(panelHandle, PANEL_STRING_VOLTAGE, data_buf);
	
	// 新加一行
	InsertTableRows (panelHandle, PANEL_TABLE_VOLTAGE, 1, 1, VAL_USE_MASTER_CELL_TYPE);
	// 设置数据
	SetTableCellVal(panelHandle, PANEL_TABLE_VOLTAGE, MakePoint(1, 1), DateTimeBuffer);
	SetTableCellVal(panelHandle, PANEL_TABLE_VOLTAGE, MakePoint(2, 1), data_buf);
	if( flag )
	{	
		SetTableCellVal(panelHandle, PANEL_TABLE_VOLTAGE, MakePoint(3, 1), "是");
	}
	else
	{
		SetTableCellVal(panelHandle, PANEL_TABLE_VOLTAGE, MakePoint(3, 1), "否");
	}
	// 高亮报警信息
	if( flag )
	{
		for( i = 1; i <= 3; ++i )
		{
			SetTableCellAttribute(panelHandle, PANEL_TABLE_VOLTAGE, MakePoint(i, 1), ATTR_TEXT_COLOR, VAL_RED);
		}
	}
	else
	{
		for( i = 1; i <= 3; ++i )
		{
			SetTableCellAttribute(panelHandle, PANEL_TABLE_VOLTAGE, MakePoint(i, 1), ATTR_TEXT_COLOR, VAL_BLACK);
		}
	}
}
	
void display_temperature(int data)
{
	char data_buf[1024] = {0};
	double dis_data = 0.0;
	int max_temperature, min_temperature, flag, i;
	max_temperature = 30;
	min_temperature = 15;
	sprintf(data_buf, "%0.1f", data/10.0);
	dis_data = data/10.0;
	
	// 绘制温度变化图
	PlotStripChart (panelHandle, PANEL_CHART_TEMPERATURE, &dis_data, 1, 0, 0, VAL_DOUBLE);
	
	GetCtrlVal(panelHandle, PANEL_MAX_TEMPERATURE, &max_temperature);
	GetCtrlVal(panelHandle, PANEL_MIN_TEMPERATURE, &min_temperature);
	if( data/10.0 > (double)(max_temperature) || data/10.0 < (double)(min_temperature) )
	{
		SetCtrlVal(panelHandle, PANEL_STATE_TEMPERATURE, 1);
		flag = 1;
	}
	else
	{
		SetCtrlVal(panelHandle, PANEL_STATE_TEMPERATURE, 0);
		flag = 0;
	}
	SetCtrlVal(panelHandle, PANEL_STRING_TEMPERATURE, data_buf);
	
	// 新加一行
	InsertTableRows (panelHandle, PANEL_TABLE_TEMPERATURE, 1, 1, VAL_USE_MASTER_CELL_TYPE);
	// 设置数据
	SetTableCellVal(panelHandle, PANEL_TABLE_TEMPERATURE, MakePoint(1, 1), DateTimeBuffer);
	SetTableCellVal(panelHandle, PANEL_TABLE_TEMPERATURE, MakePoint(2, 1), data_buf);
	if( flag )
	{	
		SetTableCellVal(panelHandle, PANEL_TABLE_TEMPERATURE, MakePoint(3, 1), "是");
	}
	else
	{
		SetTableCellVal(panelHandle, PANEL_TABLE_TEMPERATURE, MakePoint(3, 1), "否");
	}
	// 高亮报警信息
	if( flag )
	{
		for( i = 1; i <= 3; ++i )
		{
			SetTableCellAttribute(panelHandle, PANEL_TABLE_TEMPERATURE, MakePoint(i, 1), ATTR_TEXT_COLOR, VAL_RED);
		}
	}
	else
	{
		for( i = 1; i <= 3; ++i )
		{
			SetTableCellAttribute(panelHandle, PANEL_TABLE_TEMPERATURE, MakePoint(i, 1), ATTR_TEXT_COLOR, VAL_BLACK);
		}
	}
}
	
void display_ponding(int data)
{
	char data_buf[1024] = {0};
	double dis_data = 0.0;
	int flag, i;
	
	dis_data = data;
	sprintf(data_buf, "%d", data); 
	// 绘制积水通断状况
	PlotStripChart(panelHandle, PANEL_CHART_PONDING, &dis_data, 1, 0, 0, VAL_DOUBLE);
	if( data )
	{
		SetCtrlVal(panelHandle, PANEL_STATE_PONDING, 1);
		flag = 1;
	}
	else
	{
		SetCtrlVal(panelHandle, PANEL_STATE_PONDING, 0);
		flag = 0;
	}
	// 新加一行
	InsertTableRows (panelHandle, PANEL_TABLE_PONDING, 1, 1, VAL_USE_MASTER_CELL_TYPE);
	// 设置数据
	SetTableCellVal(panelHandle, PANEL_TABLE_PONDING, MakePoint(1, 1), DateTimeBuffer);
	if( flag )
	{	
		SetTableCellVal(panelHandle, PANEL_TABLE_PONDING, MakePoint(2, 1), "是");
	}
	else
	{
		SetTableCellVal(panelHandle, PANEL_TABLE_PONDING, MakePoint(2, 1), "否");
	}
	// 高亮报警信息
	if( flag )
	{
		for( i = 1; i <= 2; ++i )
		{
			SetTableCellAttribute(panelHandle, PANEL_TABLE_PONDING, MakePoint(i, 1), ATTR_TEXT_COLOR, VAL_RED);
		}
	}
	else
	{
		for( i = 1; i <= 2; ++i )
		{
			SetTableCellAttribute(panelHandle, PANEL_TABLE_PONDING, MakePoint(i, 1), ATTR_TEXT_COLOR, VAL_BLACK);
		}
	}
}
