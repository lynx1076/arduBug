#ifndef COMMANDS_H
#define COMMANDS_H

#include "result.h"

result cmd_quit(const char** tokens, int arg_cnt);
result cmd_list_devices(const char** tokens, int arg_cnt);

#define COMMANDS \
  X(cmd_list_devices, "list") \
  X(cmd_list_devices, "l") \
  X(cmd_quit, "quit") \
  X(cmd_quit, "q") \

#endif
