#include "generateRawTransaction.h"

uint8_t rawtransaction_http_send[10000];
static char rawTx[10000];
uint64_t raw_tx_bytes[700];           //250*8=2000bytesThe length may be adjusted, if you want to deploy a smart contract, maybe not enough
//char output[500];

EthereumSignTx tx;
EthereumSig signature;


const char *r_initial = "00"; //initial is none, RLP encode is 0x80
const char *s_initial = "00";
char r[32] = {0};
char s[32] = {0};
uint8_t v;
static uint8_t rawtransaction_first_hash[32];
uint8_t sig[64];
uint32_t rawtransaction_http_length;
char *strx1=0;
char *strx2=0;

/*
 * nonce: if nonce="00",that equals nonce = "". If you input former, you need to call wallet_encode_element(...,true);
 * If you input latter, you need to call wallet_encode_element(...,false);
 *
 *
 * */
void generate_transferValue_rawTransaction(
		uint8_t *privateKey,
		uint8_t * nonce,
		uint8_t * gas_price,
		uint8_t *gas_limit,
		uint8_t *to,
		uint8_t *value,
		uint8_t *data
		)
{
	int i;
	int length;
	tx.nonce.size = size_of_bytes(strlen(nonce));
	hex2byte_arr(nonce, strlen(nonce), tx.nonce.bytes, tx.nonce.size);

	tx.gas_price.size = size_of_bytes(strlen(gas_price));
	hex2byte_arr(gas_price, strlen(gas_price), tx.gas_price.bytes, tx.gas_price.size);

	tx.gas_limit.size = size_of_bytes(strlen(gas_limit));
	hex2byte_arr(gas_limit, strlen(gas_limit), tx.gas_limit.bytes, tx.gas_limit.size);

	tx.to.size = size_of_bytes(strlen(to));
	hex2byte_arr(to, strlen(to), tx.to.bytes, tx.to.size);

	tx.value.size = size_of_bytes(strlen(value));
	hex2byte_arr(value, strlen(value), tx.value.bytes, tx.value.size);

	tx.data_initial_chunk.size = size_of_bytes(strlen(data));
	hex2byte_arr(data, strlen(data), tx.data_initial_chunk.bytes,tx.data_initial_chunk.size);

	signature.signature_v = ethereum_chain_id;

	signature.signature_r.size = size_of_bytes(strlen(r_initial));
	hex2byte_arr(r_initial, strlen(r_initial), signature.signature_r.bytes, signature.signature_r.size);

	signature.signature_s.size = size_of_bytes(strlen(s_initial));
	hex2byte_arr(s_initial, strlen(s_initial), signature.signature_s.bytes, signature.signature_s.size);

	length = wallet_ethereum_assemble_tx(&tx, &signature, raw_tx_bytes);
	int8_to_char((uint8_t *) raw_tx_bytes, length, rawTx);
//	PRINTF("First raw transaction: %s\n", rawTx);

//	HexToByte(rawTx,strlen(rawTx),output);  //Keccak funtion needs to use byte other than string.
	                                        //For example, byte 0x11, string "11"='1','1'

//	PRINTF("\r\n****************Test if transfer is right************\r\n");
//	temp = strlen(rawTx)/2;
//	for(i=0;i<temp;i++)
//	{
//		PRINTF("%02x",output[i]);
//	}
//	PRINTF("\r\n*****************************************************\r\n");

	keccak_256((const unsigned char*) raw_tx_bytes,length, rawtransaction_first_hash);

	ecdsa_sign_digest(&secp256k1, (const uint8_t*) privateKey,
			(const uint8_t*) rawtransaction_first_hash, (uint8_t*) sig, &v,
			NULL);

	v = v + 2 * ethereum_chain_id + 35; //This is EIP155Signer
	//v = v + 27; // This is HomeSteadSigner

	PRINTF("The value of v is%u\r\n",v);

	signature.signature_v = v;

//	PRINTF("sig is:\r\n");
//	for(i=0;i<64;i++)
//	{
//		PRINTF("%02x",sig[i]);
//	}

	signature.signature_r.size = 32;
	for(i=0;i<32;i++)
	{
		signature.signature_r.bytes[i] = sig[i];
	}
	signature.signature_s.size = 32;
	for(i=0;i<32;i++)
	{
		signature.signature_s.bytes[i] = sig[i+32];
	}

	length = wallet_ethereum_assemble_tx(&tx, &signature, raw_tx_bytes);
	int8_to_char((uint8_t *) raw_tx_bytes, length, rawTx);
//	PRINTF("Second raw transaction: %s\n", rawTx);

	sprintf(rawtransaction_http_send,eth_sendRawTransaction,0,rawTx); //generate the http string,format
	strx1=strchr((const char*)rawtransaction_http_send,'{');
	strx2=strchr((const char*)rawtransaction_http_send,'}');
	rawtransaction_http_length = strx2-strx1+1; //calculate Content-Length;
	sprintf(rawtransaction_http_send,eth_sendRawTransaction,rawtransaction_http_length,rawTx);
	PRINTF("HTTP rawtransaction is %s",rawtransaction_http_send);
	return 0;
}


