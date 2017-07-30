void QuickExe(void)
{
	FILE *file_temperature;
	FILE *file_ponding;
	FILE *file_voltage;
	char fv_name[50] = "电压.txt";
	char ft_name[50] = "温度.txt";
	char fp_name[50] = "积水.txt";
	
	file_voltage = fopen(fv_name, "a+");
	fprintf(file_voltage, "%s\n", "---------------------------------");
	fclose(file_voltage);
	
	file_temperature = fopen(ft_name, "a+");
	fprintf(file_temperature, "%s\n", "----------------------------------");
	fclose(file_temperature);
	
	file_ponding = fopen(fp_name, "a+");
	fprintf(file_ponding, "%s\n", "------------------------------------");
	fclose(file_ponding);
}

void data_read(void)
{
	FILE * file_temperature;
	int i;
	int count = 0;
	char ch;
	char dateTime[1024] = {0};
	char date[1024] = {0};
	char time[1024] = {0};
	char data[1024] = {0};
	char flag[4] = {0};
	double dis_data = 0;
	file_temperature = fopen("温度.txt", "r");
	while( (ch = fgetc(file_temperature)) != EOF )
	{
		if( ch == '\n' )
		{
			++count;
		}
	} 
	fseek(file_temperature, 0, SEEK_SET);
	for( i = 0; i < count; ++i )
	{
		fscanf(file_temperature, "%s  %s		%s		%s", date, time, data, flag);
		sprintf(dateTime, "%s  %s", date, time);
		//新加一行
		InsertTableRows (panelHandle, PANEL_TABLE_TEMPERATURE, 1, 1, VAL_USE_MASTER_CELL_TYPE);
		// 设置数据
		SetTableCellVal(panelHandle, PANEL_TABLE_TEMPERATURE, MakePoint(1, 1), dateTime);
		SetTableCellVal(panelHandle, PANEL_TABLE_TEMPERATURE, MakePoint(2, 1), data);
		dis_data = atoi(data);
		PlotStripChart (panelHandle, PANEL_CHART_TEMPERATURE, &dis_data, 1, 0, 0, VAL_DOUBLE);
		SetTableCellVal(panelHandle, PANEL_TABLE_TEMPERATURE, MakePoint(3, 1), flag);
	}
	fclose(file_temperature);
}