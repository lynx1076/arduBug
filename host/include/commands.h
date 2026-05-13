#ifndef COMMANDS_H
#define COMMANDS_H

#include "result.h"

result cmd_quit(const char** tokens, unsigned int arg_cnt);
result cmd_scan_devices(const char** tokens, unsigned int arg_cnt);
result cmd_connect(const char** tokens, unsigned int arg_cnt);
result cmd_disconnect(const char** tokens, unsigned int arg_cnt);
result cmd_status(const char** tokens, unsigned int arg_cnt);
result cmd_ping(const char** tokens, unsigned int arg_cnt);
result cmd_version(const char** tokens, unsigned int arg_cnt);
result cmd_show_debug_logs(const char** tokens, unsigned int arg_cnt);
result cmd_write(const char** tokens, unsigned int arg_cnt);
result cmd_read(const char** tokens, unsigned int arg_cnt);
result cmd_pulse(const char** tokens, unsigned int arg_cnt);

#define COMMANDS \
  X(cmd_quit, "quit") \
  X(cmd_quit, "q") \
  X(cmd_quit, "exit") \
  X(cmd_scan_devices, "scan") \
  X(cmd_scan_devices, "s") \
  X(cmd_connect, "connect") \
  X(cmd_connect, "c") \
  X(cmd_disconnect, "disconnect") \
  X(cmd_disconnect, "d") \
  X(cmd_status, "status") \
  X(cmd_status, "s") \
  X(cmd_ping, "ping") \
  X(cmd_ping, "p") \
  X(cmd_version, "version") \
  X(cmd_version, "v") \
  X(cmd_show_debug_logs, "dbg") \
  X(cmd_write, "write") \
  X(cmd_write, "w") \
  X(cmd_read, "read") \
  X(cmd_read, "r") \
  X(cmd_pulse, "pulse") \
  X(cmd_pulse, "pl") \

#endif
