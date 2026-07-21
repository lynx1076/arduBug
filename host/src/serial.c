#include "serial.h"
#include "ucobs.h"
#include "result.h"
#include "utils.h"
#include "vector.h"
#include <ncurses.h>
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

#define DEVICE_READY_DELAY_MS         2500
#define READ_TIMEOUT_100MS            2

static int device_fd = -1;
static char device_path[SERIAL_PORT_PATH_MAX] = {0};
static size_t device_open_time_ms = 0;
static bool device_ready = false;

static bool recv_sync = false;

static uint8_t recv_buf[UCOBS_MAX_PACKET_LEN];
static size_t recv_len = 0;

int ser_update(void) {
  if (!ser_is_open()) RES_RETURN(r_ENONE, 0);

  bool _device_ready = device_open_time_ms + DEVICE_READY_DELAY_MS < millis();
  if (_device_ready && !device_ready) {
    device_ready = true;
    printw("Device is now ready\n");
  }

  RES_RETURN(r_ENONE, 0);
}

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

  cfmakeraw(&tty);

  tty.c_cflag = CLOCAL | CREAD;   // ignore modem ctrl lines, enable read
  tty.c_cflag &= ~CSIZE;          // ensure size bits empty
  tty.c_cflag |= CS8;             // 8 data bits
  tty.c_cflag &= ~PARENB;         // no parity
  tty.c_cflag &= ~CSTOPB;         // 1 stop bit
  tty.c_cflag &= ~CRTSCTS;        // no hardware flow control

  tty.c_iflag = 0;
  tty.c_iflag &= ~(IXON | IXOFF | IXANY); // no software flow control

  tty.c_oflag = 0;

  tty.c_lflag = 0; // raw input (no echo, no canonical mode)

  tty.c_cc[VMIN]  = 0;
  tty.c_cc[VTIME] = READ_TIMEOUT_100MS;

  if (cfsetispeed(&tty, SER_BAUDRATE) != 0 || cfsetospeed(&tty, SER_BAUDRATE) != 0) {
    close(fd);
    RES_RETURN(r_ESYS, -1);
  }

  if (tcsetattr(fd, TCSANOW, &tty) != 0) {
    close(fd);
    RES_RETURN(r_ESYS, -1);
  }

  if (tcflush(fd, TCIOFLUSH)) {
    close(fd);
    RES_RETURN(r_ESYS, -1);
  }

  device_fd = fd;
  snprintf(device_path, SERIAL_PORT_PATH_MAX, "%s", path);

  device_ready = false;
  device_open_time_ms = millis();

  RES_RETURN(r_ENONE, 0);
}

void ser_close(void) {
  close(device_fd);
  device_fd = -1;
  *device_path = '\0';
  recv_sync = false;
}

char* ser_get_device(void) {
  return device_path;
}

bool ser_is_open(void) {
  if (*device_path != '\0' && access(device_path, F_OK) != 0) {
    ser_close();
  }

  return *device_path != '\0';
}

int ser_write(size_t length, const uint8_t* data) {
  if (!ser_is_open()) {
    RES_RETURN(r_ESYS, -1);
  }

  if (!device_ready) {
    RES_RETURN(r_ENOT_INIT, -1);
  }

  ssize_t result = write(device_fd, data, length);

  if (result <= 0 || (size_t)result != length) {
    ser_close();
    RES_RETURN(r_ESYS, -1);
  }

  RES_RETURN(r_ENONE, 0);
}

int ser_read(size_t* length, uint8_t* data) {
  if (!ser_is_open()) {
    RES_RETURN(r_ESYS, -1);
  }

  if (!device_ready) {
    RES_RETURN(r_ENOT_INIT, -1);
  }

  if (data == NULL) {
    RES_RETURN(r_EARGS, -1);
  }

  if (length == NULL) {
    RES_RETURN(r_EARGS, -1);
  }

  ssize_t read_res = read(device_fd, data, *length);

  if (read_res < 0) {
    ser_close();
    RES_RETURN(r_ESYS, -1);
  } else if (read_res >= 1) {
    *length = read_res;
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

int ser_enc_write(size_t length, const uint8_t* data) {
  if (data == NULL) RES_RETURN(r_ENULL_PTR, -1);
  if (length > UCOBS_MAX_DATA_LEN) RES_RETURN(r_EPAYLOAD_SIZE, -1);

  if (length == 0) RES_RETURN(r_ENONE, 0);

  uint8_t buf[UCOBS_MAX_PACKET_LEN];

  int encoded_len = ucobs_encode(length, data, buf + 1);
  if (encoded_len < 0) RES_RETURN(r_EENCODING, -1);

  buf[0] = 0x00;
  buf[encoded_len + 1] = 0x00;

  if (ser_write(encoded_len + UCOBS_LEN_FRAME, buf)) return -1;

  RES_RETURN(r_ENONE, 0);
}

int ser_enc_write_va(size_t length, ...) {
  if (length > UCOBS_MAX_DATA_LEN) RES_RETURN(r_EPAYLOAD_SIZE, -1);

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

int ser_enc_read_va(size_t expected_length, ...) {
  size_t read_length;
  uint8_t read_data[UCOBS_MAX_DATA_LEN];

  if (ser_enc_read(&read_length, read_data)) return -1;
  if (_res != r_DATA_READY) RES_RETURN(r_ENO_DATA, -1);
  if (read_length != expected_length) return r_EDEVICE;

  va_list va_args;

  va_start(va_args, expected_length);

  for (size_t i = 0; i < read_length; i++) {
    *va_arg(va_args, uint8_t*) = read_data[i];
  }

  va_end(va_args);

  RES_RETURN(r_ENONE, 0);
}

