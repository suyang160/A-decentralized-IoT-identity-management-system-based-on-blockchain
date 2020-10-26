/*
 * esp32_command.c
 *
 *  Created on: 2020年3月30日
 *  Author: suyang
 */
#include "esp32_command.h"

/****************************************************************************/
/************** AT command of ESP32 and Ethereum json RPC********************/
uint8_t * at_rst = (uint8_t *)"AT+RST\r\n";
uint8_t * at_cwjap = (uint8_t *)"AT+CWJAP=\"Happy402\",\"15861517542\"\r\n";
uint8_t * at_cipmode = (uint8_t *)"AT+CIPMODE=1\r\n";
uint8_t * at_cipsend = (uint8_t *)"AT+CIPSEND\r\n";
uint8_t * at_quit = (uint8_t *)"+++";
uint8_t * at_at = (uint8_t *)"AT\r\n";
uint8_t * at_http_end = (uint8_t *)"}";

uint8_t * ok = (uint8_t *)"OK";

/****************************************************************************/


#if defined(ethereum_network_infura)
uint8_t * at_cipsslcconf = (uint8_t *)"AT+CIPSSLCCONF=1,0,0\r\n";
uint8_t * at_cipstart = (uint8_t *)"AT+CIPSTART=\"SSL\",\"rinkeby.infura.io\",443\r\n";
uint8_t * eth_blockNumber =(uint8_t *)\
"POST /v3/c44024b2f8414bbf8cd86990f3d74604 HTTP/1.1\r\n\
Host: rinkeby.infura.io:443\r\n\
User-Agent: curl/7.58.0\r\n\
Accept: */*\r\n\
Content-Type: application/json\r\n\
Content-Length: 64\r\n\
\r\n\
{\"jsonrpc\":\"2.0\",\"method\":\"eth_blockNumber\",\"params\": [],\"id\":1}";

uint8_t * eth_getTransactionCount=(uint8_t *)\
"POST /v3/c44024b2f8414bbf8cd86990f3d74604 HTTP/1.1\r\n\
Host: rinkeby.infura.io:443\r\n\
User-Agent: curl/7.58.0\r\n\
Accept: */*\r\n\
Content-Type: application/json\r\n\
Content-Length: 124\r\n\
\r\n\
{\"jsonrpc\":\"2.0\",\"method\":\"eth_getTransactionCount\",\"params\":[\"0x5d361e4fb974e4503028908588aa3d2130baabac\",\"latest\"],\"id\":1}";
#elif defined(ethereum_network_privatechain)
uint8_t * at_cipstart = (uint8_t *)"AT+CIPSTART=\"TCP\",\"192.168.31.111\",8545\r\n";
uint8_t * at_cipstart_ipfs = (uint8_t *)"AT+CIPSTART=\"TCP\",\"192.168.31.111\",7410\r\n";
uint8_t * at_cipclose = "AT+CIPCLOSE\r\n";
uint8_t * eth_blockNumber =(uint8_t *)\
"POST / HTTP/1.1\r\n\
Host: 192.168.31.111:8545\r\n\
User-Agent: curl/7.58.0\r\n\
Accept: */*\r\n\
Content-Type: application/json\r\n\
Content-Length: 64\r\n\
\r\n\
{\"jsonrpc\":\"2.0\",\"method\":\"eth_blockNumber\",\"params\": [],\"id\":1}";

uint8_t * eth_getTransactionCount=(uint8_t *)\
"POST / HTTP/1.1\r\n\
Host: 192.168.31.111:8545\r\n\
User-Agent: curl/7.58.0\r\n\
Accept: */*\r\n\
Content-Type: application/json\r\n\
Content-Length: 124\r\n\
\r\n\
{\"jsonrpc\":\"2.0\",\"method\":\"eth_getTransactionCount\",\"params\":[\"0x5d361e4fb974e4503028908588aa3d2130baabac\",\"latest\"],\"id\":1}";

uint8_t * eth_sendRawTransaction =(uint8_t *)\
"POST / HTTP/1.1\r\n\
Host: 192.168.31.111:8545\r\n\
User-Agent: curl/7.58.0\r\n\
Accept: */*\r\n\
Content-Type: application/json\r\n\
Content-Length: %u\r\n\
\r\n\
{\"jsonrpc\":\"2.0\",\"method\":\"eth_sendRawTransaction\",\"params\":[\"0x%s\"],\"id\":1}";

uint8_t * eth_getTransactionReceipt=(uint8_t *)\
"POST / HTTP/1.1\r\n\
Host: 192.168.31.111:8545\r\n\
User-Agent: curl/7.58.0\r\n\
Accept: */*\r\n\
Content-Type: application/json\r\n\
Content-Length: 141\r\n\
\r\n\
{\"jsonrpc\":\"2.0\",\"method\":\"eth_getTransactionReceipt\",\"params\":[\"0x%s\"],\"id\":1}";

uint8_t * eth_call =(uint8_t *)\
"POST / HTTP/1.1\r\n\
Host: 192.168.31.111:8545\r\n\
User-Agent: curl/7.58.0\r\n\
Accept: */*\r\n\
Content-Type: application/json\r\n\
Content-Length: %s\r\n\
\r\n\
{\"jsonrpc\":\"2.0\",\"method\":\"eth_call\",\"params\":[{\"from\": \"0x%s\", \"to\": \"0x%s\", \"data\": \"0x%s\"}, \"latest\"],\"id\":1}";

uint8_t * ipfs_add =(uint8_t *)\
"POST /api/v0/add HTTP/1.1\r\n\
Host: 192.168.31.111:7410\r\n\
User-Agent: curl/7.58.0\r\n\
Accept: */*\r\n\
Content-Length: %u\r\n\
Expect: 100-continue\r\n\
Content-Type: multipart/form-data; boundary=------------------------%s\r\n\
\r\n\
--------------------------%s\r\n\
Content-Disposition: form-data; name=\"file\"; filename=\"DIDDocument.txt\"\r\n\
Content-Type: text/plain\r\n\
\r\n\
%s\r\n\
--------------------------%s--";


#endif
