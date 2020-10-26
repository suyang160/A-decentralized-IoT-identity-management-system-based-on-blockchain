/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

/* Freescale includes. */
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"

#include "fsl_usart_freertos.h"
#include "fsl_usart.h"

#include "pin_mux.h"
#include "clock_config.h"

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif
#include "mbedtls/platform.h"
#include <string.h>
#include <stdlib.h>
#include "mbedtls/timing.h"
#include "mbedtls/ecdsa.h"
#include "mbedtls/ecdh.h"
#include "mbedtls/error.h"

#include <stdio.h>
#include <stdint.h>
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/aes.h"

#include "bip32.h"
#include "bip39.h"
#include "curves.h"

#include "utils.h"
#include "RLP.h"
#include "esp32.h"
#include "esp32_command.h"

#include "secp256k1.h"
#include "generateRawTransaction.h"
#include "iotDIDContract.h"
#include "cJSON.h"

/*****SD card and FATFS32*******/
#include "fsl_sd.h"
#include "ff.h"
#include "diskio.h"
#include "fsl_sd_disk.h"
#include "fsl_iocon.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define BLUETOOTH_USART USART2
#define BLUETOOTH_USART_CLK_SRC kCLOCK_Flexcomm2
#define BLUETOOTH_USART_CLK_FREQ CLOCK_GetFlexCommClkFreq(0U)
#define BLUETOOTH_USART_IRQHandler FLEXCOMM2_IRQHandler
#define BLUETOOTH_USART_IRQn FLEXCOMM2_IRQn
/* Task priorities. */
#define bluetooth_uart_task_PRIORITY (configMAX_PRIORITIES - 2)
#define BLUETOOTH_USART_NVIC_PRIO 5
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void bluetooth_uart_task(void *pvParameters);

/*******************************************************************************
 * Code
 ******************************************************************************/
char temp[] = {0x07,0x41,0x00,0x01,0x00,0x01,0x32,0x0A,0x0D};
const char *to_send             = "FreeRTOS USART driver example!\r\n";
const char *send_buffer_overrun = "\r\nRing buffer overrun!\r\n";
uint8_t bluetooth_background_buffer[1000];
uint8_t bluetooth_recv_char;

usart_rtos_handle_t bluetooth_handle;
struct _usart_handle bluetooth_t_handle;

struct rtos_usart_config usart_config = {
    .baudrate    = 38400,
    .parity      = kUSART_ParityDisabled,
    .stopbits    = kUSART_OneStopBit,
    .buffer      = bluetooth_background_buffer,
    .buffer_size = sizeof(bluetooth_background_buffer),
};


/*******************************************************************************
 * Definitions For DMID project
 * WiFi Interface. Use usart 1, P0-13:RXD,P0-14:TXD
 * If you change this interface, you need to change function BOARD_InitPins()
 * and add function CLOCK_AttachClk().
 *
 ******************************************************************************/
#define WiFi_USART USART1
#define WiFi_USART_CLK_SRC kCLOCK_Flexcomm1
#define WiFi_USART_CLK_FREQ CLOCK_GetFreq(kCLOCK_Flexcomm1)
#define WiFi_USART_IRQHandler FLEXCOMM1_IRQHandler
#define WiFi_USART_IRQn FLEXCOMM1_IRQn
#define WiFi_USART_NVIC_PRIO 5
#define WiFi_UART_CLK_FREQ 12000000U
#define wifi_dataProcess_task_PRIORITY (configMAX_PRIORITIES - 2)

uint8_t wifi_background_buffer[2000]; //ring buffer is set to 2000 bytes
uint8_t wifi_recv_buffer;
usart_rtos_handle_t wifi_handle; //This wifi_handle will be called by esp32.c

struct rtos_usart_config wifi_usart_config = {
    .baudrate    = 115200,
    .parity      = kUSART_ParityDisabled,
    .stopbits    = kUSART_OneStopBit,
    .buffer      = wifi_background_buffer,
    .buffer_size = sizeof(wifi_background_buffer),
	.srcclk      = WiFi_UART_CLK_FREQ,
	.base        = WiFi_USART,
};
struct _usart_handle wifi_t_handle;


uint8_t wifi_initialize_finish_flag = 0;//if 1, initialize sucessfully


/*************************SD CARD*************************************/
#define DEMO_TASK_GET_SEM_BLOCK_TICKS 1U
#define DEMO_TASK_ACCESS_SDCARD_TIMES 1U
/*! @brief Task stack size. */
#define ACCESSFILE_TASK_STACK_SIZE (1024U)
/*! @brief Task stack priority. */
#define ACCESSFILE_TASK_PRIORITY (configMAX_PRIORITIES - 2U)

/*! @brief Task stack size. */
#define CARDDETECT_TASK_STACK_SIZE (1024U)
/*! @brief Task stack priority. */
#define CARDDETECT_TASK_PRIORITY (configMAX_PRIORITIES - 1U)

static void CardDetectTask(void *pvParameters);
static void SDCARD_DetectCallBack(bool isInserted, void *userData);
static status_t DEMO_MakeFileSystem(void);
int currentDMIDNum();
void writeDIDandDIDDocumentToSD(char * DMIDcontent, char * DMIDDocumentcontent);

