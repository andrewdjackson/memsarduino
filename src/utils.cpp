#include "utils.h"

#ifdef DEBUG
void dprintf(const char *fmt, ...) {
   char buffer[LOG_BUFFER_SIZE];

   va_list arg;

   va_start(arg, fmt);
   vsnprintf(buffer, sizeof buffer, fmt, arg);
   va_end(arg);

   MonitorSerial.println(buffer);
}
#endif

void simple_current_time(char *buf) {
   unsigned long t = millis();

   // hours
   uint8_t hour = (t / 3600000) % 24;

   // minutes
   uint8_t minute = (t / 60000) % 60;

   // seconds
   uint8_t second = (t / 1000) % 60;

   // milliseconds
   uint16_t millis = t % 1000;

   snprintf(buf, TIMESTAMP_SIZE, "%02d:%02d:%02d.%03d", hour, minute, second, millis);
}

void convert_dataframe7d_to_string(char *buf, mems_data_frame_7d *dframe, int size) {
   unsigned int len = 0;

   for (int i = 0; i < size; i++) {
      unsigned char *b = (unsigned char *)&dframe[i];
      len += sprintf(buf + len, "%02x", *b);
   }
}

void convert_dataframe80_to_string(char *buf, mems_data_frame_80 *dframe, int size) {
   unsigned int len = 0;

   for (int i = 0; i < size; i++) {
      unsigned char *b = (unsigned char *)&dframe[i];
      len += sprintf(buf + len, "%02x", *b);
   }
}

void convert_hex_to_string(char *buf, uint8_t *data, int size) {
   unsigned int len = 0;

   for (int i = 0; i < size; i++) {
      unsigned char *b = (unsigned char *)&data[i];
      len += sprintf(buf + len, "%02x", *b);
   }
}

void memsdata_to_string(char *buf, mems_data *data, int size) {
   char timestamp[TIMESTAMP_SIZE];

   simple_current_time(timestamp);

   snprintf(buf, size, "%s,80%s,7d%s", timestamp, data->raw7d, data->raw80);

   /*
   snprintf(buf, size,
            "%s,"
            "%d,%d,%d,%d,%d,%s,%s,%s,%d,%d,"
            "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,"
            "%d,%d,%d,"
            "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,"
            "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,"
            "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,"
            "80%s,7d%s",
            timestamp, data->engine_rpm, data->coolant_temp_c, data->ambient_temp_c, data->intake_air_temp_c,
            data->fuel_temp_c, float_to_string(data->map_kpa, 1), float_to_string(data->battery_voltage, 1),
            float_to_string(data->throttle_pot_voltage, 1), data->idle_switch, data->uk1, data->park_neutral_switch,
            data->fault_codes, data->idle_set_point, data->idle_hot, data->uk2, data->iac_position, data->idle_error,
            data->ignition_advance_offset, data->ignition_advance, data->coil_time, data->crankshaft_position_sensor,
            data->uk4, data->uk5, data->ignition_switch, data->throttle_angle, data->uk6, data->air_fuel_ratio,
            data->dtc2, data->lambda_voltage_mv, data->lambda_sensor_frequency, data->lambda_sensor_dutycycle,
            data->lambda_sensor_status, data->closed_loop, data->long_term_fuel_trim, data->short_term_fuel_trim,
            data->carbon_canister_dutycycle, data->dtc3, data->idle_base_pos, data->uk7, data->dtc4,
            data->ignition_advance2, data->idle_speed_offset, data->idle_error2,
            (((uint16_t)data->idle_error2 << 8) | data->uk10), data->dtc5, data->uk11, data->uk12, data->uk13,
            data->uk14, data->uk15, data->uk16, data->uk1A, data->uk1B, data->uk1C, data->raw7d, data->raw80);
            */
}

void flash_error_sequence(bool error, uint8_t flashes) {
   if (error) {
      for (uint8_t i = 0; i < flashes; i++) {
         digitalWrite(LED_BUILTIN, HIGH);
         delay(FLASH_FREQ);
         digitalWrite(LED_BUILTIN, LOW);
      }
   }
}