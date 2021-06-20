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
 * along with ArduDrop. If not, see <http://www.gnu.org/licenses/>.
 *******************************************************************************/

#include <Arduino.h>

#include "command.h"
#include "controller.h"
#include "logger.h"
#include "utils.h"

// init static members
bool Command::initDone = false;
char Command::inputChar;
char Command::inputCmd[MAX_INPUT_SIZE];
unsigned char Command::inputIdx = 0;


// setup serial communication - call once at startup
void Command::Setup() {
  if (initDone) {
    return;
  }
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
    Logger::Log(INFO, ("Command to long, dismissed - max: " + (String)MAX_INPUT_SIZE).c_str());
    return;
  }
  if (Serial.available()) {
    inputChar = (char) Serial.read();
    if (inputChar == '\n') {
      inputCmd[inputIdx] = '\0';
      inputIdx = 0;
      Logger::Log(DEBUG, inputCmd);
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
    Logger::Log(DEBUG, "received set command");
    processSetCommand();
    break;
  case CMD_RESET:
    Logger::Log(DEBUG, "received reset command");
    processResetCommand();
    break;
  case CMD_RUN:
    Logger::Log(DEBUG, "received run command");
    processRunCommand();
    break;
  case CMD_CANCEL:
    Logger::Log(DEBUG, "recieved cancel command");
    processCancelCommand();
    break;
  case CMD_INFO:
    Logger::Log(DEBUG, "received info command");
    processInfoCommand();
    break;
  case CMD_HIGH:
    Logger::Log(DEBUG, "received high command");
    processHighLowCommand(HIGH);
    break;
  case CMD_LOW:
    Logger::Log(DEBUG, "received low command");
    processHighLowCommand(LOW);
    break;
  case CMD_DEBUGLEVEL:
    Logger::Log(DEBUG, "recieved set debuglevel command");
    processDebugLvlCommand();
    break;
  default:
    Logger::Log(WARN, "Command not found");;
  }
}


// parse set command
// DeviceNumber;DeviceType;StartTime|Duration[;StartTime|Duration]*^Checksum
void Command::processSetCommand() {
  unsigned char deviceNumber;
  char deviceMnemonic;
  char times[TIMES_BUFFER_SIZE] = "";
  unsigned long chksum = 0, chksumInternal = 0;
  unsigned long offset, duration;   
  // read device info and tasklist
  if(sscanf(strtok(NULL, CHKSUM_SEPARATOR), "%hhu;%c;%s", &deviceNumber, &deviceMnemonic, times) < 2) {
    Logger::Log(ERROR, "Wrong Format");
    return;
  }
  // read chksum
  if(sscanf(strtok(NULL, CMD_SEPARATOR), "%lu", &chksum) < 1) {
    Logger::Log(ERROR, "Wrong Format");
    return;
  }
  // is the target device available
  if(deviceNumber < 0 || deviceNumber > DEVICE_NUMBERS - 1) {
    Logger::Log(ERROR, "Wrong device number");
    return;
  }
  // parse times
  char *token = strtok(times, FIELD_SEPARATOR);
  while(token != NULL) {
    offset = 0,
    duration = 0;
    char remain[] = "";
    // read pair of times
    if(sscanf(token, "%lu|%lu%s", &offset, &duration, remain) == 3) {
      Logger::Log(ERROR, "Wrong Format");
      return;
    }
    // limit offset and duration to sane values and update checksum
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
    // add new actions to droplet
    Controller::AddTask(deviceMapping[deviceNumber], offset, duration);
    // read next time
    token = strtok(NULL, FIELD_SEPARATOR);
  }
  // verify checksum
  if(chksum != chksumInternal) {
    Logger::Log(ERROR, "Wrong checksum");
    return;
  }
  Logger::Log(DEBUG, "Transmission completed and checksum verified!");
}


// call reset of all tasks and memory cleaning
void Command::processResetCommand() {
  Logger::Log(INFO, ("Free memory: " + (String)freeMemory()).c_str());;
  Logger::Log(MINLEVEL, "Deleting Tasks....");
  Controller::DeleteTasks();
  Logger::Log(INFO, ("Free memory: " + (String)freeMemory()).c_str());
}


// parse run rommand
// [;NumberOfRounds[;PauseTime]]
void Command::processRunCommand() {  

  unsigned char rounds = 1;
  unsigned long roundDelay = 0; // ms
  
  // get additional arguments if available
  sscanf(strtok(NULL, "\n"), "%hhu;%lu", &rounds, &roundDelay);
  Logger::Log(DEBUG, ("rounds: " + (String)rounds + ", delay: " + (String)roundDelay).c_str());
}


// request cancel of task run
void Command::processCancelCommand() {

}


// show infos
void Command::processInfoCommand() {  
  Logger::Log(MINLEVEL, "Current device setup:");
  Logger::Log(MINLEVEL, ("Free memory: " + (String)freeMemory()).c_str()); 
  Controller::TaskInfo();
}


// parse static on/off command
// DeviceNumber -> On/Off is already specified
void Command::processHighLowCommand(unsigned char mode) {
  
  unsigned char deviceNumber;
  
  // read device infos
  if(sscanf(strtok(NULL, "\n"), "%hhu", &deviceNumber) < 1) {
    Logger::Log(ERROR, "Wrong Format");
    return;
  }
  
  // check device number bounds
  if(deviceNumber < 1 || deviceNumber > DEVICE_NUMBERS) {
    Logger::Log(WARN, "wrong device number");
    return;
  }
  
  digitalWrite(deviceMapping[deviceNumber], mode);
}


// set Level of debug-information
// 0->3 (Info->Debug)
void Command::processDebugLvlCommand() {
  unsigned char dbgLevel;
  if(sscanf(strtok(NULL, "\n"), "%hhu", &dbgLevel) < 1) {
    Logger::Log(ERROR, "Wrong Format");
    return;
  }
  Logger::SetLogLevel(dbgLevel);
}