static FATFS g_fileSystem; /* File system object */
static FIL g_fileObject;   /* File object */
static uint32_t s_taskSleepTicks = portMAX_DELAY;
static const uint8_t s_buffer1[] = {'T', 'A', 'S', 'K', '1', '\r', '\n'};
/*! @brief SDMMC host detect card configuration */
static const sdmmchost_detect_card_t s_sdCardDetect = {
#ifndef BOARD_SD_DETECT_TYPE
    .cdType = kSDMMCHOST_DetectCardByGpioCD,
#else
    .cdType = BOARD_SD_DETECT_TYPE,
#endif
    .cdTimeOut_ms = (~0U),
    .cardInserted = SDCARD_DetectCallBack,
    .cardRemoved  = SDCARD_DetectCallBack,
};

/*! @brief SD card detect flag  */
static volatile bool s_cardInserted     = false;
static volatile bool s_cardInsertStatus = false;
/*! @brief Card semaphore  */
static SemaphoreHandle_t s_fileAccessSemaphore = NULL;
static SemaphoreHandle_t s_CardDetectSemaphore = NULL;

/*********************************************************************/

typedef enum current_state{
	none,
	head_state1,
	function_state1,
	function_state2,
	date_length1,
	data_length2,
	data_receive,
	end1,
	end2
} current_state;

typedef enum serialDataProcess_return{
	data_receving,
	data_error,
	data_success
}serialDataProcess_return;

uint8_t functionCode[2];
uint8_t dataLength[2];
uint8_t dataReceive[100];
int length=0;
int functionCodeValue=0;
serialDataProcess_return serialDataProcess(uint8_t * receive_char);
void sendMessgeToAndroid(uint8_t * temp,int functioncode,int dataLength,uint8_t* data);
void sendMessageToUpperComputer(int length, uint8_t * data, uint8_t type);
void recoverIvTempValue();

static void wifi_dataProcess_task(void *pvParameters);
static void wifi_dataSend_task(void *pvParameters);
uint8_t wifi_receive_data(uint8_t byte);
void freshTransactionNonce(char * nonce);
void generateDIDDocument(char * did, char * pubKeyId, char * pubKeyHex,char * out);
/*!
 * @brief Application entry point.
 */

/****************************************************************************/


int exit_code = MBEDTLS_EXIT_FAILURE;
mbedtls_ecdh_context ctx_cli, ctx_srv;
mbedtls_entropy_context entropy;
mbedtls_ctr_drbg_context ctr_drbg;
unsigned char cli_to_srv[32], srv_to_cli[32];
unsigned char secret[32];
const char pers[] = "ecdh";

mbedtls_aes_context ctx_aes;
unsigned char iv[16]={0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15};
unsigned char iv_temp[16]={0x00};
unsigned char AES_KEY[32]={0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15};
unsigned char plain[16] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15};
unsigned char plain1[16] ="HelloWorld1!!!!!";
unsigned char cipher[16]={0};
unsigned char dec_plain[16]={0};

bool DMIDgeneration_flag = false;
int main(void)
{
    /* Init board hardware. */
    /* attach main clock divide to FLEXCOMM0 (debug console) */

	char diddocument[800];
    CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH);
    CLOCK_AttachClk(kFRO12M_to_FLEXCOMM1);
	CLOCK_AttachClk(kFRO12M_to_FLEXCOMM2);
	CLOCK_AttachClk(BOARD_SDIF_CLK_ATTACH);

    BOARD_InitPins();
    BOARD_BootClockPLL150M();
    BOARD_InitDebugConsole();
    /* need call this function to clear the halt bit in clock divider register */
    CLOCK_SetClkDiv(kCLOCK_DivSdioClk, (uint32_t)(SystemCoreClock / FSL_FEATURE_SDIF_MAX_SOURCE_CLOCK + 1U), true);

    if (xTaskCreate(bluetooth_uart_task, "Uart_task", 1024*2, NULL, bluetooth_uart_task_PRIORITY, NULL) != pdPASS)
    {
        PRINTF("Task creation failed!.\r\n");
        while (1)
            ;
    }
    if (xTaskCreate(wifi_dataProcess_task, "wifi_dataProcess_task", 1024*2, NULL, wifi_dataProcess_task_PRIORITY, NULL) != pdPASS)
	{
        PRINTF("WiFi_dataProcess_task creation failed!.\r\n");
        while (1)
            ;
	}

    if (xTaskCreate(wifi_dataSend_task, "wifi_dataSend_task", 1024*10, NULL, 3 , NULL) != pdPASS)
	{
        PRINTF("WiFi_dataSend_task creation failed!.\r\n");
        while (1)
            ;
	}
//    if (pdPASS != xTaskCreate(FileAccessTask1, "FileAccessTask1", ACCESSFILE_TASK_STACK_SIZE, NULL,
//                              ACCESSFILE_TASK_PRIORITY, NULL))
//    {
//        return -1;
//    }

    if (pdPASS !=
        xTaskCreate(CardDetectTask, "CardDetectTask", CARDDETECT_TASK_STACK_SIZE, NULL, CARDDETECT_TASK_PRIORITY, NULL))
    {
        return -1;
    }

    PRINTF("Uart task creation success.\r\n");


//    generateDIDDocument("hello", "hello","hello",diddocument);
//    PRINTF("diddocument is%s",diddocument);
//    vTaskSuspend(NULL);

    vTaskStartScheduler();
    for (;;)
        ;
}


/*!
 * @brief Task responsible for loopback.
 */
