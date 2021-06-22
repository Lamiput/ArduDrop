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
#include "serialcom.h"
#include "utils.h"





// check command and call specific subroutine for parsing
void Command::ParseCommand(char* cmd) {
  char* cmdToken = strtok(cmd, FIELD_SEPARATOR);
  switch (cmdToken[0])
  {
  case CMD_SET:
    SerialCom::Log(DEBUG, "received set command");
    processSetCommand();
    break;
  case CMD_RESET:
    SerialCom::Log(DEBUG, "received reset command");
    processResetCommand();
    break;
  case CMD_RUN:
    SerialCom::Log(DEBUG, "received run command");
    processRunCommand();
    break;
  case CMD_CANCEL:
    SerialCom::Log(DEBUG, "recieved cancel command");
    processCancelCommand();
    break;
  case CMD_INFO:
    SerialCom::Log(DEBUG, "received info command");
    processInfoCommand();
    break;
  case CMD_HIGH:
    SerialCom::Log(DEBUG, "received high command");
    processHighLowCommand(HIGH);
    break;
  case CMD_LOW:
    SerialCom::Log(DEBUG, "received low command");
    processHighLowCommand(LOW);
    break;
  case CMD_DEBUGLEVEL:
    SerialCom::Log(DEBUG, "recieved set debuglevel command");
    processDebugLvlCommand();
    break;
  default:
    SerialCom::Log(WARN, "Command not found");;
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
    SerialCom::Log(ERROR, "Wrong Format");
    return;
  }
  // read chksum
  if(sscanf(strtok(NULL, CMD_SEPARATOR), "%lu", &chksum) < 1) {
    SerialCom::Log(ERROR, "Wrong Format");
    return;
  }
  // is the target device available
  if(deviceNumber < 0 || deviceNumber > DEVICE_NUMBERS - 1) {
    SerialCom::Log(ERROR, "Wrong device number");
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
      SerialCom::Log(ERROR, "Wrong Format");
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
    SerialCom::Log(ERROR, "Wrong checksum");
    return;
  }
  SerialCom::Log(DEBUG, "Transmission completed and checksum verified!");
}


// call reset of all tasks and memory cleaning
void Command::processResetCommand() {
  SerialCom::Log(INFO, ("Free memory: " + (String)freeMemory()).c_str());;
  SerialCom::Log(MINLEVEL, "Deleting Tasks....");
  Controller::DeleteTasks();
  SerialCom::Log(INFO, ("Free memory: " + (String)freeMemory()).c_str());
}


// parse run rommand
// [;NumberOfRounds[;PauseTime]]
void Command::processRunCommand() {  

  unsigned char rounds = 1;
  unsigned long roundDelay = 0; // ms
  
  // get additional arguments if available
  sscanf(strtok(NULL, "\n"), "%hhu;%lu", &rounds, &roundDelay);
  SerialCom::Log(DEBUG, ("rounds: " + (String)rounds + ", delay: " + (String)roundDelay).c_str());
  Controller::ReqRun(rounds, roundDelay);
}


// request cancel of task run
void Command::processCancelCommand() {
  Controller::ReqCancel();
}


// show infos
void Command::processInfoCommand() {  
  SerialCom::Log(MINLEVEL, "Current device setup:");
  SerialCom::Log(MINLEVEL, ("Free memory: " + (String)freeMemory()).c_str()); 
  Controller::TaskInfo();
}


// parse static on/off command
// DeviceNumber -> On/Off is already specified
void Command::processHighLowCommand(unsigned char mode) {
  unsigned char deviceNumber;
  // read device infos
  if(sscanf(strtok(NULL, "\n"), "%hhu", &deviceNumber) < 1) {
    SerialCom::Log(ERROR, "Wrong Format");
    return;
  }
  // check device number bounds
  if(deviceNumber < 1 || deviceNumber > DEVICE_NUMBERS) {
    SerialCom::Log(WARN, "wrong device number");
    return;
  }
  Controller::ReqSwitch(deviceMapping[deviceNumber], mode);
}


// set Level of debug-information
// 0->3 (Info->Debug)
void Command::processDebugLvlCommand() {
  unsigned char dbgLevel;
  if(sscanf(strtok(NULL, "\n"), "%hhu", &dbgLevel) < 1) {
    SerialCom::Log(ERROR, "Wrong Format");
    return;
  }
  SerialCom::SetLogLevel(dbgLevel);
}
