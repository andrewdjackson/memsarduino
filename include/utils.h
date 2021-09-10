#ifndef UTILS_H
#define UTILS_H

#include "constants.h"
#include "rosco.h"
#include <Arduino.h>

void simple_current_time(char *buf);
void dprintf(const char *fmt, ...);
void convert_dataframe7d_to_string(char *buf, mems_data_frame_7d *dframe, int size);
void convert_dataframe80_to_string(char *buf, mems_data_frame_80 *dframe, int size);
void convert_hex_to_string(char *buf, uint8_t *data, int size);
char *float_to_string(float f, int p);
void memsdata_to_string(char *buf, mems_data *data, int size);
void flash_error_sequence(bool error, uint8_t flashes);

#endif