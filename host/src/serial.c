#include "serial.h"
#include "ucobs.h"
#include "result.h"
#include "utils.h"
#include "vector.h"
#include <sched.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <dirent.h>
#include <string.h>

static int current_port_fd = -1;
static char current_port[SERIAL_PORT_PATH_MAX] = {0};

static bool recv_sync = false;

static uint8_t recv_buf[UCOBS_MAX_PACKET_LEN];
static size_t recv_len = 0;

int ser_scan_ports(Vec* return_vec) {
  if (vec_init(return_vec, SERIAL_PORT_PATH_MAX)) return -1;

  DIR* dev_dir = opendir("/dev");
  if (!dev_dir) {
    RES_RETURN(r_ESYS, -1);
  }

  struct dirent* entry;

  while ((entry = readdir(dev_dir)) != NULL) {
    if (strncmp(entry->d_name, "ttyUSB", 6) == 0) {
      char full_path[PATH_MAX];
      snprintf(full_path, sizeof(full_path), "/dev/%s", entry->d_name);
      debug_log(log_INFO, "Discovered open port: %s", full_path);
      if (vec_push(return_vec, full_path)) {
        closedir(dev_dir);
        return -1;
      }
    }
  }

  closedir(dev_dir);

  RES_RETURN(r_ENONE, 0);
}

int ser_open(char* path) {
  ser_close();

  if (path == NULL) {
    return r_EARGS;
  }

  int fd = open(path, O_RDWR | O_NOCTTY);
  if (fd < 0) {
    RES_RETURN(r_ESYS, -1);
  }

  struct termios tty;
  if (tcgetattr(fd, &tty) != 0) {
    close(fd);
    RES_RETURN(r_ESYS, -1);
  }

  memset(&tty, 0, sizeof tty);

  tty.c_cflag = CLOCAL | CREAD;   // ignore modem ctrl lines, enable read
  tty.c_cflag |= CS8;             // 8 data bits
  tty.c_cflag &= ~PARENB;         // no parity
  tty.c_cflag &= ~CSTOPB;         // 1 stop bit
  tty.c_cflag &= ~CRTSCTS;        // no hardware flow control

  tty.c_iflag = 0;
  tty.c_iflag &= ~(IXON | IXOFF | IXANY); // no software flow control

  tty.c_oflag = 0;

  tty.c_lflag = 0; // raw input (no echo, no canonical mode)

  tty.c_cc[VMIN]  = 0; // non-blocking read
  tty.c_cc[VTIME] = READ_TIMEOUT_100MS; // 100 ms timeout

  if (cfsetispeed(&tty, SER_BAUDRATE) != 0 || cfsetospeed(&tty, SER_BAUDRATE) != 0) {
    close(fd);
    RES_RETURN(r_ESYS, -1);
  }

  if (tcsetattr(fd, TCSANOW, &tty) != 0) {
    close(fd);
    RES_RETURN(r_ESYS, -1);
  }

  if (tcflush(fd, TCIOFLUSH)) {
    RES_RETURN(r_ESYS, -1);
  }

  current_port_fd = fd;
  snprintf(current_port, SERIAL_PORT_PATH_MAX, "%s", path);

  RES_RETURN(r_ENONE, 0);
}

void ser_close(void) {
  close(current_port_fd);
  current_port_fd = -1;
  *current_port = '\0';
  recv_sync = false;
}

char* ser_get_current_port(void) {
  return current_port;
}

bool ser_is_open(void) {
  if (*current_port != '\0' && access(current_port, F_OK) != 0) {
    ser_close();
  }

  return *current_port != '\0';
}

int ser_write(size_t length, const uint8_t* data) {
  if (!ser_is_open()) {
    RES_RETURN(r_EDEVICE, -1);
  }

  ssize_t result = write(current_port_fd, data, length);

  if (result <= 0 || (size_t)result != length) {
    ser_close();
    RES_RETURN(r_ESYS, -1);
  }

  RES_RETURN(r_ENONE, 0);
}

int ser_read(size_t* length, uint8_t* data) {
  if (current_port_fd < 0) {
    RES_RETURN(r_ESYS, -1);
  }

  if (data == NULL) {
    RES_RETURN(r_EARGS, -1);
  }

  if (length == NULL) {
    RES_RETURN(r_EARGS, -1);
  }

  ssize_t read__res = read(current_port_fd, data, *length);

  if (read__res < 0) {
    ser_close();
    RES_RETURN(r_ESYS, -1);
  } else if (read__res >= 1) {
    *length = read__res;
    RES_RETURN(r_DATA_READY, 0);
  }

  RES_RETURN(r_ENONE, 0);
}

int ser_enc_read(size_t* length, uint8_t* data) {
	size_t free_space = UCOBS_MAX_PACKET_LEN - recv_len;

	if (free_space == 0) return r_EENCODING;

	size_t bytes_read = free_space;
	if (ser_read(&bytes_read, recv_buf + recv_len)) return -1;
  if (_res != r_DATA_READY) RES_RETURN(r_ENO_DATA, -1);

	recv_len += bytes_read;

	size_t i = 0;
	while (i < recv_len) {
		if (recv_buf[i] != 0x00) {
			i++;
			continue;
		}

		if (!recv_sync || i == 0) {
			recv_sync = true;
			size_t remaining = recv_len - i - 1;
			memmove(recv_buf,	recv_buf + i + 1, remaining);
			recv_len = remaining;
			i = 0;

			continue;
		}

		int decoded = ucobs_decode(i, recv_buf, data);
		size_t remaining = recv_len - i - 1;
		memmove(recv_buf, recv_buf + i + 1, remaining);
		recv_len = remaining;

		if (decoded < 0) {
			recv_sync = false;
      RES_RETURN(r_EENCODING, -1);
		}

		*length = decoded;
    RES_RETURN(r_DATA_READY, 0);
	}

  RES_RETURN(r_ENONE, 0);
}

int ser_enc_write_va(const size_t length, ...) {
  va_list va_args;
  uint8_t buf[UCOBS_MAX_PACKET_LEN];

  va_start(va_args, length);

  for (size_t i = 1; i <= length; i++) {
    buf[i] = va_arg(va_args, int);
  }

  va_end(va_args);

  int encoded_len = ucobs_encode(length, buf + 1, buf + 1);
  if (encoded_len < 0) RES_RETURN(r_EENCODING, -1);

  buf[0] = 0x00;
  buf[encoded_len + 1] = 0x00;

  if (ser_write(encoded_len + UCOBS_LEN_FRAME, buf)) return -1;

  RES_RETURN(r_ENONE, 0);
}

int ser_enc_read_va(const size_t expected_length, ...) {
  size_t read_length;
  uint8_t read_data[UCOBS_MAX_DATA_LEN];

  if (ser_enc_read(&read_length, read_data)) return -1;
  if (_res != r_DATA_READY) RES_RETURN(r_ENO_DATA, -1);
  if (read_length != expected_length) return r_EDEVICE;

  va_list va_args;

  va_start(va_args, expected_length);

  for (size_t i = 0; i < expected_length; i++) {
    *va_arg(va_args, uint8_t*) = read_data[i];
  }

  va_end(va_args);

  RES_RETURN(r_ENONE, 0);
}

