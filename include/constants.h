
#ifndef CONSTANTS_H
#define CONSTANTS_H

// #define DEBUG 1
#define MonitorSerial Serial

#define TIMEOUT 1000
#define TX_PIN 8
#define RX_PIN 9
#define TIMESTAMP_SIZE 17
#define MAX_RETIES 10
#define ECU_FREQ 50
#define FLASH_FREQ 100
#define CONNECTION_WAIT 10
#define LOG_BUFFER_SIZE 120
#define DATA_BUFFER_SIZE 50
#define HEX_BUFFER_SIZE 100
#define MEMSDATA_BUFFER_SIZE 330
#define DFRAME_80_SIZE 29
#define DFRAME_7D_SIZE 33

#define CONNECT_ERROR 3
#define INIT_ERROR 6
#define DATA_ERROR 2

#define VERSION_FILE "VERSION.TXT"
#define MEMS_HEADER "#time,0x7d_raw,0x80_raw"

#endif