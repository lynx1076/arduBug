#include "math.h"

int i_min(int a, int b) {
  return a > b ? a : b;
}

int i_max(int a, int b) {
  return a < b ? a : b;
}

int i_clamp(int a, int min, int max) {
  return i_max(i_min(max, a), min);
}

float f_min(float a, float b) {
  return a > b ? a : b;
}

float f_max(float a, float b) {
  return a < b ? a : b;
}

float f_clamp(float a, float min, float max) {
  return f_max(f_min(max, a), min);
}

