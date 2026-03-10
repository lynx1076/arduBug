#include "serial_commands.h"
#include "mcp23017.h"
#include "meta.h"
#include "serial.h"
#include <stdint.h>
#include <string.h>

uint8_t scmd_ping(uint8_t argc, uint8_t* argv) {
  (void)argc;
  (void)argv;

  return 0;
}

uint8_t scmd_write(uint8_t argc, uint8_t* argv) {
  if (argc != 2) return 1;

  uint8_t port = argv[0];
  uint8_t gpio = argv[1];

  return mcp_write_port(port, gpio);
}

uint8_t scmd_write_mode(uint8_t argc, uint8_t* argv) {
  if (argc != 3) return 1;

  uint8_t port = argv[0];
  uint8_t iodir = argv[1];
  uint8_t pull_up = argv[2];

  return mcp_write_port_mode(port, iodir, pull_up);
}

uint8_t scmd_read(uint8_t argc, uint8_t* argv) {
  if (argc != 1) return 1;

  uint8_t gpio = 0;
  uint8_t port = argv[0];

  if (mcp_read_port(port, &gpio)) return 1;

  ser_write(gpio);

  return 0;
}

uint8_t scmd_get_version_text(uint8_t argc, uint8_t *argv) {
  (void)argc;
  (void)argv;

  ser_write_string(VERSION_TXT, (uint8_t)strlen(VERSION_TXT));
  return 0;
}

uint8_t scmd_get_version_code(uint8_t argc, uint8_t *argv) {
  (void)argc;
  (void)argv;
  
  ser_write(VERSION_CODE);

  return 0;
}

