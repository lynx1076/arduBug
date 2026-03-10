#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <termios.h>
#include <stdbool.h>
#include "serial_protocol.h"

static int serial_fd = -1;

static void print_byte(uint8_t byte, bool recv) {
#define X(BYTE) \
  if (byte == BYTE) { \
    printf("%s 0x%02x: %s\n> ", recv ? "<-" : "->", byte, #BYTE); \
    return; \
  }
  
  SP_BYTE_LIST
#undef X
  printf("%s 0x%02x\n", recv ? "<-" : "->", byte); \
}

static void* rx_thread(void* arg) {
  (void)arg;
  uint8_t byte;
  while (read(serial_fd, &byte, 1) == 1) {
    print_byte(byte, true);
    fflush(stdout);
  }
  return NULL;
}


static int serial_open(const char* port, int baud) {
  int fd = open(port, O_RDWR | O_NOCTTY);
  if (fd < 0) {
    fprintf(stderr, "Failed to open %s: %s\n", port, strerror(errno));
    return -1;
  }

  struct termios tty = {0};
  cfsetispeed(&tty, baud);
  tty.c_cflag = CS8 | CREAD | CLOCAL;
  tty.c_iflag = 0;
  tty.c_oflag = 0;
  tty.c_lflag = 0;
  tty.c_cc[VMIN]  = 1;
  tty.c_cc[VTIME] = 0;

  if (tcsetattr(fd, TCSANOW, &tty)) {
    fprintf(stderr, "Failed to configure %s: %s\n", port, strerror(errno));
    close(fd);
    return -1;
  }

  return fd;
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <port> [baud]\n", argv[0]);
    fprintf(stderr, "  e.g. %s /dev/ttyUSB0 9600\n", argv[0]);
    return 1;
  }

  int baud = (argc >= 3) ? atoi(argv[2]) : 9600;

  serial_fd = serial_open(argv[1], baud);
  if (serial_fd < 0) return 1;

  printf("Opened %s at %d baud.\n", argv[1], baud);
  printf("Enter hex bytes to send (e.g. A0 C1 20), empty line to quit.\n\n");

  pthread_t tid;
  if (pthread_create(&tid, NULL, rx_thread, NULL)) {
    fprintf(stderr, "Failed to create RX thread: %s\n", strerror(errno));
    close(serial_fd);
    return 1;
  }
  pthread_detach(tid);

  char line[256];
  while (1) {
    printf("> ");
    fflush(stdout);

    if (!fgets(line, sizeof(line), stdin) || line[0] == '\n') break;

    char* token = strtok(line, " \t\r\n");
    while (token) {
      char* end;
      long val = strtol(token, &end, 16);
      if (end == token || val < 0 || val > 0xFF) {
        fprintf(stderr, "Invalid byte: '%s'\n", token);
      } else {
        uint8_t byte = (uint8_t)val;
        if (write(serial_fd, &byte, 1) != 1)
          fprintf(stderr, "Write error: %s\n", strerror(errno));
        else
          print_byte(byte, false);
      }
      token = strtok(NULL, " \t\r\n");
    }
  }

  close(serial_fd);
  return 0;
}

