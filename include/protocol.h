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

#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

//Basics
#define TIMES_BUFFER_SIZE   40 // be careful with memory !!!
#define DEVICE_NUMBERS      50 // how many digital pins should be mapped?
#define MIN_DURATION        10 // if 0, duration of task will be set to this value

// commands
#define CMD_RUN     "R"
#define CMD_SET     "S"
#define CMD_INFO    "I"
#define CMD_RESET   "X"
#define CMD_CANCEL  'C'
#define CMD_HIGH    "H"
#define CMD_LOW     "L"

// separators
#define FIELD_SEPARATOR   ";"
#define TIME_SEPARATOR    "|"
#define CHKSUM_SEPARATOR  "^"
#define DEVICE_SEPARATOR  "\n"

// devices
#define DEVICE_VALVE    "V"
#define DEVICE_FLASH    "F"
#define DEVICE_CAMERA   "C"
#define DEVICE_BUTTON   "B"


/*
 * Process and execute a command string
 */
void processCommand(char* cmd);


#endif
