#include "reader.h"

// Using pins 8 TX, 9 RX as software serial port
// this is so we can invert the logic when talking from MAX3232 to MAX3232
SoftwareSerial ECUSerial(TX_PIN, RX_PIN, true);

// use this if using hardware serial interface
//#define ECUSerial Serial1

static bool initialised = false;
static uint8_t retries = 0;

const unsigned char MEMSInitCommandA[1] = {0xca};
const unsigned char MEMSInitCommandB[1] = {0x75};
const unsigned char MEMSHeartbeat[1] = {0xf4};
const unsigned char MEMSInitECUID[1] = {0xd0};
const unsigned char MEMSDataframe80[1] = {0x80};
const unsigned char MEMSDataframe7d[1] = {0x7d};

mems_data_frame_80 dframe80;
mems_data_frame_7d dframe7d;
mems_data memsdata;

uint8_t response[DFRAME_7D_SIZE + 1];
uint8_t df7d[DFRAME_7D_SIZE * 2 + 1];
uint8_t df80[DFRAME_80_SIZE * 2 + 1];

void setup_ports() {
   // initialize both serial ports
   // Serial is used for monitor
   // Serial1 is used for ECU communications
   MonitorSerial.begin(9600);
   ECUSerial.begin(9600);
   ECUSerial.setTimeout(TIMEOUT);
}

void reader_loop(sd_card *card) {
   if (!initialised) {
      
#ifdef DEBUG     
      dprintf("inf: intialising");
#endif
      initialised = initialise();
   }

   if (initialised) {
      sendToECU((uint8_t *)MEMSDataframe7d, sizeof(MEMSDataframe7d));
      readECU((uint8_t *)response, DFRAME_7D_SIZE);
      convert_hex_to_string((char *)df7d, (uint8_t *)response, DFRAME_7D_SIZE);

      sendToECU((uint8_t *)MEMSDataframe80, sizeof(MEMSDataframe80));
      readECU((uint8_t *)response, DFRAME_80_SIZE);
      convert_hex_to_string((char *)df80, (uint8_t *)response, DFRAME_80_SIZE);

      char memslog[MEMSDATA_BUFFER_SIZE];
      char timestamp[TIMESTAMP_SIZE];

      simple_current_time(timestamp);
      snprintf(memslog, MEMSDATA_BUFFER_SIZE, "%s,%s,%s", timestamp, df7d, df80);

#ifdef DEBUG
      dprintf("%s", memslog);
#endif

      write_log(card, memslog);

      // wait between calls
      delay(ECU_FREQ);
   } else {
      flash_error_sequence(true, INIT_ERROR);
   }
}

bool initialise() {
   retries = 0;

   uint8_t response[DATA_BUFFER_SIZE];

   sendToECU((uint8_t *)MEMSInitCommandA, sizeof(MEMSInitCommandA));
   readECU((uint8_t *)response, 1);

   if (response[0] != MEMSInitCommandA[0]) {
#ifdef DEBUG
      dprintf("err: 0xCA command echo");
#endif
      return false;
   }

   delay(1);

   sendToECU((uint8_t *)MEMSInitCommandB, sizeof(MEMSInitCommandB));
   readECU((uint8_t *)response, 1);

   if (response[0] != MEMSInitCommandB[0]) {
#ifdef DEBUG
      dprintf("err: 0x75 command echo");
#endif
      return false;
   }

   delay(1);

   sendToECU((uint8_t *)MEMSHeartbeat, sizeof(MEMSHeartbeat));
   readECU((uint8_t *)response, 2);

   if (response[0] != MEMSHeartbeat[0]) {
#ifdef DEBUG
      dprintf("err: 0xF4 command echo");
#endif
      return false;
   }

   delay(1);

   sendToECU((uint8_t *)MEMSInitECUID, sizeof(MEMSInitECUID));
   readECU((uint8_t *)response, 5);

   if (response[0] != MEMSInitECUID[0]) {
#ifdef DEBUG
      dprintf("err: 0xD0 command echo");
#endif
      return false;
   } else {
#ifdef DEBUG
      dprintf("inf: ECU ID %02x %02x %02x %02x", response[1], response[2], response[3], response[4]);
#endif
   }

   initialised = true;

   return initialised;
}

void sendToECU(uint8_t *command, int size) {
   digitalWrite(LED_BUILTIN_TX, HIGH);
   ECUSerial.write(command, size);
   ECUSerial.flush();
   digitalWrite(LED_BUILTIN_TX, LOW);

#ifdef DEBUG
   // exclude if we're not a debug build
   char hex_buffer[HEX_BUFFER_SIZE];
   convert_hex_to_string(hex_buffer, command, size);
   dprintf("inf: tx %s", hex_buffer);
#endif
}

void readECU(uint8_t *response, int size) {
   int timeout = TIMEOUT;
   uint8_t bytesRead = 0;

   while ((bytesRead < size) && (timeout > 0)) {
      digitalWrite(LED_BUILTIN_RX, HIGH);
      if (ECUSerial.available()) {
         bytesRead = ECUSerial.readBytes(response, size);
      } else {
         // wait for data
         delay(1);
         timeout--;
      }
      digitalWrite(LED_BUILTIN_RX, LOW);
   }

   if (timeout > 0) {
      if (bytesRead != size) {
         flash_error_sequence(true, DATA_ERROR);
      }

#ifdef DEBUG
      // exclude if we're not a debug build
      char hex_buffer[HEX_BUFFER_SIZE];
      convert_hex_to_string(hex_buffer, response, size);
      dprintf("inf: rx (%d/%d) %s", bytesRead, size, hex_buffer);
#endif

   } else {
      flash_error_sequence(true, DATA_ERROR);
#ifdef DEBUG
      dprintf("err: timed out (%d)", retries++);
#endif

      if (retries >= MAX_RETIES) {
         initialised = false;
      }
   }
}

