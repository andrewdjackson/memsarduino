#include "main.h"

static bool running = true;
sd_card card;

void setup() {
   uint16_t timeout = TIMEOUT;

   // setup the serial ports
   setup_ports();

   // setup the onboard led
   pinMode(LED_BUILTIN, OUTPUT);

   // initialise the sd card
   init_sd_card((sd_card *)&card);

   // get the new log version and create
   // the log file
   get_filename((sd_card *)&card);
   write_header((sd_card *)&card);

   // wait for connection before writing to the monitor
   while ((!MonitorSerial) && (timeout > 0)) {
      timeout--;
   }
}

void loop() {
   while (true) {

      if (MonitorSerial.available()) {
         uint8_t cmd = MonitorSerial.read();

         // 'i' command - initialise the SD Card
         if (cmd == (uint8_t)'i') {
            digitalWrite(LED_BUILTIN, LOW);
            init_sd_card((sd_card *)&card);
         }

         // 'l' list files on sd card
         if (cmd == (uint8_t)'l') {
            digitalWrite(LED_BUILTIN, LOW);
            list_files((sd_card *)&card);
         }

         // 'f' get the filename
         if (cmd == (uint8_t)'f') {
            digitalWrite(LED_BUILTIN, LOW);
            get_filename((sd_card *)&card);
         }

         // 'w' test write to sd card
         if (cmd == (uint8_t)'w') {
            digitalWrite(LED_BUILTIN, LOW);
            char data[10] = "123456789";
            write_log((sd_card *)&card, data);
         }

         // 'r' command - resume ecu loop
         if (cmd == (uint8_t)'r') {
            digitalWrite(LED_BUILTIN, LOW);
            running = true;
         }

         // 'p' command - pause ecu loop
         if (cmd == (uint8_t)'p') {
            digitalWrite(LED_BUILTIN, LOW);
            MonitorSerial.println(F("paused ecu loop, 'r' to resume"));
            running = false;
         }
      }

      // read the ecu
      if (running) {
         reader_loop((sd_card *)&card);
      } else {
         delay(100);
      }
   }
}
