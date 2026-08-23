#include "devmem.h"
#include "common.h"
#include "gui.h"
#include "device.h"
#include "parse.h"
#include "result.h"
#include <ctype.h>
#include <raylib.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static bool dump_memory = false;
static size_t dump_addr;
static uint8_t* dump_buf;
static char* dump_filepath;

static size_t flash_memory = false;
static FILE* flash_file = NULL;
static size_t flash_addr;

static int devm_update_dump(void) {
  if (!dump_memory) {
    if (dump_filepath) {
      free(dump_filepath);
      dump_filepath = NULL;
    }
    if (dump_buf) {
      free(dump_buf);
      dump_buf = NULL;
    }
    RES_RETURN(r_ENONE, 0);
  }

  int read_size = MEMORY_SIZE - dump_addr > PAGE_SIZE ? PAGE_SIZE : MEMORY_SIZE - dump_addr;
  if (read_size) {
    gui_set_statusline(TextFormat("Dumping memory: %i%%", dump_addr * 100 / MEMORY_SIZE));
    if (dev_mem_page_read(dump_addr, read_size, dump_buf + dump_addr)) return -1;
    dump_addr += read_size;
  } else {
    FILE *file;

    file = fopen(dump_filepath, "w");
    if (file == NULL) {
      gui_log("Could not open dumpfile");

      RES_RETURN(r_ESYS, -1);
    }

    for (int addr = 0; addr < MEMORY_SIZE; addr++) {
      if (addr && addr % 16 == 0) {
        fprintf(file, "\n%02x ", dump_buf[addr]);
      } else if (addr && addr % 8 == 0) {
        fprintf(file, " %02x ", dump_buf[addr]);
      } else {
        fprintf(file, "%02x ", dump_buf[addr]);
      }
    }

    dump_memory = false;

    gui_set_statusline("Successfully dumped memory");
    gui_log(TextFormat("Successfully wrote memory dump to %s", dump_filepath));

    free(dump_filepath);
    free(dump_buf);
    dump_filepath = NULL;
    dump_buf = NULL;
    fclose(file);
  }

  RES_RETURN(r_ENONE, 0);
}

static int devm_update_flash(void) {
  if (!flash_memory) {
    if (flash_file) {
      fclose(flash_file);
      flash_file = NULL;
    }
    RES_RETURN(r_ENONE, 0);
  }

  int flash_size = MEMORY_SIZE - flash_addr > PAGE_SIZE ? PAGE_SIZE : MEMORY_SIZE - flash_addr;
  uint8_t flash_block[PAGE_SIZE];

  for (int i = 0; i < flash_size; i++) {
    char c;

    do {
      if (fread(&c, sizeof(char), 1, flash_file) != 1) {
        gui_log("File error");
        RES_RETURN(r_EFILE, -1);
      }
    } while (isspace(c));

    char byte_str[3];
    byte_str[0] = c;
    byte_str[2] = '\0';
    if (fread(byte_str + 1, sizeof(char), 1, flash_file) != 1) {
      gui_log("File ended");
      flash_memory = false;
      RES_RETURN(r_ENONE, -1);
    }

    if (parse_hex_byte(byte_str, &flash_block[i])) return -1;
  }

  if (dev_mem_page_write(flash_addr, flash_size, flash_block)) return -1;

  gui_set_statusline(TextFormat("Flashing: %i%%", flash_addr * 100 / MEMORY_SIZE));

  flash_addr += flash_size;
  if (flash_addr >= MEMORY_SIZE) {
    gui_set_statusline("Finished flashing");
    flash_memory = false;
  }

  RES_RETURN(r_ENONE, 0);
}

int devm_bulk_write(uint16_t addr, uint16_t len, const uint8_t* data) {
  if (addr + len > LAST_ADDR) RES_RETURN(r_EBOUNDS, -1);

  uint16_t remaining_bytes = len;

  while (remaining_bytes) {
    int write_size = remaining_bytes > PAGE_SIZE ? PAGE_SIZE : remaining_bytes;
    if (dev_mem_page_write(addr, write_size, data)) return -1;
    data += write_size;
    remaining_bytes -= write_size;
  }

  RES_RETURN(r_ENONE, 0);
}

int devm_dump_to_file(const char* filepath) {
  if (dump_memory) {
    gui_log("Already dumping memory");
    RES_RETURN(r_ENONE, 0);
  }

  if (flash_memory) {
    gui_log("Already flashing memory");
    RES_RETURN(r_ENONE, 0);
  }

  dump_filepath = strdup(filepath);
  if (dump_filepath == NULL) RES_RETURN(r_EMEM, -1);

  dump_addr = 0;
  dump_buf = malloc(MEMORY_SIZE);
  if (dump_buf == NULL) RES_RETURN(r_EMEM, -1);

  dump_memory = true;

  gui_log(TextFormat("Dumping to %s", filepath));

  RES_RETURN(r_ENONE, 0);
}

int devm_flash_from_file(const char* filepath) {
  if (dump_memory) {
    gui_log("Already dumping memory");
    RES_RETURN(r_ENONE, 0);
  }

  if (flash_memory) {
    gui_log("Already flashing memory");
    RES_RETURN(r_ENONE, 0);
  }

  flash_file = fopen(filepath, "r");
  if (flash_file == NULL) RES_RETURN(r_EFILE, -1);

  flash_addr = 0;

  flash_memory = true;

  gui_log(TextFormat("Flashing from %s", filepath));

  RES_RETURN(r_ENONE, 0);
}

int devm_update(void) {
  if (!dev_is_ready()) {
    dump_memory = false;
    flash_memory = false;
  }

  if (devm_update_dump()) {
    dump_memory = false;
    return -1;
  }

  if (devm_update_flash()) {
    gui_log(TextFormat("Failed to flash: %s", res_get_string(_res)));
    gui_set_statusline("Failed to flash");
    flash_memory = false;
    return -1;
  }

  RES_RETURN(r_ENONE, 0);
}

