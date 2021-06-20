 /*******************************************************************************
 * Project: ArduDrop - Toolkit for Liquid Art Photographers
 * Copyright (C) 2021 Holger Pasligh
 * 
 * This program incorporates a modified version of "Droplet - Toolkit for Liquid Art Photographers"
 * Copyright (C) 2012 Stefan Brenner
 *
 * This file is part of ArduDrop.
 *
 * ArduDrop is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ArduDrop is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Droplet. If not, see <http://www.gnu.org/licenses/>.
 *******************************************************************************/

#include <Arduino.h>

#include "command.h"
#include "droplet.h"
#include "logging.h"
#include "utils.h"

// init static members
bool Command::initDone = false;
char Command::inputChar;
char Command::inputCmd[MAX_INPUT_SIZE];
short int Command::inputIdx = 0;


// setup serial communication - call once at startup
void Command::Setup() {
  Serial.begin(BAUD_RATE);
  //wait until Serial Port is opened
  while (!Serial) {
    delay(100); //wait for serial port
  }
  initDone = true;
}


// cyclic called task - check serial-input for next command
void Command::Loop() {
  if (!initDone) { // exit if not connected first
    return;
  }
  if (inputIdx >= MAX_INPUT_SIZE) { // exit and reset input if cmd is too long
    inputIdx = 0;
    logging(INFO, ("Command to long, dismissed - max: " + (String)MAX_INPUT_SIZE).c_str());
    return;
  }
  if (Serial.available()) {
    inputChar = (char) Serial.read();
    if (inputChar == '\n') {
      inputCmd[inputIdx] = '\0';
      inputIdx = 0;
      logging(DEBUG, inputCmd);
      ParseCommand(inputCmd);
    } else {
      inputCmd[inputIdx] = inputChar;
      inputIdx++;
    }
  }
}


// check command and call specific subroutine for parsing
void Command::ParseCommand(char* cmd) {
  char* cmdToken = strtok(cmd, FIELD_SEPARATOR);
  switch (cmdToken[0])
  {
  case CMD_SET:
    logging(DEBUG, "received set command");
    processSetCommand();
    break;
  case CMD_RESET:
    logging(DEBUG, "received reset command");
    break;
  case CMD_RUN:
    logging(DEBUG, "received run command");
    processRunCommand();
    break;
  case CMD_CANCEL:
    logging(DEBUG, "recieved cancel command");
    break;
  case CMD_INFO:
    logging(DEBUG, "received info command");
    processInfoCommand();
    break;
  case CMD_HIGH:
    logging(DEBUG, "received high command");
    break;
  case CMD_LOW:
    logging(DEBUG, "received low command");
    break;
  case CMD_DEBUGLEVEL:
    logging(DEBUG, "recieved set debuglevel command");
    break;
  default:
    logging(WARN, "Command not found");;
  }
}


// parse set command
// DeviceNumber;DeviceType;StartTime|Duration[;StartTime|Duration]*^Checksum
void Command::processSetCommand() {
  
  short int deviceNumber;
  char deviceMnemonic;
  char times[TIMES_BUFFER_SIZE] = "";
  int chksum = 0, chksumInternal = 0;
    
  // read device info and tasklist
  if(sscanf(strtok(NULL, CHKSUM_SEPARATOR), "%hd;%c;%s", &deviceNumber, &deviceMnemonic, times) < 2) {
    logging(ERROR, "Wrong Format");
    return;
  }
  // read chksum
  if(sscanf(strtok(NULL, CMD_SEPARATOR), "%d", &chksum) < 1) {
    logging(ERROR, "Wrong Format");
    return;
  }
  // parse times
  char *token = strtok(times, FIELD_SEPARATOR);
  while(token != NULL) {
    short int offset = -1, duration = -1;
    char remain[] = "";
    
    if(sscanf(token, "%hd|%hd%s", &offset, &duration, remain) == 3) {
      logging(ERROR, "Wrong Format");
      return;
    }
    
    if(offset >= 0) {
      chksumInternal += offset;
    } else {
      offset = 0;
    }
    if(duration > 0) {
      chksumInternal += duration;
    } else {
      duration = MIN_DURATION;
    }
    
    if(deviceNumber < 0 || deviceNumber > DEVICE_NUMBERS - 1) {
      logging(ERROR, "Wrong device number");
      return;
    }
    
    // add new actions to droplet
    // addActions(deviceMapping[deviceNumber], offset, duration);
    
    // read next time
    token = strtok(NULL, FIELD_SEPARATOR);
  }
  
  // verify checksum
  if(chksum != chksumInternal) {
    logging(ERROR, "Wrong checksum");
    return;
  }
  
  logging(DEBUG, "Transmission completed and checksum verified!");
  
}


// call reset of all tasks and memory cleaning
void Command::processResetCommand() {
  Serial.print("Free memory: ");
  Serial.println(freeMemory());
  clearActions();
  Serial.print("Free memory: ");
  Serial.println(freeMemory());
}


// parse run rommand
// [;NumberOfRounds[;PauseTime]]
void Command::processRunCommand() {  

  int rounds = 1;
  int roundDelay = 0; // ms
  
  // get additional arguments if available
  sscanf(strtok(NULL, "\n"), "%d;%d", &rounds, &roundDelay);
  logging(DEBUG, ("rounds: " + (String)rounds + ", delay: " + (String)roundDelay).c_str());
}


// request cancel of task run
void Command::processCancelCommand() {

}


// show infos
void Command::processInfoCommand() {  
  logging(DEBUG, "Current device setup:");

  Serial.print("Free memory: ");
  Serial.println(freeMemory());
  
  // printActions();
  
}


// parse static on/off command
// DeviceNumber -> On/Off is already specified
void Command::processHighLowCommand(int mode) {
  
  int deviceNumber;
  
  // read device infos
  if(sscanf(strtok(NULL, "\n"), "%d", &deviceNumber) < 1) {
    logging(ERROR, "Wrong Format");
    return;
  }
  
  // check device number bounds
  if(deviceNumber < 1 || deviceNumber > DEVICE_NUMBERS) {
    logging(WARN, "wrong device number");
    return;
  }
  
  digitalWrite(deviceMapping[deviceNumber], mode);
}


// set Level of debug-information
// 0->3 (Info->Debug)
void Command::processDebugLvlCommand() {
  int dbgLevel;
  if(sscanf(strtok(NULL, "\n"), "%d", &dbgLevel) < 1) {
    logging(ERROR, "Wrong Format");
    return;
  }
  setLoggingLevel(dbgLevel);
}
