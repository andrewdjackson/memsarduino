#ifndef DATALOG_H
#define DATALOG_H

#include "constants.h"
#include "utils.h"
#include <SD.h>
#include <SPI.h>

typedef struct {
   char filename[14];
   File file;
   bool initialised;

} sd_card;

void init_sd_card(sd_card *card);
void write_log(sd_card *card, char *data);
void write_header(sd_card *card);
void list_files(sd_card *card);
void printDirectory(File dir, int numTabs);
void get_filename(sd_card *card);

#endif