#ifndef READER_H
#define READER_H

#include "constants.h"
#include "datalog.h"
#include "rosco.h"
#include "utils.h"
#include <Arduino.h>
#include <SoftwareSerial.h>

void setup_ports();
void reader_loop(sd_card *card);
bool initialise();
void sendToECU(uint8_t *command, int size);
void readECU(uint8_t *response, int size);
void get_memsdata(mems_data *data, mems_data_frame_7d *dframe7d, mems_data_frame_80 *dframe80);

#endif