#ifndef _CLIB_H
#define _CLIB_H

#include <stdint.h>

uint8_t *str_str(uint8_t *s1, uint8_t *s2);
uint8_t *str_chr(uint8_t *s, uint8_t c);
uint32_t Strncpy(uint8_t *dest, uint8_t *src, uint32_t n);
void clear_string_buf(uint8_t *buf, uint32_t len);
int32_t Atoi(uint8_t *str);
void Itoa(int32_t i, uint8_t *string);
void flodou_to_string(double number, uint8_t *str, uint8_t int_len, uint8_t dec_len);
uint32_t find_the_most_element(uint32_t *data, uint32_t len);
uint32_t str_len(uint8_t *buf);
void hex_to_str(uint8_t *str,uint8_t hex);
uint8_t str_to_hex(uint8_t str1,uint8_t str2);
void more_hex_to_str(uint8_t *str,uint8_t *hex,uint8_t len);

#endif
