#ifndef COMMANDS_H
#define COMMANDS_H

#include "result.h"
#include "debugger.h"

result cmd_quit(const char** tokens, unsigned int arg_cnt);
result cmd_scan_devices(const char** tokens, unsigned int arg_cnt);
result cmd_connect(const char** tokens, unsigned int arg_cnt);
result cmd_disconnect(const char** tokens, unsigned int arg_cnt);
result cmd_status(const char** tokens, unsigned int arg_cnt);
result cmd_ping(const char** tokens, unsigned int arg_cnt);
result cmd_version(const char** tokens, unsigned int arg_cnt);
result cmd_show_debug_logs(const char** tokens, unsigned int arg_cnt);
result cmd_debug(const char** tokens, unsigned int arg_cnt);

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
  X(cmd_status, "st") \
  X(cmd_ping, "ping") \
  X(cmd_ping, "p") \
  X(cmd_version, "version") \
  X(cmd_version, "v") \
  X(cmd_show_debug_logs, "logs") \
  X(dbg_start, "debug") \
  X(dbg_start, "dbg") \

#endif
