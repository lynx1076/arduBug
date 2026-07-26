#ifndef PARSE_H
#define PARSE_H

#include <stdint.h>
#include <stdbool.h>

int parse_long(const char* str, long* val);
int parse_hex_byte(const char* str, uint8_t* byte);
int parse_hex_word(const char* str, uint16_t* word);
int parse_bool(const char* str, bool* val);

#endif
