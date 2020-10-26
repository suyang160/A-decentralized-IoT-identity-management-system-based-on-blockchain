#ifndef _GENERATE_RAW_TRANSACTION_
#define _GENERATE_RAW_TRANSACTION_
#include <stdint.h>
#include <string.h>
#include "bip32.h"
#include "bip39.h"
#include "curves.h"

#include "utils.h"
#include "RLP.h"
#include "esp32.h"

#include "secp256k1.h"
#include "fsl_usart_freertos.h"
#include "fsl_usart.h"
#include "fsl_debug_console.h"
#include "esp32_command.h"

#define rinkeby 4
#define ethereum_chain_id rinkeby

int wallet_ethereum_assemble_tx(EthereumSignTx *msg, EthereumSig *tx, uint64_t *rawTx);
void generate_transferValue_rawTransaction(
		uint8_t *privateKey,
		uint8_t * nonce,
		uint8_t * gas_price,
		uint8_t *gas_limit,
		uint8_t *to,
		uint8_t *value,
		uint8_t *data
		);

void generate_deployContract_rawTransaction(
		uint8_t *privateKey,
		uint8_t * nonce,
		uint8_t * gas_price,
		uint8_t *gas_limit,
		uint8_t *value,
		uint8_t *data
		);

void getNonceFromHTTPResponse(uint8_t *HTTPResponse, uint8_t * nonce);

//transactionHash at least 64+1 bytes.
void getTransactionHashFromHTTPResponse(uint8_t *HTTPResponse, uint8_t * transactionHash);

//contractAddress at least 40+1 bytes
void getContractAddressFromHTTPResponse(uint8_t *HTTPResponse, uint8_t * contractAddress);

//generate ipfs http content
void generate_ipfsAddHttpSend(char * diddocument,char *randomNumberStr);

void getIPFSAddContentHash(uint8_t *HTTPResponse, uint8_t * contentHash);

void generate_contractMethodData(char * data_contract_method_AddDMIDDocument,char *contentHash);

void getIPFSAddContentHash(uint8_t *HTTPResponse, uint8_t * contentHash);

extern uint8_t rawtransaction_http_send[];

#endif