void generate_deployContract_rawTransaction(
		uint8_t *privateKey,
		uint8_t * nonce,
		uint8_t * gas_price,
		uint8_t *gas_limit,
		uint8_t *value,
		uint8_t *data
		)
{
	int i;
	int length;
	uint8_t *to = "";
	tx.nonce.size = size_of_bytes(strlen(nonce));
	hex2byte_arr(nonce, strlen(nonce), tx.nonce.bytes, tx.nonce.size);

	tx.gas_price.size = size_of_bytes(strlen(gas_price));
	hex2byte_arr(gas_price, strlen(gas_price), tx.gas_price.bytes, tx.gas_price.size);

	tx.gas_limit.size = size_of_bytes(strlen(gas_limit));
	hex2byte_arr(gas_limit, strlen(gas_limit), tx.gas_limit.bytes, tx.gas_limit.size);

	tx.to.size = size_of_bytes(strlen(to));
	hex2byte_arr(to, strlen(to), tx.to.bytes, tx.to.size);

	tx.value.size = size_of_bytes(strlen(value));
	hex2byte_arr(value, strlen(value), tx.value.bytes, tx.value.size);

	tx.data_initial_chunk.size = size_of_bytes(strlen(data));
	hex2byte_arr(data, strlen(data), tx.data_initial_chunk.bytes,tx.data_initial_chunk.size);

	signature.signature_v = ethereum_chain_id;

	signature.signature_r.size = size_of_bytes(strlen(r_initial));
	hex2byte_arr(r_initial, strlen(r_initial), signature.signature_r.bytes, signature.signature_r.size);

	signature.signature_s.size = size_of_bytes(strlen(s_initial));
	hex2byte_arr(s_initial, strlen(s_initial), signature.signature_s.bytes, signature.signature_s.size);

	length = wallet_ethereum_assemble_tx(&tx, &signature, raw_tx_bytes);
	int8_to_char((uint8_t *) raw_tx_bytes, length, rawTx);
//	PRINTF("First raw transaction: %s\n", rawTx);

//	HexToByte(rawTx,strlen(rawTx),output);  //Keccak funtion needs to use byte other than string.
	                                        //For example, byte 0x11, string "11"='1','1'

//	PRINTF("\r\n****************Test if transfer is right************\r\n");
//	temp = strlen(rawTx)/2;
//	for(i=0;i<temp;i++)
//	{
//		PRINTF("%02x",output[i]);
//	}
//	PRINTF("\r\n*****************************************************\r\n");

	keccak_256((const unsigned char*) raw_tx_bytes,length, rawtransaction_first_hash);

	ecdsa_sign_digest(&secp256k1, (const uint8_t*) privateKey,
			(const uint8_t*) rawtransaction_first_hash, (uint8_t*) sig, &v,
			NULL);

	v = v + 2 * ethereum_chain_id + 35; //This is EIP155Signer
	//v = v + 27; // This is HomeSteadSigner

	PRINTF("The value of v is%u\r\n",v);

	signature.signature_v = v;

//	PRINTF("sig is:\r\n");
//	for(i=0;i<64;i++)
//	{
//		PRINTF("%02x",sig[i]);
//	}

	signature.signature_r.size = 32;
	for(i=0;i<32;i++)
	{
		signature.signature_r.bytes[i] = sig[i];
	}
	signature.signature_s.size = 32;
	for(i=0;i<32;i++)
	{
		signature.signature_s.bytes[i] = sig[i+32];
	}

	length = wallet_ethereum_assemble_tx(&tx, &signature, raw_tx_bytes);
	int8_to_char((uint8_t *) raw_tx_bytes, length, rawTx);
//	PRINTF("Second raw transaction: %s\n", rawTx);

	sprintf(rawtransaction_http_send,eth_sendRawTransaction,0,rawTx); //generate the http string,format
	strx1=strchr((const char*)rawtransaction_http_send,'{');
	strx2=strchr((const char*)rawtransaction_http_send,'}');
	rawtransaction_http_length = strx2-strx1+1; //calculate Content-Length;
	sprintf(rawtransaction_http_send,eth_sendRawTransaction,rawtransaction_http_length,rawTx);
	PRINTF("HTTP rawtransaction is %s",rawtransaction_http_send);
	return 0;
}

