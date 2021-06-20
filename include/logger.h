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

#ifndef __LOGGING_H__
#define __LOGGING_H__


// logging message levels
#define ERROR 0
#define WARN 1
#define INFO 2
#define DEBUG 3
#define MINLEVEL 0
#define MAXLEVEL 3

//logger class
class Logger
{
private:
    static unsigned char logLevel;
public:
  static void Log(const unsigned char level, const char* message);
  static void SetLogLevel(const unsigned char level);
  static unsigned char GetLogLevel() {return logLevel; }
};


#endif