static void bluetooth_uart_task(void *pvParameters)
{
    int error;
    int i;
    int ret;
    size_t n            = 0;
    usart_config.srcclk = BOARD_DEBUG_UART_CLK_FREQ;
    usart_config.base   = BLUETOOTH_USART;
    uint8_t * sendmessage;

    NVIC_SetPriority(BLUETOOTH_USART_IRQn, BLUETOOTH_USART_NVIC_PRIO);

    if (0 > USART_RTOS_Init(&bluetooth_handle, &bluetooth_t_handle, &usart_config))
    {
        vTaskSuspend(NULL);
    }

    /* Send introduction message. */
//    if (0 > USART_RTOS_Send(&handle, (uint8_t *)to_send, strlen(to_send)))
//    {
//        vTaskSuspend(NULL);
//    }

    /* Receive user input and send it back to terminal. */
    do
    {
        error = USART_RTOS_Receive(&bluetooth_handle, &bluetooth_recv_char, 1, &n);
        if (error == kStatus_USART_RxRingBufferOverrun)
        {
            /* Notify about hardware buffer overrun */
            if (kStatus_Success !=
                USART_RTOS_Send(&bluetooth_handle, (uint8_t *)send_buffer_overrun, strlen(send_buffer_overrun)))
            {
                vTaskSuspend(NULL);
            }
        }
        if (n > 0)
        {
            /* send back the received data */
        	//PRINTF("ReceiveByte is%02x\r\n",recv_char);
            if(serialDataProcess(&bluetooth_recv_char)==data_success)
            {
//            	PRINTF("FunctionCode is%02x%02x\r\n",functionCode[0],functionCode[1]);
//            	PRINTF("DataLength is%02x%02x\r\n",dataLength[0],dataLength[1]);
//            	PRINTF("data length:%d\r\n",length);
//            	PRINTF("Have received:\r\n");
//            	for(i=0;i<length;i++)
//            	{
//            		PRINTF("%02x",dataReceive[i]);
//            	}
            	if(functionCodeValue == 1)
            	{
            		    PRINTF("\r\nFunctionCodeValue is %d\r\n",functionCodeValue);
            			PRINTF("Receive the public key from mobile phone:\r\n");
            			sendMessageToUpperComputer(32, dataReceive, 2);

            		    mbedtls_ecdh_init( &ctx_cli );
            		    mbedtls_ecdh_init( &ctx_srv );
            		    mbedtls_ctr_drbg_init( &ctr_drbg );

            		    PRINTF("LPC55S69 start to generate keypairs based on curve25519 locally\r\n");

            		    /*
            		     * Initialize random number generation
            		     */
            		    mbedtls_printf( "1. Seeding the random number generator...\r\n" );
            		    fflush( stdout );

            		    mbedtls_entropy_init( &entropy );
            		    if( ( ret = mbedtls_ctr_drbg_seed( &ctr_drbg, mbedtls_entropy_func, &entropy,
            		                               (const unsigned char *) pers,
            		                               sizeof pers ) ) != 0 )
            		    {
            		        mbedtls_printf( " failed\n  ! mbedtls_ctr_drbg_seed returned %d\n", ret );
            		        goto exit;
            		    }


            		    /*
            		     * Client: inialize context and generate keypair
            		     */
            		    mbedtls_printf( "2. Setting up client context...\r\n" );
            		    fflush( stdout );

            		    mbedtls_printf( "2.1 Set a group using CURVE25519 domain parameters\r\n" );
            		    ret = mbedtls_ecp_group_load( &ctx_cli.grp, MBEDTLS_ECP_DP_CURVE25519 );
            		    if( ret != 0 )
            		    {
            		        mbedtls_printf( " failed\n  ! mbedtls_ecp_group_load returned %d\n", ret );
            		        goto exit;
            		    }

            		    mbedtls_printf( "2.2 Generate public key\r\n" );
            		    ret = mbedtls_ecdh_gen_public( &ctx_cli.grp, &ctx_cli.d, &ctx_cli.Q,
            		                                   mbedtls_ctr_drbg_random, &ctr_drbg );
            		    if( ret != 0 )
            		    {
            		        mbedtls_printf( " failed\n  ! mbedtls_ecdh_gen_public returned %d\n", ret );
            		        goto exit;
            		    }

            		    mbedtls_printf( "2.3 Export public key into unsigned binary data, little endian\r\n" );

            		    ret = mbedtls_mpi_write_binary_le( &ctx_cli.Q.X, cli_to_srv, 32 );
            		    if( ret != 0 )
            		    {
            		        mbedtls_printf( " failed\n  ! mbedtls_mpi_write_binary returned %d\n", ret );
            		        goto exit;
            		    }
            		    sendMessageToUpperComputer(32, cli_to_srv, 2);

            		    /*
            		     * Client: read peer's key and generate shared secret
            		     */
            		    mbedtls_printf( "3. LPC55S69 read public key of mobile phone and compute secret...\r\n" );
            		    fflush( stdout );

            		    mbedtls_printf( "3.1 Store integer value 1 in MPI\r\n" );
            		    ret = mbedtls_mpi_lset( &ctx_cli.Qp.Z, 1 );
            		    if( ret != 0 )
            		    {
            		        mbedtls_printf( " failed\n  ! mbedtls_mpi_lset returned %d\n", ret );
            		        goto exit;
            		    }

            		    mbedtls_printf( "3.2 Import public key of mobile phone, little endian\r\n" );
            		    ret = mbedtls_mpi_read_binary_le( &ctx_cli.Qp.X, dataReceive, 32 );
            		    if( ret != 0 )
            		    {
            		        mbedtls_printf( " failed\n  ! mbedtls_mpi_read_binary returned %d\n", ret );
            		        goto exit;
            		    }

            		    mbedtls_printf( "3.3 Compute shared secret\r\n" );
            		    ret = mbedtls_ecdh_compute_shared( &ctx_cli.grp, &ctx_cli.z,
            		                                       &ctx_cli.Qp, &ctx_cli.d,
            		                                       mbedtls_ctr_drbg_random, &ctr_drbg );
            		    if( ret != 0 )
            		    {
            		        mbedtls_printf( " failed\n  ! mbedtls_ecdh_compute_shared returned %d\n", ret );
            		        goto exit;
            		    }

            		    mbedtls_printf( "3.3 Export shared secret from MPI, little endian\r\n" );
						ret = mbedtls_mpi_write_binary_le( &ctx_cli.z, secret, 32 );
						if( ret != 0 )
						{
							mbedtls_printf( " failed\n  ! mbedtls_mpi_write_binary returned %d\n", ret );
						}

					PRINTF("The Secret key calculated is:\r\n");
					sendMessageToUpperComputer(32, secret, 2);

					PRINTF("Send the public key of LPC55S69 to mobile phone\r\n");
            	    sendmessage =  (uint8_t *)pvPortMalloc(40) ;
            	    sendMessgeToAndroid(sendmessage,1,32,cli_to_srv);
            	    USART_RTOS_Send(&bluetooth_handle, (uint8_t *)sendmessage, 40);
            	    vPortFree(sendmessage);

exit:
            	        mbedtls_ecdh_free( &ctx_srv );
            	        mbedtls_ecdh_free( &ctx_cli );
            	        mbedtls_ctr_drbg_free( &ctr_drbg );
            	        mbedtls_entropy_free( &entropy );

            	}
            	else if(functionCodeValue == 20)
				{
        		    PRINTF("\r\nFunctionCodeValue is %d\r\n",functionCodeValue);
        			PRINTF("Test the AES CBC function:\r\n");
        			PRINTF("The cipher Text from mobile phone is:\r\n");
        			sendMessageToUpperComputer(16, dataReceive, 2);
        			PRINTF("1. Mbedtls aes init\r\n");
					mbedtls_aes_init(&ctx_aes);
					PRINTF("2. Set the AES secret key\r\n");
					mbedtls_aes_setkey_dec(&ctx_aes, secret,256);
					recoverIvTempValue();
					PRINTF("3. Start to decrypt the cipher Text\r\n");
					mbedtls_aes_crypt_cbc(&ctx_aes,MBEDTLS_AES_DECRYPT,16,iv_temp,dataReceive,dec_plain);
					PRINTF("4. The plainText is：");
					sendMessageToUpperComputer(16, dec_plain, 1);

					PRINTF("Next test AES Function of the mobile phone\r\n");
					PRINTF("1. Local plainText is\r\n");
					sendMessageToUpperComputer(16, plain1, 1);
					recoverIvTempValue();
					PRINTF("2. Set the encryption secrect key\r\n");
					mbedtls_aes_setkey_enc(&ctx_aes,secret,256);
					PRINTF("3. Start AES CBC encryption\r\n");
					mbedtls_aes_crypt_cbc( &ctx_aes,MBEDTLS_AES_ENCRYPT,16,iv_temp,plain1,cipher);
					PRINTF("4. The cipherText is:\r\n");
					sendMessageToUpperComputer(16, cipher, 2);
					PRINTF("5. Send cipherText to android mobile phone\r\n");
					sendMessgeToAndroid(sendmessage,20,16,cipher);
					mbedtls_aes_free(&ctx_aes);
					USART_RTOS_Send(&bluetooth_handle,(uint8_t *)sendmessage,24);
				}
            	else if(functionCodeValue == 30)
            	{
            		DMIDgeneration_flag = true;
            	}
            }

        }
    } while (kStatus_Success == error);

    USART_RTOS_Deinit(&bluetooth_handle);
    vTaskSuspend(NULL);
}