int wallet_ethereum_assemble_tx(EthereumSignTx *msg, EthereumSig *tx, uint64_t *rawTx) {
    EncodeEthereumSignTx new_msg;
    EncodeEthereumTxRequest new_tx;
    memset(&new_msg, 0, sizeof(new_msg));
    memset(&new_tx, 0, sizeof(new_tx));

    wallet_encode_element(msg->nonce.bytes, msg->nonce.size,
                          new_msg.nonce.bytes, &(new_msg.nonce.size), false);
    wallet_encode_element(msg->gas_price.bytes, msg->gas_price.size,
                          new_msg.gas_price.bytes, &(new_msg.gas_price.size), false);
    wallet_encode_element(msg->gas_limit.bytes, msg->gas_limit.size,
                          new_msg.gas_limit.bytes, &(new_msg.gas_limit.size), false);
    wallet_encode_element(msg->to.bytes, msg->to.size, new_msg.to.bytes,
                          &(new_msg.to.size), false);
    wallet_encode_element(msg->value.bytes, msg->value.size,
                          new_msg.value.bytes, &(new_msg.value.size), false);
    wallet_encode_element(msg->data_initial_chunk.bytes,
                          msg->data_initial_chunk.size, new_msg.data_initial_chunk.bytes,
                          &(new_msg.data_initial_chunk.size), false);

    wallet_encode_int(tx->signature_v, &(new_tx.signature_v));
    wallet_encode_element(tx->signature_r.bytes, tx->signature_r.size,
                          new_tx.signature_r.bytes, &(new_tx.signature_r.size), true);//R and S needs to remove the first zero
    wallet_encode_element(tx->signature_s.bytes, tx->signature_s.size,
                          new_tx.signature_s.bytes, &(new_tx.signature_s.size), true);

    int length = wallet_encode_list(&new_msg, &new_tx, rawTx);
    return length;
}


void getNonceFromHTTPResponse(uint8_t *HTTPResponse, uint8_t * nonce)
{
//HTTP/1.1 200 OK
//Content-Type: application/json
//Date: Mon, 30 Mar 2020 02:34:21 GMT
//Content-Length: 40
//
//{"jsonrpc":"2.0","id":1,"result":"0x1"}
//a http response example
	uint8_t * pointer;
	uint8_t length = 0;
	//uint8_t * nonce [11]; //the nonce must lower than 5 byte; 0xFFFFFFFFFF is max
	pointer = strstr(HTTPResponse,"result");
	pointer = pointer + 11;
	while((* pointer)!='"') //
	{
		nonce [length] = * pointer;
		length ++;
		pointer ++;
	}
	nonce [length] = 0;
}

//transactionHash at least 64+1 bytes.
void getTransactionHashFromHTTPResponse(uint8_t *HTTPResponse, uint8_t * transactionHash)
{
//HTTP/1.1 200 OK
//Content-Type: application/json
//Date: Tue, 31 Mar 2020 02:51:48 GMT
//Content-Length: 103
//
//{"jsonrpc":"2.0","id":1,"result":"0xf3e3ad8060d89e8358f944999bb2c613e2afcd200c247f10151c7ca3540acc0a"}
	uint8_t * pointer;
	uint8_t length = 0;
	//uint8_t * nonce [11]; //the nonce must lower than 5 byte; 0xFFFFFFFFFF is max
	pointer = strstr(HTTPResponse,"result");
	pointer = pointer + 11;
	while((* pointer)!='"') //
	{
		transactionHash [length] = * pointer;
		length ++;
		pointer ++;
	}
	transactionHash [length] = 0;

}

