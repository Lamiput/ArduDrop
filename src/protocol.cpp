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

// include arduino types and constants
#include <Arduino.h>

#include "protocol.h"
#include "droplet.h"
#include "logging.h"
#include "utils.h"


// function definitions
void processRunCommand();
void processSetCommand();
void processInfoCommand();
void processResetCommand();
void processHighLowCommand(int mode);


void processCommand(char* cmd) {
  
  // read first token
  char* cmdToken = strtok(cmd, FIELD_SEPARATOR);
  
  if(strcmp(cmdToken, CMD_RUN) == 0) {
    
    logging(DEBUG, "received run command");
    processRunCommand();
    
  } else if(strcmp(cmdToken, CMD_SET) == 0) {
    
    logging(DEBUG, "received set command");
    processSetCommand();
    
  } else if(strcmp(cmdToken, CMD_INFO) == 0) {
    
    logging(DEBUG, "received info command");
    processInfoCommand();
    
  } else if(strcmp(cmdToken, CMD_RESET) == 0) {
    
    logging(DEBUG, "received reset command");
    processResetCommand();
    
  } else if(strcmp(cmdToken, CMD_HIGH) == 0) {
    
    logging(DEBUG, "received high command");
    processHighLowCommand(HIGH);
    
  } else if(strcmp(cmdToken, CMD_LOW) == 0) {
    
    logging(DEBUG, "received low command");
    processHighLowCommand(LOW);
    
  } else if(strcmp(cmdToken, CMD_CANCEL) == 0) {
    logging(DEBUG, "recieved cancel command");
    
  } else {
    
    logging(WARN, "Command not found");
    
  }
  
}


void processRunCommand() {  

  short int rounds = 1;
  int roundDelay = 0; // ms
  
  // get additional arguments if available
  sscanf(strtok(NULL, "\n"), "%hd;%d", &rounds, &roundDelay);
  logging(DEBUG, ("rounds: " + (String)rounds + ", delay: " + (String)roundDelay).c_str());
   
  long startMillis = millis();
  
  // execute actions on droplet
  for(int i = 1; i <= rounds; ) {
    
    long roundStartMillis =  startMillis + (i * (long) roundDelay);
    long currentMillis = millis();
    
    // execute next round at the right time
    if(currentMillis - roundStartMillis >= 0) {
      Serial.println("Execute round #" + String(i));
      executeActions();
      i++;
    }
    
    // check if execution should be cancled
    if(Serial.available()) {
      char inChar[2];
      inChar[1] = '\0'; // for stringconversion
      inChar[0] = (char)Serial.peek();
      if (strcmp(inChar, CMD_CANCEL) == 0) {
        logging(INFO, "execution cancled");
        return;
      }
    }
  }
  logging(INFO, "Finished execution");
}

void processSetCommand() {
  
  short int deviceNumber;
  char deviceMnemonic;
  char times[TIMES_BUFFER_SIZE] = "";
  int chksum = 0, chksumInternal = 0;
    
  // read device infos
  if(sscanf(strtok(NULL, "^"), "%hd;%c;%s", &deviceNumber, &deviceMnemonic, times) < 2) {
    logging(ERROR, "Wrong Format");
    return;
  }
  
  // read chksum
  if(sscanf(strtok(NULL, "\n"), "%d", &chksum) < 1) {
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
    }
    if(duration >= 0) {
      chksumInternal += duration;
    } else {
      duration = MIN_DURATION;
    }
    
    if(deviceNumber < 0 || deviceNumber > DEVICE_NUMBERS - 1) {
      logging(ERROR, "Wrong device number");
      return;
    }
    
    // add new actions to droplet
    addActions(deviceMapping[deviceNumber], offset, duration);
    
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

void processInfoCommand() {  
  logging(DEBUG, "Current device setup:");

  Serial.print("Free memory: ");
  Serial.println(freeMemory());
  
  printActions();
  
}

void processResetCommand() {
  Serial.print("Free memory: ");
  Serial.println(freeMemory());
  clearActions();
  Serial.print("Free memory: ");
  Serial.println(freeMemory());
}

void processHighLowCommand(int mode) {
  
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

void processDebugLevelCommand() {
  int dbgLevel;
  if(sscanf(strtok(NULL, "\n"), "%d", &dbgLevel) < 1) {
    logging(ERROR, "Wrong Format");
    return;
  }
  setLoggingLevel(dbgLevel);
}
