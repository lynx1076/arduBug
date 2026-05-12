#include "commands.h"
#include "result.h"
#include "utils.h"
#include "vector.h"
#include "serial.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Vec device_list = VEC_ZERO;

result cmd_quit(const char** tokens, int arg_cnt) {
  (void)tokens;

  if (arg_cnt != 0) {
    return r_EUSER_CMD;
  }

  printf("Quitting...");

  ser_close();
  exit(0);

  return r_ENONE;
}

result cmd_list_devices(const char** tokens, int arg_cnt) {
  (void)tokens;

  result _res;
  if (arg_cnt != 0) {
    return r_EUSER_CMD;
  }
  
  if (device_list.memory) vec_free(&device_list);

  _res = ser_scan_ports(&device_list);
  if (_res != r_ENONE) return _res;

  printf("Found %zu open ports:\n", device_list.count);
  for (size_t i = 0; i < device_list.count; i++) {
    printf("\t[%zu]: %s\n", i, (char*)vec_get(&device_list, i));
  }

  return r_ENONE;
}

result cmd_connect(const char** tokens, int arg_cnt) {
  (void)tokens;

  result _res;
  if (arg_cnt != 1) {
    return r_EUSER_CMD;
  }

  long device_index;
  _res = parse_int(tokens[1], &device_index);
  if (_res != r_ENONE) return _res;

  return r_ENONE;
}

