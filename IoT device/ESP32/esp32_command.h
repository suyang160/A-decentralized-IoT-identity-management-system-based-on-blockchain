#ifndef _ESP32_COMMAND
#define _ESP32_COMMAND
/****************************************************************************/
/*******You can just choose one network infura or privatechain**************/
//#define ethereum_network_infura
#define ethereum_network_privatechain
/****************************************************************************/
/************** AT command of ESP32 and Ethereum json RPC********************/
#include <stdint.h>
extern uint8_t * at_rst;
extern uint8_t * at_cwjap;
extern uint8_t * at_cipmode;
extern uint8_t * at_cipsend;
extern uint8_t * at_quit;
extern uint8_t * at_at;
extern uint8_t * at_http_end;

extern uint8_t * ok;

/****************************************************************************/


#if defined(ethereum_network_infura)

extern uint8_t * at_cipsslcconf;
extern uint8_t * at_cipstart;
extern uint8_t * eth_blockNumber;
extern uint8_t * eth_getTransactionCount=;

#elif defined(ethereum_network_privatechain)

extern uint8_t * at_cipstart;
extern uint8_t * at_cipstart_ipfs;
extern uint8_t * at_cipclose;
extern uint8_t * eth_blockNumber ;
extern uint8_t * eth_getTransactionCount;
extern uint8_t * eth_sendRawTransaction;
extern uint8_t * eth_getTransactionReceipt;
extern uint8_t * eth_call;
extern uint8_t * ipfs_add;

#endif

#endif
