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

#include "logger.h"


// init static members
unsigned char Logger::logLevel = DEBUG;


void Logger::Log(const unsigned char level, const char* message) {
  if(level <= logLevel) {
    Serial.println(message);
  }
}

void Logger::SetLogLevel(const unsigned char level) {
  logLevel = level > MAXLEVEL?MAXLEVEL:level;
  logLevel = level < MINLEVEL?MINLEVEL:level;
  Log(INFO, ("Loglevel is set to " + (String)logLevel).c_str());
}
