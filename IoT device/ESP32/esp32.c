#include "esp32.h"
#include <stdint.h>
uint16_t WiFi_RX_STA;
uint8_t WiFi_RX_BUF[WiFi_MAX_RECV_LEN];
uint8_t data_end_flag = 1;
uint8_t start_receive_data_flag;
uint8_t current_network_state;




//check the response received
uint8_t* esp32_check_cmd(uint8_t *str)
{
	char *strx=0;
	if(WiFi_RX_STA&0X8000)		//接收到一次数据了
	{
		WiFi_RX_BUF[WiFi_RX_STA&0X7FFF]=0;//添加结束符
		strx=strstr((const char*)WiFi_RX_BUF,(const char*)str);
	}
	return (uint8_t*)strx;
}

//ack can be null
// waittime unit 10ms
uint8_t esp32_send_cmd(uint8_t *cmd,uint8_t *ack,uint32_t waittime)
{
	uint8_t res=0;
	WiFi_RX_STA=0;
	data_end_flag = 1; //uart data will judge end by '\n'
	USART_RTOS_Send(&wifi_handle,(uint8_t *)cmd,strlen(cmd));
	if(strcmp(cmd,"+++")==0)
	{
		vTaskDelay(1);
	}
	if(ack&&waittime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时
		{
			vTaskDelay(1);//
			if(WiFi_RX_STA&0X8000)//接收到期待的应答结果
			{
				if(esp32_check_cmd(ack))
				{
					PRINTF("\r\nHave received the ack of command \"%s\" successfully!\r\n",(uint8_t*)cmd);
					break;//得到有效数据
				}
				else if(esp32_check_cmd("ERROR"))
				{
					PRINTF("\r\nHave received the ack of command \"%s\" ERROR!\r\n",(uint8_t*)cmd);
				}

				WiFi_RX_STA=0;
			}
		}
		if(waittime==0)
		{
			res=1;
			PRINTF("\r\n%sTime is OUT!!!\r\n",(uint8_t*)cmd);
		}
	}
	return res;
}

uint8_t esp32_send_data(uint8_t *data,uint8_t *ack,uint32_t waittime)
{
	uint8_t res=0;
	WiFi_RX_STA=0;       // if not send next command or data, the mcu will never deal the test data, but it will cache it;
	data_end_flag = 2;     //uart data will judge end by '}'
	USART_RTOS_Send(&wifi_handle,(uint8_t *)data,strlen(data));
	if(ack&&waittime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时
		{
			vTaskDelay(10);
			if(WiFi_RX_STA&0X8000)//接收到期待的应答结果
			{
				if(esp32_check_cmd(ack))
				{
					PRINTF("Have received http response\r\n");
					PRINTF("%s",WiFi_RX_BUF);
					break;//得到有效数据
				}
				WiFi_RX_STA=0;
			}
		}
		if(waittime==0)
		{
			res=1;
			PRINTF("\r\nHTTP response Time is OUT!!!\r\n");
		}
	}
	return res;
}

//return 0 success; return 1 error
uint8_t esp32_quit_trans(void)
{
	uint8_t endTransFlag = "+++";
	vTaskDelay(1);
	USART_RTOS_Send(&wifi_handle,endTransFlag,strlen(endTransFlag));
	return esp32_send_cmd("AT","OK",20);
}