static void wifi_dataProcess_task(void *pvParameters)
{
    int error;
    size_t n            = 0;
    uint8_t status;
    NVIC_SetPriority(WiFi_USART_IRQn, WiFi_USART_NVIC_PRIO);
    if (0 > USART_RTOS_Init(&wifi_handle, &wifi_t_handle, &wifi_usart_config))
    {
        vTaskSuspend(NULL);
    }

    wifi_initialize_finish_flag = 1;
    do
    {
    	error = USART_RTOS_Receive(&wifi_handle, &wifi_recv_buffer, 1, &n);
        if (error == kStatus_USART_RxRingBufferOverrun)
        {
        	PRINTF("kStatus_USART_RxRingBufferOverrun\r\n");
            vTaskSuspend(NULL);
        }
    	if (n > 0)
		{
    		//PRINTF("%c",wifi_recv_buffer);
    		if(start_receive_data_flag)
    		{
				if(data_end_flag == 1)//end with '\r\n'
				{

					if((WiFi_RX_STA&(1<<15))==0)
					{
						if(WiFi_RX_STA<WiFi_MAX_RECV_LEN)
						{
							WiFi_RX_BUF[WiFi_RX_STA++]=wifi_recv_buffer;
							if(wifi_recv_buffer=='\n')
							{
								WiFi_RX_BUF[WiFi_RX_STA] = 0;
								if(strstr(WiFi_RX_BUF,"OK")||strstr(WiFi_RX_BUF,"ERROR"))
								{
									WiFi_RX_STA|=1<<15;
								}
							}
						}
						else
						{
							WiFi_RX_STA|=1<<15; //receive finished
						}
					}

				}
				else if(data_end_flag == 2)//end with '}'
				{
					if((WiFi_RX_STA&(1<<15))==0)
					{
						if(WiFi_RX_STA<WiFi_MAX_RECV_LEN)
						{
							WiFi_RX_BUF[WiFi_RX_STA++]=wifi_recv_buffer;
							if(wifi_recv_buffer=='}')
							{
								WiFi_RX_STA|=1<<15;
							}
						}
						else
						{
							WiFi_RX_STA|=1<<15; //receive finished
						}
					}
				}
    		}

		}
    } while (kStatus_Success == error);

    USART_RTOS_Deinit(&wifi_handle);
    vTaskSuspend(NULL);
}

