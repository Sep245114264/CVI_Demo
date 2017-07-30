/*****************************************************************************/
/*																	 */
/*****************************************************************************/
void crc16(unsigned char * original_data, int length, unsigned int *crc)
{
	unsigned char * ptr = original_data;
	unsigned int check = 0;
	unsigned int temp_check;
	int i, j;
	
	check = 0xFFFF;
	for( i = 0; i < length; ++i )
	{
		temp_check = *(ptr+i) & 0x00FF;
		check ^= temp_check;
		for( j = 0; j < 8; ++j )
		{
			if( check & 1 )
			{
				check >>= 1;
				check ^= POLY;
			}
			else
			{
				check >>= 1;
			}
		}
	}
	crc[0] = check & 0xFF;
	crc[1] = check >> 8;
}

// 将下位机发送回来的数据包进行解包
PackData unPack(unsigned char * pack_data)
{
	int i;
	unsigned char data[1024];
	unsigned char data_buf[256] = {0};
	unsigned int crc[2] = {0};
	PackData unpack_data;
	
	// 读取高力IP
	unpack_data.target_ip = pack_data[0];
	// 读取操作码
	unpack_data.opcode = pack_data[1];
	// 读取数据长度
	unpack_data.length = pack_data[2];
			
	// 整合待校验的数据
	for( i = 0; i < unpack_data.length + 3; ++i )
	{
		data[i] = pack_data[i];
	}
	// CRC校验
	crc16(data, unpack_data.length + 3, crc);
	if( crc[0] != pack_data[unpack_data.length+3] || crc[1] != pack_data[unpack_data.length+4] )
	{
		sprintf(data_buf, "0x%x 0x%x, %s", crc[0], crc[1], "数据传输出错,重接发送请求...");
		SetCtrlVal(panelHandle_log, PANELLOG_LOG, data_buf);
		unpack_data.data = -99;
		return unpack_data;
	}
	// 检验成功，根据不同的设备地址选择不同的解包方式
	switch( unpack_data.target_ip )
	{
		case READ_VOLTAGE:
			unpack_data.data = (pack_data[3] * 256 + pack_data[4]); // 
			break;
		case READ_TEMPERATURE:
			unpack_data.data = pack_data[5] * 256 +pack_data[6];	// 
			if( unpack_data.data > 1000 )
			{
				unpack_data.data = -99;
				return unpack_data;
			}
			if( unpack_data.data - previousData < 3 && unpack_data.data - previousData > -3 || previousData == 0)
			{
				previousData = unpack_data.data;
			}
			else
			{
				unpack_data.data = -99;
				return unpack_data.cata;
			}
			break;
		case READ_STATE_PONDING:
			unpack_data.data = pack_data[4];
			break;
	}
	return unpack_data;
}

void record_data(unsigned char * data, int kind, int action)
{
	int i, length;
	unsigned char info_data[1024] = {0};
	
	switch( action )
	{
		case READ_VOLTAGE:
			sprintf(info_data, "\n              操作：读取电压\n-------------------------------------------\n");
			SetCtrlVal(panelHandle_log, PANELLOG_LOG, info_data);
			break;
		case READ_TEMPERATURE:
			sprintf(info_data, "\n              操作：读取温度\n-------------------------------------------\n");
			SetCtrlVal(panelHandle_log, PANELLOG_LOG, info_data);
			break;
		case READ_STATE_PONDING:
			sprintf(info_data, "\n              操作：查询积水深度\n-------------------------------------------\n");
			SetCtrlVal(panelHandle_log, PANELLOG_LOG, info_data);
			break;
	}
	
	switch( kind )
	{
		case SEND_DATA:
			sprintf(info_data, "发送数据：");
			length = 6;
			break;
		case RECEIVE_DATA:
			sprintf(info_data, "接受数据：");
			length = 7;
			break;
	}
	SetCtrlVal(panelHandle_log, PANELLOG_LOG, info_data);
	
	for( i = 0; i < length; ++i )
	{
		sprintf(info_data, "%x ", *(data + i));
		SetCtrlVal(panelHandle_log, PANELLOG_LOG, info_data);
	}

	sprintf(info_data, "\nCRC校检码：%x %x\n", data[length], data[length+1]);
	SetCtrlVal(panelHandle_log, PANELLOG_LOG, info_data);
}
