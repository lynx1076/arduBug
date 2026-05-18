#ifndef COMMANDS_H
#define COMMANDS_H

#include <stddef.h>

#define COMMANDS \
  X(cmd_quit, "quit", "q") \
  X(cmd_scan_devices, "scan", "s") \
  X(cmd_connect, "connect", "c") \
  X(cmd_disconnect, "disconnect", "d") \
  X(cmd_status, "status", "st") \
  X(cmd_ping, "ping", "p") \
  X(cmd_version, "version", "v") \
  X(cmd_show_debug_logs, "logs", "") \
  X(cmd_debugger, "debug", "dbg") \

#define X(CMD, NAME, ALT) \
  int CMD(size_t arg_cnt, const char** tokens);
COMMANDS
#undef X

#endif