static void wifi_dataSend_task(void *pvParameters)
{
    HDNode node;
    char *mnemonic = "snap escape shadow school illness flip hollow label melt fetch noise install";
    char * passphrase = "PASSWORD";
    uint8_t seed[64];

    char nonce[10];
    char transactionHash[65];
    char contractAddress[41];
    char *gas_price = "04e3b29200";
    char *gas_limit = "1a60c8"; //This is gas_limit that can work, in fact the int(21000)doesn't work, will get error"intrinsic gas too low"
    char *to = "e0defb92145fef3c3a945637705fafd3aa74a241";

    char *value = "";
    char *data_contract = iotDIDContractBin;
    char *data_contract_method_set_123 = "60fe47b10000000000000000000000000000000000000000000000000000000000000123";
    char data_contract_method_AddDMIDDocument[300];
    char *data_contract_method_get ="6d4ce63c";
    uint8_t didDocumentPub[65];
    uint8_t didDocumentPubStr[128+3];
    char * did;
    char * pubKeyId;
    char * diddocument;
    char * diddocument_temp = "hahaha1234567";
    char contentHash[47];
    uint8_t randomNumber[8] = {0};
    uint8_t * randomNumberStr[17];
    status_t rngStatus;
    RNG_Init(RNG); //RNG init for generate privae key

    while(1)
    {
    	if(DMIDgeneration_flag==true)
    	{
				//mnemonic = (char *)pvPortMalloc(300) ;
				//mnemonic = mnemonic_generate(128);
				//You can use above code to generate a random mnemonic, for test conveniently, send ethereum transaction needs token
				//so I use fixed mnemonic to generate a fixed privatekey and a fixed ethereum address. and recharge the token in advance
				PRINTF("mnemonic is:\r\n%s\r\n",mnemonic);
				mnemonic_to_seed(mnemonic, passphrase,seed,NULL);
				PRINTF("seed is:\r\n");
				sendMessageToUpperComputer(64, seed, 2);

				//init m
				hdnode_from_seed(seed, 64, SECP256K1_NAME,&node);
				PRINTF("Private Key m is:\r\n");
				sendMessageToUpperComputer(32, node.private_key, 2);
				PRINTF("Chain code m is:\r\n");
				sendMessageToUpperComputer(32, node.chain_code, 2);

				//m/44'/60'/0'/0
				hdnode_private_ckd_prime(&node,44);
				hdnode_private_ckd_prime(&node,60);
				hdnode_private_ckd_prime(&node,0);
				hdnode_private_ckd(&node, 0);
				hdnode_private_ckd(&node, 0);
				PRINTF("Private Key is:\r\n");
				sendMessageToUpperComputer(32, node.private_key, 2);
				hdnode_fill_public_key(&node);
				PRINTF("Public Key is:\r\n");
				sendMessageToUpperComputer(33, node.public_key, 2);

				while(1)
				{
					if(wifi_initialize_finish_flag)
					{
						start_receive_data_flag = 1;
						esp32_send_cmd(at_quit,NULL,0);
						esp32_send_cmd(at_at,ok,1000);

						esp32_send_cmd(at_rst,ok,1000);//10s
						vTaskDelay(300);
						esp32_send_cmd(at_cwjap,ok,1000);
			#ifdef ethereum_network_infura
						esp32_send_cmd(at_cipsslcconf,ok,1000);
			#endif
						esp32_send_cmd(at_cipstart,ok,80000); //800s
						esp32_send_cmd(at_cipmode,ok,1000);
						vTaskDelay(10);//100ms
						esp32_send_cmd(at_cipsend,ok,1000);

						freshTransactionNonce(nonce);

			//			generate_transferValue_rawTransaction(
			//					node.private_key,
			//					nonce,
			//					gas_price,
			//					gas_limit,
			//					deployContract_to,
			//					value,
			//					data
			//					);

						generate_deployContract_rawTransaction(
								node.private_key,
								nonce,
								gas_price,
								gas_limit,
								value,
								data_contract
								);
						esp32_send_data(rawtransaction_http_send,at_http_end,80000);     //send depoly contract transaction
						vTaskDelay(300);
						getTransactionHashFromHTTPResponse(WiFi_RX_BUF,transactionHash);  // parse httpresponse to get transactionhash
						sprintf(rawtransaction_http_send,eth_getTransactionReceipt,transactionHash);
						esp32_send_data(rawtransaction_http_send,at_http_end,80000);     //inquire the transaction receipt
						vTaskDelay(300);
						getContractAddressFromHTTPResponse(WiFi_RX_BUF,contractAddress);  //parse httpresponse to get contract address
						PRINTF("\r\nContractAddress is%s\r\n",contractAddress);

						/*****Next, we need to generate the DID Document using cjson********/
						ecdsa_get_public_key65(&secp256k1, node.private_key, didDocumentPub);
						int8_to_char(didDocumentPub+1, 64,  didDocumentPubStr);


						did = (char *)pvPortMalloc(100);
						memset(did,0,100);
						strcpy(did,"did:example:0x");
						strcat(did,contractAddress);

						pubKeyId = (char *)pvPortMalloc(100);
						memset(pubKeyId,0,100);
						strcpy(pubKeyId,did);
						strcat(pubKeyId,"#keys-1");


						diddocument = pvPortMalloc(1000);
						generateDIDDocument(did, pubKeyId, didDocumentPubStr,diddocument);
						PRINTF("diddocument is %s\r\n",diddocument);
//						writeDIDandDIDDocumentToSD(did, diddocument); //store to SD


						//quit serial mode
						esp32_send_cmd(at_quit,NULL,0);
						vTaskDelay(100);
						esp32_send_cmd(at_cipclose,NULL,0);
						esp32_send_cmd(at_at,ok,1000);

						//connect to ipfs gateway
						esp32_send_cmd(at_cipstart_ipfs,ok,80000);
						vTaskDelay(10);//100ms
						esp32_send_cmd(at_cipsend,ok,1000);

						//generate a random number used for the boundary of ipfsAddHttpSend
						rngStatus = RNG_GetRandomData(RNG, randomNumber, 8);
						int8_to_char(randomNumber, 8,  randomNumberStr);
						randomNumberStr[16]=0;

						if (rngStatus != kStatus_Success)
						{
							return rngStatus;
						}


						//add diddocument to ipfs
						generate_ipfsAddHttpSend(diddocument,randomNumberStr);
						//vPortFree(diddocument);

						PRINTF("rawtransaction_http_send is%s",rawtransaction_http_send);
						vTaskDelay(300);
						esp32_send_data(rawtransaction_http_send,at_http_end,80000);

						//parse to get didDocumenthash
						getIPFSAddContentHash(WiFi_RX_BUF, contentHash);

						PRINTF("contentHash is%s",contentHash);

						//trigger contract method
						//first, we need to generate the transaction data

						generate_contractMethodData(data_contract_method_AddDMIDDocument,contentHash);
						PRINTF("data_contract_method_AddDMIDDocument is %s",data_contract_method_AddDMIDDocument);




						//connect to ethereum again
						//quit serial mode
						esp32_send_cmd(at_quit,NULL,0);
						vTaskDelay(200);
						esp32_send_cmd(at_cipclose,NULL,0);
						vTaskDelay(300);//3s
						esp32_send_cmd(at_at,ok,1000);
						vTaskDelay(300);//3s

						esp32_send_cmd(at_cipstart,ok,80000);
						vTaskDelay(300);//3s
						esp32_send_cmd(at_cipsend,ok,1000);
						vTaskDelay(300);


						freshTransactionNonce(nonce);

						generate_transferValue_rawTransaction(
								node.private_key,
								nonce,
								gas_price,
								gas_limit,
								contractAddress,
								value,
								data_contract_method_AddDMIDDocument
								);
						esp32_send_data(rawtransaction_http_send,at_http_end,80000);
						vTaskDelay(300);//1s


			//			freshTransactionNonce(nonce);
			//
			//			esp32_send_data(rawtransaction_http_send,at_http_end,80000);


						esp32_send_data(eth_blockNumber,at_http_end,80000);



			//			esp32_send_cmd(at_quit,NULL,0);
			//			esp32_send_cmd(at_at,ok,1000);

						vTaskSuspend(NULL);
					}
					else
					{
						vTaskDelay(10);
					}
				}
    	}
    	else
    	{
    		vTaskDelay(300);//1s
    	}
    }
}


