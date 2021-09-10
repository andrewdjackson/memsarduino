#ifndef ROSCO_H
#define ROSCO_H

/** \file mems.h
 * Header file defining the librosco functions, structs, and enums.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/**
 * Data sequence returned by the ECU in reply to the command 0x7D.
 */
typedef struct {
   uint8_t command;
   uint8_t bytes_in_frame;
   uint8_t ignition_switch;
   uint8_t throttle_angle;
   uint8_t uk6;
   uint8_t air_fuel_ratio;
   uint8_t dtc2;
   uint8_t lambda_voltage;
   uint8_t lambda_sensor_frequency;
   uint8_t lambda_sensor_dutycycle;
   uint8_t lambda_sensor_status;
   uint8_t closed_loop;
   uint8_t long_term_fuel_trim;
   uint8_t short_term_fuel_trim;
   uint8_t carbon_canister_dutycycle;
   uint8_t dtc3;
   uint8_t idle_base_pos;
   uint8_t uk7;
   uint8_t dtc4;
   uint8_t ignition_advance2;
   uint8_t idle_speed_offset;
   uint8_t idle_error2;
   uint8_t uk10;
   uint8_t dtc5;
   uint8_t uk11;
   uint8_t uk12;
   uint8_t uk13;
   uint8_t uk14;
   uint8_t uk15;
   uint8_t uk16;
   uint8_t uk17;
   uint8_t uk18;
   uint8_t uk19;
} mems_data_frame_7d;

/**
 * Data sequence returned by the ECU in reply to the command 0x80.
 */
typedef struct {
   uint8_t command;
   uint8_t bytes_in_frame;
   uint8_t engine_rpm_hi;
   uint8_t engine_rpm_lo;
   uint8_t coolant_temp;
   uint8_t ambient_temp;
   uint8_t intake_air_temp;
   uint8_t fuel_temp;
   uint8_t map_kpa;
   uint8_t battery_voltage;
   uint8_t throttle_pot;
   uint8_t idle_switch;
   uint8_t uk1;
   uint8_t park_neutral_switch;
   uint8_t dtc0;
   uint8_t dtc1;
   uint8_t idle_set_point;
   uint8_t idle_hot;
   uint8_t uk2;
   uint8_t iac_position;
   uint8_t idle_error_hi;
   uint8_t idle_error_lo;
   uint8_t ignition_advance_offset;
   uint8_t ignition_advance;
   uint8_t coil_time_hi;
   uint8_t coil_time_lo;
   uint8_t crankshaft_position_sensor;
   uint8_t uk4;
   uint8_t uk5;
} mems_data_frame_80;

/**
 * Compact structure containing only the relevant data from the ECU.
 */
typedef struct {
   int engine_rpm;
   int coolant_temp_c;
   int ambient_temp_c;
   int intake_air_temp_c;
   int fuel_temp_c;
   float map_kpa;
   float battery_voltage;
   float throttle_pot_voltage;
   int idle_switch;
   int uk1;
   int park_neutral_switch;
   /**
    * Bit 0: Coolant temp sensor CCT fault (1)
    * Bit 1: Inlet air temp sensor CCT fault (2)
    * Bit 2: Fuel pump circuit fault (10)
    * Bit 3: Throttle pot circuit fault (16)
    */
   int fault_codes;
   int idle_set_point;
   int idle_hot;
   int uk2;
   int iac_position;
   int idle_error;
   int ignition_advance_offset;
   int ignition_advance;
   int coil_time;
   int crankshaft_position_sensor;
   int uk4;
   int uk5;
   //******dframe 7**********************************
   int ignition_switch;
   int throttle_angle;
   int uk6;
   int air_fuel_ratio;
   int dtc2;
   int lambda_voltage_mv;
   int lambda_sensor_frequency;
   int lambda_sensor_dutycycle;
   int lambda_sensor_status;
   int closed_loop;
   int long_term_fuel_trim;
   int short_term_fuel_trim;
   int carbon_canister_dutycycle;
   int dtc3;
   int idle_base_pos;
   int uk7;
   int dtc4;
   int ignition_advance2;
   int idle_speed_offset;
   int idle_error2;
   int uk10;
   int dtc5;
   int uk11;
   int uk12;
   int uk13;
   int uk14;
   int uk15;
   int uk16;
   int uk1A;
   int uk1B;
   int uk1C;
   int uk1E;
   int uk1F;
   bool coolant_temp_sensor_fault;
   bool intake_air_temp_sensor_fault;
   bool fuel_pump_circuit_fault;
   bool throttle_pot_circuit_fault;
   char raw7d[70];
   char raw80[70];
} mems_data;

#endif
