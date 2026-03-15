#include "serial.h"
#include "serial_protocol.h"
#include "vector.h"
#include <fcntl.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>

static int serial_handle = -1;
Vec ports;

int ser_init(void) {
  if (vec_init(&ports, SER_MAX_PORT_PATH_LENGTH * sizeof(char))) {
    return -1;
  }
  return 0;
}

void ser_free(void) {
  vec_free(&ports);
}

int ser_open(const char* port) {
  if (serial_handle >= 0) {
    ser_close();
  }

  serial_handle = open(port, O_RDWR | O_NOCTTY | O_NONBLOCK);
  if (serial_handle < 0) {
    return -1;
  }

  struct termios tty = {0};
  if (tcgetattr(serial_handle, &tty) != 0) {
    ser_close();
    return -1;
  }

  if (cfsetispeed(&tty, SER_BAUD_RATE) != 0 || cfsetospeed(&tty, SP_BAUD_RATE) != 0) {
    ser_close();
    return -1;
  }

  tty.c_cflag = CS8 | CREAD | CLOCAL;
  tty.c_iflag = 0;
  tty.c_oflag = 0;
  tty.c_lflag = 0;
  tty.c_cc[VMIN] = 0;
  tty.c_cc[VTIME] = 0;

  if (tcsetattr(serial_handle, TCSANOW, &tty) != 0) {
    ser_close();
    return -1;
  }

  return 0;
}

void ser_close(void) {
  if (serial_handle < 0) {
    return;
  }

  close(serial_handle);
  serial_handle = -1;
}

int ser_available(void) {
  if (serial_handle < 0) {
    return -1;
  }

  int bytes = 0;
  if (ioctl(serial_handle, FIONREAD, &bytes) != 0) {
    return -1;
  }

  return bytes;
}

int ser_read(uint8_t *buf, size_t len) {
  if (serial_handle < 0 || buf == NULL || len == 0) {
    return -1;
  }

  ssize_t r = read(serial_handle, buf, len);

  return r;
}

int ser_write(const uint8_t *buf, size_t len) {
  if (serial_handle < 0 || buf == NULL || len == 0) {
    return -1;
  }

  return write(serial_handle, buf, len) == (ssize_t)len ? 0 : -1;
}

int ser_scan_ports(void) {
  const char *tty_dir = "/sys/class/tty";
  DIR *d = opendir(tty_dir);
  if (d == NULL) {
    return -1;
  }

  if (vec_clear(&ports)) {
    return -1;
  }

  struct dirent *dir;
  while ((dir = readdir(d)) != NULL) {
    if (strncmp(dir->d_name, "ttyACM", 6) == 0 ||
        strncmp(dir->d_name, "ttyUSB", 6) == 0) {
      char buf[SER_MAX_PORT_PATH_LENGTH];
      sprintf(buf, "/dev/%s", dir->d_name);
      vec_push(&ports, buf);
    }
  }
  closedir(d);

  return ports.count;
}

char* ser_get_port_name(int index) {
  return vec_get(&ports, index);
}