//
void generateDIDDocument(char * did, char * pubKeyId, char * pubKeyHex,char * out) {

	cJSON * didDocument, * publicKey, * publicKey_list, *authentication, *authentication_list;
	char * out1;

	didDocument = cJSON_CreateObject();   //generate transaction object
	cJSON_AddStringToObject(didDocument, "@context", "https://w3c.org/did/v1");
	cJSON_AddStringToObject(didDocument, "created", "2019-06-1T18:00:00Z");
	cJSON_AddStringToObject(didDocument, "id", did);

	cJSON_AddItemToObject(didDocument,"publicKey", publicKey = cJSON_CreateArray());
	cJSON_AddItemToArray(publicKey, publicKey_list = cJSON_CreateObject());
	cJSON_AddStringToObject(publicKey_list,"id",pubKeyId);
	cJSON_AddStringToObject(publicKey_list,"type","Secp256k1VerificationKey2019");
	cJSON_AddStringToObject(publicKey_list,"controller",did);
	cJSON_AddStringToObject(publicKey_list,"publicKeyHex",pubKeyHex);

	cJSON_AddItemToObject(didDocument,"authentication", authentication = cJSON_CreateArray());
	cJSON_AddItemToArray(authentication, authentication_list = cJSON_CreateObject());
	cJSON_AddStringToObject(authentication_list,"type","Secp256k1VerificationKey2019");
	cJSON_AddStringToObject(authentication_list,"publicKey",pubKeyId);

	out1 = cJSON_PrintUnformatted(didDocument);   //将json形式打印成正常字符串形式

//	PRINTF("DID Document:%s",out1);
	memcpy(out,out1,strlen(out1)+1);
// 释放内存
	cJSON_Delete(didDocument);
//    free(out);
}

