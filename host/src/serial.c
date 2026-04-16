#include "serial.h"
#include "result.h"
#include "serial_handler.h"
#include "utils.h"
#include "vector.h"
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

static Vec open_ports;

static int current_port_fd = -1;
static char current_port_name[SERIAL_PORT_PATH_MAX] = {0};
static char current_port_path[SERIAL_PORT_PATH_MAX] = {0};

static bool is_open = false;
static bool was_open = false;

#define CONFIG_DELAY_MS         3000

result ser_init(void) {
  return vec_init(&open_ports, sizeof(ser_SerialPort));
}

void ser_free(void) {
  ser_close();
  vec_free(&open_ports);
}

void ser_update(void) {
  was_open = is_open;
  is_open = current_port_fd >= 0;

  if (ser_is_open() && access(current_port_path, F_OK) != 0) {
    ser_close();
  }
}

result ser_scan_ports(int* count) {
  result res;
  DIR* dev_dir = opendir("/dev");
  if (!dev_dir) {
    return r_ESYS;
  }

  res = vec_clear(&open_ports);
  if (res != r_ENONE) {
    closedir(dev_dir);
    return res;
  }

  struct dirent* entry;
  int count_ = 0;

  while ((entry = readdir(dev_dir)) != NULL) {
    if (strncmp(entry->d_name, "ttyUSB", 6) == 0) {
      ser_SerialPort port;
      snprintf(port.port_name, SERIAL_PORT_PATH_MAX, "%s", entry->d_name);
      snprintf(port.full_path, SERIAL_PORT_PATH_MAX, "/dev/%s", entry->d_name);

      if (vec_push(&open_ports, &port) == 0) {
        count_++;
      }
    }
  }

  if (count) *count = count_;
  closedir(dev_dir);
  return r_ENONE;
}

char* ser_get_port_name(int index) {
  return vec_get(&open_ports, index);
}

result ser_open_by_id(int id) {
  result res;

  ser_SerialPort* port = (ser_SerialPort*)vec_get(&open_ports, id);
  if (port == NULL) {
    return r_EBOUNDS;
  }

  int fd = open(port->full_path, O_RDWR | O_NOCTTY | O_SYNC | O_NONBLOCK);
  if (fd < 0) {
    return r_ESYS;
  }

  struct termios tty;
  if (tcgetattr(fd, &tty) != 0) {
    return r_ESYS;
  }

  cfsetospeed(&tty, B115200);
  cfsetispeed(&tty, B115200);

  tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
  tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON | IXOFF | IXANY);
  tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
  tty.c_oflag &= ~OPOST;
  tty.c_cc[VMIN]  = 0;
  tty.c_cc[VTIME] = 0;

  tty.c_cflag |= (CLOCAL | CREAD);
  tty.c_cflag &= ~(PARENB | PARODD);
  tty.c_cflag &= ~CSTOPB;
  tty.c_cflag &= ~CRTSCTS;

  if (tcsetattr(fd, TCSANOW, &tty) != 0) {
    return r_ESYS;
  }

  current_port_fd = fd;
  snprintf(current_port_name, SERIAL_PORT_PATH_MAX, "%s", port->port_name);
  snprintf(current_port_path, SERIAL_PORT_PATH_MAX, "%s", port->full_path);

  sleep_ms(CONFIG_DELAY_MS);

  res = ser_flush();
  if (res != r_ENONE) goto EXIT_ERR;

  res = seh_config();
  if (res != r_ENONE) goto EXIT_ERR;

  return r_ENONE;

EXIT_ERR:
  ser_close();
  return res;
}

void ser_close(void) {
  if (ser_is_open()) {
    close(current_port_fd);
    is_open = false;
    current_port_fd = -1;
    current_port_name[0] = '\0';
    current_port_path[0] = '\0';
  }
}

bool ser_is_open(void) {
  return current_port_fd >= 0;
}

char* ser_get_open(void) {
  return current_port_name;
}

bool ser_just_closed(void) {
  return was_open && !is_open;
}

bool ser_just_opened(void) {
  return !was_open && is_open;
}

result ser_write(uint8_t data) {
  if (current_port_fd < 0) {
    return r_ESYS;
  }

  ssize_t result = write(current_port_fd, &data, 1);

  if (result != 1) {
    ser_close();
    return r_ESYS;
  }

  return r_ENONE;
}

result ser_read(uint8_t* data) {
  if (current_port_fd < 0) {
    return r_ESYS;
  }

  if (data == NULL) {
    return r_EARGS;
  }

  ssize_t read_res = read(current_port_fd, data, 1);

  if (read_res < 0) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      return r_ENONE;
    }
    ser_close();
    return r_ESYS;
  } else if (read_res == 1) {
    return r_DATA_READY;
  }
  
  return r_ENONE;
}

result ser_flush() {
  if (current_port_fd < 0) return r_ESYS;
  
  if (tcflush(current_port_fd, TCIOFLUSH)) return r_ESYS;

  return r_ENONE;
}

