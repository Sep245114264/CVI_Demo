/*******************************************************************************/
/*						对采集到是数据进行排序								   */
/*******************************************************************************/	

int GetClickedColumnLabel(int panel, int control, int mouseX, int mouseY)
{
	int colWidth, colIndex, xLabelTraverse, gridTop, tableTop;
	Rect labelBarCoords;
	
	GetTableCellRangeRect(panel, control, VAL_TABLE_ENTIRE_RANGE,
						  &labelBarCoords);
	
	// 获取数值
	GetCtrlAttribute(panel, control, ATTR_COLUMN_LABELS_HEIGHT,
					 &labelBarCoords.height);
	GetCtrlAttribute(panel, control, ATTR_GRID_AREA_TOP, &gridTop);
	GetCtrlAttribute(panel, control, ATTR_TOP, &tableTop);
	labelBarCoords.top = tableTop + gridTop - labelBarCoords.height;
	
	// 判断鼠标是否停留在标签上
	if( RectContainsPoint(labelBarCoords, MakePoint(mouseX, mouseY)) )
	{
		xLabelTraverse = labelBarCoords.left;
		for( colIndex = 1; colIndex <= 3; ++colIndex )
		{
			GetTableColumnAttribute(panel, control, colIndex,
								   ATTR_COLUMN_ACTUAL_WIDTH, &colWidth);
			if( (mouseX >= xLabelTraverse) && (mouseX <= (xLabelTraverse + colWidth)) )
			{
				return colIndex;
			}
			xLabelTraverse += colWidth;
		}
	}
	return 0;
}

int CVICALLBACK SortByDate (int panel, int control, Point item1, Point item2, 
                            void *pcallbackData)
{
    int  month1;
    int  month2;
    int  day1;
    int  day2;
    int  year1;
    int  year2;
    int  hour1;
    int  hour2;
    int  min1;
    int  min2;
    int  sec1;
    int  sec2;
    char firstDateStr[35];
    char secondDateStr[35];
    
    // 读取待比较的数据
    GetTableCellVal (panel, control, item1, firstDateStr);
    GetTableCellVal (panel, control, item2, secondDateStr);				  
    sscanf (firstDateStr, "%d-%d-%d  %d:%d:%d", &year1, &month1, &day1,
            &hour1, &min1, &sec1);
    sscanf (secondDateStr, "%d-%d-%d  %d:%d:%d", &year2, &month2, &day2,
            &hour2, &min2, &sec2);
    
	if( year2 == year1 )
	{
		if( month2 == month1 )
		{
			if( day2 == day1 )
			{
				if( hour2 == hour1 )
				{
					if( min2 == min1 )
					{
						if( sec2 == sec1 )
						{
							return 0;
						}
						else
						{
							return (sec2 < sec1 ) ? -1 : 1;
						}
					}
					else
					{
						return (min2 < min1) ? -1 : 1;
					}
				}
				else
				{
					return (hour2 < hour1) ? -1 : 1;
				}
			}
			else
			{
				return (day2 < day1) ? -1 : 1;
			}
		}
		else
		{
			return (month2 < month1) ? -1 : 1;
		}
	}
	else
	{
		return (year2 < year1) ? -1 : 1;
	}
}

int CVICALLBACK SortByData(int panel, int control, Point item1, Point item2, void *pcallbackData)
{
	double data1, data2;
	char firstDataStr[1024] = {0};
	char secondDataStr[1024] = {0};
	
	GetTableCellVal(panel, control, item1, firstDataStr);
	GetTableCellVal(panel, control, item2, secondDataStr);
	sscanf(firstDataStr, "%lf", &data1);
	sscanf(secondDataStr, "%lf", &data2);
	
	if( data1 == data2 )
	{
		return 0;
	}
	else
	{
		return (data2 < data1) ? -1 : 1;
	}
}

int CVICALLBACK SortByWarning(int panel, int control, Point item1, Point item2, void *pcallbackData)
{
	char warning1[10] = {0};
	char warning2[10] = {0};
	
	GetTableCellVal(panel, control, item1, warning1);
	GetTableCellVal(panel, control, item2, warning2);
	
	if( strcmp(warning1, warning2) == 0 )
	{
		return 0;
	}
	else
	{
		return ( strcmp(warning1, warning2) > 0 ) ? -1 : 1;
	}
}