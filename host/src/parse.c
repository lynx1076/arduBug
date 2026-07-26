#include "parse.h"
#include "result.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>

int parse_long(const char* str, long* val) {
  char* endptr;

  errno = 0;
  *val = strtol(str, &endptr, 10);

  if (errno != 0) RES_RETURN(r_EPARSE, -1);
  if (str + strlen(str) != endptr) RES_RETURN(r_EPARSE, -1);

  RES_RETURN(r_ENONE, 0);
}

int parse_hex_byte(const char* str, uint8_t* byte) {
	char* endptr;

	if (str == NULL || byte == NULL)
		RES_RETURN(r_EARGS, -1);

	if (strncasecmp(str, "0x", 2) == 0)
		str += 2;

	if (*str == '\0')
		RES_RETURN(r_EPARSE, -1);

	if (strlen(str) > 2)
		RES_RETURN(r_EPARSE, -1);

	errno = 0;

	long value = strtol(str, &endptr, 16);

	if (errno != 0)
		RES_RETURN(r_EPARSE, -1);

	if (*endptr != '\0')
		RES_RETURN(r_EPARSE, -1);

	if (value < 0 || value > 0xFF)
		RES_RETURN(r_EPARSE, -1);

	*byte = (uint8_t)value;

	RES_RETURN(r_ENONE, 0);
}

int parse_hex_word(const char* str, uint16_t* word) {
	char* endptr;

	if (str == NULL || word == NULL)
		RES_RETURN(r_EARGS, -1);

	if (strncasecmp(str, "0x", 2) == 0)
		str += 2;

	if (*str == '\0')
		RES_RETURN(r_EPARSE, -1);

	if (strlen(str) > 4)
		RES_RETURN(r_EPARSE, -1);

	errno = 0;

	long value = strtol(str, &endptr, 16);

	if (errno != 0)
		RES_RETURN(r_EPARSE, -1);

	if (*endptr != '\0')
		RES_RETURN(r_EPARSE, -1);

	if (value < 0 || value > 0xFFFF)
		RES_RETURN(r_EPARSE, -1);

	*word = (uint16_t)value;

	RES_RETURN(r_ENONE, 0);
}

int parse_bool(const char* str, bool* val) {
  if (strcmp(str, "true") == 0) *val = true;
  else if (strcmp(str, "1") == 0) *val = true;
  else if (strcmp(str, "false") == 0) *val = false;
  else if (strcmp(str, "0") == 0) *val = false;
  else {
    RES_RETURN(r_EPARSE, -1);
  }

  RES_RETURN(r_ENONE, 0);
}

