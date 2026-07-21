#ifndef COMMANDS_H
#define COMMANDS_H

#include <stddef.h>

#define COMMANDS \
  X(cmd_help, "help", "h") \
  X(cmd_quit, "quit", "q") \
  X(cmd_scan_devices, "scan", "s") \
  X(cmd_connect, "connect", "c") \
  X(cmd_disconnect, "disconnect", "disc") \
  X(cmd_status, "status", "") \
  X(cmd_ping, "ping", "p") \
  X(cmd_version, "version", "v") \
  X(cmd_show_debug_logs, "logs", "") \
  X(cmd_ext_clock_en, "clke", "ce") \

#define X(CMD, NAME, ALT) \
  int CMD(size_t arg_cnt, const char** tokens);
COMMANDS
#undef X


/**
 * The debugger is very confusing. It exists, but it's very confusing.
 * I don't understand my debugger.
 * It's a low-level debugger and if you're not familiar with
 * a low-level debugger you don't know what you're talking about.
 * - Terry A. Davis
*/

#endif
