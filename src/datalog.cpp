#include "datalog.h"

// set up variables using the SD utility library functions:
// Sd2Card card;
// SdVolume volume;
// SdFile root;
#define chipSelect 10

void init_sd_card(sd_card *card) {
   pinMode(chipSelect, INPUT); // change this to 53 on a mega

   card->initialised = SD.begin(chipSelect);

   if (!card->initialised) {
      MonitorSerial.println(F("err: sd failed"));
   }
}

void get_filename(sd_card *card) {
   File versionFile;
   long version = 0;
   char newVersion[6];
   String fileVersion = "";

   if (card->initialised) {
      if (SD.exists(F(VERSION_FILE))) {
         // read the value from the version file
         versionFile = SD.open(VERSION_FILE, O_READ);
         if (versionFile) {
            fileVersion = "";
            while (versionFile.available()) {
               fileVersion += (char)versionFile.read();
            }

            // increment the value and construct the filename
            version = fileVersion.toInt();
            version++;
         }
      }

      // write the new value to the version file, this will be
      // 0 if no version file exists
      snprintf(newVersion, 6, "%05d", version);
      versionFile.close();
      versionFile = SD.open(VERSION_FILE, O_WRITE | O_CREAT | O_TRUNC);
      dprintf("inf: log version %s", newVersion);
      versionFile.println(newVersion);
      versionFile.close();

      // set the filename
      snprintf(card->filename, 13, "RUN%05d.CSV", version);
      dprintf("inf: log filename %s", card->filename);
   }
}

void write_header(sd_card *card) {
   if (card->initialised) {
      File log = SD.open(card->filename, O_WRITE | O_CREAT | O_APPEND);
      if (SD.exists(card->filename)) {
         log.println(F(MEMS_HEADER));
      } else {
         MonitorSerial.println(F("err: file write error"));
      }

      log.close();
   } else {
      MonitorSerial.println(F("err: card not initialised"));
   }
}

void write_log(sd_card *card, char *data) {
   if (card->initialised) {
      MonitorSerial.println(F("inf: write log"));
      File log = SD.open(card->filename, O_WRITE | O_CREAT | O_APPEND);

      if (SD.exists(card->filename)) {
         log.println(data);
      } else {
         MonitorSerial.println(F("err: file write error"));
      }

      log.close();
   } else {
      MonitorSerial.println(F("err: card not initialised"));
   }
}

void list_files(sd_card *card) {
   if (card->initialised) {
      MonitorSerial.println(F("inf: list files"));
      File root = SD.open("/");
      printDirectory(root, 0);
   } else {
      MonitorSerial.println(F("err: card not initialised"));
   }
}

void printDirectory(File dir, int numTabs) {
   while (true) {
      File entry = dir.openNextFile();

      if (!entry) {
         // no more files
         break;
      }

      for (uint8_t i = 0; i < numTabs; i++) {
         MonitorSerial.print('\t');
      }

      MonitorSerial.print(entry.name());

      if (entry.isDirectory()) {
         MonitorSerial.println("/");
         printDirectory(entry, numTabs + 1);

      } else {

         // files have sizes, directories do not
         MonitorSerial.print("\t\t");
         MonitorSerial.println(entry.size(), DEC);
      }

      entry.close();
   }
}