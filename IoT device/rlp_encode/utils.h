/*
 * Copyright (c) 2016-2018 . All Rights Reserved.
 */

#ifndef _RLP_UTILS_H
#define _RLP_UTILS_H

#include <stdint.h>

typedef uint32_t pb_size_t;
typedef uint_least8_t pb_byte_t;

int size_of_bytes(int str_len);
uint8_t strtohex(char c);
int hex2byte_arr(char *buf, int len, uint8_t *out, int outbuf_size);
void int8_to_char(uint8_t *buffer, int len, char *out); //byte to 16 hex output
void HexToByte(const uint8_t *input,uint32_t inlen,uint8_t *output);

#endif //RLP_UTİLS_H
