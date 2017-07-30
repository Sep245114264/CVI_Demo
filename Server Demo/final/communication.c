/************************************************************************************/
/*										通信模块			   						*/
/************************************************************************************/
void Read_Voltage(void)
{
	unsigned char transmit_buf[1024] = {0x03, 0x03, 0x0, 0x0, 0x0, 0x02};
	unsigned int crc[2] = {0};
	
	crc16(transmit_buf, 6, crc); 
	transmit_buf[6] = crc[0];
	transmit_buf[7] = crc[1];
	
	if( ServerTCPWrite(hconversation, transmit_buf, 8, 100) < 0 )
	{
		MessagePopup("警告", "发送失败，请等待下位机连接......");
		SetCtrlVal(panelHandle, PANEL_STATE_MONITOR, 0);
		start = 0;
		//return -1;
	}
	else
	{
		// 记录信息
		record_data(transmit_buf, SEND_DATA, READ_VOLTAGE);
	}
	inf_index = READ_VOLTAGE;
}
	
void Read_Temperature(void)
{
	unsigned char transmit_buf[1024] = {0x05, 0x03, 0x0, 0x0, 0x0, 0x02};
	unsigned int crc[2] = {0};
	
	crc16(transmit_buf, 6, crc);
	transmit_buf[6] = crc[0];
	transmit_buf[7] = crc[1];
	
	if( ServerTCPWrite(hconversation, transmit_buf, 8, 100) < 0 )
	{
		MessagePopup("警告", "发送失败，请等待下位机连接......");
		start = 0;
		SetCtrlVal(panelHandle, PANEL_STATE_MONITOR, 0);
		//return -1;
	}
	else
	{
		// 
		record_data(transmit_buf, SEND_DATA, READ_TEMPERATURE);
	}
	inf_index = READ_TEMPERATURE;
}

void Read_StatePonding(void)
{
	char transmit_buf[1024] = {0x04, 0x03, 0x00, 0x00, 0x00, 0x02};
	unsigned int crc[2] = {0};
	
	crc16(transmit_buf, 6, crc);
	transmit_buf[6] = crc[0];
	transmit_buf[7] = crc[1];

	if( ServerTCPWrite(hconversation, transmit_buf, 8, 5000) < 0 )
	{
		MessagePopup("警告", "查询发送失败，请等待下位机连接......");
		start = 0;
		SetCtrlVal(panelHandle, PANEL_STATE_MONITOR, 0);
		//return -1;
	}
	else
	{
		record_data(transmit_buf, SEND_DATA, READ_STATE_PONDING);
	}
	inf_index = READ_STATE_PONDING;
}