//contractAddress at least 40+1 bytes
void getContractAddressFromHTTPResponse(uint8_t *HTTPResponse, uint8_t * contractAddress)
{
//HTTP/1.1 200 OK
//Content-Type: application/json
//Date: Tue, 31 Mar 2020 07:49:30 GMT
//Content-Length: 979
//
//{"jsonrpc":"2.0","id":1,"result":{"blockHash":"0xde17cd8b2a891c11e8669b1132c3fb11feb483602762aa4b58611c7601ea8646","blockNumber":"0x4e02","contractAddress":"0xace62e69d93fb7ba11c4d25d2606cf58c7ee3cf8","cumulativeGasUsed":"0xdb11","from":"0x5d361e4fb974e4503028908588aa3d2130baabac","gasUsed":"0xdb11","logs":[],"logsBloom":"0x00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000","status":"0x0","to":null,"transactionHash":"0xf3e3ad8060d89e8358f944999bb2c613e2afcd200c247f10151c7ca3540acc0a","transactionIndex":"0x0"}}
//a http transaction receipt success example

//HTTP/1.1 200 OK
//Content-Type: application/json
//Date: Tue, 31 Mar 2020 07:43:40 GMT
//Content-Length: 39
//
//{"jsonrpc":"2.0","id":1,"result":null}
//a http transaction receipt failure example

	uint8_t * pointer;
	uint8_t length = 0;
	//uint8_t * nonce [11]; //the nonce must lower than 5 byte; 0xFFFFFFFFFF is max
	pointer = strstr(HTTPResponse,"contractAddress");
	pointer = pointer + 20;
	while((* pointer)!='"') //
	{
		contractAddress [length] = * pointer;
		length ++;
		pointer ++;
	}
	contractAddress [length] = 0;
}

void generate_ipfsAddHttpSend(char * diddocument,char *randomNumberStr)
{
//"POST /api/v0/add HTTP/1.1\r\n\
//Host: 192.168.31.111:7410\r\n\
//User-Agent: curl/7.58.0\r\n\
//Accept: */*\r\n\
//Content-Length: %u\r\n\
//Expect: 100-continue\r\n\
//Content-Type: multipart/form-data; boundary=------------------------fb1aa4ad8a4b3f8e\r\n\
//\r\n\
//--------------------------fb1aa4ad8a4b3f8e\r\n\
//Content-Disposition: form-data; name=\"file\"; filename=\"DIDDocument.txt\"\r\n\
//Content-Type: text/plain\r\n\
//\r\n\
//%s\r\n\
//--------------------------fb1aa4ad8a4b3f8e--"
    uint8_t * pointer;
    uint32_t ipfs_sendHttpContent_length = 0;
    uint32_t whole_length;
	sprintf(rawtransaction_http_send,ipfs_add,ipfs_sendHttpContent_length,randomNumberStr,randomNumberStr,diddocument,randomNumberStr);
	pointer = strstr(rawtransaction_http_send,"boundary");
	ipfs_sendHttpContent_length = pointer-rawtransaction_http_send;
	whole_length = strlen(rawtransaction_http_send);
	ipfs_sendHttpContent_length = whole_length-ipfs_sendHttpContent_length-53;
	sprintf(rawtransaction_http_send,ipfs_add,ipfs_sendHttpContent_length,randomNumberStr,randomNumberStr,diddocument,randomNumberStr);
}


//contentHash at least 47 bytes
void getIPFSAddContentHash(uint8_t *HTTPResponse, uint8_t * contentHash)
{

//HTTP/1.1 200 OK
//Server: nginx/1.14.0 (Ubuntu)
//Date: Wed, 01 Apr 2020 14:48:20 GMT
//Content-Type: application/json
//Transfer-Encoding: chunked
//Connection: keep-alive
//Access-Control-Allow-Headers: X-Stream-Output, X-Chunked-Output, X-Content-Length
//Access-Control-Expose-Headers: X-Stream-Output, X-Chunked-Output, X-Content-Length
//Trailer: X-Stream-Error
//Vary: Origin
//X-Chunked-Output: 1
//
//{"Name":"hello3.txt","Hash":"QmYbL6SCVesDBmcpHyX2ymw7bYA4eZwByF8nuz1qpixwWL","Size":"23"}
//ipfs response example

	uint8_t * pointer;
	uint8_t length = 0;
	//uint8_t * nonce [11]; //the nonce must lower than 5 byte; 0xFFFFFFFFFF is max
	pointer = strstr(HTTPResponse,"Hash");
	pointer = pointer + 7;
	while((* pointer)!='"') //
	{
		contentHash [length] = * pointer;
		length ++;
		pointer ++;
	}
	contentHash [length] = 0;
}

void generate_contractMethodData(char * data_contract_method_AddDMIDDocument,char *contentHash)
{
   char * methodADDDocument_hash = "3363ae2f0000000000000000000000000000000000000000000000000000000000000020000000000000000000000000000000000000000000000000000000000000002e%s000000000000000000000000000000000000";
   char * contentHash_char[93];
   int8_to_char(contentHash, 46, contentHash_char);
   contentHash_char[92] = 0;
   sprintf(data_contract_method_AddDMIDDocument,methodADDDocument_hash,contentHash_char);
}