/*
 * when call this function, please ensure your transaction has been mined.
 * */
void freshTransactionNonce(char * nonce)
{
	esp32_send_data(eth_getTransactionCount,at_http_end,80000);  //inquire the transaction count
	PRINTF("Have received");
	getNonceFromHTTPResponse(WiFi_RX_BUF,nonce);
	PRINTF("Nonce is%s",nonce);
	vTaskDelay(100);//1s
}

void sendMessgeToAndroid(uint8_t * temp,int functioncode,int dataLength,uint8_t* data)
{
    int i;
    temp[0]=0x07;
    temp[1]=0x41;
    temp[2]=(uint8_t)(functioncode/256);
    temp[3]=(uint8_t)(functioncode%256);
    temp[4]=(uint8_t)(dataLength/256);
    temp[5]=(uint8_t)(dataLength%256);
    for(i=0;i<dataLength;i++)
    {
        temp[6+i]=data[i];
    }
    temp[6+dataLength]=0x0A;
    temp[7+dataLength]=0x0D;
    return;
}




serialDataProcess_return serialDataProcess(uint8_t * receive_char)
{
	uint8_t data = *receive_char;
	static current_state state=none;
	static int i=0;

	switch(state)
	{
		case none:{
			if(data == 0x07)
			{
				state = head_state1;
				return data_receving;
			}
			else
			{
				return data_error;
			}

		}break;
		case head_state1:{
			if(data == 0x41)
			{
				state = function_state1;
				return data_receving;
			}
			else
			{
				state = none;
				return data_error;
			}
		}break;
		case function_state1:{
			functionCode[0]=data;
			state = function_state2;
			return data_receving;
		}break;
		case function_state2:{
			functionCode[1]=data;
			functionCodeValue = functionCode[0]*256+functionCode[1];
			state = date_length1;
			return data_receving;
		}break;
		case date_length1:{
			dataLength[0]=data;
			state = data_length2;
			return data_receving;
		}break;
		case data_length2:{
			dataLength[1]=data;
			length = dataLength[0]*256 + dataLength[1];
			state = data_receive;
			return data_receving;
		}break;
		case data_receive:{
			dataReceive[i] = data;
			i++;
			if(i==length)
			{
				i=0;
				state = end1;
			}
			return data_receving;
		}break;
		case end1:{
			if(data == 0x0a)
			{
				state = end2;
				return 	data_receving;
			}
			else
			{
				state = none;
				return 	data_error;
			}
		}break;
		case end2:{
			if(data == 0x0D)
			{
				state = none;
				return data_success;
			}
			else
			{
				return data_error;
			}
		}break;
	}
}

void sendMessageToUpperComputer(int length, uint8_t * data, uint8_t type)//type 1, send char, type2, send hex
{
	int i;
	if(type == 1)
	{
		for(i=0;i<length;i++)
		{
			PRINTF("%c",data[i]);
		}
		PRINTF("\r\n");
	}else if(type == 2)
	{
		PRINTF("0x");
		for(i=0;i<length;i++)
		{
			PRINTF("%02x",data[i]);
		}
		PRINTF("\r\n");
	}
}

void recoverIvTempValue()// mbedtls will change the iv array input
{
	int i;
	for(i=0;i<16;i++)
	{
		iv_temp[i]=iv[i];
	}
}

static void SDCARD_DetectCallBack(bool isInserted, void *userData)
{
    s_cardInsertStatus = isInserted;
    xSemaphoreGiveFromISR(s_CardDetectSemaphore, NULL);
}

static void CardDetectTask(void *pvParameters)
{
    s_fileAccessSemaphore = xSemaphoreCreateBinary();
    s_CardDetectSemaphore = xSemaphoreCreateBinary();

    g_sd.host.base           = SD_HOST_BASEADDR;
    g_sd.host.sourceClock_Hz = SD_HOST_CLK_FREQ;
    g_sd.usrParam.cd         = &s_sdCardDetect;
#if defined DEMO_SDCARD_POWER_CTRL_FUNCTION_EXIST
    g_sd.usrParam.pwr = &s_sdCardPwrCtrl;
#endif
#if defined DEMO_SDCARD_SWITCH_VOLTAGE_FUNCTION_EXIST
    g_sd.usrParam.cardVoltage = &s_sdCardVoltageSwitch;
#endif
    /* SD host init function */
    if (SD_HostInit(&g_sd) == kStatus_Success)
    {
        while (true)
        {
            /* take card detect semaphore */
            if (xSemaphoreTake(s_CardDetectSemaphore, portMAX_DELAY) == pdTRUE)
            {
                if (s_cardInserted != s_cardInsertStatus)
                {
                    s_cardInserted = s_cardInsertStatus;

                    /* power off card */
                    SD_PowerOffCard(g_sd.host.base, g_sd.usrParam.pwr);

                    if (s_cardInserted)
                    {
                        PRINTF("\r\nCard inserted.\r\n");
                        /* power on the card */
                        SD_PowerOnCard(g_sd.host.base, g_sd.usrParam.pwr);
                        /* make file system */
                        if (DEMO_MakeFileSystem() != kStatus_Success)
                        {
                            continue;
                        }
                        xSemaphoreGive(s_fileAccessSemaphore);
                        s_taskSleepTicks = DEMO_TASK_GET_SEM_BLOCK_TICKS;
                    }
                }

                if (!s_cardInserted)
                {
                    PRINTF("\r\nPlease insert a card into board.\r\n");
                }
            }
        }
    }
    else
    {
        PRINTF("\r\nSD host init fail\r\n");
    }

    vTaskSuspend(NULL);
}