void get_memsdata(mems_data *data, mems_data_frame_7d *dframe7d, mems_data_frame_80 *dframe80) {
   memset(data, 0, sizeof(mems_data));

   // dataframe 0x80
   data->engine_rpm = ((uint16_t)dframe80->engine_rpm_hi << 8) | dframe80->engine_rpm_lo;
   data->coolant_temp_c = dframe80->coolant_temp - 55;
   data->ambient_temp_c = dframe80->ambient_temp - 55;
   data->intake_air_temp_c = dframe80->intake_air_temp - 55;
   data->fuel_temp_c = dframe80->fuel_temp - 55;
   data->map_kpa = dframe80->map_kpa;
   data->battery_voltage = dframe80->battery_voltage / 10.0;
   data->throttle_pot_voltage = dframe80->throttle_pot * 0.02;
   data->idle_switch = (dframe80->idle_switch == 0) ? 0 : 1;
   data->uk1 = dframe80->uk1;
   data->park_neutral_switch = (dframe80->park_neutral_switch == 0) ? 0 : 1;
   data->fault_codes = 0;
   data->idle_set_point = dframe80->idle_set_point;
   data->idle_hot = dframe80->idle_hot;
   data->uk2 = dframe80->uk2;
   data->iac_position = dframe80->iac_position;
   data->idle_error = ((uint16_t)dframe80->idle_error_hi << 8) | dframe80->idle_error_lo;
   data->ignition_advance_offset = dframe80->ignition_advance_offset;
   data->ignition_advance = (dframe80->ignition_advance * 0.5) - 24.0;
   data->coil_time = (((uint16_t)dframe80->coil_time_hi << 8) | dframe80->coil_time_lo) * 0.002;
   data->crankshaft_position_sensor = dframe80->crankshaft_position_sensor;
   data->uk4 = dframe80->uk4;
   data->uk5 = dframe80->uk5;

   // update fault codes
   if (dframe80->dtc0 & 0x01) { // coolant temp sensor fault
      data->fault_codes |= (1 << 0);
      data->coolant_temp_sensor_fault = true;
   }

   if (dframe80->dtc0 & 0x02) { // intake air temp sensor fault
      data->fault_codes |= (1 << 1);
      data->intake_air_temp_sensor_fault = true;
   }

   if (dframe80->dtc1 & 0x02) { // fuel pump circuit fault
      data->fault_codes |= (1 << 2);
      data->fuel_pump_circuit_fault = true;
   }

   if (dframe80->dtc1 & 0x80) { // throttle pot circuit fault
      data->fault_codes |= (1 << 3);
      data->throttle_pot_circuit_fault = true;
   }

   // dataframe 0x7d

   data->ignition_switch = dframe7d->ignition_switch;
   data->throttle_angle = dframe7d->throttle_angle;
   data->uk6 = dframe7d->uk6;
   data->air_fuel_ratio = dframe7d->air_fuel_ratio;
   data->dtc2 = dframe7d->dtc2;
   data->lambda_voltage_mv = dframe7d->lambda_voltage * 5;
   data->lambda_sensor_frequency = dframe7d->lambda_sensor_frequency;
   data->lambda_sensor_dutycycle = dframe7d->lambda_sensor_dutycycle;
   data->lambda_sensor_status = dframe7d->lambda_sensor_status;
   data->closed_loop = dframe7d->closed_loop;
   data->long_term_fuel_trim = dframe7d->long_term_fuel_trim;
   data->short_term_fuel_trim = dframe7d->short_term_fuel_trim;
   data->carbon_canister_dutycycle = dframe7d->carbon_canister_dutycycle;
   data->dtc3 = dframe7d->dtc3;
   data->idle_base_pos = dframe7d->idle_base_pos;
   data->uk7 = dframe7d->uk7;
   data->dtc4 = dframe7d->dtc4;
   data->ignition_advance2 = dframe7d->ignition_advance2;
   data->idle_speed_offset = dframe7d->idle_speed_offset;
   data->idle_error2 = dframe7d->idle_error2;
   data->uk10 = dframe7d->uk10;
   data->dtc5 = dframe7d->dtc5;
   data->uk11 = dframe7d->uk11;
   data->uk12 = dframe7d->uk12;
   data->uk13 = dframe7d->uk13;
   data->uk14 = dframe7d->uk14;
   data->uk15 = dframe7d->uk15;
   data->uk16 = dframe7d->uk16;
   data->uk1A = dframe7d->uk17;
   data->uk1B = dframe7d->uk18;
   data->uk1C = dframe7d->uk19;

   convert_dataframe80_to_string(data->raw80, dframe80, sizeof(mems_data_frame_80));
   convert_dataframe7d_to_string(data->raw7d, dframe7d, sizeof(mems_data_frame_7d));
}