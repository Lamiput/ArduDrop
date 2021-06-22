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

// include arduino types and constants
#include <Arduino.h>

#include "serialcom.h"
#include "command.h"


// init static members
bool SerialCom::initDone = false;
char SerialCom::inputChar;
char SerialCom::inputCmd[MAX_INPUT_SIZE];
unsigned char SerialCom::inputIdx = 0;
unsigned char SerialCom::logLevel = DEBUG;


// setup serial communication - call once at startup
void SerialCom::Setup() {
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
void SerialCom::Loop() {
  if (!initDone) { // exit if not connected first
    return;
  }
  if (inputIdx >= MAX_INPUT_SIZE) { // exit and reset input if cmd is too long
    inputIdx = 0;
    Log(INFO, ("Command to long, dismissed - max: " + (String)MAX_INPUT_SIZE).c_str());
    return;
  }
  if (Serial.available()) {
    inputChar = (char) Serial.read();
    if (inputChar == '\n') {
      inputCmd[inputIdx] = '\0';
      inputIdx = 0;
      Log(DEBUG, inputCmd);
      Command::ParseCommand(inputCmd);
    } else {
      inputCmd[inputIdx] = inputChar;
      inputIdx++;
    }
  }
}


void SerialCom::Log(const unsigned char level, const char* message) {
  if (!initDone) { // exit if not connected first
    return;
  }
  if(level <= logLevel) {
    Serial.println(message);
  }
}


void SerialCom::SetLogLevel(const unsigned char level) {
  logLevel = level > MAXLEVEL?MAXLEVEL:level;
  logLevel = level < MINLEVEL?MINLEVEL:level;
  Log(INFO, ("Loglevel is set to " + (String)logLevel).c_str());
}