static status_t DEMO_MakeFileSystem(void)
{
    FRESULT error;
    const TCHAR driverNumberBuffer[3U] = {SDDISK + '0', ':', '/'};
    BYTE work[FF_MAX_SS];

    if (f_mount(&g_fileSystem, driverNumberBuffer, 0U))
    {
        PRINTF("Mount volume failed.\r\n");
        return kStatus_Fail;
    }

#if (FF_FS_RPATH >= 2U)
    error = f_chdrive((char const *)&driverNumberBuffer[0U]);
    if (error)
    {
        PRINTF("Change drive failed.\r\n");
        return kStatus_Fail;
    }
#endif

//#if FF_USE_MKFS
//    PRINTF("\r\nMake file system......The time may be long if the card capacity is big.\r\n");
//    if (f_mkfs(driverNumberBuffer, FM_ANY, 0U, work, sizeof work))
//    {
//        PRINTF("Make file system failed.\r\n");
//        return kStatus_Fail;
//    }
//#endif /* FF_USE_MKFS */

//    PRINTF("\r\nCreate directory......\r\n");
//    error = f_mkdir(_T("/dir_3"));
//    if (error)
//    {
//        if (error == FR_EXIST)
//        {
//            PRINTF("Directory exists.\r\n");
//        }
//        else
//        {
//            PRINTF("Make directory failed.\r\n");
//            return kStatus_Fail;
//        }
//    }

    return kStatus_Success;
}

void writeDIDandDIDDocumentToSD(char * DMIDcontent, char * DMIDDocumentcontent)
{
    UINT bytesWritten   = 0U;
    uint32_t writeTimes = 1U;
    FRESULT error;
    FRESULT frs;

	char * DMIDdir = "/DMID%d";
	char * DMIDstr = "/DMID%d/DMIDStr.txt";
	char * DMIDDocument = "/DMID%d/DMIDDocument.txt";
	char PATH[30];
    int i;
    if (xSemaphoreTake(s_fileAccessSemaphore, s_taskSleepTicks) == pdTRUE)
    {
			i = currentDMIDNum();
			sprintf(PATH,DMIDdir,i);
			PRINTF("\r\nCreate directory %s......\r\n",PATH);
			error = f_mkdir(_T(PATH));
			if (error)
			{
				if (error == FR_EXIST)
				{
					PRINTF("Directory exists.\r\n");
				}
				else
				{
					PRINTF("Make directory failed.\r\n");
					return kStatus_Fail;
				}
			}


			//DMIDstr write
			sprintf(PATH,DMIDstr,i);
			error = f_open(&g_fileObject, _T(PATH), FA_WRITE);
			if (error)
			{
				if (error == FR_EXIST)
				{
					PRINTF("File exists.\r\n");
				}
				/* if file not exist, creat a new file */
				else if (error == FR_NO_FILE)
				{
					if (f_open(&g_fileObject, _T(PATH), (FA_WRITE | FA_CREATE_NEW)) != FR_OK)
					{
						PRINTF("Create file failed.\r\n");
					}
				}
				else
				{
					PRINTF("Open file failed.\r\n");
				}
			}
			/* write append */
			if (f_lseek(&g_fileObject, g_fileObject.obj.objsize) != FR_OK)
			{
				PRINTF("lseek file failed.\r\n");
			}
			error = f_write(&g_fileObject, DMIDcontent, strlen(DMIDcontent), &bytesWritten);
			if ((error) || (bytesWritten != strlen(DMIDcontent)))
			{
				PRINTF("Write file failed.\r\n");
			}
			f_close(&g_fileObject);


			//DID document write

			sprintf(PATH,DMIDDocument,i);
			error = f_open(&g_fileObject, _T(PATH), FA_WRITE);
			if (error)
			{
				if (error == FR_EXIST)
				{
					PRINTF("File exists.\r\n");
				}
				/* if file not exist, creat a new file */
				else if (error == FR_NO_FILE)
				{
					if (f_open(&g_fileObject, _T(PATH), (FA_WRITE | FA_CREATE_NEW)) != FR_OK)
					{
						PRINTF("Create file failed.\r\n");
					}
				}
				else
				{
					PRINTF("Open file failed.\r\n");
				}
			}
			/* write append */
			if (f_lseek(&g_fileObject, g_fileObject.obj.objsize) != FR_OK)
			{
				PRINTF("lseek file failed.\r\n");
			}
			error = f_write(&g_fileObject, DMIDDocumentcontent, strlen(DMIDDocumentcontent), &bytesWritten);
			if ((error) || (bytesWritten != strlen(DMIDDocumentcontent)))
			{
				PRINTF("Write file failed.\r\n");
			}
			f_close(&g_fileObject);
			xSemaphoreGive(s_fileAccessSemaphore);
    }
}

//calculate the DMID number in SD card.

int currentDMIDNum()
{
	int i;
	char * DMID = "DMID%d";
	char DMIDWithNum[10];
    FRESULT frs;
	for(i=0;i<1000;i++)
	{
		sprintf(DMIDWithNum,DMID,i);
		frs = f_stat(_T(DMIDWithNum), NULL);
		PRINTF("frs is %d",frs);
		if(frs != FR_OK)
		{
			return i;
		}
	}
	return 0;
}
